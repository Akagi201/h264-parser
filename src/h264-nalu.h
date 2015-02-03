/*
 * @file h264-nalu.h
 * @author Akagi201
 * @date 2015/02/02
 */

#ifndef H264_NALU_H_
#define H264_NALU_H_ (1)

typedef struct h264_nalu {
    uint8_t nal_ref_idc;
    uint8_t nal_unit_type;
    void *rbsp;
}h264_nalu_t;

#endif // H264_NALU_H_
