#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "wave.h"

#define NDEBUG

static int read_chunk_header(FILE *fd, ChunkHeader *chunk_header) {
	size_t read_bytes = fread(chunk_header, sizeof *chunk_header, 1, fd);
	return read_bytes == 1;
}

Wave *wave_create(int bits_per_sample, int channels) {
	Wave *wave = malloc(sizeof *wave);
	if (wave != NULL) {
		memcpy(wave->riff_chunk.header.ChunkID, "RIFF", 4);
		memcpy(wave->riff_chunk.Format, "WAVE", 4);
		memcpy(wave->subchunk_fmt.header.ChunkID, "fmt ", 4);
		wave->subchunk_fmt.header.ChunkSize = 16;
		wave->subchunk_fmt.AudioFormat = 1;
		wave->subchunk_fmt.BitsPerSample = bits_per_sample;
		wave->subchunk_fmt.NumChannels = channels;
		wave->subchunk_fmt.BlockAlign =
			(wave->subchunk_fmt.BitsPerSample + 7) / 8 * wave->subchunk_fmt.NumChannels;
		memcpy(wave->subchunk_data.header.ChunkID, "data", 4);
		wave->samples = g_array_new(FALSE, FALSE, wave->subchunk_fmt.BlockAlign);
	}
	return wave;
}

void wave_destroy(Wave *wave) {
//	g_array_free(wave->samples, TRUE);
	g_array_unref(wave->samples);
	fclose(wave->fd);
	free(wave);
}

static size_t file_write_int(FILE *fd, unsigned value, size_t nbytes) {
	return fwrite(&value, nbytes, 1, fd);
}

static size_t file_write_array_char(FILE *fd, char array[], size_t nchars) {
	return fwrite(array, 1, nchars, fd);
}

void wave_format_update(Wave *wave) {
	wave->subchunk_data.header.ChunkSize = wave->samples->len;
	wave->riff_chunk.header.ChunkSize
		= wave->subchunk_fmt.header.ChunkSize + wave->subchunk_data.header.ChunkSize;
	wave->subchunk_fmt.ByteRate = wave->subchunk_fmt.SampleRate * wave->subchunk_fmt.BlockAlign;
}

int wave_store(Wave *wave, const char *filename) {
	FILE *fd = fopen(filename, "w");
	if (fd == NULL)
		return 0;
	wave->fd = fd;
	wave_format_update(wave);
	file_write_array_char(fd, wave->riff_chunk.header.ChunkID, 4);
	file_write_int(fd, wave->riff_chunk.header.ChunkSize, 4);
	file_write_array_char(fd, wave->riff_chunk.Format, 4);

	file_write_array_char(fd, wave->subchunk_fmt.header.ChunkID, 4);
	file_write_int(fd, wave->subchunk_fmt.header.ChunkSize, 4);
	file_write_int(fd, wave->subchunk_fmt.AudioFormat, 2);
	file_write_int(fd, wave->subchunk_fmt.NumChannels, 2);
	file_write_int(fd, wave->subchunk_fmt.SampleRate, 4);
	file_write_int(fd, wave->subchunk_fmt.ByteRate, 4);
	file_write_int(fd, wave->subchunk_fmt.BlockAlign, 2);
	file_write_int(fd, wave->subchunk_fmt.BitsPerSample, 2);

	file_write_array_char(fd, wave->subchunk_data.header.ChunkID, 4);
	file_write_int(fd, wave->subchunk_data.header.ChunkSize, 4);
	size_t wrote_frames = fwrite(wave->samples->data,
		g_array_get_element_size(wave->samples), wave->samples->len, fd);
	fclose(wave->fd);
	return wrote_frames == wave->samples->len;
}

