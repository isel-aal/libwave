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
#include <stdlib.h>
#include <alsa/asoundlib.h>

#include "wave.h"

static void capture(Wave *wave, unsigned duration) {
	snd_pcm_t *handle;
	int result = snd_pcm_open (&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
	if (result < 0) {
		fprintf (stderr, "cannot open audio device %s (%s)\n",
			 "default",
			 snd_strerror (result));
			 exit(EXIT_FAILURE);
	}
	result = snd_pcm_set_params(handle,
					  SND_PCM_FORMAT_S16_LE,
					  SND_PCM_ACCESS_RW_INTERLEAVED,
					  wave_get_number_of_channels(wave),
					  wave_get_sample_rate(wave),
					  1,
					  500000);   /* 0.5 sec */
	if (result < 0) {
		fprintf(stderr, "snd_pcm_set_params: %s\n",
			snd_strerror(result));
			exit(EXIT_FAILURE);
    }
	result = snd_pcm_prepare(handle);
	if (result < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
			 snd_strerror(result));
		exit(EXIT_FAILURE);
	}

	const snd_pcm_sframes_t period_size = wave_get_sample_rate(wave) / 10;

	int frame_size = snd_pcm_frames_to_bytes(handle, 1);
	char buffer[period_size * frame_size];
	int frame_index = 0;
	for (unsigned time = 0; time / 10 < duration; time++) {
		snd_pcm_sframes_t read_frames =
			snd_pcm_readi(handle, buffer, period_size);
		if (read_frames < 0) {
			fprintf (stderr, "read from audio interface failed (%s)\n",
					snd_strerror(read_frames));
			exit(EXIT_FAILURE);
		}

		wave_put_samples(wave, frame_index, buffer, read_frames);
		frame_index += read_frames;
		fputc('.', stderr);
		//print_samples(buffer, read_frames, wave_get_number_of_channels(wave));
	}
	snd_pcm_close(handle);
	wave_format_update(wave);
}

int main(int argc, char *argv[]) {
	if (argc != 2 && argc != 3) {
		fprintf(stderr, "usage: %s <wave filename> <seconds>", argv[0]);
		return -1;
	}
	int duration = 5;
	if (argc == 3)
		duration = atoi(argv[2]);

	Wave *wave = wave_create(16, 1);
	if (wave == NULL) {
		fprintf(stderr, "Error creating WAVE support\n");
		return -1;
	}
	wave_set_sample_rate(wave, 44100);

	capture(wave, duration);
	
	wave_store(wave, argv[1]);
	wave_destroy(wave);
}
