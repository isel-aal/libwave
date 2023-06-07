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
 * Junta num ficheiro WAVE o conteúdo de multiplos ficheiros WAVE.
 * O ficheiro WAVE produzido possui um canal por cada ficheiro de entrada.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <getopt.h>
#include "wave.h"

static void help(char *prog_name) {
	printf("Usage: %s [options] <input WAVE 1> <input WAVE 2> ...\n"
		"options:\n"
		"\t--verbose\n"
		"\t-h, --help\n"
		"\t-v, --version\n"
		"\t-o, --output <file name>\n"
		"\t-f, --format <S16_LE | S32_LE>\n"
		"\t-r, --sample_rate <rate>\n",
		prog_name);
}

#define CONFIG_VERSION "0.00"

static void about() {
	printf("join v" CONFIG_VERSION " (" __DATE__ ")\n"
		"Ezequiel Conde (ezeq@cc.isel.ipl.pt)\n");
}

int main(int argc, char *argv[]) {
	static int verbose_flag = false;
	static struct option long_options[] = {
		{"verbose", no_argument, &verbose_flag, 1},
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"output", required_argument, 0, 'o'},
		{0, 0, 0, 0}
	};

	int option_index, option_char;
	int error_in_options = false;

	char *option_output_filename = NULL;

	while ((option_char = getopt_long(argc, argv, ":hvi:o:f:r:d:l:t:c:g:",
			long_options, &option_index)) != -1) {
		switch (option_char) {
		case 0:	//	Opções longas com afetação de flag
			break;
		case 'h':
			help(argv[0]);
			return 0;
		case 'v':
			about();
			break;
		case 'o':
			option_output_filename = optarg;
			break;
		case ':':
			fprintf(stderr, "Error in option -%c argument\n", optopt);
			error_in_options = true;
			break;
		case '?':
			error_in_options = true;
			break;
		}
	}
	if (error_in_options) {
		help(argv[0]);
		exit(EXIT_FAILURE);
	}

	int channels = argc - optind;
	if (channels == 0) {
		fprintf(stderr, "\nError: no WAVE input files, at least one is necessary\n\n");
		help(argv[0]);
		exit(EXIT_FAILURE);
	}

	int bits_per_sample;
	int sample_rate;

	Wave *wave_in[channels];
	for (int i = 0; i < channels; ++i) {
		wave_in[i] = wave_load(argv[optind + i]);
		if (wave_in[i] == NULL) {
			fprintf(stderr, "Error loading WAVE file %s\n", argv[optind + i]);
			exit(EXIT_FAILURE);
		}
		if (wave_get_number_of_channels(wave_in[i]) > 1) {
			fprintf(stderr, "Error: WAVE file %s have more than 1 channel\n", argv[optind + i]);
			exit(EXIT_FAILURE);
		}
		if (i == 0)
			bits_per_sample = wave_get_bits_per_sample(wave_in[0]);
		else if (bits_per_sample != wave_get_bits_per_sample(wave_in[i])) {
			fprintf(stderr, "Error: WAVE file %s have a wrong value of bits-per-sample.\n"
					"\tReference value: %d, actual value: %d\n", argv[optind + i],
					bits_per_sample, wave_get_bits_per_sample(wave_in[i]));
			exit(EXIT_FAILURE);
		}
		if (i == 0)
			sample_rate = wave_get_sample_rate(wave_in[0]);
		else if (sample_rate != wave_get_sample_rate(wave_in[i])) {
			fprintf(stderr, "Error: WAVE file %s have a wrong sample rate.\n"
					"\tReference value: %d, actual value: %d\n", argv[optind + i],
					sample_rate, wave_get_sample_rate(wave_in[i]));
			exit(EXIT_FAILURE);
		}
	}

	if (verbose_flag) {
		printf("channels = %d\n", channels);
		printf("sample rate = %d\n", sample_rate);
		printf("bits_per_sample = %d\n", bits_per_sample);
		printf("output filename = %s\n", option_output_filename);
		for (int i = 0; i < channels; i++)
			printf("input filename [%d] = %s\n", i, argv[optind + i]);
	}

	Wave *out = wave_create(bits_per_sample, channels);
	wave_set_sample_rate(out, sample_rate);

	while (true) {
		unsigned bytes_per_sample = bits_per_sample / CHAR_BIT;
		char framme[channels * bytes_per_sample];
		char *framme_ptr = framme;
		for (int i = 0; i < channels; ++i) {
			size_t result = wave_read_samples(wave_in[i], framme_ptr, 1);
			if (result < 1)
				goto break2;
			framme_ptr += bytes_per_sample;
		}
		wave_append_samples(out, framme, 1);
	}
break2:
	wave_format_update(out);
	wave_store(out, option_output_filename);
}


