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

/*
 * Extrai de um ficheiro WAVE com múltiplos canais,
 * as amostras de um dado canal, para um ficheiro WAVE
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "wave.h"

static char *make_output_filename(char *filename, int channel) {
	size_t filename_size = strlen(filename);
	char *p = filename + filename_size;
	while (*p != '.' && p > filename)
		--p;
	if (p == filename)
		p = filename + filename_size;

	size_t buffer_size = filename_size + 3;
	char *buffer = malloc(buffer_size);
	size_t basename_size = p - filename;
	memcpy(buffer, filename, basename_size);
	snprintf(buffer + basename_size, buffer_size - basename_size, "-%d%s", channel, p);
	return buffer;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <wave filename> <channel>\n", argv[0]);
		return -1;
	}

	Wave *wave = wave_load(argv[1]);
	if (wave == NULL) {
		fprintf(stderr, "Error loading file \"%s\"\n", argv[1]);
		return -1;
	}

	int channel = atoi(argv[2]);
	if (channel < 1 || channel > wave_get_number_of_channels(wave))  {
		fprintf(stderr, "Wrong channel number: %d. ", channel);
		fprintf(stderr, "Must be between 1 and %d.\n", wave_get_number_of_channels(wave));
		return -1;
	}

	Wave *out = wave_create(wave_get_bits_per_sample(wave), 1);

	size_t bytes_per_sample = wave_get_bits_per_sample(wave) / CHAR_BIT;
	#define BLOCK_SAMPLES	1000
	char buffer[bytes_per_sample * BLOCK_SAMPLES];

	size_t frame_index = 0;
	size_t read_samples = wave_get_samples_by_channel(wave, frame_index, buffer, BLOCK_SAMPLES, channel - 1);
	while (read_samples > 0) {
		frame_index += read_samples;
		wave_append_samples(out, buffer, read_samples);
		read_samples = wave_get_samples_by_channel(wave, frame_index, buffer, BLOCK_SAMPLES, channel - 1);
	}
	wave_set_sample_rate(out, wave_get_sample_rate(wave));
	wave_format_update(out);
	wave_destroy(wave);
	wave_store(out, make_output_filename(argv[1], channel));
}

