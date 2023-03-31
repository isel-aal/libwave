/*
Copyright 2023 Ezequiel Conde

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#include "wave.h"

void play(Wave* wave);

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <wave filename>", argv[0]);
		return -1;
	}

	Wave *wave = wave_load(argv[1]);
	if (wave == NULL) {
		fprintf(stderr, "Error loading file \"%s\"\n", argv[1]);
		return -1;
	}
	play(wave);
	wave_destroy(wave);
}

//#define	SOUND_DEVICE "hw:0,0"

#define	SOUND_DEVICE "default"

/*----------------------------------------------------------------------
 * Função para reprodução de um ficheiro WAVE
 * Baseado em:
 * https://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm_min_8c-example.html
 */
void play(Wave *wave) {
	snd_pcm_t *handle = NULL;
	int result = snd_pcm_open(&handle, SOUND_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
	if (result < 0) {
		printf("snd_pcm_open(&handle, %s, SND_PCM_STREAM_PLAYBACK, 0): %s\n",
				SOUND_DEVICE, snd_strerror(result));
		exit(EXIT_FAILURE);
	}

	snd_config_update_free_global();

	result = snd_pcm_set_params(handle,
					  SND_PCM_FORMAT_S16_LE,
					  SND_PCM_ACCESS_RW_INTERLEAVED,
					  wave_get_number_of_channels(wave),
					  wave_get_sample_rate(wave),
					  1,
					  500000);
	if (result < 0) {
		fprintf(stderr, "Playback open error: %s\n", snd_strerror(result));
		exit(EXIT_FAILURE);
	}

	const snd_pcm_sframes_t period_size = 64;
	int frame_size = snd_pcm_frames_to_bytes(handle, 1);

	char buffer[period_size * frame_size];

	size_t read_frames = wave_read_samples(wave, buffer, period_size);
	while (read_frames > 0) {
		snd_pcm_sframes_t wrote_frames = snd_pcm_writei(handle, buffer, read_frames);
		if (wrote_frames < 0)
			wrote_frames = snd_pcm_recover(handle, wrote_frames, 0);
		if (wrote_frames < 0) {
			printf("snd_pcm_writei failed: %s\n", snd_strerror(wrote_frames));
			break;
		}

		if (wrote_frames < read_frames)
			fprintf(stderr, "Short write (expected %zd, wrote %li)\n",
					read_frames, wrote_frames);

		read_frames = wave_read_samples(wave, buffer, period_size);
	}
	/* pass the remaining samples, otherwise they're dropped in close */
	result = snd_pcm_drain(handle);
	if (result < 0)
		printf("snd_pcm_drain failed: %s\n", snd_strerror(result));

	snd_pcm_close(handle);
	snd_config_update_free_global();
}
