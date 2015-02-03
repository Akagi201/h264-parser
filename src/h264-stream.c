/*
 * @file h264-stream.c
 * @author Akagi201
 * @date 2015/02/02
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "h264-nalu.h"
#include "h264-stream.h"
#include "util.h"
#include "lwlog/lwlog.h"

void print_byte(unsigned char c) {
    char s[9] = {0};
    int i = 0;
    for (i = 7; i >= 0; --i) {
        s[7 - i] = BITAT(c, i) ? '1' : '0';
    }
    s[8] = '\0';
    printf("%s = %d\n", s, c);

    return;
}

uint32_t h264_next_bits(h264_stream_t *s, int n) {
    if (0 == n % 8) {
        return h264_stream_peek_bytes(s, n / 8);
    }
    return h264_stream_peek_bits(s, n);
}

uint32_t h264_u(h264_stream_t *s, uint32_t n) {
    //if (n % 8 == 0) {
    //    return h264_stream_read_bytes(s, n / 8);
    //}
    return h264_stream_read_bits(s, n);
}

void h264_f(h264_stream_t *s, uint32_t n, uint32_t pattern) {
    uint32_t val = h264_u(s, n);
    if (val != pattern) {
        lwlog_err("h264_f Error: fixed-pattern doesn't match. \nexpected: %x \nactual: %x \n", pattern, (unsigned int) val);
        exit(0);
    }
}

h264_nalu_t *h264_byte_stream_get_nalu(h264_stream_t *s) {

    h264_nalu_t *nalu = NULL;

    if ((NULL == s) || (h264_stream_bytes_remaining(s) <= 0)) {
        return NULL;
    }

    while (h264_next_bits(s, 24) != 0x000001 &&
            h264_next_bits(s, 32) != 0x00000001) {
        h264_f(s, 8, 0x00); // leading_zero_8bits
    }
    if (h264_next_bits(s, 24) != 0x000001) {
        h264_f(s, 8, 0x00); // zero_byte
    }
    if (h264_stream_bytes_remaining(s) > 0) {
        //h264_u(s, 24);
        h264_f(s, 24, 0x000001); // start_code_prefix_one_3bytes
        nalu = h264_nal_unit(s);
    }
    while (h264_more_data_in_byte_stream(s) &&
            h264_next_bits(s, 24) != 0x000001 &&
            h264_next_bits(s, 36) != 0x00000001) {
        h264_f(s, 8, 0x00); // trailing_zero_8bits
    }
    return nu;
}

static uint8_t h264_exp_golomb_bits[256] = {
        8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,
};

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
    s->bit_pos = 7;
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

    result = fread(buffer, 1, (size_t) file_size, fp);
    assert(result == file_size);

    fclose(fp);

    s = h264_stream_alloc();

    (void) h264_stream_init(s, buffer, (uint32_t) file_size);

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

/*
 * @brief read n bits from stream s
 */
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

/*
 * @brief peek n bits from stream s
 */
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

/*
 * @brief read n bytes from stream s
 */
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

/*
 * @brief peek n bytes from stream s
 */
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

    return (s->size - s->byte_pos) * 8 + s->bit_pos - 1;
}

int h264_stream_bytes_remaining(h264_stream_t *s) {
    if (NULL == s) {
        lwlog_err("NULL == s");
        return 0;
    }

    return s->size - s->byte_pos - 1;
}
