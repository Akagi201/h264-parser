/*
 * @file h264-pps.h
 * @author Akagi201
 * @date 2015/02/03
 */

#ifndef H264_PPS_H_
#defien H264_PPS_H_ (1)

#include "h264_stream.h"

typedef struct h264_pps {
    int pic_parameter_set_id;
    int seq_parameter_set_id;
    int entropy_coding_mode_flag;
    int pic_order_present_flag;
    int num_slice_groups_minus1;
    int slice_group_map_type;
    int run_length_minus1;
    int top_left;
    int bottom_right;
    int slice_group_change_direction_flag;
    int slice_group_change_rate_minus1;
    int pic_size_in_map_units_minus1;
    int slice_group_id;
    int num_ref_idx_l0_active_minus1;
    int num_ref_idx_l1_active_minus1;
    int weighted_pred_flag;
    int weighted_bipred_idc;
    int pic_init_qp_minus26;
    int pic_init_qs_minus26;
    int chroma_qp_index_offset;
    int deblocking_filter_control_present_flag;
    int constrained_intra_pred_flag;
    int redundant_pic_cnt_present_flag;
} h264_pps_t;

h264_pps_t *h264_pps_get(h264_stream_t *s);

void h264_pps_print(h264_pps *pps);

#endif // H264_PPS_H_
