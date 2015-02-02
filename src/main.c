/*
 * @file main.c
 * @author Akagi201
 * @date 2015/02/02
 */

#include "h264-parser.h"

int main(void) {
    h264_stream_t *s = h264_stream_from_file("f_200_baseline.264");
    h264_stream_free(s);
    return 0;
}

