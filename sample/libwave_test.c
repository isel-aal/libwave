#include "wave.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <wave filename>\n", argv[0]);
		return -1;
	}

	Wave *wave = wave_load(argv[1]);
	if (wave == NULL)
		fprintf(stderr, "Error\n");
	else
		fprintf(stderr, "OK\n");
	printf("Number of channels: %d\n", wave_get_number_of_channels(wave));
	printf("Sample rate: %d\n", wave_get_sample_rate(wave));
	printf("Bits per sample: %d\n", wave_get_bits_per_sample(wave));
//	printf("Block align: %d\n", wave_get_block_align(wave));
}
