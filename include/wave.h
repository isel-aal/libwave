#ifndef WAVELIB_H
#define WAVELIB_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

typedef struct {
	char ChunkID[4];
	uint32_t ChunkSize;
} ChunkHeader;

typedef struct {
	ChunkHeader header;
	char Format[4];		/* WAVE */
} RiffChunk;

typedef struct {
	ChunkHeader header;
	uint16_t AudioFormat;
	uint16_t NumChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
} SubChunkFmt;

typedef struct {
	ChunkHeader header;
} SubChunkData;

typedef struct wave {
	FILE *fd;
	RiffChunk riff_chunk;
	SubChunkFmt subchunk_fmt;
	SubChunkData subchunk_data;
	long file_offset;
	GArray *samples;
	size_t current;
} Wave;

Wave *wave_create(int bits_per_sample, int channels);
void wave_destroy(Wave *wave);
Wave *wave_load(const char *filename);
int wave_store(Wave *wave, const char *filename);

int wave_get_bytes_per_frame(Wave *wave);
int wave_get_bits_per_sample(Wave *wave);
int wave_get_sample_rate(Wave *wave);
int wave_get_number_of_channels(Wave *wave);

void wave_set_bits_per_sample(Wave *wave, int bits_per_sample);
void wave_set_sample_rate(Wave *wave, int sample_rate);
void wave_set_number_of_channels(Wave *wave, int number_of_channels);

size_t wave_get_samples_by_channel(Wave *wave, int channel, size_t frame_index, char *buffer, size_t sample_count);
size_t wave_get_samples(Wave *wave, size_t frame_index, char *buffer, size_t frame_count);
size_t wave_put_samples(Wave *wave, size_t frame_index, char *buffer, size_t frame_count);

void wave_set_current_position(Wave *wave, size_t frame_index);
size_t wave_read_samples(Wave *wave, char *buffer, size_t frame_count);
size_t wave_write_samples(Wave *wave, char *buffer, size_t frame_count);

size_t wave_append_samples(Wave *wave, char *buffer, size_t frame_count);
size_t wave_get_size(Wave *wave);
void wave_format_update(Wave *wave);

size_t wave_get_duration(Wave *wave);

#endif
