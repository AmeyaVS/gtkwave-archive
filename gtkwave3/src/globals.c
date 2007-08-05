#include"globals.h"
#include"ae2.h"
#include"analyzer.h"
#include"bsearch.h"
#include"busy.h"
#include"clipping.h"
#include"color.h"
#include"currenttime.h"
#include"debug.h"
#include"fgetdynamic.h"
#include"ghw.h"
#include"ghwlib.h"
#include"globals.h"
#include"gnu-getopt.h"
#include"gnu_regex.h"
#include"gtk12compat.h"
#include"lx2.h"
#include"lxt.h"
#include"main.h"
#include"menu.h"
#include"pipeio.h"
#include"pixmaps.h"
#include"print.h"
#include"ptranslate.h"
#include"rc.h"
#include"regex_wave.h"
#include"rgb.h"
#include"strace.h"
#include"symbol.h"
#include"translate.h"
#include"tree.h"
#include"vcd.h"
#include"vcd_saver.h"
#include"vlist.h"
#include"vzt.h"
#include"wavealloca.h"
struct Global GLOBALS = {
{ "Sorry, AET2 support was not compiled into this executable, exiting.\n\n"},//ae2_loader_fail_msg_ae2_c_1 4
{TR_RJUSTIFY},//default_flags 5
{ 0, 0, 0, 0, 0, 0, 0},//tims 6
{ 0, 0, NULL, NULL, NULL, NULL, 0},//traces 7
{0},//hier_max_level 8
{ 0},//color_active_in_filter 9
{LLDescriptor(0)},//shift_timebase 10
{LLDescriptor(0)},//shift_timebase_default_for_add 11
{0},//max_compare_time_tc_bsearch_c_1 12
{0},//max_compare_pos_tc_bsearch_c_1 13
{0},//max_compare_time_bsearch_c_1 14
{0},//max_compare_pos_bsearch_c_1 15
{0},//max_compare_index 16
{0},//vmax_compare_time_bsearch_c_1 17
{0},//vmax_compare_pos_bsearch_c_1 18
{0},//vmax_compare_index 19
{0},//maxlen_trunc 20
{0},//maxlen_trunc_pos_bsearch_c_1 21
{0},//trunc_asciibase_bsearch_c_1 22
{ NULL},//busycursor_busy_c_1 23
{ 0},//busy_busy_c_1 24
{-1},//color_back 25
{-1},//color_baseline 26
{-1},//color_grid 27
{-1},//color_high 28
{-1},//color_low 29
{-1},//color_mark 30
{-1},//color_mid 31
{-1},//color_time 32
{-1},//color_timeb 33
{-1},//color_trans 34
{-1},//color_umark 35
{-1},//color_value 36
{-1},//color_vbox 37
{-1},//color_vtrans 38
{-1},//color_x 39
{-1},//color_xfill 40
{-1},//color_0 41
{-1},//color_1 42
{-1},//color_ufill 43
{-1},//color_u 44
{-1},//color_wfill 45
{-1},//color_w 46
{-1},//color_dashfill 47
{-1},//color_dash 48
{ -1},//color_white 49
{ -1},//color_black 50
{ -1},//color_ltgray 51
{ -1},//color_normal 52
{ -1},//color_mdgray 53
{ -1},//color_dkgray 54
{ -1},//color_dkblue 55
{0},//is_vcd 56
{0},//partial_vcd
{1},//use_maxtime_display 57
{0},//use_frequency_delta 58
{NULL},//max_or_marker_label_currenttime_c_1 59
{NULL},//base_or_curtime_label_currenttime_c_1 60
{0},//cached_currenttimeval_currenttime_c_1 61
{0},//currenttime 62
{0},//max_time 63
{-1},//min_time 64
{~0},//display_grid 65
{1},//time_scale 66
{'n'},//time_dimension 67
{" munpf"},//time_prefix_currenttime_c_1 68 
{0},//maxtimewid_currenttime_c_1 69
{0},//curtimewid_currenttime_c_1 70
{0},//maxtext_currenttime_c_1 71
{0},//curtext_currenttime_c_1 72
{"Maximum Time"},//maxtime_label_text_currenttime_c_1 73
{"Marker Time"},//marker_label_text_currenttime_c_1 74
{ 1.0, 1.0e-3, 1.0e-6, 1.0e-9, 1.0e-12, 1.0e-15 },//negpow_currenttime_c_1 75
{1},//time_trunc_val_currenttime_c_1 76
{0},//use_full_precision 77
{NULL},//atoi_cont_ptr 78
{0},//disable_tooltips 79
{0},//window_entry_c_1 80
{0},//entry_entry_c_1 81
{NULL},//entrybox_text 82
{0},//cleanup_entry_c_1 83
{100},//fetchwindow 84
{0},//fgetmalloc_len 85
{0},//fs_file_c_1 86
{NULL},//fileselbox_text 87
{0},//filesel_ok 88
{0},//cleanup_file_c_2 89
{0},//bad_cleanup_file_c1
{NULL},//fontname_signals 90
{NULL},//fontname_waves 91
{ 
  { "\0\0\0", NULL },
  { "STR", ghw_read_str },
  { "HIE", ghw_read_hie },
  { "TYP", ghw_read_type },
  { "WKT", ghw_read_wk_types },
  { "EOH", ghw_read_eoh },
  { "SNP", ghw_read_snapshot },
  { "CYC", ghw_read_cycle },
  { "DIR", ghw_read_directory },
  { "TAI", ghw_read_tailer }
},//ghw_sections 92
{0},//nxp_ghw_c_1 93
{ NULL},//sym_head_ghw_c_1 94
{NULL},
{ 0},//sym_which_ghw_c_1 95
{ NULL},//gwt_ghw_c_1 96
{ NULL},//gwt_corr_ghw_c_1 97
{ 1},//xlat_1164_ghw_c_1 98
{ 0},//is_ghw 99
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//asbuf_ghw_c_1 100 (char 4097)
{0},//nbr_sig_ref_ghw_c_1 101
{ 0},//num_glitches_ghw_c_1 102
{0}, //  num_glitch_regions_ghw_c_1
{ 0, 0 },//dummy_en_ghw_c_1 103
{0},//fac_name_ghw_c_1 104
{0},//fac_name_len_ghw_c_1 105
{0},//fac_name_max_ghw_c_1 106
{0},//last_fac_ghw_c_1 107
{0},//warned_ghw_c_1 108
{ 
            /* U */ AN_U, /* X */ AN_X, /* 0 */ AN_0, /* 1 */ AN_1,
	    /* Z */ AN_Z, /* W */ AN_W, /* L */ AN_L, /* H */ AN_H,
	    /* - */ AN_DASH
	  },//map_su2vlg_ghw_c_1 109
{0},//helpbox_is_active 110
{0},//text_help_c_1 111
{0},//vscrollbar_help_c_1 112
{0},//iter_help_c_1 113
{0},//bold_tag_help_c_1 114
{0},//window_help_c_2 115
{1},//hier_grouping 116
{0},//window_hiersearch_c_3 117
{0},//entry_main_hiersearch_c_1 118
{0},//clist_hiersearch_c_1 119
{0},//bundle_direction_hiersearch_c_1 120
{0},//cleanup_hiersearch_c_3 121
{0},//num_rows_hiersearch_c_1 122
{0},//selected_rows_hiersearch_c_1 123
{0},//window1_hiersearch_c_1 124
{0},//entry_hiersearch_c_2 125
{NULL},//entrybox_text_local_hiersearch_c_1 126
{NULL},//cleanup_e_hiersearch_c_1 127
{NULL},//h_selectedtree_hiersearch_c_1 128
{NULL},//current_tree_hiersearch_c_1 129
{NULL},//treechain_hiersearch_c_1 130
{0},//is_active_hiersearch_c_1 131
{".."},//dotdot_hiersearch_c_1 132
{ NULL},//fontname_logfile 133
{ NULL},//font_logfile_c_1 134
{0},//iter_logfile_c_2 135
{ NULL},//bold_tag_logfile_c_2 136
{ NULL},//mono_tag_logfile_c_1 137
{ NULL},//size_tag_logfile_c_1 138
{ LXT2_IS_INACTIVE},//is_lx2 139
{NULL},//lx2_lx2_c_1 140
{0},//first_cycle_lx2_c_1 141
{0},//last_cycle
{0},//total_cycles
{ NULL},//lx2_table_lx2_c_1 142
{NULL},//mvlfacs_lx2_c_1 143
{ 0},//busycnt_lx2_c_1 144
{0},//fpos_lxt_c_1 145
{ 0},//is_lxt 146
{ 0},//lxt_clock_compress_to_z 147
{NULL},//mm_lxt_c_1 148
{NULL},//mmcache_lxt_c_1
{0},//version_lxt_c_1 149
{NULL},//mvlfacs_lxt_c_2 150
{0},//first_cycle_lxt_c_2 151
{0},//last_cycle
{0},//total_cycles
{0},//maxchange_lxt_c_1 152
{0},//maxindex
{0},//f_len_lxt_c_1 153
{NULL},//positional_information_lxt_c_1 154
{NULL},//time_information 155
{0},//change_field_offset_lxt_c_1 156
{0},//facname_offset_lxt_c_1 157
{0},//facgeometry_offset_lxt_c_1 158
{0},//time_table_offset_lxt_c_1 159
{0},//time_table_offset64_lxt_c_1 160
{0},//sync_table_offset_lxt_c_1 161
{0},//initial_value_offset_lxt_c_1 162
{0},//timescale_offset_lxt_c_1 163
{0},//double_test_offset_lxt_c_1 164
{0},//zdictionary_offset_lxt_c_1 165
{0},//zfacname_predec_size_lxt_c_1 166
{0},//zfacname_size_lxt_c_1 167
{0},//zfacgeometry_size_lxt_c_1 168
{0},//zsync_table_size_lxt_c_1 169
{0},//ztime_table_size_lxt_c_1 170
{0},//zchg_predec_size_lxt_c_1 171
{0},//zchg_size_lxt_c_1 172
{0},//zdictionary_predec_size_lxt_c_1 173
{ AN_X},//initial_value_lxt_c_1 174
{0},//dict_num_entries_lxt_c_1 175
{0},//dict_string_mem_required_lxt_c_1 176
{0},//dict_16_offset_lxt_c_1 177
{0},//dict_24_offset_lxt_c_1 178
{0},//dict_32_offset_lxt_c_1 179
{0},//dict_width_lxt_c_1 180
{NULL},//dict_string_mem_array_lxt_c_1 181
{0},//exclude_offset_lxt_c_1 182
{NULL},//lt_buf_lxt_c_1 183
{ 0},//lt_len_lxt_c_1 184
{0},//fd_lxt_c_1 185
{0,0,0,0,0,0,0,0},//double_mask_lxt_c_1 186
{0},//double_is_native_lxt_c_1 187
{ 3.14159},//p_lxt_c_1 188
{0},//max_compare_time_tc_lxt_c_2 189
{0},//max_compare_pos_tc_lxt_c_2
{NULL},//whoami 190
{NULL},//logfile 191
{ NULL},//stems_name 192
{ WAVE_ANNO_NONE},//stems_type 193
{ NULL},//aet_name 194
{ NULL},//anno_ctx 195
{ NULL},//dual_ctx 196
{ 0},//dual_id 197
{ 0},//dual_attach_id_main_c_1 198
{ 0},//dual_race_lock 199
{ NULL},//mainwindow 200
{ NULL},//signalwindow 201
{ NULL},//wavewindow 202
{ NULL},//toppanedwindow 203
{ NULL},//sstpane 204
{ NULL},//expanderwindow 205
{0},//disable_window_manager 206
{1},//paned_pack_semantics 207
{0},//zoom_was_explicitly_set 208
{800},//initial_window_x 209
{400},//initial_window_y
{-1},// initial_window_width
{ -1},//initial_window_height 210
{ 0},//xy_ignore_main_c_1 211
{ 0},//optimize_vcd 212
{1},//num_cpus 213
{-1},//initial_window_xpos 214
{-1},//initial_window_ypos 214
{ 0},//initial_window_set_valid 215
{-1},//initial_window_xpos_set 216
{-1},//initial_window_ypos_set
{ 0},//initial_window_get_valid 217
{-1},//initial_window_xpos_get 218
{-1},//initial_window_ypos_get 218
{ 0},//xpos_delta 219
{ 0},//ypos_delta 219
{0},//use_scrollbar_only 220
{0},//force_toolbars 221
{ 0},//hide_sst 222
{ 1},//sst_expanded 223
{ 0},//socket_xid 224
{ 0},//disable_menus 225
{ NULL},//ftext_main_main_c_1 226
{"GTKWave - "},//winprefix_main_c_1 227
{"GTKWave (stdio) "},//winstd_main_c_1 228
{"vcd_autosave.sav"},//vcd_autosave_name_main_c_1 229
{
                
                {"dump", 1, 0, 'f'},
                {"optimize", 0, 0, 'o'},
                {"nocli", 1, 0, 'n'},
                {"save", 1, 0, 'a'},
                {"autosavename", 0, 0, 'A'},
		{"rcfile", 1, 0, 'r'},
		{"defaultskip", 0, 0, 'd'},
		{"indirect", 1, 0, 'i'},
		{"logfile", 1, 0, 'l'},
                {"start", 1, 0, 's'},
                {"end", 1, 0, 'e'},
                {"cpus", 1, 0, 'c'},
		{"stems", 1, 0, 't'},
		{"nowm", 0, 0, 'N'},
		{"script", 1, 0, 'S'},
                {"vcd", 0, 0, 'v'},
                {"version", 0, 0, 'V'},
                {"help", 0, 0, 'h'},
                {"exit", 0, 0, 'x'},
                {"xid", 1, 0, 'X'},
		{"nomenus", 0, 0, 'M'},
		{"dualid", 1, 0, 'D'},
		{"interactive", 0, 0, 'I'},
		{"legacy", 0, 0, 'L'},
                {0, 0, 0, 0}
                },//long_options_main_c_1 230
{NULL},//window_markerbox_c_4 231
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//entries_markerbox_c_1 232
{NULL},//cleanup_markerbox_c_4 233
{0},//dirty_markerbox_c_1 234
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//shadow_markers_markerbox_c_1 235
{ 0},//enable_fast_exit 236
{ NULL},//script_handle 237
{ 0},//ignore_savefile_pos 238
{ 0},//ignore_savefile_size 239
{

#if !defined __MINGW32__ && !defined _MSC_VER 
    WAVE_GTKIFE("/File/Open New Viewer", "Pause", menu_new_viewer, WV_MENU_FONV, "<Item>"),
#endif
    WAVE_GTKIFE("/File/Export/Write VCD File As", NULL, menu_write_vcd_file, WV_MENU_WRVCD, "<Item>"),
    WAVE_GTKIFE("/File/Export/Write LXT File As", NULL, menu_write_lxt_file, WV_MENU_WRLXT, "<Item>"),
    WAVE_GTKIFE("/File/<separator>", NULL, NULL, WV_MENU_SEP2VCD, "<Separator>"),
    WAVE_GTKIFE("/File/Print To File", "Print", menu_print, WV_MENU_FPTF, "<Item>"),
    WAVE_GTKIFE("/File/<separator>", NULL, NULL, WV_MENU_SEP1, "<Separator>"),
    WAVE_GTKIFE("/File/Read Save File", "<Control>R", menu_read_save_file, WV_MENU_FRSF, "<Item>"),
    WAVE_GTKIFE("/File/Write Save File", "<Control>W", menu_write_save_file, WV_MENU_FWSF, "<Item>"),
    WAVE_GTKIFE("/File/Write Save File As", "<Shift><Control>W", menu_write_save_file_as, WV_MENU_FWSFAS, "<Item>"),
    WAVE_GTKIFE("/File/<separator>", NULL, NULL, WV_MENU_SEP2, "<Separator>"),
    WAVE_GTKIFE("/File/Read Sim Logfile", "<Control>L", menu_read_log_file, WV_MENU_FRLF, "<Item>"),
    //10
    WAVE_GTKIFE("/File/<separator>", NULL, NULL, WV_MENU_SEP2LF, "<Separator>"),
#if !defined __MINGW32__ && !defined _MSC_VER
    WAVE_GTKIFE("/File/Read Verilog Stemsfile", NULL, menu_read_stems_file, WV_MENU_FRSTMF, "<Item>"),
    WAVE_GTKIFE("/File/<separator>", NULL, NULL, WV_MENU_SEP2STMF, "<Separator>"),
#endif
    WAVE_GTKIFE("/File/Quit/Yes, Quit", "<Alt>Q", menu_quit, WV_MENU_FQY, "<Item>"),
    WAVE_GTKIFE("/File/Quit/Don't Quit", NULL, NULL, WV_MENU_FQN, "<Item>"),

    WAVE_GTKIFE("/Edit/Set Trace Max Hier", "<Control>T", menu_set_max_hier, WV_MENU_ESTMH, "<Item>"),
    WAVE_GTKIFE("/Edit/<separator>", NULL, NULL, WV_MENU_SEP3, "<Separator>"),
    WAVE_GTKIFE("/Edit/Insert Blank", "<Control>B", menu_insert_blank_traces, WV_MENU_EIB, "<Item>"),
    WAVE_GTKIFE("/Edit/Insert Comment", "<Control>C", menu_insert_comment_traces, WV_MENU_EIC, "<Item>"),
    WAVE_GTKIFE("/Edit/Insert Analog Height Extension", "<Control>A", menu_insert_analog_height_extension, WV_MENU_EIA, "<Item>"),
    WAVE_GTKIFE("/Edit/Alias Highlighted Trace", "<Alt>A", menu_alias, WV_MENU_EAHT, "<Item>"),
    WAVE_GTKIFE("/Edit/Remove Highlighted Aliases", "<Shift><Alt>A", menu_remove_aliases, WV_MENU_ERHA, "<Item>"),
      //20
    WAVE_GTKIFE("/Edit/Cut", "<Alt>C", menu_cut_traces, WV_MENU_EC, "<Item>"),
    WAVE_GTKIFE("/Edit/Paste", "<Alt>P", menu_paste_traces, WV_MENU_EP, "<Item>"),
    WAVE_GTKIFE("/Edit/<separator>", NULL, NULL, WV_MENU_SEP4, "<Separator>"),
    WAVE_GTKIFE("/Edit/Expand", "F3", menu_expand, WV_MENU_EE, "<Item>"),
    WAVE_GTKIFE("/Edit/Combine Down", "F4", menu_combine_down, WV_MENU_ECD, "<Item>"),
    WAVE_GTKIFE("/Edit/Combine Up", "F5", menu_combine_up, WV_MENU_ECU, "<Item>"),
    WAVE_GTKIFE("/Edit/Reduce Single Bit Vectors", "F6", menu_reduce_singlebit_vex, WV_MENU_ERSBV, "<Item>"),
    WAVE_GTKIFE("/Edit/<separator>", NULL, NULL, WV_MENU_SEP5, "<Separator>"),
    WAVE_GTKIFE("/Edit/Data Format/Hex", "<Alt>X", menu_dataformat_hex, WV_MENU_EDFH, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Decimal", "<Alt>D", menu_dataformat_dec, WV_MENU_EDFD, "<Item>"),
      //30
    WAVE_GTKIFE("/Edit/Data Format/Signed Decimal", NULL, menu_dataformat_signed, WV_MENU_EDFSD, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Binary", "<Alt>B", menu_dataformat_bin, WV_MENU_EDFB, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Octal", "<Alt>O", menu_dataformat_oct, WV_MENU_EDFO, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/ASCII", NULL, menu_dataformat_ascii, WV_MENU_EDFA, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/BitsToReal", NULL, menu_dataformat_real, WV_MENU_EDRL, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Right Justify/On", "<Alt>J", menu_dataformat_rjustify_on, WV_MENU_EDFRJON, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Right Justify/Off", "<Shift><Alt>J", menu_dataformat_rjustify_off, WV_MENU_EDFRJOFF, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Invert/On", "<Alt>I", menu_dataformat_invert_on, WV_MENU_EDFION, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Invert/Off", "<Shift><Alt>I", menu_dataformat_invert_off, WV_MENU_EDFIOFF, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Reverse Bits/On", "<Alt>V", menu_dataformat_reverse_on, WV_MENU_EDFRON, "<Item>"),
      //40  
    WAVE_GTKIFE("/Edit/Data Format/Reverse Bits/Off", "<Shift><Alt>V", menu_dataformat_reverse_off, WV_MENU_EDFROFF, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Translate Filter File/Disable", NULL, menu_dataformat_xlate_file_0, WV_MENU_XLF_0, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Translate Filter File/Enable and Select", NULL, menu_dataformat_xlate_file_1, WV_MENU_XLF_1, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Translate Filter Process/Disable", NULL, menu_dataformat_xlate_proc_0, WV_MENU_XLP_0, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Translate Filter Process/Enable and Select", NULL, menu_dataformat_xlate_proc_1, WV_MENU_XLP_1, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Analog/Off", NULL, menu_dataformat_analog_off, WV_MENU_EDFAOFF, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Analog/Step", NULL, menu_dataformat_analog_step, WV_MENU_EDFASTEP, "<Item>"),
    WAVE_GTKIFE("/Edit/Data Format/Analog/Interpolated", NULL, menu_dataformat_analog_interpol, WV_MENU_EDFAINTERPOL, "<Item>"),
    WAVE_GTKIFE("/Edit/Show-Change All Highlighted", "<Control>S", menu_showchangeall, WV_MENU_ESCAH, "<Item>"),
    WAVE_GTKIFE("/Edit/Show-Change First Highlighted", "<Control>F", menu_showchange, WV_MENU_ESCFH, "<Item>"),
      //50
    WAVE_GTKIFE("/Edit/<separator>", NULL, NULL, WV_MENU_SEP6, "<Separator>"),
    WAVE_GTKIFE("/Edit/Time Warp/Warp Marked", NULL, menu_warp_traces, WV_MENU_WARP, "<Item>"),
    WAVE_GTKIFE("/Edit/Time Warp/Unwarp Marked", NULL, menu_unwarp_traces, WV_MENU_UNWARP, "<Item>"),
    WAVE_GTKIFE("/Edit/Time Warp/Unwarp All", NULL, menu_unwarp_traces_all, WV_MENU_UNWARPA, "<Item>"),
    WAVE_GTKIFE("/Edit/<separator>", NULL, NULL, WV_MENU_SEP7A, "<Separator>"),
    WAVE_GTKIFE("/Edit/Exclude", "<Shift><Alt>E", menu_dataformat_exclude_on, WV_MENU_EEX, "<Item>"),
    WAVE_GTKIFE("/Edit/Show", "<Shift><Alt>S", menu_dataformat_exclude_off, WV_MENU_ESH, "<Item>"),
    WAVE_GTKIFE("/Edit/<separator>", NULL, NULL, WV_MENU_SEP6A, "<Separator>"),
    WAVE_GTKIFE("/Edit/Expand All Groups", "F12", menu_expand_all, WV_MENU_EXA, "<Item>"),
    WAVE_GTKIFE("/Edit/Collapse All Groups", "<Shift>F12", menu_collapse_all, WV_MENU_CPA, "<Item>"),
      //60
    WAVE_GTKIFE("/Edit/<separator>", NULL, NULL, WV_MENU_SEP6A1, "<Separator>"),
    WAVE_GTKIFE("/Edit/Highlight Regexp", "<Alt>R", menu_regexp_highlight, WV_MENU_EHR, "<Item>"),
    WAVE_GTKIFE("/Edit/UnHighlight Regexp", "<Shift><Alt>R", menu_regexp_unhighlight, WV_MENU_EUHR, "<Item>"),
    WAVE_GTKIFE("/Edit/Highlight All", "<Alt>H", menu_dataformat_highlight_all, WV_MENU_EHA, "<Item>"),
    WAVE_GTKIFE("/Edit/UnHighlight All", "<Shift><Alt>H", menu_dataformat_unhighlight_all, WV_MENU_EUHA, "<Item>"),
    WAVE_GTKIFE("/Edit/<separator>", NULL, NULL, WV_MENU_SEP6B, "<Separator>"),
    WAVE_GTKIFE("/Edit/Sort/Alphabetize All", NULL, menu_alphabetize, WV_MENU_ALPHA, "<Item>"),
    WAVE_GTKIFE("/Edit/Sort/Alphabetize All (CaseIns)", NULL, menu_alphabetize2, WV_MENU_ALPHA2, "<Item>"),
    WAVE_GTKIFE("/Edit/Sort/Sigsort All", NULL, menu_lexize, WV_MENU_LEX, "<Item>"),
    WAVE_GTKIFE("/Edit/Sort/Reverse All", NULL, menu_reverse, WV_MENU_RVS, "<Item>"),
      //70
    WAVE_GTKIFE("/Search/Pattern Search", "<Control>P", menu_tracesearchbox, WV_MENU_SPS, "<Item>"),
    WAVE_GTKIFE("/Search/<separator>", NULL, NULL, WV_MENU_SEP7B, "<Separator>"),
    WAVE_GTKIFE("/Search/Signal Search Regexp", "<Alt>S", menu_signalsearch, WV_MENU_SSR, "<Item>"),
    WAVE_GTKIFE("/Search/Signal Search Hierarchy", "<Alt>T", menu_hiersearch, WV_MENU_SSH, "<Item>"),
    WAVE_GTKIFE("/Search/Signal Search Tree", "<Shift><Alt>T", menu_treesearch, WV_MENU_SST, "<Item>"),
    WAVE_GTKIFE("/Search/<separator>", NULL, NULL, WV_MENU_SEP7, "<Separator>"),
    WAVE_GTKIFE("/Search/Autocoalesce", NULL, menu_autocoalesce, WV_MENU_ACOL, "<ToggleItem>"),
    WAVE_GTKIFE("/Search/Autocoalesce Reversal", NULL, menu_autocoalesce_reversal, WV_MENU_ACOLR, "<ToggleItem>"),
    WAVE_GTKIFE("/Search/Autoname Bundles", NULL, menu_autoname_bundles_on, WV_MENU_ABON, "<ToggleItem>"),
    WAVE_GTKIFE("/Search/Search Hierarchy Grouping", NULL, menu_hgrouping, WV_MENU_HTGP, "<ToggleItem>"),
      //80
    WAVE_GTKIFE("/Time/Move To Time", "F1", menu_movetotime, WV_MENU_TMTT, "<Item>"),
    WAVE_GTKIFE("/Time/Zoom/Zoom Amount", "F2", menu_zoomsize, WV_MENU_TZZA, "<Item>"),
    WAVE_GTKIFE("/Time/Zoom/Zoom Base", "<Shift>F2", menu_zoombase, WV_MENU_TZZB, "<Item>"),
    WAVE_GTKIFE("/Time/Zoom/Zoom In", "<Alt>Z", service_zoom_in, WV_MENU_TZZI, "<Item>"),
    WAVE_GTKIFE("/Time/Zoom/Zoom Out", "<Shift><Alt>Z", service_zoom_out, WV_MENU_TZZO, "<Item>"),
    WAVE_GTKIFE("/Time/Zoom/Zoom Full", "<Alt>F", service_zoom_full, WV_MENU_TZZBFL, "<Item>"),
    WAVE_GTKIFE("/Time/Zoom/Zoom Best Fit", "<Shift><Alt>F", service_zoom_fit, WV_MENU_TZZBF, "<Item>"),
    WAVE_GTKIFE("/Time/Zoom/Zoom To Start", "Home", service_zoom_left, WV_MENU_TZZTS, "<Item>"),
    WAVE_GTKIFE("/Time/Zoom/Zoom To End", "End", service_zoom_right, WV_MENU_TZZTE, "<Item>"),
    WAVE_GTKIFE("/Time/Zoom/Undo Zoom", "<Alt>U", service_zoom_undo, WV_MENU_TZUZ, "<Item>"),
      //90
    WAVE_GTKIFE("/Time/Fetch/Fetch Size", "F7", menu_fetchsize, WV_MENU_TFFS, "<Item>"),
    WAVE_GTKIFE("/Time/Fetch/Fetch ->", "<Alt>2", fetch_right, WV_MENU_TFFR, "<Item>"),
    WAVE_GTKIFE("/Time/Fetch/Fetch <-", "<Alt>1", fetch_left, WV_MENU_TFFL, "<Item>"),
    WAVE_GTKIFE("/Time/Discard/Discard ->", "<Alt>4", discard_right, WV_MENU_TDDR, "<Item>"),
    WAVE_GTKIFE("/Time/Discard/Discard <-", "<Alt>3", discard_left, WV_MENU_TDDL, "<Item>"),
    WAVE_GTKIFE("/Time/Shift/Shift ->", "<Alt>6", service_right_shift, WV_MENU_TSSR, "<Item>"),
    WAVE_GTKIFE("/Time/Shift/Shift <-", "<Alt>5", service_left_shift, WV_MENU_TSSL, "<Item>"),
    WAVE_GTKIFE("/Time/Page/Page ->", "<Alt>8", service_right_page, WV_MENU_TPPR, "<Item>"),
    WAVE_GTKIFE("/Time/Page/Page <-", "<Alt>7", service_left_page, WV_MENU_TPPL, "<Item>"),
    WAVE_GTKIFE("/Markers/Show-Change Marker Data", "<Alt>M", menu_markerbox, WV_MENU_MSCMD, "<Item>"),
    //100
    WAVE_GTKIFE("/Markers/Drop Named Marker", "<Alt>N", drop_named_marker, WV_MENU_MDNM, "<Item>"),
    WAVE_GTKIFE("/Markers/Collect Named Marker", "<Shift><Alt>N", collect_named_marker, WV_MENU_MCNM, "<Item>"),
    WAVE_GTKIFE("/Markers/Collect All Named Markers", "<Shift><Control><Alt>N", collect_all_named_markers, WV_MENU_MCANM, "<Item>"),
    WAVE_GTKIFE("/Markers/Delete Primary Marker", "<Shift><Alt>M", delete_unnamed_marker, WV_MENU_MDPM, "<Item>"),
    WAVE_GTKIFE("/Markers/<separator>", NULL, NULL, WV_MENU_SEP8, "<Separator>"),
    WAVE_GTKIFE("/Markers/Wave Scrolling", "F9", wave_scrolling_on, WV_MENU_MWSON, "<ToggleItem>"),

    WAVE_GTKIFE("/View/Show Grid", "<Alt>G", menu_show_grid, WV_MENU_VSG, "<ToggleItem>"),
    WAVE_GTKIFE("/View/<separator>", NULL, NULL, WV_MENU_SEP9, "<Separator>"),
#if !defined _MSC_VER && !defined __MINGW32__
    WAVE_GTKIFE("/View/Show Mouseover", NULL, menu_show_mouseover, WV_MENU_VSMO, "<ToggleItem>"),
    WAVE_GTKIFE("/View/<separator>", NULL, NULL, WV_MENU_SEP9A, "<Separator>"),
#endif
    WAVE_GTKIFE("/View/Show Base Symbols", "<Alt>F1", menu_show_base, WV_MENU_VSBS, "<ToggleItem>"),
    WAVE_GTKIFE("/View/<separator>", NULL, NULL, WV_MENU_SEP10, "<Separator>"),
      //110
    WAVE_GTKIFE("/View/Dynamic Resize", "<Alt>9", menu_enable_dynamic_resize, WV_MENU_VDR, "<ToggleItem>"),
    WAVE_GTKIFE("/View/<separator>", NULL, NULL, WV_MENU_SEP11, "<Separator>"),
    WAVE_GTKIFE("/View/Center Zooms", "F8", menu_center_zooms, WV_MENU_VCZ, "<ToggleItem>"),
    WAVE_GTKIFE("/View/<separator>", NULL, NULL, WV_MENU_SEP12, "<Separator>"),
    WAVE_GTKIFE("/View/Toggle Delta-Frequency", NULL, menu_toggle_delta_or_frequency, WV_MENU_VTDF, "<Item>"),
    WAVE_GTKIFE("/View/Toggle Max-Marker", "F10", menu_toggle_max_or_marker, WV_MENU_VTMM, "<Item>"),
    WAVE_GTKIFE("/View/<separator>", NULL, NULL, WV_MENU_SEP13, "<Separator>"),
    WAVE_GTKIFE("/View/Constant Marker Update", "F11", menu_enable_constant_marker_update, WV_MENU_VCMU, "<ToggleItem>"),
    WAVE_GTKIFE("/View/<separator>", NULL, NULL, WV_MENU_SEP14, "<Separator>"),
    WAVE_GTKIFE("/View/Draw Roundcapped Vectors", "<Alt>F2", menu_use_roundcaps, WV_MENU_VDRV, "<ToggleItem>"),
      //120
    WAVE_GTKIFE("/View/<separator>", NULL, NULL, WV_MENU_SEP15, "<Separator>"),
    WAVE_GTKIFE("/View/Left Justified Signals", "<Shift>Home", menu_left_justify, WV_MENU_VLJS, "<Item>"),
    WAVE_GTKIFE("/View/Right Justified Signals", "<Shift>End", menu_right_justify, WV_MENU_VRJS, "<Item>"),
    WAVE_GTKIFE("/View/<separator>", NULL, NULL, WV_MENU_SEP16, "<Separator>"),
    WAVE_GTKIFE("/View/Zoom Pow10 Snap", "<Shift>Pause", menu_zoom10_snap, WV_MENU_VZPS, "<ToggleItem>"),
    WAVE_GTKIFE("/View/Full Precision", "<Alt>Pause", menu_use_full_precision, WV_MENU_VFTP, "<ToggleItem>"),
    WAVE_GTKIFE("/View/<separator>", NULL, NULL, WV_MENU_SEP17, "<Separator>"),
    WAVE_GTKIFE("/View/Remove Pattern Marks", NULL, menu_remove_marked, WV_MENU_RMRKS, "<Item>"),
    WAVE_GTKIFE("/View/<separator>", NULL, NULL, WV_MENU_SEP18, "<Separator>"),
    WAVE_GTKIFE("/View/LXT Clock Compress to Z", NULL, menu_lxt_clk_compress, WV_MENU_LXTCC2Z, "<ToggleItem>"),
      //130
    WAVE_GTKIFE("/Help/WAVE Help", "<Control>H", menu_help, WV_MENU_HWH, "<Item>"),
    WAVE_GTKIFE("/Help/Wave Version", "<Control>V", menu_version, WV_MENU_HWV, "<Item>"),
},//menu_items_menu_c_1 240
{NULL},//item_factory_menu_c_1 241
{""},//regexp_string_menu_c_1 242
{NULL},//trace_to_alias_menu_c_1 243
{NULL},//showchangeall_menu_c_1 244
{NULL},//filesel_newviewer_menu_c_1 245
{NULL},//filesel_logfile_menu_c_1 246
{NULL},//filesel_writesave 247
{ 0},//save_success_menu_c_1 248
{NULL},//filesel_vcd_writesave 249
{NULL},//filesel_lxt_writesave 250
{ 0},//lock_menu_c_1 251
{ 0},//lock_menu_c_2 252
{
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /*128*/
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 

0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /*128*/
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 

0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /*128*/
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 

0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /*128*/
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 

0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /*128*/
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 

0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /*128*/
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 

0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /*128*/
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 

0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  /*128*/
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 

0
},//buf_menu_c_1 253 128
{ 1},//disable_mouseover 254
{ NULL},//mouseover_mouseover_c_1 255
{NULL},//mo_area_mouseover_c_1 256
{ NULL},//mo_pixmap_mouseover_c_1 257
{ NULL},//mo_dk_gray_mouseover_c_1 258
{ NULL},//mo_black_mouseover_c_1 259
{ 0},//mo_width_mouseover_c_1 260
{0}, //mo_height_mouseover_c_1 260
{1.0},//page_divisor 261
{ 
"24 24 43 1",
" 	c None",
".	c #000000",
"+	c #B9D0B9",
"@	c #CDDECB",
"#	c #B6C7B6",
"$	c #B1C9B0",
"%	c #B3C4B3",
"&	c #B4CBB2",
"*	c #B5CEB5",
"=	c #B7CCB5",
"-	c #B9CEB7",
";	c #BAD1BA",
">	c #BBCFBA",
",	c #BBD0B9",
"'	c #B2C9B0",
")	c #7EAB78",
"!	c #AAC7A8",
"~	c #B3CAB1",
"{	c #B0C9B0",
"]	c #B0C9AE",
"^	c #AEC7AC",
"/	c #AAC5A8",
"(	c #A9C4A7",
"_	c #698267",
":	c #2D2D2D",
"<	c #CFDFCC",
"[	c #ADC8AB",
"}	c #B0C7AE",
"|	c #ADC6AB",
"1	c #678C63",
"2	c #9BAD9A",
"3	c #85AE81",
"4	c #87AF84",
"5	c #87B083",
"6	c #88AF84",
"7	c #88B085",
"8	c #86AF82",
"9	c #547150",
"0	c #3C5235",
"a	c #5B7950",
"b	c #4A6342",
"c	c #3B5035",
"d	c #415639",
"                        ",
"                        ",
"                        ",
"            .           ",
"           ..           ",
"          .+.           ",
"         .@#.           ",
"        .@$%........    ",
"       .@&*=-;->,').    ",
"      .@!~{]^///^(_.    ",
"     :<[}||[!^^}^[1.    ",
"    .23444445645789.    ",
"     .0aaaaaaaaaaab.    ",
"      .0aaaaaaaaaab.    ",
"       .0aabccccccd.    ",
"        .0ab........    ",
"         .0b.           ",
"          .b.           ",
"           ..           ",
"            .           ",
"                        ",
"                        ",
"                        ",
"                        "},//icon_larrow_pixmaps_c_1 262
{NULL},//larrow_pixmap 263
{NULL},//larrow_mask 264
{ 
"24 24 41 1",
" 	c None",
".	c #000000",
"+	c #8CA782",
"@	c #B1CDAE",
"#	c #77A16E",
"$	c #B4CEB1",
"%	c #ACC8A9",
"&	c #709867",
"*	c #C1D6BD",
"=	c #BDD3B8",
"-	c #BFD4BB",
";	c #C2D7BE",
">	c #B0CAAD",
",	c #B2CBB0",
"'	c #AAC7A8",
")	c #0F1308",
"!	c #AEC5A8",
"~	c #AEC8AD",
"{	c #ABC7A8",
"]	c #AAC6A7",
"^	c #A8C6A5",
"/	c #ADC8AD",
"(	c #A8C7A8",
"_	c #A5C4A3",
":	c #7F9F76",
"<	c #A6BFA0",
"[	c #ABC7AA",
"}	c #A7C5A4",
"|	c #A9C7A6",
"1	c #AFC8AD",
"2	c #A4C3A2",
"3	c #6B9060",
"4	c #778E6F",
"5	c #698D60",
"6	c #6B9063",
"7	c #445B2C",
"8	c #6B8661",
"9	c #5B7950",
"0	c #6C8562",
"a	c #65815C",
"b	c #506B46",
"                        ",
"                        ",
"                        ",
"           .            ",
"           ..           ",
"           .+.          ",
"           .@#.         ",
"    ........$%&.        ",
"    .*=-;;;;>,'&)       ",
"    .!~{{{]^'/(_:.      ",
"    .<[^}^|{%'{123.     ",
"    .45666666666657.    ",
"    .8999999999997.     ",
"    .099999999997.      ",
"    .abbbbbb9997.       ",
"    ........b97.        ",
"           .b7.         ",
"           .7.          ",
"           ..           ",
"           .            ",
"                        ",
"                        ",
"                        ",
"                        "},//icon_rarrow_pixmaps_c_1 265
{NULL},//rarrow_pixmap 266
{NULL},//rarrow_mask 267
{ 
"24 24 131 2",
"  	c None",
". 	c #343434",
"+ 	c #2D2D2D",
"@ 	c #292929",
"# 	c #262626",
"$ 	c #2E2E2E",
"% 	c #303030",
"& 	c #737373",
"* 	c #A1A1A1",
"= 	c #B4B4B4",
"- 	c #B2B2B2",
"; 	c #9D9D9D",
"> 	c #676767",
", 	c #202020",
"' 	c #1C1C1C",
") 	c #272727",
"! 	c #616161",
"~ 	c #CACACA",
"{ 	c #CFCFCF",
"] 	c #D0D0D0",
"^ 	c #CECECE",
"/ 	c #C9C9C9",
"( 	c #C1C1C1",
"_ 	c #A7A7A7",
": 	c #4C4C4C",
"< 	c #131313",
"[ 	c #222222",
"} 	c #757575",
"| 	c #D3D3D3",
"1 	c #DBDBDB",
"2 	c #E7E7E7",
"3 	c #EFEFEF",
"4 	c #F3F3F3",
"5 	c #F1F1F1",
"6 	c #E5E5E5",
"7 	c #D2D2D2",
"8 	c #BCBCBC",
"9 	c #5E5E5E",
"0 	c #101010",
"a 	c #212121",
"b 	c #5B5B5B",
"c 	c #CCCCCC",
"d 	c #D7D7D7",
"e 	c #F5F5F5",
"f 	c #FAFAFA",
"g 	c #FBFBFB",
"h 	c #F8F8F8",
"i 	c #F0F0F0",
"j 	c #E1E1E1",
"k 	c #C2C2C2",
"l 	c #434343",
"m 	c #0F0F0F",
"n 	c #1F1F1F",
"o 	c #B9B9B9",
"p 	c #D6D6D6",
"q 	c #F9F9F9",
"r 	c #FDFDFD",
"s 	c #FCFCFC",
"t 	c #F2F2F2",
"u 	c #ECECEC",
"v 	c #E4E4E4",
"w 	c #ABABAB",
"x 	c #0E0E0E",
"y 	c #1B1B1B",
"z 	c #6D6D6D",
"A 	c #FEFEFE",
"B 	c #EEEEEE",
"C 	c #E6E6E6",
"D 	c #575757",
"E 	c #090909",
"F 	c #141414",
"G 	c #A8A8A8",
"H 	c #D8D8D8",
"I 	c #F6F6F6",
"J 	c #F4F4F4",
"K 	c #DCDCDC",
"L 	c #9B9B9B",
"M 	c #060606",
"N 	c #111111",
"O 	c #C5C5C5",
"P 	c #DFDFDF",
"Q 	c #444444",
"R 	c #454545",
"S 	c #424242",
"T 	c #EDEDED",
"U 	c #BFBFBF",
"V 	c #C6C6C6",
"W 	c #E3E3E3",
"X 	c #414141",
"Y 	c #EAEAEA",
"Z 	c #E0E0E0",
"` 	c #BABABA",
" .	c #050505",
"..	c #0B0B0B",
"+.	c #A5A5A5",
"@.	c #D1D1D1",
"#.	c #939393",
"$.	c #020202",
"%.	c #0A0A0A",
"&.	c #5F5F5F",
"*.	c #D9D9D9",
"=.	c #EBEBEB",
"-.	c #E9E9E9",
";.	c #D4D4D4",
">.	c #000000",
",.	c #E2E2E2",
"'.	c #DADADA",
").	c #CBCBCB",
"!.	c #3B3B3B",
"~.	c #D5D5D5",
"{.	c #C8C8C8",
"].	c #BDBDBD",
"^.	c #515151",
"/.	c #C7C7C7",
"(.	c #CDCDCD",
"_.	c #B8B8B8",
":.	c #030303",
"<.	c #313131",
"[.	c #999999",
"}.	c #BBBBBB",
"|.	c #B6B6B6",
"1.	c #909090",
"2.	c #2B2B2B",
"3.	c #010101",
"4.	c #7A7A7A",
"5.	c #9A9A9A",
"6.	c #777777",
"7.	c #3C3C3C",
"8.	c #686868",
"9.	c #797979",
"0.	c #3A3A3A",
"                                                ",
"              . + @ # # #                       ",
"          $ % & * = - ; > , '                   ",
"        ) ! = ~ { ] ^ / ( _ : <                 ",
"      [ } ~ | 1 2 3 4 5 6 7 8 9 0               ",
"    a b c d 6 e f g f h e i j k l m             ",
"    n o p 2 q g r r s g h t u v w x             ",
"  y z 7 j e f r A A r s q e B C 1 D E           ",
"  F G H B I q s A A r g h J B C K L M           ",
"  N O P 3 e Q Q R R R Q l S T 6 1 U M           ",
"  x V W u t l Q Q Q Q l S X Y Z d `  .          ",
"  ..+.j 2 T 5 J I I e 4 3 u v 1 @.#.$.          ",
"  %.&.*.j C =.B 3 3 B T -.v 1 ;.~ : >.          ",
"    %.= H K ,.6 C C 2 W P '.;.).* $.            ",
"    M !.c 7 ;.'.1 '.1 H ~.] {.].$ >.            ",
"       .^.k /.).c (.).).{.k _.R >.              ",
"        :.<.[.}.].8 8 8 |.1.2.>.>.>.            ",
"          3.:.X 4.5.5.6.7.>.>.  >.>.>.>.        ",
"              >.>.>.>.>.>.        , , >.>.      ",
"                                  >.8.. >.>.    ",
"                                    >.9.0.>.>.  ",
"                                      >.8.n >.  ",
"                                        >.>.    ",
"                                                "},//icon_zoomin_pixmaps_c_1 268
{NULL},//zoomin_pixmap 269
{NULL},//zoomin_mask 270
{ 
"24 24 132 2",
"  	c None",
". 	c #343434",
"+ 	c #2D2D2D",
"@ 	c #292929",
"# 	c #262626",
"$ 	c #2E2E2E",
"% 	c #303030",
"& 	c #737373",
"* 	c #A1A1A1",
"= 	c #B4B4B4",
"- 	c #B2B2B2",
"; 	c #9D9D9D",
"> 	c #676767",
", 	c #202020",
"' 	c #1C1C1C",
") 	c #272727",
"! 	c #616161",
"~ 	c #CACACA",
"{ 	c #CFCFCF",
"] 	c #D0D0D0",
"^ 	c #CECECE",
"/ 	c #C9C9C9",
"( 	c #C1C1C1",
"_ 	c #A7A7A7",
": 	c #4C4C4C",
"< 	c #131313",
"[ 	c #222222",
"} 	c #757575",
"| 	c #D3D3D3",
"1 	c #DBDBDB",
"2 	c #E7E7E7",
"3 	c #EFEFEF",
"4 	c #F3F3F3",
"5 	c #F1F1F1",
"6 	c #E5E5E5",
"7 	c #D2D2D2",
"8 	c #BCBCBC",
"9 	c #5E5E5E",
"0 	c #101010",
"a 	c #212121",
"b 	c #5B5B5B",
"c 	c #CCCCCC",
"d 	c #D7D7D7",
"e 	c #F5F5F5",
"f 	c #FAFAFA",
"g 	c #FBFBFB",
"h 	c #F8F8F8",
"i 	c #F0F0F0",
"j 	c #E1E1E1",
"k 	c #C2C2C2",
"l 	c #434343",
"m 	c #0F0F0F",
"n 	c #1F1F1F",
"o 	c #B9B9B9",
"p 	c #D6D6D6",
"q 	c #F9F9F9",
"r 	c #FDFDFD",
"s 	c #454545",
"t 	c #F2F2F2",
"u 	c #ECECEC",
"v 	c #E4E4E4",
"w 	c #ABABAB",
"x 	c #0E0E0E",
"y 	c #1B1B1B",
"z 	c #6D6D6D",
"A 	c #FEFEFE",
"B 	c #FCFCFC",
"C 	c #EEEEEE",
"D 	c #E6E6E6",
"E 	c #575757",
"F 	c #090909",
"G 	c #141414",
"H 	c #A8A8A8",
"I 	c #D8D8D8",
"J 	c #F6F6F6",
"K 	c #F4F4F4",
"L 	c #DCDCDC",
"M 	c #9B9B9B",
"N 	c #060606",
"O 	c #111111",
"P 	c #C5C5C5",
"Q 	c #DFDFDF",
"R 	c #444444",
"S 	c #424242",
"T 	c #EDEDED",
"U 	c #BFBFBF",
"V 	c #C6C6C6",
"W 	c #E3E3E3",
"X 	c #414141",
"Y 	c #EAEAEA",
"Z 	c #E0E0E0",
"` 	c #BABABA",
" .	c #050505",
"..	c #0B0B0B",
"+.	c #A5A5A5",
"@.	c #D1D1D1",
"#.	c #939393",
"$.	c #020202",
"%.	c #0A0A0A",
"&.	c #5F5F5F",
"*.	c #D9D9D9",
"=.	c #EBEBEB",
"-.	c #E9E9E9",
";.	c #D4D4D4",
">.	c #000000",
",.	c #E2E2E2",
"'.	c #3F3F3F",
").	c #DADADA",
"!.	c #CBCBCB",
"~.	c #3B3B3B",
"{.	c #D5D5D5",
"].	c #C8C8C8",
"^.	c #BDBDBD",
"/.	c #515151",
"(.	c #C7C7C7",
"_.	c #CDCDCD",
":.	c #B8B8B8",
"<.	c #030303",
"[.	c #313131",
"}.	c #999999",
"|.	c #BBBBBB",
"1.	c #B6B6B6",
"2.	c #909090",
"3.	c #2B2B2B",
"4.	c #010101",
"5.	c #7A7A7A",
"6.	c #9A9A9A",
"7.	c #777777",
"8.	c #3C3C3C",
"9.	c #686868",
"0.	c #797979",
"a.	c #3A3A3A",
"                                                ",
"              . + @ # # #                       ",
"          $ % & * = - ; > , '                   ",
"        ) ! = ~ { ] ^ / ( _ : <                 ",
"      [ } ~ | 1 2 3 4 5 6 7 8 9 0               ",
"    a b c d 6 e f g f h e i j k l m             ",
"    n o p 2 q g r s s g h t u v w x             ",
"  y z 7 j e f r A s s B q e C D 1 E F           ",
"  G H I C J q B A s s g h K C D L M N           ",
"  O P Q 3 e R R s s s R l S T 6 1 U N           ",
"  x V W u t l R R R R l S X Y Z d `  .          ",
"  ..+.j 2 T 5 K J l l 4 3 u v 1 @.#.$.          ",
"  %.&.*.j D =.C 3 X X T -.v 1 ;.~ : >.          ",
"    %.= I L ,.6 D '.'.W Q ).;.!.* $.            ",
"    N ~.c 7 ;.).1 ).1 I {.] ].^.$ >.            ",
"       ./.k (.!.c _.!.!.].k :.s >.              ",
"        <.[.}.|.^.8 8 8 1.2.3.>.>.>.            ",
"          4.<.X 5.6.6.7.8.>.>.  >.>.>.>.        ",
"              >.>.>.>.>.>.        , , >.>.      ",
"                                  >.9.. >.>.    ",
"                                    >.0.a.>.>.  ",
"                                      >.9.n >.  ",
"                                        >.>.    ",
"                                                "},//icon_zoomout_pixmaps_c_1 271
{NULL},//zoomout_pixmap 272
{NULL},//zoomout_mask 273
{ 
"24 24 140 2",
"  	c None",
". 	c #343434",
"+ 	c #2D2D2D",
"@ 	c #292929",
"# 	c #262626",
"$ 	c #2E2E2E",
"% 	c #303030",
"& 	c #737373",
"* 	c #A1A1A1",
"= 	c #B4B4B4",
"- 	c #B2B2B2",
"; 	c #9D9D9D",
"> 	c #676767",
", 	c #202020",
"' 	c #1C1C1C",
") 	c #272727",
"! 	c #616161",
"~ 	c #CACACA",
"{ 	c #CFCFCF",
"] 	c #D0D0D0",
"^ 	c #CECECE",
"/ 	c #C9C9C9",
"( 	c #C1C1C1",
"_ 	c #A7A7A7",
": 	c #4C4C4C",
"< 	c #131313",
"[ 	c #222222",
"} 	c #757575",
"| 	c #D3D3D3",
"1 	c #DBDBDB",
"2 	c #E7E7E7",
"3 	c #EFEFEF",
"4 	c #F3F3F3",
"5 	c #F1F1F1",
"6 	c #E5E5E5",
"7 	c #D2D2D2",
"8 	c #BCBCBC",
"9 	c #5E5E5E",
"0 	c #101010",
"a 	c #212121",
"b 	c #5B5B5B",
"c 	c #CCCCCC",
"d 	c #464646",
"e 	c #4B4B4B",
"f 	c #505050",
"g 	c #525252",
"h 	c #FBFBFB",
"i 	c #FAFAFA",
"j 	c #515151",
"k 	c #4F4F4F",
"l 	c #4A4A4A",
"m 	c #C2C2C2",
"n 	c #434343",
"o 	c #0F0F0F",
"p 	c #1F1F1F",
"q 	c #B9B9B9",
"r 	c #D6D6D6",
"s 	c #535353",
"t 	c #FDFDFD",
"u 	c #FCFCFC",
"v 	c #4D4D4D",
"w 	c #E4E4E4",
"x 	c #ABABAB",
"y 	c #0E0E0E",
"z 	c #1B1B1B",
"A 	c #6D6D6D",
"B 	c #E1E1E1",
"C 	c #FEFEFE",
"D 	c #F9F9F9",
"E 	c #4E4E4E",
"F 	c #E6E6E6",
"G 	c #575757",
"H 	c #090909",
"I 	c #141414",
"J 	c #A8A8A8",
"K 	c #D8D8D8",
"L 	c #EEEEEE",
"M 	c #F8F8F8",
"N 	c #DCDCDC",
"O 	c #9B9B9B",
"P 	c #060606",
"Q 	c #111111",
"R 	c #C5C5C5",
"S 	c #DFDFDF",
"T 	c #F5F5F5",
"U 	c #F7F7F7",
"V 	c #F2F2F2",
"W 	c #EDEDED",
"X 	c #BFBFBF",
"Y 	c #C6C6C6",
"Z 	c #E3E3E3",
"` 	c #ECECEC",
" .	c #F4F4F4",
"..	c #F0F0F0",
"+.	c #EAEAEA",
"@.	c #E0E0E0",
"#.	c #D7D7D7",
"$.	c #BABABA",
"%.	c #050505",
"&.	c #0B0B0B",
"*.	c #A5A5A5",
"=.	c #F6F6F6",
"-.	c #D1D1D1",
";.	c #939393",
">.	c #020202",
",.	c #0A0A0A",
"'.	c #5F5F5F",
").	c #D9D9D9",
"!.	c #E9E9E9",
"~.	c #484848",
"{.	c #D4D4D4",
"].	c #000000",
"^.	c #494949",
"/.	c #474747",
"(.	c #454545",
"_.	c #CBCBCB",
":.	c #3B3B3B",
"<.	c #DADADA",
"[.	c #444444",
"}.	c #414141",
"|.	c #BDBDBD",
"1.	c #C7C7C7",
"2.	c #CDCDCD",
"3.	c #C8C8C8",
"4.	c #B8B8B8",
"5.	c #030303",
"6.	c #313131",
"7.	c #999999",
"8.	c #BBBBBB",
"9.	c #B6B6B6",
"0.	c #909090",
"a.	c #2B2B2B",
"b.	c #010101",
"c.	c #7A7A7A",
"d.	c #9A9A9A",
"e.	c #777777",
"f.	c #3C3C3C",
"g.	c #686868",
"h.	c #797979",
"i.	c #3A3A3A",
"                                                ",
"              . + @ # # #                       ",
"          $ % & * = - ; > , '                   ",
"        ) ! = ~ { ] ^ / ( _ : <                 ",
"      [ } ~ | 1 2 3 4 5 6 7 8 9 0               ",
"    a b c d e f g h i j f k l m n o             ",
"    p q r : g g s t u g j k v w x y             ",
"  z A 7 B f g t C C t u D f E F 1 G H           ",
"  I J K L j g u C C t h M f E F N O P           ",
"  Q R S 3 T M h t t u i U V W 6 1 X P           ",
"  y Y Z ` V T M i i i U  ...+.@.#.$.%.          ",
"  &.*.B 2 E k  .=.=.T 4 3 v e 1 -.;.>.          ",
"  ,.'.).B e v L 3 3 L W !.e ~.{.~ : ].          ",
"    ,.= K ~.l e e F 2 l ^./.(._.* >.            ",
"    P :.c (.(./.~.<.1 /.d [.}.|.$ ].            ",
"      %.j m 1._.c 2._._.3.m 4.(.].              ",
"        5.6.7.8.|.8 8 8 9.0.a.].].].            ",
"          b.5.}.c.d.d.e.f.].].  ].].].].        ",
"              ].].].].].].        , , ].].      ",
"                                  ].g.. ].].    ",
"                                    ].h.i.].].  ",
"                                      ].g.p ].  ",
"                                        ].].    ",
"                                                "},//icon_zoomfit_pixmaps_c_1 274
{NULL},//zoomfit_pixmap 275
{NULL},//zoomfit_mask 276
{ 
"24 24 31 1",
" 	c None",
".	c #000000",
"+	c #EFE5BA",
"@	c #EFE7C1",
"#	c #EED680",
"$	c #EFE4B6",
"%	c #D5B75D",
"&	c #B29544",
"*	c #D1B051",
"=	c #C0AF73",
"-	c #C0A048",
";	c #986B07",
">	c #D1940C",
",	c #E0B74C",
"'	c #D9C374",
")	c #8F6406",
"!	c #D59D1C",
"~	c #B1933F",
"{	c #DFB74A",
"]	c #CCB76D",
"^	c #B8820A",
"/	c #D9A72E",
"(	c #D7A62C",
"_	c #C7B26A",
":	c #D4B150",
"<	c #A39256",
"[	c #E2CB79",
"}	c #C9B46B",
"|	c #8D7E4A",
"1	c #AE9C5C",
"2	c #96864F",
"                        ",
"                        ",
"                        ",
"          .             ",
"         ..             ",
"        .+.             ",
"       .@#....          ",
"      .$####%&.         ",
"     .+#######*.        ",
"    .=#########-.       ",
"     .;>>>>>>,#'..      ",
"      .)>>>>>>!#~.      ",
"       .)>...;>{].      ",
"        .;. ..^/#.      ",
"         ..  ..>#.      ",
"          .   .(_.      ",
"              .:<.      ",
"              .[.       ",
"             .}|.       ",
"            .12.        ",
"             ..         ",
"                        ",
"                        ",
"                        "},//icon_zoomundo_pixmaps_c_1 277
{NULL},//zoomundo_pixmap 278
{NULL},//zoomundo_mask 279
{
"24 24 57 1",
" 	c None",
".	c #000000",
"+	c #F7F7F7",
"@	c #CBD6CA",
"#	c #E7EFE7",
"$	c #ACC8A9",
"%	c #C9DBC9",
"&	c #E6EEE5",
"*	c #BFCEBF",
"=	c #E7EFE6",
"-	c #BBCFBA",
";	c #B3C4B3",
">	c #E6EEE6",
",	c #B9CEB7",
"'	c #B5CEB5",
")	c #B7CCB5",
"!	c #BFD4BF",
"~	c #C7D7C5",
"{	c #DBE5DB",
"]	c #DAE5D9",
"^	c #CBDAC9",
"/	c #7EAB78",
"(	c #BAD1B9",
"_	c #B3CAB1",
":	c #B0C9B0",
"<	c #B0C9AE",
"[	c #AEC7AC",
"}	c #AAC5A8",
"|	c #A9C4A7",
"1	c #698267",
"2	c #E4ECE3",
"3	c #2D2D2D",
"4	c #E0EADE",
"5	c #B3CCB1",
"6	c #B0C7AE",
"7	c #ADC6AB",
"8	c #ADC8AB",
"9	c #AAC7A8",
"0	c #678C63",
"a	c #9FB79B",
"b	c #6B9063",
"c	c #C2CDC2",
"d	c #8EB48A",
"e	c #87AF84",
"f	c #87B083",
"g	c #88AF84",
"h	c #88B085",
"i	c #86AF82",
"j	c #547150",
"k	c #95A88F",
"l	c #5B7950",
"m	c #3C5235",
"n	c #4A6342",
"o	c #3B5035",
"p	c #415639",
"q	c #889D7F",
"r	c #475E3E",
"                        ",
"                        ",
"                        ",
"  ....        .         ",
"  .+@.       ..         ",
"  .#$.      .%.         ",
"  .#$.     .&*.         ",
"  .#$.    .=-;........  ",
"  .#$.   .>,'),!~{]^/.  ",
"  .#$.  .>(_:<[}}}[|1.  ",
"  .2$. 34567789[[6[80.  ",
"  .ab..cdeeeeefgefhij.  ",
"  .kl. .mllllllllllln.  ",
"  .kl.  .mlllllllllln.  ",
"  .kl.   .mllnoooooop.  ",
"  .kl.    .mln........  ",
"  .kl.     .mn.         ",
"  .kl.      .n.         ",
"  .qr.       ..         ",
"  ....        .         ",
"                        ",
"                        ",
"                        ",
"                        "},//zoom_larrow_pixmaps_c_1 280
{NULL},//zoom_larrow_pixmap 281
{NULL},//zoom_larrow_mask 282
{
"24 24 52 1",
" 	c None",
".	c #000000",
"+	c #F7F7F7",
"@	c #CBD6CA",
"#	c #BECEBA",
"$	c #E7EFE7",
"%	c #ACC8A9",
"&	c #EBF2EA",
"*	c #77A16E",
"=	c #E3EBE2",
"-	c #709867",
";	c #F8F8F7",
">	c #F1F5F0",
",	c #ECF2EB",
"'	c #E5EEE3",
")	c #E0EBDF",
"!	c #D8E6D6",
"~	c #C6D9C2",
"{	c #C5D7C3",
"]	c #B2CBB0",
"^	c #AAC7A8",
"/	c #0F1308",
"(	c #DDE6DB",
"_	c #AEC8AD",
":	c #ABC7A8",
"<	c #AAC6A7",
"[	c #A8C6A5",
"}	c #ADC8AD",
"|	c #A8C7A8",
"1	c #A5C4A3",
"2	c #7F9F76",
"3	c #D6E1D4",
"4	c #ABC7AA",
"5	c #A7C5A4",
"6	c #A9C7A6",
"7	c #AFC8AD",
"8	c #A4C3A2",
"9	c #6B9060",
"0	c #E4ECE3",
"a	c #A7B6A2",
"b	c #698D60",
"c	c #6B9063",
"d	c #445B2C",
"e	c #9FB79B",
"f	c #9FB199",
"g	c #5B7950",
"h	c #95A88F",
"i	c #9FAF99",
"j	c #789171",
"k	c #506B46",
"l	c #889D7F",
"m	c #475E3E",
"                        ",
"                        ",
"                        ",
"        .        ....   ",
"        ..       .+@.   ",
"        .#.      .$%.   ",
"        .&*.     .$%.   ",
" ........=%-.    .$%.   ",
" .;>,')!~{]^-/   .$%.   ",
" .(_:::<[^}|12.  .$%.   ",
" .34[5[6:%^:789. .0%.   ",
" .abccccccccccbd..ec.   ",
" .fgggggggggggd. .hg.   ",
" .iggggggggggd.  .hg.   ",
" .jkkkkkkgggd.   .hg.   ",
" ........kgd.    .hg.   ",
"        .kd.     .hg.   ",
"        .d.      .hg.   ",
"        ..       .lm.   ",
"        .        ....   ",
"                        ",
"                        ",
"                        ",
"                        "},//zoom_rarrow_pixmaps_c_1 283
{NULL},//zoom_rarrow_pixmap 284
{NULL},//zoom_rarrow_mask 285
{ 
"24 24 170 2",
"  	c None",
". 	c #000000",
"+ 	c #040506",
"@ 	c #0B0F12",
"# 	c #959FAA",
"$ 	c #C3C6CA",
"% 	c #82909E",
"& 	c #F1F1F1",
"* 	c #D5D7D8",
"= 	c #0A0D10",
"- 	c #11171C",
"; 	c #8693A0",
"> 	c #EFF0F0",
", 	c #DEDEDE",
"' 	c #D5D5D5",
") 	c #9B9FA4",
"! 	c #0E1317",
"~ 	c #85929F",
"{ 	c #DBDBDB",
"] 	c #CACACA",
"^ 	c #C8C8C8",
"/ 	c #C8C9CA",
"( 	c #0C0F13",
"_ 	c #141A20",
": 	c #798895",
"< 	c #D2D2D2",
"[ 	c #C3C3C3",
"} 	c #CDCDCD",
"| 	c #74797E",
"1 	c #171F26",
"2 	c #7F8D9A",
"3 	c #DADADA",
"4 	c #D3D3D3",
"5 	c #C4C4C4",
"6 	c #CECECE",
"7 	c #C0C0C0",
"8 	c #B1B1B1",
"9 	c #0D1115",
"0 	c #768592",
"a 	c #EEEFEF",
"b 	c #D9D9D9",
"c 	c #D0D0D0",
"d 	c #C2C2C2",
"e 	c #CBCBCB",
"f 	c #C9C9C9",
"g 	c #BCBCBC",
"h 	c #A4A7A9",
"i 	c #686B6C",
"j 	c #333333",
"k 	c #414F5D",
"l 	c #ADB5BC",
"m 	c #EAEAEA",
"n 	c #BFBFBF",
"o 	c #BDBDBD",
"p 	c #C6C6C6",
"q 	c #B5B6B7",
"r 	c #899096",
"s 	c #B8B8B8",
"t 	c #FFFFFF",
"u 	c #404142",
"v 	c #4D6074",
"w 	c #EBEBEB",
"x 	c #D1D1D1",
"y 	c #C1C1C1",
"z 	c #B7B7B7",
"A 	c #979FA7",
"B 	c #A1A3A4",
"C 	c #FCFDFD",
"D 	c #F3F4F4",
"E 	c #EBECED",
"F 	c #EAEBEC",
"G 	c #E9EAEB",
"H 	c #EEEFF1",
"I 	c #F6F7F8",
"J 	c #E4E5E6",
"K 	c #929395",
"L 	c #32414F",
"M 	c #8C98A2",
"N 	c #E7E7E7",
"O 	c #BBBBBB",
"P 	c #A4A7AB",
"Q 	c #939699",
"R 	c #D7D8D8",
"S 	c #FAFCFC",
"T 	c #F9FAFB",
"U 	c #F8F9FA",
"V 	c #F7F8F9",
"W 	c #F5F6F8",
"X 	c #F7F7F9",
"Y 	c #C6C7C7",
"Z 	c #4F5E6A",
"` 	c #4C5F72",
" .	c #E8E8E8",
"..	c #BEBEBE",
"+.	c #1F1F1F",
"@.	c #B5B5B6",
"#.	c #979EA4",
"$.	c #A8A8A8",
"%.	c #FDFEFE",
"&.	c #F5F6F6",
"*.	c #E7E9E9",
"=.	c #F0F1F2",
"-.	c #E0E2E3",
";.	c #8F969D",
">.	c #151C22",
",.	c #354453",
"'.	c #8D99A2",
").	c #E3E3E3",
"!.	c #8E949A",
"~.	c #9D9F9F",
"{.	c #F2F3F4",
"].	c #F1F2F3",
"^.	c #F6F6F7",
"/.	c #A6A8AB",
"(.	c #737F8A",
"_.	c #4B5F71",
":.	c #CDDECB",
"<.	c #97999B",
"[.	c #E7E8E8",
"}.	c #F4F5F5",
"|.	c #FBFBFB",
"1.	c #D8D8DA",
"2.	c #939BA3",
"3.	c #0F1418",
"4.	c #B1C9B0",
"5.	c #E6E7E8",
"6.	c #FAFAFB",
"7.	c #F5F5F7",
"8.	c #AEB2B6",
"9.	c #5C6C7C",
"0.	c #B4CBB2",
"a.	c #B5CEB5",
"b.	c #B9CEB7",
"c.	c #BAD1BA",
"d.	c #BBD0B9",
"e.	c #B2C9B0",
"f.	c #7EAB78",
"g.	c #FBFCFC",
"h.	c #DEE1E3",
"i.	c #9DA6AE",
"j.	c #AAC7A8",
"k.	c #B3CAB1",
"l.	c #B0C9B0",
"m.	c #AEC7AC",
"n.	c #AAC5A8",
"o.	c #A9C4A7",
"p.	c #698267",
"q.	c #F9F9FA",
"r.	c #BBBFC4",
"s.	c #566779",
"t.	c #2D2D2D",
"u.	c #CFDFCC",
"v.	c #ADC8AB",
"w.	c #B0C7AE",
"x.	c #ADC6AB",
"y.	c #678C63",
"z.	c #D1D2D4",
"A.	c #9AA3AC",
"B.	c #3C5235",
"C.	c #5B7950",
"D.	c #4A6342",
"E.	c #E8ECEE",
"F.	c #B2B8BE",
"G.	c #717D88",
"H.	c #3B5035",
"I.	c #415639",
"J.	c #7A8695",
"K.	c #768594",
"L.	c #2C343D",
"M.	c #21272E",
"              . +                               ",
"            @ # $ .                             ",
"          @ % & * =                             ",
"        - ; > , ' ) .                           ",
"      ! ~ > { ] ^ / (                           ",
"    _ : & ' ] < [ } | .                         ",
"  1 2 > 3 4 5 6 7 ] 8 9                         ",
". 0 a b 4 c d e f g h i j . . . . . . .         ",
"k l m < n } e o p q r s t t t t t t t u .       ",
". v w x [ y ^ z [ A B C D E F G H I J K .       ",
"  L M N f O 5 d P Q R C S T U V W X Y Z .       ",
"  . `  .p ..+.@.#.$.%.&.*.E F =.U -.;.>..       ",
"    ,.'.).. . !.~.t %.C S {.].V ^./.(.+         ",
"    . _.. :.. <.b t [.a }.T U |.1.2.3.          ",
"      . :.4.. . . . . . . 5.6.7.8.9..           ",
"    . :.0.a.b.c.b.d.e.f.. T g.h.i.=             ",
"  . :.j.k.l.m.n.n.m.o.p.. g.q.r.s..             ",
"t.u.v.w.x.x.j.m.m.m.v.y.. t z.A.+               ",
"  . B.C.C.C.C.C.C.C.C.D.. E.F.G..               ",
"    . B.C.C.H.H.H.H.H.I.. J.K..                 ",
"      . B.C.. . . . . . . L.M.                  ",
"        . B..                                   ",
"          . .                                   ",
"            .                                   "},//prev_page_xpm_pixmaps_c_1 286
{NULL},//prev_page_pixmap 287
{NULL},//prev_page_mask 288
{ 
"24 24 172 2",
"  	c None",
". 	c #000000",
"+ 	c #040506",
"@ 	c #0B0F12",
"# 	c #959FAA",
"$ 	c #C3C6CA",
"% 	c #82909E",
"& 	c #F1F1F1",
"* 	c #D5D7D8",
"= 	c #0A0D10",
"- 	c #11171C",
"; 	c #8693A0",
"> 	c #EFF0F0",
", 	c #DEDEDE",
"' 	c #D5D5D5",
") 	c #9B9FA4",
"! 	c #0E1317",
"~ 	c #85929F",
"{ 	c #DBDBDB",
"] 	c #CACACA",
"^ 	c #C8C8C8",
"/ 	c #C8C9CA",
"( 	c #0C0F13",
"_ 	c #141A20",
": 	c #798895",
"< 	c #D2D2D2",
"[ 	c #C3C3C3",
"} 	c #CDCDCD",
"| 	c #74797E",
"1 	c #171F26",
"2 	c #7F8D9A",
"3 	c #DADADA",
"4 	c #D3D3D3",
"5 	c #C4C4C4",
"6 	c #CECECE",
"7 	c #C0C0C0",
"8 	c #B1B1B1",
"9 	c #0D1115",
"0 	c #768592",
"a 	c #EEEFEF",
"b 	c #D9D9D9",
"c 	c #D0D0D0",
"d 	c #C2C2C2",
"e 	c #CBCBCB",
"f 	c #C9C9C9",
"g 	c #BCBCBC",
"h 	c #A4A7A9",
"i 	c #686B6C",
"j 	c #333333",
"k 	c #414F5D",
"l 	c #ADB5BC",
"m 	c #EAEAEA",
"n 	c #BFBFBF",
"o 	c #BDBDBD",
"p 	c #C6C6C6",
"q 	c #B5B6B7",
"r 	c #899096",
"s 	c #B8B8B8",
"t 	c #FFFFFF",
"u 	c #404142",
"v 	c #4D6074",
"w 	c #EBEBEB",
"x 	c #D1D1D1",
"y 	c #C1C1C1",
"z 	c #B7B7B7",
"A 	c #979FA7",
"B 	c #A1A3A4",
"C 	c #FCFDFD",
"D 	c #F3F4F4",
"E 	c #EBECED",
"F 	c #EAEBEC",
"G 	c #E9EAEB",
"H 	c #EEEFF1",
"I 	c #F6F7F8",
"J 	c #E4E5E6",
"K 	c #929395",
"L 	c #32414F",
"M 	c #8C98A2",
"N 	c #E7E7E7",
"O 	c #BBBBBB",
"P 	c #A4A7AB",
"Q 	c #939699",
"R 	c #D7D8D8",
"S 	c #FAFCFC",
"T 	c #F9FAFB",
"U 	c #F8F9FA",
"V 	c #F7F8F9",
"W 	c #F5F6F8",
"X 	c #F7F7F9",
"Y 	c #C6C7C7",
"Z 	c #4F5E6A",
"` 	c #4C5F72",
" .	c #E8E8E8",
"..	c #BEBEBE",
"+.	c #B5B5B6",
"@.	c #979EA4",
"#.	c #A8A8A8",
"$.	c #FDFEFE",
"%.	c #F5F6F6",
"&.	c #E7E9E9",
"*.	c #F0F1F2",
"=.	c #8F969D",
"-.	c #151C22",
";.	c #354453",
">.	c #8D99A2",
",.	c #E3E3E3",
"'.	c #8E949A",
").	c #9D9F9F",
"!.	c #F2F3F4",
"~.	c #F1F2F3",
"{.	c #F6F6F7",
"].	c #4B5F71",
"^.	c #E5E5E5",
"/.	c #9E9FA0",
"(.	c #97999B",
"_.	c #E7E8E8",
":.	c #F4F5F5",
"<.	c #FBFBFB",
"[.	c #D8D8DA",
"}.	c #77A16E",
"|.	c #394957",
"1.	c #828F97",
"2.	c #8A8E92",
"3.	c #B4B4B4",
"4.	c #ACC8A9",
"5.	c #709867",
"6.	c #4A5D70",
"7.	c #ABABAB",
"8.	c #F8F8F8",
"9.	c #C1D6BD",
"0.	c #BDD3B8",
"a.	c #BFD4BB",
"b.	c #C2D7BE",
"c.	c #B2CBB0",
"d.	c #AAC7A8",
"e.	c #0F1308",
"f.	c #242F3A",
"g.	c #696D71",
"h.	c #777879",
"i.	c #F2F3F3",
"j.	c #AEC5A8",
"k.	c #AEC8AD",
"l.	c #ABC7A8",
"m.	c #AAC6A7",
"n.	c #A8C6A5",
"o.	c #ADC8AD",
"p.	c #A8C7A8",
"q.	c #A5C4A3",
"r.	c #7F9F76",
"s.	c #4A5F74",
"t.	c #313E4B",
"u.	c #B3BDC6",
"v.	c #D1D7DD",
"w.	c #D9DEE3",
"x.	c #F6F7F7",
"y.	c #A6BFA0",
"z.	c #ABC7AA",
"A.	c #A9C7A6",
"B.	c #AFC8AD",
"C.	c #A4C3A2",
"D.	c #6B9060",
"E.	c #2B343C",
"F.	c #29323B",
"G.	c #4F5F70",
"H.	c #5E7184",
"I.	c #919EAB",
"J.	c #6C8562",
"K.	c #5B7950",
"L.	c #445B2C",
"M.	c #252D35",
"N.	c #65815C",
"O.	c #506B46",
"              . +                               ",
"            @ # $ .                             ",
"          @ % & * =                             ",
"        - ; > , ' ) .                           ",
"      ! ~ > { ] ^ / (                           ",
"    _ : & ' ] < [ } | .                         ",
"  1 2 > 3 4 5 6 7 ] 8 9                         ",
". 0 a b 4 c d e f g h i j . . . . . . .         ",
"k l m < n } e o p q r s t t t t t t t u .       ",
". v w x [ y ^ z [ A B C D E F G H I J K .       ",
"  L M N f O 5 d P Q R C S T U V W X Y Z .       ",
"  . `  .p ..s +.@.#.$.%.&.E F *.U . =.-..       ",
"    ;.>.,.y ..'.).t $.C S !.~.V {.. . +         ",
"    . ].^.y /.(.b t _.a :.T U <.[.. }..         ",
"      |.1.{ 2.3.t t $.. . . . . . . 4.5..       ",
"      . 6.O 7.t 8.& > . 9.0.a.b.b.b.c.d.5.e.    ",
"        f.g.h.i.t t t . j.k.l.l.m.n.o.p.q.r..   ",
"        . s.t.u.v.w.x.. y.z.n.n.A.l.d.l.B.C.D.. ",
"          . E.F.G.H.I.. J.K.K.K.K.K.K.K.K.L..   ",
"                . . M.. N.O.O.O.O.O.K.K.L..     ",
"                      . . . . . . . K.L..       ",
"                                  . L..         ",
"                                  . .           ",
"                                  .             "},//next_page_xpm_pixmaps_c_1 289
{NULL},//next_page_pixmap 290
{NULL},//next_page_mask 291
{ 
"48 52 677 2",
"  	c None",
". 	c #000000",
"+ 	c #232323",
"@ 	c #1F1F1F",
"# 	c #DADADA",
"$ 	c #FFFFFF",
"% 	c #F6F6F6",
"& 	c #CFCFCF",
"* 	c #707070",
"= 	c #FEFEFE",
"- 	c #FBFBFB",
"; 	c #EDEDED",
"> 	c #C0C0C0",
", 	c #FF0000",
"' 	c #F7F7F7",
") 	c #D7D7D7",
"! 	c #F1F1F1",
"~ 	c #B7B7B7",
"{ 	c #D2D2D2",
"] 	c #AEAEAE",
"^ 	c #070707",
"/ 	c #FDFDFD",
"( 	c #B8B8B8",
"_ 	c #DCDCDC",
": 	c #A4A4A3",
"< 	c #0E0E0E",
"[ 	c #F5F5F4",
"} 	c #B5B5B5",
"| 	c #F9F9F9",
"1 	c #FCFCFC",
"2 	c #CBCBCB",
"3 	c #A5A5A5",
"4 	c #FDFDFC",
"5 	c #F4F4F4",
"6 	c #ACACAC",
"7 	c #ECECEC",
"8 	c #C3C3C3",
"9 	c #C2C2C2",
"0 	c #C9C9C8",
"a 	c #A8A8A8",
"b 	c #FCFCFB",
"c 	c #F4F4F3",
"d 	c #ABABAB",
"e 	c #515151",
"f 	c #474747",
"g 	c #464645",
"h 	c #464646",
"i 	c #5D5D5C",
"j 	c #A9A9A9",
"k 	c #FAFAFA",
"l 	c #F2F2F2",
"m 	c #F3F3F3",
"n 	c #AAAAAA",
"o 	c #A0A09F",
"p 	c #8A8A8A",
"q 	c #7A7A7A",
"r 	c #6C6C6C",
"s 	c #454545",
"t 	c #FAFAF9",
"u 	c #E7E7E7",
"v 	c #C6C6C5",
"w 	c #BABAB9",
"x 	c #F5F5F5",
"y 	c #EDEDEB",
"z 	c #B2B2B2",
"A 	c #949494",
"B 	c #7F7F7F",
"C 	c #8B8B8B",
"D 	c #8D8D8D",
"E 	c #EFEFEF",
"F 	c #737373",
"G 	c #727272",
"H 	c #666666",
"I 	c #4F4F4F",
"J 	c #4E4E4E",
"K 	c #4D4D4D",
"L 	c #505050",
"M 	c #CACACA",
"N 	c #4B4B4B",
"O 	c #4A4A4A",
"P 	c #4C4C4C",
"Q 	c #525252",
"R 	c #575757",
"S 	c #373737",
"T 	c #A1A1A1",
"U 	c #363636",
"V 	c #333333",
"W 	c #353535",
"X 	c #3F3F3F",
"Y 	c #5C5C5C",
"Z 	c #6B6B6B",
"` 	c #B1B1B1",
" .	c #A6A6A6",
"..	c #E4E4E4",
"+.	c #2E2E2E",
"@.	c #2D2D2D",
"#.	c #252525",
"$.	c #191919",
"%.	c #1B1B1B",
"&.	c #1C1C1C",
"*.	c #1E1E1E",
"=.	c #212121",
"-.	c #B9B9B9",
";.	c #1D1D1D",
">.	c #171717",
",.	c #242424",
"'.	c #222222",
").	c #919191",
"!.	c #292929",
"~.	c #F3F3F2",
"{.	c #9C9C9C",
"].	c #696969",
"^.	c #030303",
"/.	c #010101",
"(.	c #B0B0B0",
"_.	c #080808",
":.	c #060606",
"<.	c #868686",
"[.	c #040404",
"}.	c #0C0C0C",
"|.	c #2A2A2A",
"1.	c #F2F2F1",
"2.	c #979797",
"3.	c #5F5F5F",
"4.	c #131413",
"5.	c #222421",
"6.	c #2B302A",
"7.	c #B4BAB3",
"8.	c #2B312A",
"9.	c #2A3029",
"0.	c #292E28",
"a.	c #282D26",
"b.	c #272D26",
"c.	c #262C25",
"d.	c #252C24",
"e.	c #242B23",
"f.	c #242B22",
"g.	c #8B9289",
"h.	c #1E241D",
"i.	c #151814",
"j.	c #101010",
"k.	c #303030",
"l.	c #F1F1F0",
"m.	c #8E8E8E",
"n.	c #D4D4D4",
"o.	c #272A26",
"p.	c #495546",
"q.	c #64825E",
"r.	c #6B9063",
"s.	c #ABD3A2",
"t.	c #658D5C",
"u.	c #628C59",
"v.	c #608A56",
"w.	c #5E8B54",
"x.	c #598550",
"y.	c #57864D",
"z.	c #54844A",
"A.	c #518046",
"B.	c #4F8143",
"C.	c #84B779",
"D.	c #46773C",
"E.	c #344E2F",
"F.	c #1C241A",
"G.	c #F0F0F0",
"H.	c #848484",
"I.	c #D0D0D0",
"J.	c #262825",
"K.	c #5A6E56",
"L.	c #86C476",
"M.	c #8FE878",
"N.	c #8DEC74",
"O.	c #93F479",
"P.	c #84E66A",
"Q.	c #81E665",
"R.	c #7DE661",
"S.	c #7AE75C",
"T.	c #75E259",
"U.	c #72E653",
"V.	c #6EE14F",
"W.	c #6BE14A",
"X.	c #67E446",
"Y.	c #6FEC4D",
"Z.	c #60E53D",
"`.	c #53B13A",
" +	c #32522B",
".+	c #141A13",
"++	c #0B0B0B",
"@+	c #2B2B2B",
"#+	c #EFEFEE",
"$+	c #EFEFED",
"%+	c #777777",
"&+	c #CDCDCD",
"*+	c #414B3F",
"=+	c #7FB372",
"-+	c #93F778",
";+	c #90F974",
">+	c #8AF06F",
",+	c #90F474",
"'+	c #81E666",
")+	c #79E55C",
"!+	c #76E758",
"~+	c #71E153",
"{+	c #6FE64E",
"]+	c #67E145",
"^+	c #63E441",
"/+	c #69EE46",
"(+	c #60F537",
"_+	c #5AF032",
":+	c #449030",
"<+	c #20301D",
"[+	c #080908",
"}+	c #0A0A0A",
"|+	c #EEEEED",
"1+	c #EEEEEC",
"2+	c #FBFBFA",
"3+	c #747474",
"4+	c #313131",
"5+	c #1C1E1C",
"6+	c #586D53",
"7+	c #8AD676",
"8+	c #90FB73",
"9+	c #87E66E",
"0+	c #74B065",
"a+	c #9FD095",
"b+	c #5E8B53",
"c+	c #5A8850",
"d+	c #57864C",
"e+	c #548649",
"f+	c #508145",
"g+	c #4E8242",
"h+	c #4B7F3F",
"i+	c #487E3C",
"j+	c #47823A",
"k+	c #73C060",
"l+	c #56DA34",
"m+	c #58F72C",
"n+	c #4BBE2D",
"o+	c #284821",
"p+	c #0F140E",
"q+	c #343434",
"r+	c #414141",
"s+	c #EDEDEC",
"t+	c #E2E2E2",
"u+	c #D8D8D8",
"v+	c #C6C6C6",
"w+	c #BEBEBE",
"x+	c #BABABA",
"y+	c #BDC1BC",
"z+	c #B0D6A8",
"A+	c #96F57C",
"B+	c #8FFA72",
"C+	c #9AE089",
"D+	c #A7C0A2",
"E+	c #A6B0A3",
"F+	c #A0A79F",
"G+	c #9CA39B",
"H+	c #989E96",
"I+	c #949B92",
"J+	c #90978E",
"K+	c #878F85",
"L+	c #828B80",
"M+	c #7E8C7B",
"N+	c #799972",
"O+	c #67C750",
"P+	c #56F62C",
"Q+	c #56E92E",
"R+	c #619E53",
"S+	c #606C5E",
"T+	c #565756",
"U+	c #5A5A5A",
"V+	c #7B7B7B",
"W+	c #828282",
"X+	c #ECECEB",
"Y+	c #757575",
"Z+	c #3B4439",
"`+	c #77B069",
" @	c #8AF66D",
".@	c #84EC68",
"+@	c #65965A",
"@@	c #313C2F",
"#@	c #9C9D9C",
"$@	c #050505",
"%@	c #0D0F0C",
"&@	c #758672",
"*@	c #3D832D",
"=@	c #4DE625",
"-@	c #4AF21E",
";@	c #369320",
">@	c #182E13",
",@	c #050704",
"'@	c #121212",
")@	c #494949",
"!@	c #565656",
"~@	c #2C2C2C",
"{@	c #171917",
"]@	c #4D6149",
"^@	c #7FCE6C",
"/@	c #87FB68",
"(@	c #7DDB65",
"_@	c #53744C",
":@	c #1F241E",
"<@	c #959595",
"[@	c #020202",
"}@	c #70786E",
"|@	c #2E5B24",
"1@	c #45CA22",
"2@	c #48F719",
"3@	c #39B719",
"4@	c #1A3E12",
"5@	c #050C04",
"6@	c #141514",
"7@	c #595959",
"8@	c #EBEBEA",
"9@	c #F8F8F8",
"0@	c #5E5E5E",
"a@	c #BCBCBC",
"b@	c #638A5A",
"c@	c #82E867",
"d@	c #83FA62",
"e@	c #72C15E",
"f@	c #40543C",
"g@	c #151714",
"h@	c #6B6E6A",
"i@	c #203B1B",
"j@	c #39A51E",
"k@	c #42F112",
"l@	c #3AD410",
"m@	c #1E5C10",
"n@	c #061103",
"o@	c #111211",
"p@	c #555555",
"q@	c #EAEAE9",
"r@	c #EAEAE8",
"s@	c #F8F8F7",
"t@	c #5B5B5B",
"u@	c #0D0D0D",
"v@	c #3A4638",
"w@	c #71AE62",
"x@	c #81F563",
"y@	c #7DF15E",
"z@	c #64A155",
"A@	c #313D2E",
"B@	c #646664",
"C@	c #162813",
"D@	c #2C7919",
"E@	c #3ADE0E",
"F@	c #36DF09",
"G@	c #21780D",
"H@	c #091A05",
"I@	c #0E0F0D",
"J@	c #444444",
"K@	c #E9E9E8",
"L@	c #E9E9E7",
"M@	c #5D5D5D",
"N@	c #B6B6B6",
"O@	c #1A1C19",
"P@	c #4B6146",
"Q@	c #75C262",
"R@	c #7EF65D",
"S@	c #75DA5A",
"T@	c #517949",
"U@	c #212720",
"V@	c #0C160A",
"W@	c #1E5213",
"X@	c #30BA0C",
"Y@	c #33DD04",
"Z@	c #25890C",
"`@	c #0C2507",
" #	c #10130F",
".#	c #F6F6F5",
"+#	c #262B25",
"@#	c #5A8052",
"##	c #79E05E",
"$#	c #7BF957",
"%#	c #6BC554",
"&#	c #3E5639",
"*#	c #131613",
"=#	c #878787",
"-#	c #050A04",
";#	c #13350B",
">#	c #299E0B",
",#	c #32DA04",
"'#	c #2BB208",
")#	c #133D0A",
"!#	c #0F160E",
"~#	c #3E3E3E",
"{#	c #E8E8E7",
"]#	c #E8E8E6",
"^#	c #AFAFAF",
"/#	c #323D30",
"(#	c #66A257",
"_#	c #7AF458",
":#	c #75F553",
"<#	c #5EA44E",
"[#	c #2F3D2C",
"}#	c #0A0B0A",
"|#	c #838383",
"1#	c #585858",
"2#	c #010201",
"3#	c #091D06",
"4#	c #20720B",
"5#	c #30D004",
"6#	c #2FCB04",
"7#	c #1B610A",
"8#	c #0F1A0D",
"9#	c #404040",
"0#	c #131513",
"a#	c #3F513B",
"b#	c #6BB957",
"c#	c #75F752",
"d#	c #6EDC50",
"e#	c #4D7A43",
"f#	c #212A20",
"g#	c #808080",
"h#	c #545454",
"i#	c #040D02",
"j#	c #143F0A",
"k#	c #29A807",
"l#	c #2FCA04",
"m#	c #1F760A",
"n#	c #142511",
"o#	c #404240",
"p#	c #E7E7E6",
"q#	c #E6E6E4",
"r#	c #3C3C3C",
"s#	c #151515",
"t#	c #222721",
"u#	c #507348",
"v#	c #6FD654",
"w#	c #72F94D",
"x#	c #64C14B",
"y#	c #3B5836",
"z#	c #161A15",
"A#	c #7D7D7D",
"B#	c #010400",
"C#	c #0B2006",
"D#	c #218109",
"E#	c #2EC903",
"F#	c #249407",
"G#	c #163210",
"H#	c #3A3D3A",
"I#	c #565655",
"J#	c #E5E5E4",
"K#	c #CCCCCC",
"L#	c #C1C1C1",
"M#	c #B4B4B4",
"N#	c #AAABA9",
"O#	c #A8B5A5",
"P#	c #92D084",
"Q#	c #77F555",
"R#	c #73F84E",
"S#	c #84CE72",
"T#	c #93A38F",
"U#	c #8C8D8B",
"V#	c #7E7E7E",
"W#	c #797979",
"X#	c #717171",
"Y#	c #686868",
"Z#	c #636363",
"`#	c #424342",
" $	c #3A4538",
".$	c #337622",
"+$	c #2EBC08",
"@$	c #2DB00A",
"#$	c #2F5F23",
"$$	c #515750",
"%$	c #686867",
"&$	c #E1E1E0",
"*$	c #E3E3E2",
"=$	c #111111",
"-$	c #FBFDFA",
";$	c #EDF2EC",
">$	c #2E342D",
",$	c #575D56",
"'$	c #3F453E",
")$	c #2B322A",
"!$	c #2A3129",
"~$	c #2A3128",
"{$	c #293127",
"]$	c #A9B5A7",
"^$	c #466341",
"/$	c #66C050",
"($	c #6DFA47",
"_$	c #66E644",
":$	c #477D3C",
"<$	c #212A1F",
"[$	c #040403",
"}$	c #040504",
"|$	c #787978",
"1$	c #020302",
"2$	c #030801",
"3$	c #103807",
"4$	c #26A305",
"5$	c #2AB602",
"6$	c #1D6809",
"7$	c #283725",
"8$	c #3F443E",
"9$	c #CFD1CD",
"0$	c #D7D8D5",
"a$	c #323831",
"b$	c #E3F8DE",
"c$	c #CAEFC1",
"d$	c #6A9460",
"e$	c #7AA670",
"f$	c #6C9A62",
"g$	c #619057",
"h$	c #608F54",
"i$	c #5C8C51",
"j$	c #5A8C4E",
"k$	c #96CD89",
"l$	c #5EA44D",
"m$	c #69DF4A",
"n$	c #6AF945",
"o$	c #63C949",
"p$	c #45693D",
"q$	c #2C3B28",
"r$	c #1C2919",
"s$	c #1B2817",
"t$	c #1D2D1A",
"u$	c #7A8B76",
"v$	c #172614",
"w$	c #10190E",
"x$	c #152312",
"y$	c #1A2C15",
"z$	c #1C2C19",
"A$	c #1D291A",
"B$	c #1C2819",
"C$	c #172214",
"D$	c #152013",
"E$	c #515B4E",
"F$	c #111C0F",
"G$	c #111F0D",
"H$	c #15350D",
"I$	c #25850C",
"J$	c #2AB603",
"K$	c #218D05",
"L$	c #1E5111",
"M$	c #2B4D22",
"N$	c #0C2A04",
"O$	c #99B392",
"P$	c #AEBBA9",
"Q$	c #A6A7A5",
"R$	c #51644D",
"S$	c #B4F4A4",
"T$	c #9BF783",
"U$	c #88EC6D",
"V$	c #86EB6B",
"W$	c #81EC65",
"X$	c #7DE860",
"Y$	c #79E85B",
"Z$	c #75E757",
"`$	c #71E651",
" %	c #79F159",
".%	c #6BE84A",
"+%	c #6DF649",
"@%	c #6EF44A",
"#%	c #64BB4E",
"$%	c #4C7E3F",
"%%	c #39652E",
"&%	c #315C26",
"*%	c #2E5822",
"=%	c #2D5921",
"-%	c #73A266",
";%	c #2A591E",
">%	c #2C5821",
",%	c #3A6E2C",
"'%	c #407C30",
")%	c #427634",
"!%	c #38632E",
"~%	c #346029",
"{%	c #325D26",
"]%	c #305C24",
"^%	c #59864D",
"/%	c #2C591F",
"(%	c #2B5A1E",
"_%	c #28571B",
":%	c #265919",
"<%	c #2C7818",
"[%	c #2CAB08",
"}%	c #26A603",
"|%	c #259506",
"1%	c #288E0B",
"2%	c #1E8404",
"3%	c #42A029",
"4%	c #5EA14B",
"5%	c #90998D",
"6%	c #4F624A",
"7%	c #B0F49F",
"8%	c #97F77E",
"9%	c #84EB67",
"0%	c #83EA67",
"a%	c #7DEC60",
"b%	c #77E75A",
"c%	c #75E857",
"d%	c #70E552",
"e%	c #6DE64D",
"f%	c #74F053",
"g%	c #6BE44A",
"h%	c #71EC52",
"i%	c #6ACE51",
"j%	c #569546",
"k%	c #416E35",
"l%	c #336128",
"m%	c #315E24",
"n%	c #305F24",
"o%	c #2E5E21",
"p%	c #BEBCCB",
"q%	c #736F79",
"r%	c #2F362E",
"s%	c #DFF7DA",
"t%	c #C4EEBA",
"u%	c #619056",
"v%	c #71A165",
"w%	c #639558",
"x%	c #5B8E4F",
"y%	c #588D4C",
"z%	c #558A49",
"A%	c #538945",
"B%	c #8DC67F",
"C%	c #538A46",
"D%	c #59914C",
"E%	c #4F7846",
"F%	c #405A3A",
"G%	c #364A31",
"H%	c #34482F",
"I%	c #32472D",
"J%	c #32482D",
"K%	c #30462B",
"L%	c #625B81",
"M%	c #272534",
"N%	c #0F100F",
"O%	c #FAFDF9",
"P%	c #E8EEE5",
"Q%	c #2A3228",
"R%	c #60685F",
"S%	c #4C544A",
"T%	c #424B41",
"U%	c #434C42",
"V%	c #4F584D",
"W%	c #50594E",
"X%	c #ABB5A9",
"Y%	c #4C554A",
"Z%	c #3F473D",
"`%	c #373D36",
" &	c #3B3E3A",
".&	c #515450",
"+&	c #525652",
"@&	c #535652",
"#&	c #CAC8D5",
"$&	c #ECECEA",
"%&	c #555655",
"&&	c #383938",
"*&	c #474847",
"=&	c #626262",
"-&	c #616161",
";&	c #606060",
">&	c #A7A3B8",
",&	c #847F9C",
"'&	c #736D8E",
")&	c #DBDAE2",
"!&	c #9691AB",
"~&	c #EBEBE9",
"{&	c #B8B5C6",
"]&	c #E7E7E5",
"^&	c #E4E4E3",
"/&	c #E4E4E2",
"(&	c #EDECF0",
"_&	c #E5E5E3",
":&	c #E6E6E5",
"<&	c #E3E3E1",
"[&	c #A9A9A7",
"}&	c #A3A3A2",
"|&	c #A3A3A1",
"1&	c #E2E2E0",
"2&	c #E1E1DF",
"3&	c #9A9A9A",
"4&	c #8C8C8C",
"  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . + @                       ",
". # $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ % & * .                   ",
". $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = - % ; > .                 ",
". $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = = = ' ) $ ! ~ .               ",
". $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = = = = = ' { $ $ ; ] ^             ",
". $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = = = = = / / % ( $ / $ _ : <           ",
". $ $ $ $ $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = = = / / / / / / [ } $ | 1 $ 2 3 .         ",
". $ $ $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = = / / / / / 4 1 1 1 5 6 7 8 2 ( 9 0 a .       ",
". $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = / / / / / / 4 1 1 1 1 1 b c d e f g h h i j .       ",
". $ $ $ $ $ $ $ $ = = = = = = = = = = / / / / / 4 1 1 1 1 1 1 b - - k l m > n o p q r s .       ",
". $ $ $ $ $ $ = = = = = = = = / / / / / 1 / / / / 1 1 b - - - - k t t | - ' ' u v 0 w q .       ",
". $ $ . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . x y j .       ",
". $ $ . z A q B C D E F G H e I J K L K M N h h O P Q R P S T U V W U V W X Y Z . m m ` .       ",
". $ = .  .B R Q h S ..+.@.#.$.%.&.$.*.=.-.$.;.>.$.@ *.,.'.>.).;.>.$.;.>.$.!.X X . ~.~.} .       ",
". $ / . {.].U ;.^.. # . . . . . . /.^.^ (._.^ ^ :.^ :.^ ^ :.<.[.^.. . . . }.|.|.. 1.1.} .       ",
". $ / . 2.3.@ ^.. . ) . . . . . :.4.5.6.7.8.9.0.a.b.c.d.e.f.g.h.i._.. . . j.k.k.. l.l.} .       ",
". $ 1 . m.h ^.. . . n.. . . . _.o.p.q.r.s.t.u.v.w.x.y.z.A.B.C.D.E.F._.. . < @.@.. G.G.} .       ",
". $ b . H.V . . . . I.. . . [.J.K.L.M.N.O.P.Q.R.S.T.U.V.W.X.Y.Z.`. +.+/.. ++!.@+. #+$+} .       ",
". $ - . %+k.. . . . &+. . . j.*+=+-+;+>+,+'+R.)+!+~+{+W.]+^+/+(+_+:+<+[+. }+@+W . |+1+} .       ",
". $ 2+. 3+4+. . . . M . . /.5+6+7+8+9+0+a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+. }+q+r+. s+s+} .       ",
". $ k . E t+u+n.& 2 v+9 w+x+y+z+A+B+C+D+E+F+G+H+I+J+g.K+L+M+N+O+P+Q+R+S+T+U+V+W+. 7 X+} .       ",
". $ t . Y+k.. . . . 8 . . < Z+`+ @.@+@@@#@^ :.^ $@^ :.:.:.%@&@*@=@-@;@>@,@'@)@!@. X+X+} .       ",
". $ | . Z ~@. . . . > . . {@]@^@/@(@_@:@<@. . . . . . . . [@}@|@1@2@3@4@5@6@P 7@. 8@8@} .       ",
". $ 9@. 0@#.. . . . a@. [.0.b@c@d@e@f@g@).. . . . . . . . . h@i@j@k@l@m@n@o@s p@. q@r@} .       ",
". $ s@. t@#.. . . . -.. u@v@w@x@y@z@A@++D . . . . . . . . . B@C@D@E@F@G@H@I@J@R . K@L@} .       ",
". $ % . M@!.. . . . N@/.O@P@Q@R@S@T@U@^.p . . . . . . . . . 0@V@W@X@Y@Z@`@ #J@t@. L@L@} .       ",
". $ .#. U++ . . . . z [.+#@###$#%#&#*#. =#. . . . . . . . . U+-#;#>#,#'#)#!#~#R . {#]#} .       ",
". $ x . t@#.. . . . ^#++/#(#_#:#<#[#}#. |#. . . . . . . . . 1#2#3#4#5#6#7#8#9#R . ]#]#} .       ",
". $ c . t@#.. . . . 6 0#a#b#c#d#e#f#[.. g#. . . . . . . . . h#. i#j#k#l#m#n#o#U+. p#q#} .       ",
". $ l . r#s#. . . . n t#u#v#w#x#y#z#/.. A#. . . . . . . . . e . B#C#D#E#F#G#H#I#. q#J#} .       ",
"[@= 1.^ K#L#-.M#(.6 N#O#P#Q#R#S#T#U#W+V#W#Y+X#r Y#Z#3.t@!@Q J )@`# $.$+$@$#$$$%$. &$*$} .       ",
"=$-$;$>$,$'$)$!$~${$]$^$/$($_$:$<$^ [$}$|$1$. . . . . . . . O . . 2$3$4$5$6$7$8$B#9$0$(..       ",
"a$b$c$d$e$f$g$h$i$j$k$l$m$n$o$p$q$r$s$t$u$v$w$x$y$z$A$B$C$D$E$w$F$G$H$I$J$K$L$M$N$O$P$Q$.       ",
"R$S$T$U$V$W$X$Y$Z$`$ %.%+%@%#%$%%%&%*%=%-%;%>%,%'%)%!%~%{%]%^%/%(%_%:%<%[%}%|%1%2%3%4%5%.       ",
"6%7%8%9%0%a%b%c%d%e%f%g%h%i%j%k%l%m%n%o%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%q%",
"r%s%t%u%v%w%x%y%z%A%B%C%D%E%F%G%H%I%J%K%p%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%M%",
"N%O%P%Q%R%S%T%U%V%W%X%Y%Z%`% &.&+&+&@&.&p%L%L%L%#&L%L%#&L%L%#&L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%M%",
"/.= $&^ %&`#&&*&=&-&( ;&`#q+r#h#R 1#R 7@p%L%L%L%>&,&'&)&>&L%#&L%'&!&'&L%,&L%L%,&L%'&,&L%L%L%L%M%",
". $ ~&. . . . . . . . . . . . . . . . . p%L%L%L%!&!&!&!&#&L%#&L%#&!&)&L%#&L%'&{&,&#&>&{&L%L%L%M%",
". $ q@q@q@r@r@K@K@{#]#]#]&q#q#J#^&/&/&/&p%L%L%L%L%#&{&'&#&!&!&L%,&{&(&L%{&,&{&,&#&!&!&)&L%L%L%M%",
". $ r@K@K@K@K@{#]#]#p#p#]&]&q#q#_&_&/&/&p%L%L%L%L%#&#&L%#&{&'&,&#&'&#&L%'&{&#&L%#&!&!&!&L%L%L%M%",
". $ K@K@K@]#p#p#p#p#]&]&]&:&:&q#_&_&_&_&p%L%L%L%L%>&{&L%,&)&L%,&#&>&(&'&L%#&>&L%,&#&!&)&L%L%L%M%",
". $ L@p#p#p#p#]&]&]&:&:&:&q#_&_&_&^&^&/&p%L%L%L%L%'&'&L%L%,&L%L%,&,&'&'&L%,&'&L%L%'&!&L%L%L%L%M%",
". $ p#]&]&]&:&:&:&:&q#q#_&^&^&^&/&/&/&/&p%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%M%",
". $ ]&:&:&:&q#q#q#q#^&^&/&/&/&/&/&/&/&/&q%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%",
". $ :&q#q#q#q#J#^&/&/&/&/&/&/&/&/&/&/&<&<&[&: }&}&}&}&|&|&|&        |&|&|&|&|&|&|&|&|&D . . .   ",
". # q#q#q#_&/&/&/&/&/&/&/&/&/&/&*$<&<&<&<&<&<&<&<&1&1&1&1&2&2&2&2&2&2&2&2&2&2&2&2&2&2&3&.       ",
". 9 } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } 3&4&.       ",
"  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .       ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                "},//wave_info_pixmaps_c_1 292
{NULL},//wave_info_pixmap 293
{NULL},//wave_info_mask 294
{ 
"48 52 677 2",
"  	c None",
". 	c #000000",
"+ 	c #232323",
"@ 	c #1F1F1F",
"# 	c #DADADA",
"$ 	c #FFFFFF",
"% 	c #F6F6F6",
"& 	c #CFCFCF",
"* 	c #707070",
"= 	c #FEFEFE",
"- 	c #FBFBFB",
"; 	c #EDEDED",
"> 	c #C0C0C0",
", 	c #FF0000",
"' 	c #F7F7F7",
") 	c #D7D7D7",
"! 	c #F1F1F1",
"~ 	c #B7B7B7",
"{ 	c #D2D2D2",
"] 	c #AEAEAE",
"^ 	c #070707",
"/ 	c #FDFDFD",
"( 	c #B8B8B8",
"_ 	c #DCDCDC",
": 	c #A4A4A3",
"< 	c #0E0E0E",
"[ 	c #F5F5F4",
"} 	c #B5B5B5",
"| 	c #F9F9F9",
"1 	c #FCFCFC",
"2 	c #CBCBCB",
"3 	c #A5A5A5",
"4 	c #FDFDFC",
"5 	c #F4F4F4",
"6 	c #ACACAC",
"7 	c #ECECEC",
"8 	c #C3C3C3",
"9 	c #C2C2C2",
"0 	c #C9C9C8",
"a 	c #A8A8A8",
"b 	c #FCFCFB",
"c 	c #F4F4F3",
"d 	c #ABABAB",
"e 	c #515151",
"f 	c #474747",
"g 	c #464645",
"h 	c #464646",
"i 	c #5D5D5C",
"j 	c #A9A9A9",
"k 	c #FAFAFA",
"l 	c #F2F2F2",
"m 	c #F3F3F3",
"n 	c #AAAAAA",
"o 	c #A0A09F",
"p 	c #8A8A8A",
"q 	c #7A7A7A",
"r 	c #6C6C6C",
"s 	c #454545",
"t 	c #FAFAF9",
"u 	c #E7E7E7",
"v 	c #C6C6C5",
"w 	c #BABAB9",
"x 	c #F5F5F5",
"y 	c #EDEDEB",
"z 	c #B2B2B2",
"A 	c #949494",
"B 	c #7F7F7F",
"C 	c #8B8B8B",
"D 	c #8D8D8D",
"E 	c #EFEFEF",
"F 	c #737373",
"G 	c #727272",
"H 	c #666666",
"I 	c #4F4F4F",
"J 	c #4E4E4E",
"K 	c #4D4D4D",
"L 	c #505050",
"M 	c #CACACA",
"N 	c #4B4B4B",
"O 	c #4A4A4A",
"P 	c #4C4C4C",
"Q 	c #525252",
"R 	c #575757",
"S 	c #373737",
"T 	c #A1A1A1",
"U 	c #363636",
"V 	c #333333",
"W 	c #353535",
"X 	c #3F3F3F",
"Y 	c #5C5C5C",
"Z 	c #6B6B6B",
"` 	c #B1B1B1",
" .	c #A6A6A6",
"..	c #E4E4E4",
"+.	c #2E2E2E",
"@.	c #2D2D2D",
"#.	c #252525",
"$.	c #191919",
"%.	c #1B1B1B",
"&.	c #1C1C1C",
"*.	c #1E1E1E",
"=.	c #212121",
"-.	c #B9B9B9",
";.	c #1D1D1D",
">.	c #171717",
",.	c #242424",
"'.	c #222222",
").	c #919191",
"!.	c #292929",
"~.	c #F3F3F2",
"{.	c #9C9C9C",
"].	c #696969",
"^.	c #030303",
"/.	c #010101",
"(.	c #B0B0B0",
"_.	c #080808",
":.	c #060606",
"<.	c #868686",
"[.	c #040404",
"}.	c #0C0C0C",
"|.	c #2A2A2A",
"1.	c #F2F2F1",
"2.	c #979797",
"3.	c #5F5F5F",
"4.	c #131413",
"5.	c #222421",
"6.	c #2B302A",
"7.	c #B4BAB3",
"8.	c #2B312A",
"9.	c #2A3029",
"0.	c #292E28",
"a.	c #282D26",
"b.	c #272D26",
"c.	c #262C25",
"d.	c #252C24",
"e.	c #242B23",
"f.	c #242B22",
"g.	c #8B9289",
"h.	c #1E241D",
"i.	c #151814",
"j.	c #101010",
"k.	c #303030",
"l.	c #F1F1F0",
"m.	c #8E8E8E",
"n.	c #D4D4D4",
"o.	c #272A26",
"p.	c #495546",
"q.	c #64825E",
"r.	c #6B9063",
"s.	c #ABD3A2",
"t.	c #658D5C",
"u.	c #628C59",
"v.	c #608A56",
"w.	c #5E8B54",
"x.	c #598550",
"y.	c #57864D",
"z.	c #54844A",
"A.	c #518046",
"B.	c #4F8143",
"C.	c #84B779",
"D.	c #46773C",
"E.	c #344E2F",
"F.	c #1C241A",
"G.	c #F0F0F0",
"H.	c #848484",
"I.	c #D0D0D0",
"J.	c #262825",
"K.	c #5A6E56",
"L.	c #86C476",
"M.	c #8FE878",
"N.	c #8DEC74",
"O.	c #93F479",
"P.	c #84E66A",
"Q.	c #81E665",
"R.	c #7DE661",
"S.	c #7AE75C",
"T.	c #75E259",
"U.	c #72E653",
"V.	c #6EE14F",
"W.	c #6BE14A",
"X.	c #67E446",
"Y.	c #6FEC4D",
"Z.	c #60E53D",
"`.	c #53B13A",
" +	c #32522B",
".+	c #141A13",
"++	c #0B0B0B",
"@+	c #2B2B2B",
"#+	c #EFEFEE",
"$+	c #EFEFED",
"%+	c #777777",
"&+	c #CDCDCD",
"*+	c #414B3F",
"=+	c #7FB372",
"-+	c #93F778",
";+	c #90F974",
">+	c #8AF06F",
",+	c #90F474",
"'+	c #81E666",
")+	c #79E55C",
"!+	c #76E758",
"~+	c #71E153",
"{+	c #6FE64E",
"]+	c #67E145",
"^+	c #63E441",
"/+	c #69EE46",
"(+	c #60F537",
"_+	c #5AF032",
":+	c #449030",
"<+	c #20301D",
"[+	c #080908",
"}+	c #0A0A0A",
"|+	c #EEEEED",
"1+	c #EEEEEC",
"2+	c #FBFBFA",
"3+	c #747474",
"4+	c #313131",
"5+	c #1C1E1C",
"6+	c #586D53",
"7+	c #8AD676",
"8+	c #90FB73",
"9+	c #87E66E",
"0+	c #74B065",
"a+	c #9FD095",
"b+	c #5E8B53",
"c+	c #5A8850",
"d+	c #57864C",
"e+	c #548649",
"f+	c #508145",
"g+	c #4E8242",
"h+	c #4B7F3F",
"i+	c #487E3C",
"j+	c #47823A",
"k+	c #73C060",
"l+	c #56DA34",
"m+	c #58F72C",
"n+	c #4BBE2D",
"o+	c #284821",
"p+	c #0F140E",
"q+	c #343434",
"r+	c #414141",
"s+	c #EDEDEC",
"t+	c #E2E2E2",
"u+	c #D8D8D8",
"v+	c #C6C6C6",
"w+	c #BEBEBE",
"x+	c #BABABA",
"y+	c #BDC1BC",
"z+	c #B0D6A8",
"A+	c #96F57C",
"B+	c #8FFA72",
"C+	c #9AE089",
"D+	c #A7C0A2",
"E+	c #A6B0A3",
"F+	c #A0A79F",
"G+	c #9CA39B",
"H+	c #989E96",
"I+	c #949B92",
"J+	c #90978E",
"K+	c #878F85",
"L+	c #828B80",
"M+	c #7E8C7B",
"N+	c #799972",
"O+	c #67C750",
"P+	c #56F62C",
"Q+	c #56E92E",
"R+	c #619E53",
"S+	c #606C5E",
"T+	c #565756",
"U+	c #5A5A5A",
"V+	c #7B7B7B",
"W+	c #828282",
"X+	c #ECECEB",
"Y+	c #757575",
"Z+	c #3B4439",
"`+	c #77B069",
" @	c #8AF66D",
".@	c #84EC68",
"+@	c #65965A",
"@@	c #313C2F",
"#@	c #9C9D9C",
"$@	c #050505",
"%@	c #0D0F0C",
"&@	c #758672",
"*@	c #3D832D",
"=@	c #4DE625",
"-@	c #4AF21E",
";@	c #369320",
">@	c #182E13",
",@	c #050704",
"'@	c #121212",
")@	c #494949",
"!@	c #565656",
"~@	c #2C2C2C",
"{@	c #171917",
"]@	c #4D6149",
"^@	c #7FCE6C",
"/@	c #87FB68",
"(@	c #7DDB65",
"_@	c #53744C",
":@	c #1F241E",
"<@	c #959595",
"[@	c #020202",
"}@	c #70786E",
"|@	c #2E5B24",
"1@	c #45CA22",
"2@	c #48F719",
"3@	c #39B719",
"4@	c #1A3E12",
"5@	c #050C04",
"6@	c #141514",
"7@	c #595959",
"8@	c #EBEBEA",
"9@	c #F8F8F8",
"0@	c #5E5E5E",
"a@	c #BCBCBC",
"b@	c #638A5A",
"c@	c #82E867",
"d@	c #83FA62",
"e@	c #72C15E",
"f@	c #40543C",
"g@	c #151714",
"h@	c #6B6E6A",
"i@	c #203B1B",
"j@	c #39A51E",
"k@	c #42F112",
"l@	c #3AD410",
"m@	c #1E5C10",
"n@	c #061103",
"o@	c #111211",
"p@	c #555555",
"q@	c #EAEAE9",
"r@	c #EAEAE8",
"s@	c #F8F8F7",
"t@	c #5B5B5B",
"u@	c #0D0D0D",
"v@	c #3A4638",
"w@	c #71AE62",
"x@	c #81F563",
"y@	c #7DF15E",
"z@	c #64A155",
"A@	c #313D2E",
"B@	c #646664",
"C@	c #162813",
"D@	c #2C7919",
"E@	c #3ADE0E",
"F@	c #36DF09",
"G@	c #21780D",
"H@	c #091A05",
"I@	c #0E0F0D",
"J@	c #444444",
"K@	c #E9E9E8",
"L@	c #E9E9E7",
"M@	c #5D5D5D",
"N@	c #B6B6B6",
"O@	c #1A1C19",
"P@	c #4B6146",
"Q@	c #75C262",
"R@	c #7EF65D",
"S@	c #75DA5A",
"T@	c #517949",
"U@	c #212720",
"V@	c #0C160A",
"W@	c #1E5213",
"X@	c #30BA0C",
"Y@	c #33DD04",
"Z@	c #25890C",
"`@	c #0C2507",
" #	c #10130F",
".#	c #F6F6F5",
"+#	c #262B25",
"@#	c #5A8052",
"##	c #79E05E",
"$#	c #7BF957",
"%#	c #6BC554",
"&#	c #3E5639",
"*#	c #131613",
"=#	c #878787",
"-#	c #050A04",
";#	c #13350B",
">#	c #299E0B",
",#	c #32DA04",
"'#	c #2BB208",
")#	c #133D0A",
"!#	c #0F160E",
"~#	c #3E3E3E",
"{#	c #E8E8E7",
"]#	c #E8E8E6",
"^#	c #AFAFAF",
"/#	c #323D30",
"(#	c #66A257",
"_#	c #7AF458",
":#	c #75F553",
"<#	c #5EA44E",
"[#	c #2F3D2C",
"}#	c #0A0B0A",
"|#	c #838383",
"1#	c #585858",
"2#	c #010201",
"3#	c #091D06",
"4#	c #20720B",
"5#	c #30D004",
"6#	c #2FCB04",
"7#	c #1B610A",
"8#	c #0F1A0D",
"9#	c #404040",
"0#	c #131513",
"a#	c #3F513B",
"b#	c #6BB957",
"c#	c #75F752",
"d#	c #6EDC50",
"e#	c #4D7A43",
"f#	c #212A20",
"g#	c #808080",
"h#	c #545454",
"i#	c #040D02",
"j#	c #143F0A",
"k#	c #29A807",
"l#	c #2FCA04",
"m#	c #1F760A",
"n#	c #142511",
"o#	c #404240",
"p#	c #E7E7E6",
"q#	c #E6E6E4",
"r#	c #3C3C3C",
"s#	c #151515",
"t#	c #222721",
"u#	c #507348",
"v#	c #6FD654",
"w#	c #72F94D",
"x#	c #64C14B",
"y#	c #3B5836",
"z#	c #161A15",
"A#	c #7D7D7D",
"B#	c #010400",
"C#	c #0B2006",
"D#	c #218109",
"E#	c #2EC903",
"F#	c #249407",
"G#	c #163210",
"H#	c #3A3D3A",
"I#	c #565655",
"J#	c #E5E5E4",
"K#	c #CCCCCC",
"L#	c #C1C1C1",
"M#	c #B4B4B4",
"N#	c #AAABA9",
"O#	c #A8B5A5",
"P#	c #92D084",
"Q#	c #77F555",
"R#	c #73F84E",
"S#	c #84CE72",
"T#	c #93A38F",
"U#	c #8C8D8B",
"V#	c #7E7E7E",
"W#	c #797979",
"X#	c #717171",
"Y#	c #686868",
"Z#	c #636363",
"`#	c #424342",
" $	c #3A4538",
".$	c #337622",
"+$	c #2EBC08",
"@$	c #2DB00A",
"#$	c #2F5F23",
"$$	c #515750",
"%$	c #686867",
"&$	c #E1E1E0",
"*$	c #E3E3E2",
"=$	c #111111",
"-$	c #FBFDFA",
";$	c #EDF2EC",
">$	c #2E342D",
",$	c #575D56",
"'$	c #3F453E",
")$	c #2B322A",
"!$	c #2A3129",
"~$	c #2A3128",
"{$	c #293127",
"]$	c #A9B5A7",
"^$	c #466341",
"/$	c #66C050",
"($	c #6DFA47",
"_$	c #66E644",
":$	c #477D3C",
"<$	c #212A1F",
"[$	c #040403",
"}$	c #040504",
"|$	c #787978",
"1$	c #020302",
"2$	c #030801",
"3$	c #103807",
"4$	c #26A305",
"5$	c #2AB602",
"6$	c #1D6809",
"7$	c #283725",
"8$	c #3F443E",
"9$	c #CFD1CD",
"0$	c #D7D8D5",
"a$	c #323831",
"b$	c #E3F8DE",
"c$	c #CAEFC1",
"d$	c #6A9460",
"e$	c #7AA670",
"f$	c #6C9A62",
"g$	c #619057",
"h$	c #608F54",
"i$	c #5C8C51",
"j$	c #5A8C4E",
"k$	c #96CD89",
"l$	c #5EA44D",
"m$	c #69DF4A",
"n$	c #6AF945",
"o$	c #63C949",
"p$	c #45693D",
"q$	c #2C3B28",
"r$	c #1C2919",
"s$	c #1B2817",
"t$	c #1D2D1A",
"u$	c #7A8B76",
"v$	c #172614",
"w$	c #10190E",
"x$	c #152312",
"y$	c #1A2C15",
"z$	c #1C2C19",
"A$	c #1D291A",
"B$	c #1C2819",
"C$	c #172214",
"D$	c #152013",
"E$	c #515B4E",
"F$	c #111C0F",
"G$	c #111F0D",
"H$	c #15350D",
"I$	c #25850C",
"J$	c #2AB603",
"K$	c #218D05",
"L$	c #1E5111",
"M$	c #2B4D22",
"N$	c #0C2A04",
"O$	c #99B392",
"P$	c #AEBBA9",
"Q$	c #A6A7A5",
"R$	c #51644D",
"S$	c #B4F4A4",
"T$	c #9BF783",
"U$	c #88EC6D",
"V$	c #86EB6B",
"W$	c #81EC65",
"X$	c #7DE860",
"Y$	c #79E85B",
"Z$	c #75E757",
"`$	c #71E651",
" %	c #79F159",
".%	c #6BE84A",
"+%	c #6DF649",
"@%	c #6EF44A",
"#%	c #64BB4E",
"$%	c #4C7E3F",
"%%	c #39652E",
"&%	c #315C26",
"*%	c #2E5822",
"=%	c #2D5921",
"-%	c #73A266",
";%	c #2A591E",
">%	c #2C5821",
",%	c #3A6E2C",
"'%	c #407C30",
")%	c #427634",
"!%	c #38632E",
"~%	c #346029",
"{%	c #325D26",
"]%	c #305C24",
"^%	c #59864D",
"/%	c #2C591F",
"(%	c #2B5A1E",
"_%	c #28571B",
":%	c #265919",
"<%	c #2C7818",
"[%	c #2CAB08",
"}%	c #26A603",
"|%	c #259506",
"1%	c #288E0B",
"2%	c #1E8404",
"3%	c #42A029",
"4%	c #5EA14B",
"5%	c #90998D",
"6%	c #4F624A",
"7%	c #B0F49F",
"8%	c #97F77E",
"9%	c #84EB67",
"0%	c #83EA67",
"a%	c #7DEC60",
"b%	c #77E75A",
"c%	c #75E857",
"d%	c #70E552",
"e%	c #6DE64D",
"f%	c #74F053",
"g%	c #6BE44A",
"h%	c #71EC52",
"i%	c #6ACE51",
"j%	c #569546",
"k%	c #416E35",
"l%	c #336128",
"m%	c #315E24",
"n%	c #305F24",
"o%	c #2E5E21",
"p%	c #BEBCCB",
"q%	c #736F79",
"r%	c #2F362E",
"s%	c #DFF7DA",
"t%	c #C4EEBA",
"u%	c #619056",
"v%	c #71A165",
"w%	c #639558",
"x%	c #5B8E4F",
"y%	c #588D4C",
"z%	c #558A49",
"A%	c #538945",
"B%	c #8DC67F",
"C%	c #538A46",
"D%	c #59914C",
"E%	c #4F7846",
"F%	c #405A3A",
"G%	c #364A31",
"H%	c #34482F",
"I%	c #32472D",
"J%	c #32482D",
"K%	c #30462B",
"L%	c #625B81",
"M%	c #272534",
"N%	c #0F100F",
"O%	c #FAFDF9",
"P%	c #E8EEE5",
"Q%	c #2A3228",
"R%	c #60685F",
"S%	c #4C544A",
"T%	c #424B41",
"U%	c #434C42",
"V%	c #4F584D",
"W%	c #50594E",
"X%	c #ABB5A9",
"Y%	c #4C554A",
"Z%	c #3F473D",
"`%	c #373D36",
" &	c #3B3E3A",
".&	c #515450",
"+&	c #525652",
"@&	c #535652",
"#&	c #CAC8D5",
"$&	c #ECECEA",
"%&	c #555655",
"&&	c #383938",
"*&	c #474847",
"=&	c #626262",
"-&	c #616161",
";&	c #606060",
">&	c #A7A3B8",
",&	c #847F9C",
"'&	c #736D8E",
")&	c #DBDAE2",
"!&	c #9691AB",
"~&	c #EBEBE9",
"{&	c #B8B5C6",
"]&	c #E7E7E5",
"^&	c #E4E4E3",
"/&	c #E4E4E2",
"(&	c #EDECF0",
"_&	c #E5E5E3",
":&	c #E6E6E5",
"<&	c #E3E3E1",
"[&	c #A9A9A7",
"}&	c #A3A3A2",
"|&	c #A3A3A1",
"1&	c #E2E2E0",
"2&	c #E1E1DF",
"3&	c #9A9A9A",
"4&	c #8C8C8C",
"  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . + @                       ",
". # $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ % & * .                   ",
". $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = - % ; > .                 ",
". $ $ $ , $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = = = ' ) $ ! ~ .               ",
". $ $ $ , $ $ $ $ $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = = = = = ' { $ $ ; ] ^             ",
". $ $ $ , $ $ $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = = = = = / / % ( $ / $ _ : <           ",
". $ $ $ , $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = = = / / / / / / [ } $ | 1 $ 2 3 .         ",
". $ $ $ $ $ $ $ $ $ $ $ $ = = = = = = = = = = = = = / / / / / 4 1 1 1 5 6 7 8 2 ( 9 0 a .       ",
". $ $ $ , $ $ $ $ $ $ = = = = = = = = = = = / / / / / / 4 1 1 1 1 1 b c d e f g h h i j .       ",
". $ $ $ $ $ $ $ $ = = = = = = = = = = / / / / / 4 1 1 1 1 1 1 b - - k l m > n o p q r s .       ",
". $ $ $ $ $ $ = = = = = = = = / / / / / 1 / / / / 1 1 b - - - - k t t | - ' ' u v 0 w q .       ",
". $ $ . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . x y j .       ",
". $ $ . z A q B C D E F G H e I J K L K M N h h O P Q R P S T U V W U V W X Y Z . m m ` .       ",
". $ = .  .B R Q h S ..+.@.#.$.%.&.$.*.=.-.$.;.>.$.@ *.,.'.>.).;.>.$.;.>.$.!.X X . ~.~.} .       ",
". $ / . {.].U ;.^.. # . . . . . . /.^.^ (._.^ ^ :.^ :.^ ^ :.<.[.^.. . . . }.|.|.. 1.1.} .       ",
". $ / . 2.3.@ ^.. . ) . . . . . :.4.5.6.7.8.9.0.a.b.c.d.e.f.g.h.i._.. . . j.k.k.. l.l.} .       ",
". $ 1 . m.h ^.. . . n.. . . . _.o.p.q.r.s.t.u.v.w.x.y.z.A.B.C.D.E.F._.. . < @.@.. G.G.} .       ",
". $ b . H.V . . . . I.. . . [.J.K.L.M.N.O.P.Q.R.S.T.U.V.W.X.Y.Z.`. +.+/.. ++!.@+. #+$+} .       ",
". $ - . %+k.. . . . &+. . . j.*+=+-+;+>+,+'+R.)+!+~+{+W.]+^+/+(+_+:+<+[+. }+@+W . |+1+} .       ",
". $ 2+. 3+4+. . . . M . . /.5+6+7+8+9+0+a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+. }+q+r+. s+s+} .       ",
". $ k . E t+u+n.& 2 v+9 w+x+y+z+A+B+C+D+E+F+G+H+I+J+g.K+L+M+N+O+P+Q+R+S+T+U+V+W+. 7 X+} .       ",
". $ t . Y+k.. . . . 8 . . < Z+`+ @.@+@@@#@^ :.^ $@^ :.:.:.%@&@*@=@-@;@>@,@'@)@!@. X+X+} .       ",
". $ | . Z ~@. . . . > . . {@]@^@/@(@_@:@<@. . . . . . . . [@}@|@1@2@3@4@5@6@P 7@. 8@8@} .       ",
". $ 9@. 0@#.. . . . a@. [.0.b@c@d@e@f@g@).. . . . . . . . . h@i@j@k@l@m@n@o@s p@. q@r@} .       ",
". $ s@. t@#.. . . . -.. u@v@w@x@y@z@A@++D . . . . . . . . . B@C@D@E@F@G@H@I@J@R . K@L@} .       ",
". $ % . M@!.. . . . N@/.O@P@Q@R@S@T@U@^.p . . . . . . . . . 0@V@W@X@Y@Z@`@ #J@t@. L@L@} .       ",
". $ .#. U++ . . . . z [.+#@###$#%#&#*#. =#. . . . . . . . . U+-#;#>#,#'#)#!#~#R . {#]#} .       ",
". $ x . t@#.. . . . ^#++/#(#_#:#<#[#}#. |#. . . . . . . . . 1#2#3#4#5#6#7#8#9#R . ]#]#} .       ",
". $ c . t@#.. . . . 6 0#a#b#c#d#e#f#[.. g#. . . . . . . . . h#. i#j#k#l#m#n#o#U+. p#q#} .       ",
". $ l . r#s#. . . . n t#u#v#w#x#y#z#/.. A#. . . . . . . . . e . B#C#D#E#F#G#H#I#. q#J#} .       ",
"[@= 1.^ K#L#-.M#(.6 N#O#P#Q#R#S#T#U#W+V#W#Y+X#r Y#Z#3.t@!@Q J )@`# $.$+$@$#$$$%$. &$*$} .       ",
"=$-$;$>$,$'$)$!$~${$]$^$/$($_$:$<$^ [$}$|$1$. . . . . . . . O . . 2$3$4$5$6$7$8$B#9$0$(..       ",
"a$b$c$d$e$f$g$h$i$j$k$l$m$n$o$p$q$r$s$t$u$v$w$x$y$z$A$B$C$D$E$w$F$G$H$I$J$K$L$M$N$O$P$Q$.       ",
"R$S$T$U$V$W$X$Y$Z$`$ %.%+%@%#%$%%%&%*%=%-%;%>%,%'%)%!%~%{%]%^%/%(%_%:%<%[%}%|%1%2%3%4%5%.       ",
"6%7%8%9%0%a%b%c%d%e%f%g%h%i%j%k%l%m%n%o%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%p%q%",
"r%s%t%u%v%w%x%y%z%A%B%C%D%E%F%G%H%I%J%K%p%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%M%",
"N%O%P%Q%R%S%T%U%V%W%X%Y%Z%`% &.&+&+&@&.&p%L%L%L%#&L%L%#&L%L%#&L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%M%",
"/.= $&^ %&`#&&*&=&-&( ;&`#q+r#h#R 1#R 7@p%L%L%L%>&,&'&)&>&L%#&L%'&!&'&L%,&L%L%,&L%'&,&L%L%L%L%M%",
". $ ~&. . . . . . . . . . . . . . . . . p%L%L%L%!&!&!&!&#&L%#&L%#&!&)&L%#&L%'&{&,&#&>&{&L%L%L%M%",
". $ q@q@q@r@r@K@K@{#]#]#]&q#q#J#^&/&/&/&p%L%L%L%L%#&{&'&#&!&!&L%,&{&(&L%{&,&{&,&#&!&!&)&L%L%L%M%",
". $ r@K@K@K@K@{#]#]#p#p#]&]&q#q#_&_&/&/&p%L%L%L%L%#&#&L%#&{&'&,&#&'&#&L%'&{&#&L%#&!&!&!&L%L%L%M%",
". $ K@K@K@]#p#p#p#p#]&]&]&:&:&q#_&_&_&_&p%L%L%L%L%>&{&L%,&)&L%,&#&>&(&'&L%#&>&L%,&#&!&)&L%L%L%M%",
". $ L@p#p#p#p#]&]&]&:&:&:&q#_&_&_&^&^&/&p%L%L%L%L%'&'&L%L%,&L%L%,&,&'&'&L%,&'&L%L%'&!&L%L%L%L%M%",
". $ p#]&]&]&:&:&:&:&q#q#_&^&^&^&/&/&/&/&p%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%L%M%",
". $ ]&:&:&:&q#q#q#q#^&^&/&/&/&/&/&/&/&/&q%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%M%",
". $ :&q#q#q#q#J#^&/&/&/&/&/&/&/&/&/&/&<&<&[&: }&}&}&}&|&|&|&        |&|&|&|&|&|&|&|&|&D . . .   ",
". # q#q#q#_&/&/&/&/&/&/&/&/&/&/&*$<&<&<&<&<&<&<&<&1&1&1&1&2&2&2&2&2&2&2&2&2&2&2&2&2&2&3&.       ",
". 9 } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } 3&4&.       ",
"  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .       ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                "},//wave_alert_pixmaps_c_1 295
{NULL},//wave_alert_pixmap 296
{NULL},//wave_alert_mask 297
{ 72},//inch_print_c_1 298
{ 1.0},//ps_chwidth_print_c_1 299
{0},//ybound_print_c_1 300
{0},//pr_signal_fill_width_print_c_1 301
{0},//ps_nummaxchars_print_c_1 302
{ 1},//ps_fullpage 303
{ 66},//ps_maxveclen 304
{ 0},//liney_max 305
{ 
				    ps_header,
				    ps_trailer,
				    ps_signal_init,
				    ps_setgray,
				    ps_draw_line,
				    ps_draw_box,
				    ps_draw_string
},//ps_print_device 306
{ 
				     mif_header,
				     mif_trailer,
				     mif_signal_init,
				     mif_setgray,
				     mif_draw_line,
				     mif_draw_box,
				     mif_draw_string
},//mif_print_device 307
{ 0},//current_translate_proc 308
{ 0},//current_filter_ptranslate_c_1 309
{ 0},//num_proc_filters 310
{},//procsel_filter 311
{},//proc_filter 312
{0},//is_active_ptranslate_c_2 313
{ NULL},//fcurr_ptranslate_c_1 314
{NULL},//window_ptranslate_c_5 315
{NULL},//clist_ptranslate_c_2 316
{".gtkwaverc"},//rcname_rc_c_1 317
{0},//rc_line_no 318
{ 1},//possibly_use_rc_defaults 319
{
{ "accel", f_accel },
{ "alt_hier_delimeter", f_alt_hier_delimeter },
{ "append_vcd_hier", f_append_vcd_hier },
{ "atomic_vectors", f_atomic_vectors },
{ "autocoalesce", f_autocoalesce },
{ "autocoalesce_reversal", f_autocoalesce_reversal },
{ "autoname_bundles", f_autoname_bundles },
{ "color_0", f_color_0 },
{ "color_1", f_color_1 },
{ "color_back", f_color_back },
{ "color_baseline", f_color_baseline },
{ "color_black", f_color_black },
{ "color_dash", f_color_dash },
{ "color_dashfill", f_color_dashfill },
{ "color_dkblue", f_color_dkblue },
{ "color_dkgray", f_color_dkgray },
{ "color_grid", f_color_grid },
{ "color_high", f_color_high },
{ "color_low", f_color_low },
{ "color_ltgray", f_color_ltgray },
{ "color_mark", f_color_mark },
{ "color_mdgray", f_color_mdgray },
{ "color_mid", f_color_mid },
{ "color_normal", f_color_normal },
{ "color_time", f_color_time },
{ "color_timeb", f_color_timeb },
{ "color_trans", f_color_trans },
{ "color_u", f_color_u },
{ "color_ufill", f_color_ufill },
{ "color_umark", f_color_umark },
{ "color_value", f_color_value },
{ "color_vbox", f_color_vbox },
{ "color_vtrans", f_color_vtrans },
{ "color_w", f_color_w },
{ "color_wfill", f_color_wfill },
{ "color_white", f_color_white },
{ "color_x", f_color_x },
{ "color_xfill", f_color_xfill },
{ "constant_marker_update", f_constant_marker_update },
{ "convert_to_reals", f_convert_to_reals },
{ "cursor_snap", f_cursor_snap },
{ "disable_mouseover", f_disable_mouseover },
{ "disable_tooltips", f_disable_tooltips },
{ "do_initial_zoom_fit", f_do_initial_zoom_fit },
{ "dynamic_resizing", f_dynamic_resizing },
{ "enable_fast_exit", f_enable_fast_exit },
{ "enable_ghost_marker", f_enable_ghost_marker },
{ "enable_horiz_grid", f_enable_horiz_grid }, 
{ "enable_vcd_autosave", f_enable_vcd_autosave },
{ "enable_vert_grid", f_enable_vert_grid }, 
{ "fontname_logfile", f_fontname_logfile }, 
{ "fontname_signals", f_fontname_signals }, 
{ "fontname_waves", f_fontname_waves }, 
{ "force_toolbars", f_force_toolbars }, 
{ "hide_sst", f_hide_sst },
{ "hier_delimeter", f_hier_delimeter },
{ "hier_grouping", f_hier_grouping },
{ "hier_max_level", f_hier_max_level },
{ "hpane_pack", f_hpane_pack },
{ "ignore_savefile_pos", f_ignore_savefile_pos },
{ "ignore_savefile_size", f_ignore_savefile_size },
{ "initial_window_x", f_initial_window_x },
{ "initial_window_xpos", f_initial_window_xpos },
{ "initial_window_y", f_initial_window_y },
{ "initial_window_ypos", f_initial_window_ypos },
{ "left_justify_sigs", f_left_justify_sigs },
{ "lxt_clock_compress_to_z", f_lxt_clock_compress_to_z },
{ "page_divisor", f_page_divisor },
{ "ps_maxveclen", f_ps_maxveclen },
{ "show_base_symbols", f_show_base_symbols },
{ "show_grid", f_show_grid },
{ "splash_disable", f_splash_disable },
{ "sst_expanded", f_sst_expanded },
{ "use_big_fonts", f_use_big_fonts },
{ "use_frequency_display", f_use_frequency_display },
{ "use_full_precision", f_use_full_precision },
{ "use_maxtime_display", f_use_maxtime_display },
{ "use_nonprop_fonts", f_use_nonprop_fonts },
{ "use_roundcaps", f_use_roundcaps },
{ "use_scrollbar_only", f_use_scrollbar_only },
{ "vcd_explicit_zero_subscripts", f_vcd_explicit_zero_subscripts },
{ "vcd_preserve_glitches", f_vcd_preserve_glitches },
{ "vcd_warning_filesize", f_vcd_warning_filesize },
{ "vector_padding", f_vector_padding },
{ "vlist_compression", f_vlist_compression },
{ "wave_scrolling", f_wave_scrolling },
{ "zoom_base", f_zoom_base },
{ "zoom_center", f_zoom_center },
{ "zoom_pow10_snap", f_zoom_pow10_snap }
},//rcitems_rc_c_1 320
{},//preg_regex_c_1 321 XXX
{0,0,0},//regex_ok_regex_c_1 322
{0},//is_active_renderopt_c_3 323
{0},//window_renderopt_c_6 324
{NULL},//filesel_print_ps_renderopt_c_1 325
{NULL},//filesel_print_mif_renderopt_c_1 326
{"PS", "MIF"},//render_targets_renderopt_c_1 327
{0,0},//target_mutex_renderopt_c_1 328
{"Letter (8.5\" x 11\")", "A4 (11.68\" x 8.26\")", "Legal (14\" x 8.5\")", "Letter Prop (6.57\" x 8.5\")", "A4 Prop (8.26\" x 5.84\")"},//page_size_renderopt_c_1 329
{0,0,0,0,0},//page_mutex_renderopt_c_1 330
{"Full", "Minimal"},//render_type_renderopt_c_1 331
{0,0},//render_mutex_renderopt_c_1 332
{0},//page_size_type_renderopt_c_1 333
{11.00, 11.68, 14.00, 8.50, 8.26},//px_renderopt_c_1 334
{ 8.50,  8.26,  8.50, 6.57, 5.84},//py_renderopt_c_1 335
{NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, 
 NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, 
 NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,  NULL,NULL,NULL,NULL,},// rgb_contexts
{NULL},//window1_search_c_2 340
{NULL},//entry_a_search_c_1 341
{NULL},//entrybox_text_local_search_c_2 342
{NULL},//cleanup_e_search_c_2 343
{NULL},//pdata 344
{0},//is_active_search_c_4 345
{ 0},//is_insert_running_search_c_1 346
{ 0},//is_replace_running_search_c_1 347
{ 0},//is_append_running_search_c_1 348
{ 0},//is_searching_running_search_c_1 349
{"\\(\\[.*\\]\\)*$", 
"\\>.\\([0-9]\\)*$", 
"\\(\\[.*\\]\\)*$", 
"\\>.\\([0-9]\\)*$", 
""},//regex_type_search_c_1 350
{"WRange", "WStrand", "Range", "Strand", "None"},//regex_name_search_c_1 351
{0,0,0,0,0},//regex_mutex_search_c_1 352
{0},//regex_which_search_c_1 353
{NULL},//window_search_c_7 354
{NULL},//entry_search_c_3 355
{NULL},//clist_search_c_3 356
{ 0},//default_null_searchbox_text_search_c_1 357
{&GLOBALS.default_null_searchbox_text_search_c_1},//searchbox_text_search_c_1 358
{0},//bundle_direction_search_c_2 359
{NULL},//cleanup_search_c_5 360
{0},//num_rows_search_c_2 361
{0},//selected_rows_search_c_2 362
{NULL},//button1_showchange_c_1 363
{NULL},//button1_showchange_c_2 363
{NULL},//button1_showchange_c_3 363
{NULL},//button1_showchange_c_4 363
{NULL},//button1_showchange_c_5 363
{NULL},//button1_showchange_c_6 363
{NULL},//toggle1_showchange_c_1 364
{NULL},//toggle2_showchange_c_1 364
{NULL},//toggle3_showchange_c_1 364
{NULL},//toggle4_showchange_c_1 364
{NULL},//window_showchange_c_8 365
{NULL},//cleanup_showchange_c_6 366
{NULL},//tcache_showchange_c_1 367
{0},//flags_showchange_c_1 368
{NULL},//signalarea 369
{NULL},//signalfont 370
{NULL},//signalpixmap 371
{ 0},//max_signal_name_pixel_width 372
{ 0},//signal_pixmap_width 373
{ 0},//signal_fill_width 374
{0},//old_signal_fill_width 375
{0},//old_signal_fill_height
{ 1},//fontheight 376
{0},//dnd_state 377
{NULL},//hscroll_signalwindow_c_1 378
{NULL},//signal_hslider 379
{0},//cachedhiflag_signalwindow_c_1 380
{-1},//cachedwhich_signalwindow_c_1 381
{NULL},//cachedtrace 382
{NULL},//shift_click_trace 383
{0},//trtarget_signalwindow_c_1 384
{ NULL},//window_simplereq_c_9 385
{NULL},//cleanup 386
{1},//splash_disable 387
{NULL},//wave_splash_pixmap 389
{NULL},//wave_splash_mask 390
{ NULL},//splash_splash_c_1 391
{ NULL},//darea_splash_c_1 392
{ NULL},//gt_splash_c_1 393
{ 0},//timeout_tag 394
{0},//load_complete_splash_c_1 395
{ 2},//cnt_splash_c_1 396
{ 0},//prev_bar_x_splash_c_1 397
{NULL},//text_status_c_2 398
{NULL},//vscrollbar_status_c_2 399
{0},//iter_status_c_3 400
{NULL},//bold_tag_status_c_3 401
{NULL},//timearray 402
{0},//timearray_size 403
{NULL},//ptr_mark_count_label_strace_c_1 404
{NULL},//straces 405
{NULL},//shadow_straces 406
{NULL},//strace_defer_free_head 407
{NULL},//window_strace_c_10 408
{NULL},//cleanup_strace_c_7 409
{"AND", "OR", "XOR", "NAND", "NOR", "XNOR"},//logical_strace_c_1 410
{"Don't Care", "High", "Z (Mid)", "X", "Low", "String",
 "Rising Edge", "Falling Edge", "Any Edge"},//stype_strace_c_1 411
{0,0,0,0,0,0},//logical_mutex 412
{0,0,0,0,0,0},//shadow_logical_mutex 413
{ 0},//shadow_active 414
{ 0},//shadow_type 415
{ NULL},//shadow_string 416
{ 0},//mark_idx_start 417
{ 0},//mark_idx_end 418
{ 0},//shadow_mark_idx_start 419
{ 0},//shadow_mark_idx_end 420
	{
	   { "Start of Time"  },
	   { "Named Marker A" },
	   { "Named Marker B" },
	   { "Named Marker C" },
	   { "Named Marker D" },
	   { "Named Marker E" },
	   { "Named Marker F" },
	   { "Named Marker G" },
	   { "Named Marker H" },
	   { "Named Marker I" },
	   { "Named Marker J" },
	   { "Named Marker K" },
	   { "Named Marker L" },
	   { "Named Marker M" },
	   { "Named Marker N" },
	   { "Named Marker O" },
	   { "Named Marker P" },
	   { "Named Marker Q" },
	   { "Named Marker R" },
	   { "Named Marker S" },
	   { "Named Marker T" },
	   { "Named Marker U" },
	   { "Named Marker V" },
	   { "Named Marker W" },
	   { "Named Marker X" },
	   { "Named Marker Y" },
	   { "Named Marker Z" }
	},//item_mark_start_strings_strace_c_1 421

	{
	   { "End of Time"    },
	   { "Named Marker A" },
	   { "Named Marker B" },
	   { "Named Marker C" },
	   { "Named Marker D" },
	   { "Named Marker E" },
	   { "Named Marker F" },
	   { "Named Marker G" },
	   { "Named Marker H" },
	   { "Named Marker I" },
	   { "Named Marker J" },
	   { "Named Marker K" },
	   { "Named Marker L" },
	   { "Named Marker M" },
	   { "Named Marker N" },
	   { "Named Marker O" },
	   { "Named Marker P" },
	   { "Named Marker Q" },
	   { "Named Marker R" },
	   { "Named Marker S" },
	   { "Named Marker T" },
	   { "Named Marker U" },
	   { "Named Marker V" },
	   { "Named Marker W" },
	   { "Named Marker X" },
	   { "Named Marker Y" },
	   { "Named Marker Z" }
	},//item_mark_end_strings_strace_c_1 422
{NULL},//mprintf_buff_head 423
{NULL},//mprintf_buff_current
{NULL},//sym 424
{NULL},//facs 425
{0},//facs_are_sorted 426
{0},//numfacs 427
{0},//regions 428
{0},//longestname 429
{NULL},//firstnode 430
{NULL},//curnode 431
{0},//hashcache 432
{NULL},//from_entry 433
{NULL},//to_entry 
{ 0},//current_translate_file 434
{ 0},//current_filter_translate_c_2 435
{ 0},//num_file_filters 436
{
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
},//filesel_filter 437
{
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
},//xl_file_filter 438
{0},//is_active_translate_c_5 439
{ NULL},//fcurr_translate_c_2 440
{NULL},//window_translate_c_11 441
{NULL},//clist_translate_c_4 442
{NULL},//treeroot 443
{NULL},//module_tree_c_1 444
{0},//module_len_tree_c_1 445
{ NULL},//terminals_tchain_tree_c_1 446
{'.'},//hier_delimeter 447
{0},//hier_was_explicitly_set 448
{0x00},//alt_hier_delimeter 449
{ 1},//fast_tree_sort 450
{NULL},//facs2_tree_c_1 451
{0},//facs2_pos_tree_c_1 452
{0},//autoname_bundles 453
{NULL},//window1_treesearch_gtk2_c_3 454
{NULL},//entry_a_treesearch_gtk2_c_2 455
{NULL},//entrybox_text_local_treesearch_gtk2_c_3 456
{NULL},//cleanup_e_treesearch_gtk2_c_3 457
{ NULL},//sig_root_treesearch_gtk2_c_1 458
{ NULL},//filter_str_treesearch_gtk2_c_1 459
{NULL},//sig_store_treesearch_gtk2_c_1 460
{NULL},//sig_selection_treesearch_gtk2_c_1 461
{0},//is_active_treesearch_gtk2_c_6 462
{NULL},//ctree_main 463
{ NULL},//afl_treesearch_gtk2_c_1 464
{ NULL},//window_treesearch_gtk2_c_12 465
{NULL},//tree_treesearch_gtk2_c_1 466
{0},//bundle_direction_treesearch_gtk2_c_3 467
{NULL},//cleanup_treesearch_gtk2_c_8 468
{ 0},//pre_import_treesearch_gtk2_c_1 469
{0,0,0,NULL,NULL,NULL,0},//tcache_treesearch_gtk2_c_2 470
{ 0},//dnd_tgt_on_signalarea_treesearch_gtk2_c_1 471
{ -1},//vcd_warning_filesize 472
{1},//autocoalesce 473
{0},//autocoalesce_reversal
{-1},//vcd_explicit_zero_subscripts 474
{0},//convert_to_reals 475
{1},//atomic_vectors 476
{0},//make_vcd_save_file 477
{0},//vcd_preserve_glitches 478
{NULL},//vcd_save_handle 479
{NULL},//vcd_handle_vcd_c_1 480
{0},//vcd_is_compressed_vcd_c_1 481
{0},//vcdbyteno_vcd_c_1 482
{0},//error_count_vcd_c_1 483
{0},//header_over_vcd_c_1 484
{0},//dumping_off_vcd_c_1 485
{-1},//start_time_vcd_c_1 486
{-1},//end_time_vcd_c_1 487
{-1},//current_time_vcd_c_1 488
{0},//num_glitches_vcd_c_2 489
{0},//num_glitch_regions_vcd_c_2 490
{0, 0},//vcd_hier_delimeter 491
{NULL},//pv_vcd_c_1 492
{NULL},//rootv_vcd_c_1
{NULL},//vcdbuf_vcd_c_1 493
{NULL},//vst
{NULL},//vend
{ 0},//escaped_names_found_vcd_c_1 494
{NULL},//slistroot 495
{NULL},//slistcurr
{NULL},//slisthier 496
{0},//slisthier_len 497x
{ "var", 
  "end", 
  "scope", 
  "upscope",
  "comment", 
  "date", 
  "dumpall", 
  "dumpoff", 
  "dumpon",
  "dumpvars", 
  "enddefinitions",
  "dumpports", 
  "dumpportsoff", 
  "dumpportson", 
  "dumpportsall",
  "timescale", 
  "version", 
  "vcdclose",
  "", 
  "", 
  "" },//tokens_vcd_c_1 498
{1024},//T_MAX_STR_vcd_c_1 499
{NULL},//yytext_vcd_c_1 500
{0},//yylen_vcd_c_1 501
{0},//yylen_cache
{NULL},//vcdsymroot_vcd_c_1 502
{NULL},//vcdsymcurr
{NULL},//sorted_vcd_c_1 503
{NULL},//indexed_vcd_c_1 504
{0},//numsyms_vcd_c_1 505
{NULL},//he_curr_vcd_c_1 506
{NULL},//he_fini
{NULL},//queuedevents_vcd_c_1 507
{ ~0},//vcd_minid_vcd_c_1 508
{ 0},//vcd_maxid_vcd_c_1 509
{ 0},//err_vcd_c_1 510
{ 0},//vcd_fsiz_vcd_c_1 511
{NULL},//varsplit_vcd_c_1 512
{NULL},//varsplitcurr
{0},//var_prevch_vcd_c_1 513
{"DUNZduLHXTlh01?FAaBbCcf"},//evcd_vcd_c_1 514
{"01xz0101xz0101xzxxxxxxx"},//vcd_vcd_c_1 515
{0},//vcdbyteno_vcd_partial_c_2 516
{0},//error_count_vcd_partial_c_2 517
{0},//header_over_vcd_partial_c_2 518
{0},//dumping_off_vcd_partial_c_2 519
{-1},//start_time_vcd_partial_c_2 520
{-1},//end_time_vcd_partial_c_2 521
{-1},//current_time_vcd_partial_c_2 522
{0},//num_glitches_vcd_partial_c_3 523
{0},//num_glitch_regions_vcd_partial_c_3 524
{NULL},//pv_vcd_partial_c_2 525
{NULL},//rootv
{NULL},//vcdbuf_vcd_partial_c_2 526
{NULL},//vst
{NULL},//vend
{NULL},//consume_ptr_vcd_partial_c_1 527
{NULL},//buf_vcd_partial_c_2 528
{ 100000},//consume_countdown_vcd_partial_c_1 529
{ "var", "end", "scope", "upscope",
		 "comment", "date", "dumpall", "dumpoff", "dumpon",
		 "dumpvars", "enddefinitions",
		 "dumpports", "dumpportsoff", "dumpportson", "dumpportsall",
		 "timescale", "version", "vcdclose",
		 "", "", "" },//tokens_vcd_partial_c_2 530
{1024},//T_MAX_STR_vcd_partial_c_2 531
{NULL},//yytext_vcd_partial_c_2 532
{0},//yylen_vcd_partial_c_2 533
{0},//yylen_cache
{NULL},//vcdsymroot_vcd_partial_c_2 534
{NULL},//vcdsymcurr
{NULL},//sorted_vcd_partial_c_2 535
{NULL},//indexed_vcd_partial_c_2 536
{V_EVENT, V_PARAMETER,
 V_INTEGER, V_REAL, V_REAL_PARAMETER, V_REG, V_SUPPLY0,
 V_SUPPLY1, V_TIME, V_TRI, V_TRIAND, V_TRIOR,
 V_TRIREG, V_TRI0, V_TRI1, V_WAND, V_WIRE, V_WOR, V_PORT, 
 V_IN, V_OUT, V_INOUT,
 V_END, V_LB, V_COLON, V_RB, V_STRING },//varenums_vcd_partial_c_1 537
{0},//numsyms_vcd_partial_c_2 538
{NULL},//queuedevents_vcd_partial_c_2 539
{ ~0},//vcd_minid_vcd_partial_c_2 540
{ 0},//vcd_maxid_vcd_partial_c_2 541
{ 0},//err_vcd_partial_c_2 542
{NULL},//varsplit_vcd_partial_c_2 543
{NULL},//vsplitcurr
{0},//var_prevch_vcd_partial_c_2 544
{"DUNZduLHXTlh01?FAaBbCcf"},//evcd_vcd_partial_c_2 545
{"01xz0101xz0101xzxxxxxxx"},//vcd_vcd_partial_c_2 546
{ 0},//timeset_vcd_partial_c_1 547
{ NULL},//time_vlist_vcd_recoder_c_1 548
{ 0},//time_vlist_count_vcd_recoder_c_1 549
{NULL},//vcd_handle_vcd_recoder_c_2 550
{0},//vcd_is_compressed_vcd_recoder_c_2 551
{0},//vcdbyteno_vcd_recoder_c_3 552
{0},//error_count_vcd_recoder_c_3 553
{0},//header_over_vcd_recoder_c_3 554
{0},//dumping_off_vcd_recoder_c_3 555
{-1},//start_time_vcd_recoder_c_3 556
{-1},//end_time_vcd_recoder_c_3 557
{-1},//current_time_vcd_recoder_c_3 558
{0},//num_glitches_vcd_recoder_c_4 559
{0},//num_glitch_regions_vcd_recoder_c_4 560
{NULL},//pv_vcd_recoder_c_3 561
{NULL},//rootv
{NULL},//vcdbuf_vcd_recoder_c_3 562
{NULL},//vst
{NULL},//vend
{ "var", "end", "scope", "upscope",
  "comment", "date", "dumpall", "dumpoff", "dumpon",
  "dumpvars", "enddefinitions",
  "dumpports", "dumpportsoff", "dumpportson", 
  "dumpportsall",
  "timescale", "version", "vcdclose",
  "", "", "" },//tokens_vcd_recoder_c_3 563
{1024},//T_MAX_STR_vcd_recoder_c_3 564
{NULL},//yytext_vcd_recoder_c_3 565
{0},//yylen_vcd_recoder_c_3 566
{0},//yylen_cache
{NULL},//vcdsymroot_vcd_recoder_c_3 567
{NULL},//vcdsymcurr
{NULL},//sorted_vcd_recoder_c_3 568
{NULL},//indexed_vcd_recoder_c_3 569
{0},//numsyms_vcd_recoder_c_3 570
{ ~0},//vcd_minid_vcd_recoder_c_3 571
{ 0},//vcd_maxid_vcd_recoder_c_3 572
{ 0},//err_vcd_recoder_c_3 573
{ 0},//vcd_fsiz_vcd_recoder_c_2 574
{NULL},//varsplit_vcd_recoder_c_3 575
{NULL},//vsplitcurr
{0},//var_prevch_vcd_recoder_c_3 576
{"DUNZduLHXTlh01?FAaBbCcf"},//evcd_vcd_recoder_c_3 577
{"01xz0101xz0101xzxxxxxxx"},//vcd_vcd_recoder_c_3 578
{ NULL},//f_vcd_saver_c_1 579
{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},//buf_vcd_saver_c_3 580
{ NULL},//hp_vcd_saver_c_1 581  XXXX 
{NULL},//nhold_vcd_saver_c_1 582
{ 4},//vlist_compression_depth 583
{NULL},//vzt_vzt_c_1 584
{0},//first_cycle_vzt_c_3 585
{0},//last_cycle
{0},//total_cycles
{ NULL},//vzt_table_vzt_c_1 586
{NULL},//mvlfacs_vzt_c_3 587
{ 0},//busycnt_vzt_c_2 588
{0},//m1x_wavewindow_c_1 589
{0},//m2x
{1},//signalwindow_width_dirty 590
{1},//enable_ghost_marker 591
{1},//enable_horiz_grid 592
{1},//enable_vert_grid 593
{0},//use_big_fonts 594
{0},//use_nonprop_fonts
{~0},//do_resize_signals 595
{0},//constant_marker_update 596
{0},//use_roundcaps 597
{~0},//show_base 598
{~0},//wave_scrolling 599
{4},//vector_padding 600
{0},//in_button_press_wavewindow_c_1 601
{0},//left_justify_sigs 602
{0},//zoom_pow10_snap 603
{0},//cursor_snap 604
{-1.0},//old_wvalue 605
{ NULL},//blackout_regions 606
{0},//zoom 607
{1},//scale
{1},//nsperframe
{1},//pixelsperframe 608
{1.0},//hashstep 609
{-1},//prevtim_wavewindow_c_1 610
{1.0},//pxns 611
{1.0},//nspx
{2.0},//zoombase 612
{NULL},//topmost_trace 613
{1},//waveheight 614
{0},//wavecrosspiec
{1},//wavewidth 615
{NULL},//wavefont 616
{NULL},//wavefont_smaller 617
{NULL},//wavearea 618
{NULL},//vscroll_wavewindow_c_1 619
{NULL},//hscroll_wavewindow_c_2 620
{ NULL},//wavepixmap_wavewindow_c_1 621
{NULL},//wave_vslider 622
{NULL},//wave_hslider
{0,0,0,0, 
 0,0,0,0, 
 0,0,0,0, 
 0,0,0,0, 

 0,0,0,0, 
 0,0,0,0, 0,0},//named_markers 623
{0},//made_gc_contexts_wavewindow_c_1 624
{NULL},//gc_back_wavewindow_c_1 625
{NULL},//gc_baseline_wavewindow_c_1 626
{NULL},//gc_grid_wavewindow_c_1 627
{NULL},//gc_time_wavewindow_c_1 628
{NULL},//gc_timeb_wavewindow_c_1 629
{NULL},//gc_value_wavewindow_c_1 630
{NULL},//gc_low_wavewindow_c_1 631
{NULL},//gc_high_wavewindow_c_1 632
{NULL},//gc_trans_wavewindow_c_1 633
{NULL},//gc_mid_wavewindow_c_1 634
{NULL},//gc_xfill_wavewindow_c_1 635
{NULL},//gc_x_wavewindow_c_1 636
{NULL},//gc_vbox_wavewindow_c_1 637
{NULL},//gc_vtrans_wavewindow_c_1 638
{NULL},//gc_mark_wavewindow_c_1 639
{NULL},//gc_umark_wavewindow_c_1 640
{NULL},//gc_0_wavewindow_c_1 641
{NULL},//gc_1_wavewindow_c_1 642
{NULL},//gc_ufill_wavewindow_c_1 643
{NULL},//gc_u_wavewindow_c_1 644
{NULL},//gc_wfill_wavewindow_c_1 645
{NULL},//gc_w_wavewindow_c_1 646
{NULL},//gc_dashfill_wavewindow_c_1 647
{NULL},//gc_dash_wavewindow_c_1 648
{0},//made_sgc_contexts_wavewindow_c_1 649
{NULL},//gc_white 650
{NULL},//gc_black 651
{NULL},//gc_ltgray 652
{NULL},//gc_normal 653
{NULL},//gc_mdgray 654
{NULL},//gc_dkgray 655
{NULL},//gc_dkblue 656
{ 0, GDK_BUTTON1_MASK, 0, GDK_BUTTON3_MASK },//bmask_wavewindow_c_1 657
{ 0, GDK_BUTTON1_MOTION_MASK, 0, GDK_BUTTON3_MOTION_MASK },//m_bmask_wavewindow_c_1 658
{ 0},//fill_in_smaller_rgb_areas_wavewindow_c_1 659
{1},//do_zoom_center 660
{0},//do_initial_zoom_fit 661
};