Wave *wave_load(const char *filename) {
	Wave *wave = NULL;
	FILE *fd = fopen(filename, "r");
	if (fd == NULL)
		goto exit_error0;

	wave = malloc(sizeof *wave);
	if (wave == NULL)
		goto exit_error1;

	wave->fd = fd;

	size_t size_riff_chunk = fread(&wave->riff_chunk, sizeof wave->riff_chunk, 1, fd);
	if (size_riff_chunk != 1)
		goto exit_error2;

#ifndef NDEBUG
	printf("ChunkID=");
	fwrite(wave->riff_chunk.header.ChunkID, 1, 4, stdout);
	putchar('\n');
	printf("ChunkSize=%u\n", wave->riff_chunk.header.ChunkSize);
	printf("Format=");
	fwrite(wave->riff_chunk.Format, 1, 4, stdout);
	putchar('\n');

	putchar('\n');
#endif

	while (1) {
		ChunkHeader chunk_header;
		int result = read_chunk_header(fd, &chunk_header);
		if (!result)
			goto exit_error0;

		if (strncmp(chunk_header.ChunkID, "fmt ", 4) == 0) {
			SubChunkFmt *subchunk_fmt = &wave->subchunk_fmt;

			subchunk_fmt->header = chunk_header;
			size_t size_subchunk_fmt = fread(&subchunk_fmt->AudioFormat,
										subchunk_fmt->header.ChunkSize, 1, fd);
			if (size_subchunk_fmt != 1)
				goto exit_error2;
#ifndef NDEBUG
			printf("SubChunk1ID=");
			fwrite(subchunk_fmt->header.ChunkID, 1, 4, stdout);
			putchar('\n');
			printf("SubChunk1Size=%u\n", subchunk_fmt->header.ChunkSize);
			printf("AudioFormat=%u\n", subchunk_fmt->AudioFormat);
			printf("NumChannels=%u\n", subchunk_fmt->NumChannels);
			printf("SampleRate=%d\n", subchunk_fmt->SampleRate);
			printf("ByteRate=%d\n", subchunk_fmt->ByteRate);
			printf("BlockAligh=%d\n", subchunk_fmt->BlockAlign);
			printf("BitsPerSample=%d\n", subchunk_fmt->BitsPerSample);
			putchar('\n');
#endif
		}

		else if (strncmp(chunk_header.ChunkID, "LIST", 4) == 0) {
#ifndef NDEBUG
			printf("SubChunkID=");
			fwrite(chunk_header.ChunkID, 1, 4, stdout);
			putchar('\n');
			printf("SubChunkSize=%u\n", chunk_header.ChunkSize);
			putchar('\n');
#endif
			fseek(fd, chunk_header.ChunkSize, SEEK_CUR);
		}

		else if (strncmp(chunk_header.ChunkID, "data", 4) == 0) {
			wave->subchunk_data.header = chunk_header;
			wave->file_offset = ftell(fd);
#if 0
			int result = fseek(wave->fd, chunk_header.ChunkSize, SEEK_CUR);
			if (result != 0) {
				fprintf(stderr, "fseek(): %s\n", strerror(errno));
				goto exit_error2;
			}
#endif
			wave->current = 0;
			size_t nframes = chunk_header.ChunkSize / wave_get_bytes_per_frame(wave);
			wave->samples = g_array_sized_new(FALSE, FALSE, wave_get_bytes_per_frame(wave), nframes);
			g_array_set_size(wave->samples, nframes);
			size_t frame_index = 0;
			while (nframes > 0) {
				size_t read_frames = fread(wave->samples->data + frame_index,
					g_array_get_element_size(wave->samples), wave->samples->len, fd);
				nframes -= read_frames;
				frame_index += read_frames;
				if (read_frames == 0) {
					g_array_free(wave->samples, TRUE);
					goto exit_error2;
				}
			}
#ifndef NDEBUG
			printf("SubChunk2ID=");
			fwrite(wave->subchunk_data.header.ChunkID, 1, 4, stdout);
			putchar('\n');
			printf("SubChunk2Size=%u\n", wave->subchunk_data.header.ChunkSize);
			putchar('\n');
#endif
		}
		else {
			int result = fseek(wave->fd, chunk_header.ChunkSize, SEEK_CUR);
			if (result != 0) {
				fprintf(stderr, "fseek(): %s\n", strerror(errno));
				goto exit_error2;
			}
		}
	}
exit_error2:
	free(wave);
	wave = NULL;
exit_error1:
	fclose(fd);
exit_error0:
	return wave;
}

int wave_get_bytes_per_frame(Wave *wave) {
	return wave->subchunk_fmt.BlockAlign;
}

int wave_get_bits_per_sample(Wave *wave) {
	return wave->subchunk_fmt.BitsPerSample;
}

int wave_get_sample_rate(Wave *wave) {
	return wave->subchunk_fmt.SampleRate;
}

int wave_get_number_of_channels(Wave *wave) {
	return wave->subchunk_fmt.NumChannels;
}

#define min(a, b)	(((a) < (b))? (a) : (b))

size_t wave_get_samples(Wave *wave, size_t frame_index, char *buffer, size_t frame_count) {
	size_t read_frames = min(frame_count, wave->samples->len - frame_index);
	size_t read_bytes = read_frames * wave_get_bytes_per_frame(wave);
	memcpy(buffer, wave->samples->data + frame_index * wave_get_bytes_per_frame(wave), read_bytes);
	return read_frames;
}

size_t wave_read_samples(Wave *wave, char *buffer, size_t frame_count) {
	size_t read_frames = min(frame_count, wave->samples->len - wave->current);
	size_t read_bytes = read_frames * wave_get_bytes_per_frame(wave);
	memcpy(buffer, wave->samples->data + wave->current * wave_get_bytes_per_frame(wave), read_bytes);
	wave->current += read_frames;
	return read_frames;
}

void wave_set_block_align(Wave *wave, int block_align) {
	wave->subchunk_fmt.BlockAlign = block_align;
}

void wave_set_bits_per_sample(Wave *wave, int bits_per_sample) {
	wave->subchunk_fmt.BitsPerSample = bits_per_sample;
	wave->subchunk_fmt.BlockAlign = wave->subchunk_fmt.BitsPerSample * wave->subchunk_fmt.NumChannels / 8;
}

void wave_set_sample_rate(Wave *wave, int sample_rate) {
	wave->subchunk_fmt.SampleRate = sample_rate;
}

void wave_set_number_of_channels(Wave *wave, int number_of_channels) {
	wave->subchunk_fmt.NumChannels = number_of_channels;
	wave->subchunk_fmt.BlockAlign = wave->subchunk_fmt.BitsPerSample * wave->subchunk_fmt.NumChannels / 8;
}

size_t wave_append_samples(Wave *wave, char *buffer, size_t frame_count) {
	g_array_append_vals(wave->samples, buffer, frame_count);
	return frame_count;
}

size_t wave_put_samples(Wave *wave, size_t frame_index, char *buffer, size_t frame_count) {
	g_array_insert_vals(wave->samples, frame_index, buffer, frame_count);
	return frame_count;
}

size_t wave_write_samples(Wave *wave, char *buffer, size_t frame_count) {
	size_t write_frames = min(frame_count, wave->samples->len - wave->current);
	g_array_insert_vals(wave->samples, wave->current, buffer, write_frames);
	wave->current += write_frames;
	return write_frames;
}

size_t wave_get_size(Wave *wave)  {
	return wave->subchunk_data.header.ChunkSize;
}

size_t wave_get_duration(Wave *wave) {
	return wave_get_size(wave)
		/ wave_get_bytes_per_frame(wave)
			/ wave_get_sample_rate(wave);
}
