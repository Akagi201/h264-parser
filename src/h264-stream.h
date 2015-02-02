/*
 * @file h264-stream.h
 * @author Akagi201
 * @date 2015/02/02
 */

#ifndef H264_STREAM_H_
#define H264_STREAM_H_ (1)

#include <stdint.h>

typedef struct h264_stream {
    uint8_t *data;
    uint32_t size;
    int bit_pos;
    int byte_pos;
}h264_stream_t;

h264_stream_t *h264_stream_alloc(void);

int h264_stream_init(h264_stream_t *s, uint8_t *data, uint32_t size);

h264_stream_t *h264_stream_from_file(char *path);

void h264_stream_free(h264_stream_t *s);

uint32_t h264_stream_read_bits(h264_stream_t *s, uint32_t n);

uint32_t h264_stream_peek_bits(h264_stream_t *s, uint32_t n);

uint32_t h264_stream_read_bytes(h264_stream_t *s, uint32_t n);

uint32_t h264_stream_peek_bytes(h264_stream_t *s, uint32_t n);

int h264_stream_bits_remaining(h264_stream_t *s);

int h264_stream_bytes_remaining(h264_stream_t *s);

#endif // H264_STREAM_H_
