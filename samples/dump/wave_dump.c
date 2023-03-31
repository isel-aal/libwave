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

/*----------------------------------------------------------------------
 *	Mostra parâmetros de codificação de um ficheiro WAVE
 */
 
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "wave.h"

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

	printf("Número de canais: %d\n", wave_get_number_of_channels(wave));
	printf("Bits por amostra: %d\n", wave_get_bits_per_sample(wave));
	printf("Ritmo de amostragem: %d\n", wave_get_sample_rate(wave));
	printf("Duração: %zd\n", wave_get_duration(wave));

	wave_destroy(wave);
}
