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
#include "wave.h"

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <input file> <output file>", argv[0]);
		return -1;
	}

	Wave *in_wave = wave_load(argv[1]);
	if (in_wave == NULL) {
		fprintf(stderr, "Error loading file \"%s\"\n", argv[1]);
		return -1;
	}

	Wave *out_wave = wave_create(wave_get_bits_per_sample(in_wave),
								 wave_get_number_of_channels(in_wave));
	if (out_wave == NULL) {
		fprintf(stderr, "Error creating WAVE support\n");
		return -1;
	}

	wave_set_sample_rate(out_wave, wave_get_sample_rate(in_wave));

	const unsigned period_size = wave_get_sample_rate(in_wave) / 10;
	int frame_size = wave_get_bytes_per_frame(in_wave);

	char buffer[period_size * frame_size];

	size_t read_frames = wave_read_samples(in_wave, buffer, period_size);
	while (read_frames > 0) {

		/* TRANSFORMAÇÃO */

		wave_write_samples(out_wave, buffer, read_frames);
		read_frames = wave_read_samples(in_wave, buffer, period_size);
	}
	wave_format_update(out_wave);
	wave_store(out_wave, argv[2]);

	wave_destroy(in_wave);
	wave_destroy(out_wave);
}
