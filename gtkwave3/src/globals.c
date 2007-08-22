#include "globals.h"
#include "analyzer.h"
#include "bsearch.h"
#include "busy.h"
#include "clipping.h"
#include "color.h"
#include "currenttime.h"
#include "debug.h"
#include "fgetdynamic.h"
#include "ghw.h"
#include "globals.h"
#include "gnu-getopt.h"
#include "gnu_regex.h"
#include "gtk12compat.h"
#include "lx2.h"
#include "lxt.h"
#include "main.h"
#include "menu.h"
#include "pipeio.h"
#include "pixmaps.h"
#include "print.h"
#include "ptranslate.h"
#include "rc.h"
#include "regex_wave.h"
#include "strace.h"
#include "symbol.h"
#include "translate.h"
#include "tree.h"
#include "vcd.h"
#include "vcd_saver.h"
#include "vlist.h"
#include "vzt.h"
#include "wavealloca.h"

struct Global *GLOBALS = NULL;

/* make this const so if we try to write to it we coredump */
static const struct Global globals_base_values = 
{ 
/*
 * ae2.c
 */
0, //ae2_num_sections
NULL, //ae2_lx2_table
NULL, //ae2_f
NULL, //ae2
NULL, //ae2_fr
NULL, //ae2_process_mask
0, //ae2_msg_suppress
NULL, //ae2_regex_head
0, // ae2_regex_matches
0, // ae2_twirl_pos
0, // ae2_did_twirl


/*
 * analyzer.c
 */
TR_RJUSTIFY,//default_flags 5
{0, 0, 0, 0, 0, 0, 0},//tims 6
{0, 0, NULL, NULL, NULL, NULL, 0},//traces 7
0,//hier_max_level 8


/*
 * baseconvert.c
 */
0,//color_active_in_filter 9


/* 
 * bsearch.c
 */
LLDescriptor(0),//shift_timebase 10
LLDescriptor(0),//shift_timebase_default_for_add 11
0,//max_compare_time_tc_bsearch_c_1 12
0,//max_compare_pos_tc_bsearch_c_1 13
0,//max_compare_time_bsearch_c_1 14
0,//max_compare_pos_bsearch_c_1 15
0,//max_compare_index 16
0,//vmax_compare_time_bsearch_c_1 17
0,//vmax_compare_pos_bsearch_c_1 18
0,//vmax_compare_index 19
0,//maxlen_trunc 20
0,//maxlen_trunc_pos_bsearch_c_1 21
0,//trunc_asciibase_bsearch_c_1 22


/*
 * busy.c
 */
NULL,//busycursor_busy_c_1 23
0,//busy_busy_c_1 24


/*
 * color.c
 */ 
-1,//color_back 25
-1,//color_baseline 26
-1,//color_grid 27
-1,//color_high 28
-1,//color_low 29
-1,//color_mark 30
-1,//color_mid 31
-1,//color_time 32
-1,//color_timeb 33
-1,//color_trans 34
-1,//color_umark 35
-1,//color_value 36
-1,//color_vbox 37
-1,//color_vtrans 38
-1,//color_x 39
-1,//color_xfill 40
-1,//color_0 41
-1,//color_1 42
-1,//color_ufill 43
-1,//color_u 44
-1,//color_wfill 45
-1,//color_w 46
-1,//color_dashfill 47
-1,//color_dash 48
-1,//color_white 49
-1,//color_black 50
-1,//color_ltgray 51
-1,//color_normal 52
-1,//color_mdgray 53
-1,//color_dkgray 54
-1,//color_dkblue 55


/*
 * currenttime.c
 */
0,//is_vcd 56
0,//partial_vcd
1,//use_maxtime_display 57
0,//use_frequency_delta 58
NULL,//max_or_marker_label_currenttime_c_1 59
NULL,//base_or_curtime_label_currenttime_c_1 60
0,//cached_currenttimeval_currenttime_c_1 61
0,//currenttime 62
0,//max_time 63
-1,//min_time 64
~0,//display_grid 65
1,//time_scale 66
'n',//time_dimension 67
0,//maxtimewid_currenttime_c_1 69
0,//curtimewid_currenttime_c_1 70
0,//maxtext_currenttime_c_1 71
0,//curtext_currenttime_c_1 72
1,//time_trunc_val_currenttime_c_1 76
0,//use_full_precision 77


/*
 * debug.c
 */
NULL,//alloc2_chain
0,//outstanding
NULL,//atoi_cont_ptr 78
0,//disable_tooltips 79


/*
 * entry.c
 */
0,//window_entry_c_1 80
0,//entry_entry_c_1 81
NULL,//entrybox_text 82
0,//cleanup_entry_c_1 83


/*
 * fetchbuttons.c
 */
100,//fetchwindow 84


/*
 * fgetdynamic.c
 */
0,//fgetmalloc_len 85


/*
 * file.c
 */
0,//fs_file_c_1 86
NULL,//fileselbox_text 87
0,//filesel_ok 88
0,//cleanup_file_c_2 89
0,//bad_cleanup_file_c1


/*
 * fonts.c
 */
NULL,//fontname_signals 90
NULL,//fontname_waves 91


/*
 * ghw.c
 */
0,//nxp_ghw_c_1 93
NULL,//sym_head_ghw_c_1 94
NULL,
0,//sym_which_ghw_c_1 95
NULL,//gwt_ghw_c_1 96
NULL,//gwt_corr_ghw_c_1 97
1,//xlat_1164_ghw_c_1 98
0,//is_ghw 99
NULL, //asbuf
0,//nbr_sig_ref_ghw_c_1 101
0,//num_glitches_ghw_c_1 102
0, //  num_glitch_regions_ghw_c_1
{0, 0 },//dummy_en_ghw_c_1 103
0,//fac_name_ghw_c_1 104
0,//fac_name_len_ghw_c_1 105
0,//fac_name_max_ghw_c_1 106
0,//last_fac_ghw_c_1 107
0,//warned_ghw_c_1 108


/*
 * help.c
 */
0,//helpbox_is_active 110
0,//text_help_c_1 111
0,//vscrollbar_help_c_1 112
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
{0},//iter_help_c_1 113
#endif
0,//bold_tag_help_c_1 114
0,//window_help_c_2 115


/*
 * hiersearch.c
 */
1,//hier_grouping 116
0,//window_hiersearch_c_3 117
0,//entry_main_hiersearch_c_1 118
0,//clist_hiersearch_c_1 119
0,//bundle_direction_hiersearch_c_1 120
0,//cleanup_hiersearch_c_3 121
0,//num_rows_hiersearch_c_1 122
0,//selected_rows_hiersearch_c_1 123
0,//window1_hiersearch_c_1 124
0,//entry_hiersearch_c_2 125
NULL,//entrybox_text_local_hiersearch_c_1 126
NULL,//cleanup_e_hiersearch_c_1 127
NULL,//h_selectedtree_hiersearch_c_1 128
NULL,//current_tree_hiersearch_c_1 129
NULL,//treechain_hiersearch_c_1 130
0,//is_active_hiersearch_c_1 131


/* 
 * logfile.c
 */
NULL,//fontname_logfile 133
NULL,//font_logfile_c_1 134
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
{0},//iter_logfile_c_2 135
#endif
NULL,//bold_tag_logfile_c_2 136
NULL,//mono_tag_logfile_c_1 137
NULL,//size_tag_logfile_c_1 138


/*
 * lx2.c
 */
LXT2_IS_INACTIVE,//is_lx2 139
NULL,//lx2_lx2_c_1 140
0,//first_cycle_lx2_c_1 141
0,//last_cycle
0,//total_cycles
NULL,//lx2_table_lx2_c_1 142
NULL,//mvlfacs_lx2_c_1 143
0,//busycnt_lx2_c_1 144


/*
 * lxt.c
 */
NULL, // mm_lxt_mmap_addr
0, // mm_lxt_mmap_len
#if defined __MINGW32__ || defined _MSC_VER
NULL, // HANDLE hIn
NULL, // HANDLE hInMap
NULL, // char *win_fname = NULL;
#endif
0,//fpos_lxt_c_1 145
0,//is_lxt 146
0,//lxt_clock_compress_to_z 147
NULL,//mm_lxt_c_1 148
NULL,//mmcache_lxt_c_1
0,//version_lxt_c_1 149
NULL,//mvlfacs_lxt_c_2 150
0,//first_cycle_lxt_c_2 151
0,//last_cycle
0,//total_cycles
0,//maxchange_lxt_c_1 152
0,//maxindex
0,//f_len_lxt_c_1 153
NULL,//positional_information_lxt_c_1 154
NULL,//time_information 155
0,//change_field_offset_lxt_c_1 156
0,//facname_offset_lxt_c_1 157
0,//facgeometry_offset_lxt_c_1 158
0,//time_table_offset_lxt_c_1 159
0,//time_table_offset64_lxt_c_1 160
0,//sync_table_offset_lxt_c_1 161
0,//initial_value_offset_lxt_c_1 162
0,//timescale_offset_lxt_c_1 163
0,//double_test_offset_lxt_c_1 164
0,//zdictionary_offset_lxt_c_1 165
0,//zfacname_predec_size_lxt_c_1 166
0,//zfacname_size_lxt_c_1 167
0,//zfacgeometry_size_lxt_c_1 168
0,//zsync_table_size_lxt_c_1 169
0,//ztime_table_size_lxt_c_1 170
0,//zchg_predec_size_lxt_c_1 171
0,//zchg_size_lxt_c_1 172
0,//zdictionary_predec_size_lxt_c_1 173
AN_X,//initial_value_lxt_c_1 174
0,//dict_num_entries_lxt_c_1 175
0,//dict_string_mem_required_lxt_c_1 176
0,//dict_16_offset_lxt_c_1 177
0,//dict_24_offset_lxt_c_1 178
0,//dict_32_offset_lxt_c_1 179
0,//dict_width_lxt_c_1 180
NULL,//dict_string_mem_array_lxt_c_1 181
0,//exclude_offset_lxt_c_1 182
NULL,//lt_buf_lxt_c_1 183
0,//lt_len_lxt_c_1 184
0,//fd_lxt_c_1 185
{0,0,0,0,0,0,0,0},//double_mask_lxt_c_1 186
0,//double_is_native_lxt_c_1 187
0,//max_compare_time_tc_lxt_c_2 189
0,//max_compare_pos_tc_lxt_c_2


/* 
 * main.c
 */
NULL, // loaded_file_name
NULL, // unoptimized_vcd_file_name   
NULL, // skip_start
NULL, // skip_end
NULL, // indirect_fname
NO_FILE, // loaded_file_type
NULL,//whoami 190
NULL,//logfile 191
NULL,//stems_name 192
WAVE_ANNO_NONE,//stems_type 193
NULL,//aet_name 194
NULL,//anno_ctx 195
NULL,//dual_ctx 196
0,//dual_id 197
0,//dual_attach_id_main_c_1 198
0,//dual_race_lock 199
NULL,//mainwindow 200
NULL,//signalwindow 201
NULL,//wavewindow 202
NULL,//toppanedwindow 203
NULL,//sstpane 204
NULL,//expanderwindow 205
0,//disable_window_manager 206
1,//paned_pack_semantics 207
0,//zoom_was_explicitly_set 208
800,//initial_window_x 209
400,//initial_window_y
-1,// initial_window_width
-1,//initial_window_height 210
0,//xy_ignore_main_c_1 211
0,//optimize_vcd 212
1,//num_cpus 213
-1,//initial_window_xpos 214
-1,//initial_window_ypos 214
0,//initial_window_set_valid 215
-1,//initial_window_xpos_set 216
-1,//initial_window_ypos_set
0,//initial_window_get_valid 217
-1,//initial_window_xpos_get 218
-1,//initial_window_ypos_get 218
0,//xpos_delta 219
0,//ypos_delta 219
0,//use_scrollbar_only 220
0,//force_toolbars 221
0,//hide_sst 222
1,//sst_expanded 223
0,//socket_xid 224
0,//disable_menus 225
NULL,//ftext_main_main_c_1 226


/*
 * markerbox.c
 */
NULL,//window_markerbox_c_4 231
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//entries_markerbox_c_1 232
NULL,//cleanup_markerbox_c_4 233
0,//dirty_markerbox_c_1 234
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//shadow_markers_markerbox_c_1 235


/*
 * menu.c
 */ 
0,//enable_fast_exit 236
NULL,//script_handle 237
0,//ignore_savefile_pos 238
0,//ignore_savefile_size 239
NULL,//item_factory_menu_c_1 241
NULL,//regexp_string_menu_c_1 242
NULL,//trace_to_alias_menu_c_1 243
NULL,//showchangeall_menu_c_1 244
NULL,//filesel_newviewer_menu_c_1 245
NULL,//filesel_logfile_menu_c_1 246
NULL,//filesel_writesave 247
0,//save_success_menu_c_1 248
NULL,//filesel_vcd_writesave 249
NULL,//filesel_lxt_writesave 250
0,//lock_menu_c_1 251
0,//lock_menu_c_2 252
NULL,//buf_menu_c_1 253 128

/*
 * mouseover.c
 */
1,//disable_mouseover 254
NULL,//mouseover_mouseover_c_1 255
NULL,//mo_area_mouseover_c_1 256
NULL,//mo_pixmap_mouseover_c_1 257
NULL,//mo_dk_gray_mouseover_c_1 258
NULL,//mo_black_mouseover_c_1 259
0,//mo_width_mouseover_c_1 260
0, //mo_height_mouseover_c_1 260


/*
 * pagebuttons.c
 */
1.0,//page_divisor 261


/*
 * pixmaps.c
 */
NULL,//larrow_pixmap 263
NULL,//larrow_mask 264
NULL,//rarrow_pixmap 266
NULL,//rarrow_mask 267
NULL,//zoomin_pixmap 269
NULL,//zoomin_mask 270
NULL,//zoomout_pixmap 272
NULL,//zoomout_mask 273
NULL,//zoomfit_pixmap 275
NULL,//zoomfit_mask 276
NULL,//zoomundo_pixmap 278
NULL,//zoomundo_mask 279
NULL,//zoom_larrow_pixmap 281
NULL,//zoom_larrow_mask 282
NULL,//zoom_rarrow_pixmap 284
NULL,//zoom_rarrow_mask 285
NULL,//prev_page_pixmap 287
NULL,//prev_page_mask 288
NULL,//next_page_pixmap 290
NULL,//next_page_mask 291
NULL,//wave_info_pixmap 293
NULL,//wave_info_mask 294
NULL,//wave_alert_pixmap 296
NULL,//wave_alert_mask 297


/*
 * print.c
 */
72,//inch_print_c_1 298
1.0,//ps_chwidth_print_c_1 299
0,//ybound_print_c_1 300
0,//pr_signal_fill_width_print_c_1 301
0,//ps_nummaxchars_print_c_1 302
1,//ps_fullpage 303
66,//ps_maxveclen 304
0,//liney_max 305


/*
 * ptranslate.c
 */
0,//current_translate_proc 308
0,//current_filter_ptranslate_c_1 309
0,//num_proc_filters 310
NULL,//procsel_filter 311
NULL,//proc_filter 312
0,//is_active_ptranslate_c_2 313
NULL,//fcurr_ptranslate_c_1 314
NULL,//window_ptranslate_c_5 315
NULL,//clist_ptranslate_c_2 316


/*
 * rc.c
 */
0,//rc_line_no 318
1,//possibly_use_rc_defaults 319


/*
 * regex.c
 */
NULL,//preg_regex_c_1 321
NULL,//regex_ok_regex_c_1 322


/*
 * renderopt.c
 */
0,//is_active_renderopt_c_3 323
0,//window_renderopt_c_6 324
NULL,//filesel_print_ps_renderopt_c_1 325
NULL,//filesel_print_mif_renderopt_c_1 326
{0,0},//target_mutex_renderopt_c_1 328
{0,0,0,0,0},//page_mutex_renderopt_c_1 330
{0,0},//render_mutex_renderopt_c_1 332
0,//page_size_type_renderopt_c_1 333


/*
 * search.c
 */
{NULL,NULL,NULL,NULL,NULL},//menuitem_search
NULL,//window1_search_c_2 340
NULL,//entry_a_search_c_1 341
NULL,//entrybox_text_local_search_c_2 342
NULL,//cleanup_e_search_c_2 343
NULL,//pdata 344
0,//is_active_search_c_4 345
0,//is_insert_running_search_c_1 346
0,//is_replace_running_search_c_1 347
0,//is_append_running_search_c_1 348
0,//is_searching_running_search_c_1 349
{0,0,0,0,0},//regex_mutex_search_c_1 352
0,//regex_which_search_c_1 353
NULL,//window_search_c_7 354
NULL,//entry_search_c_3 355
NULL,//clist_search_c_3 356
NULL,//searchbox_text_search_c_1 358
0,//bundle_direction_search_c_2 359
NULL,//cleanup_search_c_5 360
0,//num_rows_search_c_2 361
0,//selected_rows_search_c_2 362


/*
 * showchange.c
 */
NULL,//button1_showchange_c_1 363
NULL,//button1_showchange_c_2 363
NULL,//button1_showchange_c_3 363
NULL,//button1_showchange_c_4 363
NULL,//button1_showchange_c_5 363
NULL,//button1_showchange_c_6 363
NULL,//toggle1_showchange_c_1 364
NULL,//toggle2_showchange_c_1 364
NULL,//toggle3_showchange_c_1 364
NULL,//toggle4_showchange_c_1 364
NULL,//window_showchange_c_8 365
NULL,//cleanup_showchange_c_6 366
NULL,//tcache_showchange_c_1 367
0,//flags_showchange_c_1 368


/*  
 * signalwindow.c
 */
NULL,//signalarea 369
NULL,//signalfont 370
NULL,//signalpixmap 371
0,//max_signal_name_pixel_width 372
0,//signal_pixmap_width 373
0,//signal_fill_width 374
0,//old_signal_fill_width 375
0,//old_signal_fill_height
1,//fontheight 376
0,//dnd_state 377
NULL,//hscroll_signalwindow_c_1 378
NULL,//signal_hslider 379
0,//cachedhiflag_signalwindow_c_1 380
-1,//cachedwhich_signalwindow_c_1 381
NULL,//cachedtrace 382
NULL,//shift_click_trace 383
0,//trtarget_signalwindow_c_1 384


/*
 * simplereq.c
 */
NULL,//window_simplereq_c_9 385
NULL,//cleanup 386


/*
 * splash.c
 */ 
1,//splash_disable 387
NULL,//wave_splash_pixmap 389
NULL,//wave_splash_mask 390
NULL,//splash_splash_c_1 391
NULL,//darea_splash_c_1 392
NULL,//gt_splash_c_1 393
0,//timeout_tag 394
0,//load_complete_splash_c_1 395
2,//cnt_splash_c_1 396
0,//prev_bar_x_splash_c_1 397


/*
 * status.c
 */
NULL,//text_status_c_2 398
NULL,//vscrollbar_status_c_2 399
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
{0},//iter_status_c_3 400
#endif
NULL,//bold_tag_status_c_3 401


/*
 * strace.c
 */
NULL,//timearray 402
0,//timearray_size 403
NULL,//ptr_mark_count_label_strace_c_1 404
NULL,//straces 405
NULL,//shadow_straces 406
NULL,//strace_defer_free_head 407
NULL,//window_strace_c_10 408
NULL,//cleanup_strace_c_7 409
{0,0,0,0,0,0},//logical_mutex 412
{0,0,0,0,0,0},//shadow_logical_mutex 413
0,//shadow_active 414
0,//shadow_type 415
NULL,//shadow_string 416
0,//mark_idx_start 417
0,//mark_idx_end 418
0,//shadow_mark_idx_start 419
0,//shadow_mark_idx_end 420
NULL,//mprintf_buff_head 423
NULL,//mprintf_buff_current


/*
 * symbol.c
 */
NULL,//sym 424
NULL,//facs 425
0,//facs_are_sorted 426
0,//numfacs 427
0,//regions 428
0,//longestname 429
NULL,//firstnode 430
NULL,//curnode 431
0,//hashcache 432


/*
 * timeentry.c
 */
NULL,//from_entry 433
NULL,//to_entry 


/*
 * translate.c
 */
0,//current_translate_file 434
0,//current_filter_translate_c_2 435
0,//num_file_filters 436
NULL,//filesel_filter 437
NULL,//xl_file_filter 438
0,//is_active_translate_c_5 439
NULL,//fcurr_translate_c_2 440
NULL,//window_translate_c_11 441
NULL,//clist_translate_c_4 442


/*
 * tree.c
 */
NULL,//treeroot 443
NULL,//module_tree_c_1 444
0,//module_len_tree_c_1 445
NULL,//terminals_tchain_tree_c_1 446
'.',//hier_delimeter 447
0,//hier_was_explicitly_set 448
0x00,//alt_hier_delimeter 449
1,//fast_tree_sort 450
NULL,//facs2_tree_c_1 451
0,//facs2_pos_tree_c_1 452


/*
 * treesearch_gtk1.c
 */
NULL, //GtkWidget *window1_treesearch_gtk1_c;  
NULL, //GtkWidget *entry_a_treesearch_gtk1_c;  
NULL, //char *entrybox_text_local_treesearch_gtk1_c;  
NULL, //void (*cleanup_e_treesearch_gtk1_c)();
NULL, //struct tree *selectedtree_treesearch_gtk1_c;  
0, //int is_active_treesearch_gtk1_c;  
NULL, //GtkWidget *window_treesearch_gtk1_c;  
NULL, //GtkWidget *tree_treesearch_gtk1_c;
0, //char bundle_direction_treesearch_gtk1_c;  
NULL, //void (*cleanup_treesearch_gtk1_c)();


/*
 * treesearch_gtk2.c
 */
NULL,//treesearch_gtk2_window_vbox
NULL,//selected_hierarchy_name
NULL,//gtk2_tree_frame
NULL,//filter_entry
NULL,//any_tree_node
NULL,//open_tree_nodes
0,//autoname_bundles 453
NULL,//window1_treesearch_gtk2_c_3 454
NULL,//entry_a_treesearch_gtk2_c_2 455
NULL,//entrybox_text_local_treesearch_gtk2_c_3 456
NULL,//cleanup_e_treesearch_gtk2_c_3 457
NULL,//sig_root_treesearch_gtk2_c_1 458
NULL,//filter_str_treesearch_gtk2_c_1 459
NULL,//sig_store_treesearch_gtk2_c_1 460
NULL,//sig_selection_treesearch_gtk2_c_1 461
0,//is_active_treesearch_gtk2_c_6 462
NULL,//ctree_main 463
NULL,//afl_treesearch_gtk2_c_1 464
NULL,//window_treesearch_gtk2_c_12 465
NULL,//tree_treesearch_gtk2_c_1 466
0,//bundle_direction_treesearch_gtk2_c_3 467
NULL,//cleanup_treesearch_gtk2_c_8 468
0,//pre_import_treesearch_gtk2_c_1 469
{0,0,0,NULL,NULL,NULL,0},//tcache_treesearch_gtk2_c_2 470
0,//dnd_tgt_on_signalarea_treesearch_gtk2_c_1 471


/*
 * vcd.c
 */
-1,//vcd_warning_filesize 472
1,//autocoalesce 473
0,//autocoalesce_reversal
-1,//vcd_explicit_zero_subscripts 474
0,//convert_to_reals 475
1,//atomic_vectors 476
0,//make_vcd_save_file 477
0,//vcd_preserve_glitches 478
NULL,//vcd_save_handle 479
NULL,//vcd_handle_vcd_c_1 480
0,//vcd_is_compressed_vcd_c_1 481
0,//vcdbyteno_vcd_c_1 482
0,//error_count_vcd_c_1 483
0,//header_over_vcd_c_1 484
0,//dumping_off_vcd_c_1 485
-1,//start_time_vcd_c_1 486
-1,//end_time_vcd_c_1 487
-1,//current_time_vcd_c_1 488
0,//num_glitches_vcd_c_2 489
0,//num_glitch_regions_vcd_c_2 490
{0, 0},//vcd_hier_delimeter 491
NULL,//pv_vcd_c_1 492
NULL,//rootv_vcd_c_1
NULL,//vcdbuf_vcd_c_1 493
NULL,//vst
NULL,//vend
0,//escaped_names_found_vcd_c_1 494
NULL,//slistroot 495
NULL,//slistcurr
NULL,//slisthier 496
0,//slisthier_len 497x
1024,//T_MAX_STR_vcd_c_1 499
NULL,//yytext_vcd_c_1 500
0,//yylen_vcd_c_1 501
0,//yylen_cache
NULL,//vcdsymroot_vcd_c_1 502
NULL,//vcdsymcurr
NULL,//sorted_vcd_c_1 503
NULL,//indexed_vcd_c_1 504
0,//numsyms_vcd_c_1 505
NULL,//he_curr_vcd_c_1 506
NULL,//he_fini
NULL,//queuedevents_vcd_c_1 507
~0,//vcd_minid_vcd_c_1 508
0,//vcd_maxid_vcd_c_1 509
0,//err_vcd_c_1 510
0,//vcd_fsiz_vcd_c_1 511
NULL,//varsplit_vcd_c_1 512
NULL,//varsplitcurr
0,//var_prevch_vcd_c_1 513


/*
 * vcd_partial.c
 */
0,//vcdbyteno_vcd_partial_c_2 516
0,//error_count_vcd_partial_c_2 517
0,//header_over_vcd_partial_c_2 518
0,//dumping_off_vcd_partial_c_2 519
-1,//start_time_vcd_partial_c_2 520
-1,//end_time_vcd_partial_c_2 521
-1,//current_time_vcd_partial_c_2 522
0,//num_glitches_vcd_partial_c_3 523
0,//num_glitch_regions_vcd_partial_c_3 524
NULL,//pv_vcd_partial_c_2 525
NULL,//rootv
NULL,//vcdbuf_vcd_partial_c_2 526
NULL,//vst
NULL,//vend
NULL,//consume_ptr_vcd_partial_c_1 527
NULL,//buf_vcd_partial_c_2 528
100000,//consume_countdown_vcd_partial_c_1 529
1024,//T_MAX_STR_vcd_partial_c_2 531
NULL,//yytext_vcd_partial_c_2 532
0,//yylen_vcd_partial_c_2 533
0,//yylen_cache
NULL,//vcdsymroot_vcd_partial_c_2 534
NULL,//vcdsymcurr
NULL,//sorted_vcd_partial_c_2 535
NULL,//indexed_vcd_partial_c_2 536
0,//numsyms_vcd_partial_c_2 538
NULL,//queuedevents_vcd_partial_c_2 539
~0,//vcd_minid_vcd_partial_c_2 540
0,//vcd_maxid_vcd_partial_c_2 541
0,//err_vcd_partial_c_2 542
NULL,//varsplit_vcd_partial_c_2 543
NULL,//vsplitcurr
0,//var_prevch_vcd_partial_c_2 544
0,//timeset_vcd_partial_c_1 547


/*
 * vcd_recoder.c
 */
NULL,//time_vlist_vcd_recoder_c_1 548
0,//time_vlist_count_vcd_recoder_c_1 549
NULL,//vcd_handle_vcd_recoder_c_2 550
0,//vcd_is_compressed_vcd_recoder_c_2 551
0,//vcdbyteno_vcd_recoder_c_3 552
0,//error_count_vcd_recoder_c_3 553
0,//header_over_vcd_recoder_c_3 554
0,//dumping_off_vcd_recoder_c_3 555
-1,//start_time_vcd_recoder_c_3 556
-1,//end_time_vcd_recoder_c_3 557
-1,//current_time_vcd_recoder_c_3 558
0,//num_glitches_vcd_recoder_c_4 559
0,//num_glitch_regions_vcd_recoder_c_4 560
NULL,//pv_vcd_recoder_c_3 561
NULL,//rootv
NULL,//vcdbuf_vcd_recoder_c_3 562
NULL,//vst
NULL,//vend
1024,//T_MAX_STR_vcd_recoder_c_3 564
NULL,//yytext_vcd_recoder_c_3 565
0,//yylen_vcd_recoder_c_3 566
0,//yylen_cache
NULL,//vcdsymroot_vcd_recoder_c_3 567
NULL,//vcdsymcurr
NULL,//sorted_vcd_recoder_c_3 568
NULL,//indexed_vcd_recoder_c_3 569
0,//numsyms_vcd_recoder_c_3 570
~0,//vcd_minid_vcd_recoder_c_3 571
0,//vcd_maxid_vcd_recoder_c_3 572
0,//err_vcd_recoder_c_3 573
0,//vcd_fsiz_vcd_recoder_c_2 574
NULL,//varsplit_vcd_recoder_c_3 575
NULL,//vsplitcurr
0,//var_prevch_vcd_recoder_c_3 576


/*
 * vcd_saver.c
 */
NULL,//f_vcd_saver_c_1 579
{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},//buf_vcd_saver_c_3 580
NULL,//hp_vcd_saver_c_1 581
NULL,//nhold_vcd_saver_c_1 582


/*
 * vlist.c
 */
4,//vlist_compression_depth 583


/*
 * vzt.c
 */
NULL,//vzt_vzt_c_1 584
0,//first_cycle_vzt_c_3 585
0,//last_cycle
0,//total_cycles
NULL,//vzt_table_vzt_c_1 586
NULL,//mvlfacs_vzt_c_3 587
0,//busycnt_vzt_c_2 588


/*
 * wavewindow.c
 */
0,//m1x_wavewindow_c_1 589
0,//m2x
1,//signalwindow_width_dirty 590
1,//enable_ghost_marker 591
1,//enable_horiz_grid 592
1,//enable_vert_grid 593
0,//use_big_fonts 594
0,//use_nonprop_fonts
~0,//do_resize_signals 595
0,//constant_marker_update 596
0,//use_roundcaps 597
~0,//show_base 598
~0,//wave_scrolling 599
4,//vector_padding 600
0,//in_button_press_wavewindow_c_1 601
0,//left_justify_sigs 602
0,//zoom_pow10_snap 603
0,//cursor_snap 604
-1.0,//old_wvalue 605
NULL,//blackout_regions 606
0,//zoom 607
1,//scale
1,//nsperframe
1,//pixelsperframe 608
1.0,//hashstep 609
-1,//prevtim_wavewindow_c_1 610
1.0,//pxns 611
1.0,//nspx
2.0,//zoombase 612
NULL,//topmost_trace 613
1,//waveheight 614
0,//wavecrosspiec
1,//wavewidth 615
NULL,//wavefont 616
NULL,//wavefont_smaller 617
NULL,//wavearea 618
NULL,//vscroll_wavewindow_c_1 619
NULL,//hscroll_wavewindow_c_2 620
NULL,//wavepixmap_wavewindow_c_1 621
NULL,//wave_vslider 622
NULL,//wave_hslider
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//named_markers 623
0,//made_gc_contexts_wavewindow_c_1 624
NULL,//gc_back_wavewindow_c_1 625
NULL,//gc_baseline_wavewindow_c_1 626
NULL,//gc_grid_wavewindow_c_1 627
NULL,//gc_time_wavewindow_c_1 628
NULL,//gc_timeb_wavewindow_c_1 629
NULL,//gc_value_wavewindow_c_1 630
NULL,//gc_low_wavewindow_c_1 631
NULL,//gc_high_wavewindow_c_1 632
NULL,//gc_trans_wavewindow_c_1 633
NULL,//gc_mid_wavewindow_c_1 634
NULL,//gc_xfill_wavewindow_c_1 635
NULL,//gc_x_wavewindow_c_1 636
NULL,//gc_vbox_wavewindow_c_1 637
NULL,//gc_vtrans_wavewindow_c_1 638
NULL,//gc_mark_wavewindow_c_1 639
NULL,//gc_umark_wavewindow_c_1 640
NULL,//gc_0_wavewindow_c_1 641
NULL,//gc_1_wavewindow_c_1 642
NULL,//gc_ufill_wavewindow_c_1 643
NULL,//gc_u_wavewindow_c_1 644
NULL,//gc_wfill_wavewindow_c_1 645
NULL,//gc_w_wavewindow_c_1 646
NULL,//gc_dashfill_wavewindow_c_1 647
NULL,//gc_dash_wavewindow_c_1 648
0,//made_sgc_contexts_wavewindow_c_1 649
NULL,//gc_white 650
NULL,//gc_black 651
NULL,//gc_ltgray 652
NULL,//gc_normal 653
NULL,//gc_mdgray 654
NULL,//gc_dkgray 655
NULL,//gc_dkblue 656
0,//fill_in_smaller_rgb_areas_wavewindow_c_1 659


/*
 * zoombuttons.c
 */
1,//do_zoom_center 660
0,//do_initial_zoom_fit 661
};


/*
 * prototypes (because of struct Global header recursion issues
 */
void *calloc_2_into_context(struct Global *g, size_t nmemb, size_t size);


/*
 * context manipulation functions
 */
struct Global *initialize_globals(void) 
{
struct Global *g = calloc(1,sizeof(struct Global));	/* allocate viewer context */

memcpy(g, &globals_base_values, sizeof(struct Global));	/* fill in the blanks */

g->buf_menu_c_1 = calloc_2_into_context(g, 1, 65537);	/* do remaining mallocs into new ctx */
g->regexp_string_menu_c_1 = calloc_2_into_context(g, 1, 129);
g->regex_ok_regex_c_1 = calloc_2_into_context(g, WAVE_REGEX_TOTAL, sizeof(int));
g->preg_regex_c_1 = calloc_2_into_context(g, WAVE_REGEX_TOTAL, sizeof(regex_t));

return(g);						/* what to do with ctx is at discretion of caller */
}
