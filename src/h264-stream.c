/*
 * @file h264-stream.c
 * @author Akagi201
 * @date 2015/02/02
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "h264-stream.h"
#include "util.h"
#include "lwlog/lwlog.h"

h264_stream_t *h264_stream_alloc(void) {
    h264_stream_t *s = NULL;

    s = malloc(sizeof(h264_stream_t));

    return s;
}

int h264_stream_init(h264_stream_t *s, uint8_t *data, uint32_t size) {

    if (NULL == s) {
        lwlog_err("NULL == s");
        return -1;
    }

    s->data = data;
    s->size = size;
    s->bit_pos = 7; // TODO: why not zero?
    s->byte_pos = 0;

    return 0;
}

h264_stream_t *h264_stream_from_file(char *path) {
    FILE *fp = NULL;
    long file_size = 0;
    uint8_t *buffer = NULL;
    size_t result = 0;
    h264_stream_t *s = NULL;

    fp = fopen(path, "rb");
    if (NULL == fp) {
        lwlog_err("NULL == fp");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    buffer = (uint8_t *) malloc((size_t) file_size);
    assert(buffer);

    result = fread(buffer, 1, (size_t)file_size, fp);
    assert(result == file_size);

    fclose(fp);

    s = h264_stream_alloc();

    (void)h264_stream_init(s, buffer, (uint32_t)file_size);

    return s;
}

void h264_stream_free(h264_stream_t *s) {
    if (NULL != s->data) {
        free(s->data);
    }
    s->size = 0;
    s->bit_pos = 7;
    s->byte_pos = 0;

    return;
}

uint32_t h264_stream_read_bits(h264_stream_t *s, uint32_t n) {
    int i = 0;
    uint32_t ret = 0;
    uint8_t b = 0;

    if ((NULL == s) || (0 == n)) {
        lwlog_err("h264_stream_read_bits param error");
        return 0;
    }

    for (i = 0; i < n; ++i) {
        if (h264_stream_bits_remaining(s) == 0) {
            ret <<= n - i - 1;
        }
        b = s->data[s->byte_pos];
        if (n - i <= 32) {
            ret = ret << 1 | BITAT(b, s->bit_pos);
        }
        if (s->bit_pos == 0) {
            s->bit_pos = 7;
            ++s->byte_pos;
        } else {
            --s->bit_pos;
        }
    }
    return ret;
}

uint32_t h264_stream_peek_bits(h264_stream_t *s, uint32_t n) {
    uint32_t ret = 0;
    int prev_bit_pos = 0;
    int prev_byte_pos = 0;

    if ((NULL == s) || (0 == n)) {
        lwlog_err("h264_stream_peek_bits param error");
        return 0;
    }

    prev_bit_pos = s->bit_pos;
    prev_byte_pos = s->byte_pos;
    ret = h264_stream_read_bits(s, n);
    s->bit_pos = prev_bit_pos;
    s->byte_pos = prev_byte_pos;

    return ret;
}

uint32_t h264_stream_read_bytes(h264_stream_t *s, uint32_t n) {
    uint32_t ret = 0;
    int i = 0;

    if ((NULL == s) || (0 == n)) {
        lwlog_err("h264_stream_read_bytes param error");
        return 0;
    }

    for (i = 0; i < n; ++i) {
        if (h264_stream_bytes_remaining(s) == 0) {
            ret <<= (n - i - 1) * 8;
            break;
        }
        if (n - i <= 4) {
            ret = ret << 8 | s->data[s->byte_pos];
        }
        ++s->byte_pos;
    }

    return ret;
}

uint32_t h264_stream_peek_bytes(h264_stream_t *s, uint32_t n) {

    uint32_t ret = 0;
    int prev_byte_pos = 0;

    if ((NULL == s) || (0 == n)) {
        lwlog_err("h264_stream_peek_bytes param error");
        return 0;
    }

    prev_byte_pos = s->byte_pos;
    ret = h264_stream_read_bytes(s, n);
    s->byte_pos = prev_byte_pos;

    return ret;
}

int h264_stream_bits_remaining(h264_stream_t *s) {
    if (NULL == s) {
        lwlog_err("NULL == s");
        return 0;
    }

    return (s->size - s->byte_pos) * 8 + s->bit_pos;
}

int h264_stream_bytes_remaining(h264_stream_t *s) {
    if (NULL == s) {
        lwlog_err("NULL == s");
        return 0;
    }

    return s->size - s->byte_pos;
}
