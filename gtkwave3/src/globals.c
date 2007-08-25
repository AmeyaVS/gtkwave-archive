/*
 * Copyright (c) Kermin Elliott Fleming 2007.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

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
0, /* ae2_num_sections */
NULL, /* ae2_lx2_table */
NULL, /* ae2_f */
NULL, /* ae2 */
NULL, /* ae2_fr */
NULL, /* ae2_process_mask */
0, /* ae2_msg_suppress */
NULL, /* ae2_regex_head */
0, /* ae2_regex_matches */
0, /* ae2_twirl_pos */
0, /* ae2_did_twirl */


/*
 * analyzer.c
 */
TR_RJUSTIFY, /* default_flags 5 */
{0, 0, 0, 0, 0, 0, 0}, /* tims 6 */
{0, 0, NULL, NULL, NULL, NULL, 0}, /* traces 7 */
0, /* hier_max_level 8 */


/*
 * baseconvert.c
 */
0, /* color_active_in_filter 9 */


/* 
 * bsearch.c
 */
LLDescriptor(0), /* shift_timebase 10 */
LLDescriptor(0), /* shift_timebase_default_for_add 11 */
0, /* max_compare_time_tc_bsearch_c_1 12 */
0, /* max_compare_pos_tc_bsearch_c_1 13 */
0, /* max_compare_time_bsearch_c_1 14 */
0, /* max_compare_pos_bsearch_c_1 15 */
0, /* max_compare_index 16 */
0, /* vmax_compare_time_bsearch_c_1 17 */
0, /* vmax_compare_pos_bsearch_c_1 18 */
0, /* vmax_compare_index 19 */
0, /* maxlen_trunc 20 */
0, /* maxlen_trunc_pos_bsearch_c_1 21 */
0, /* trunc_asciibase_bsearch_c_1 22 */


/*
 * busy.c
 */
NULL, /* busycursor_busy_c_1 23 */
0, /* busy_busy_c_1 24 */


/*
 * color.c
 */ 
-1, /* color_back 25 */
-1, /* color_baseline 26 */
-1, /* color_grid 27 */
-1, /* color_high 28 */
-1, /* color_low 29 */
-1, /* color_mark 30 */
-1, /* color_mid 31 */
-1, /* color_time 32 */
-1, /* color_timeb 33 */
-1, /* color_trans 34 */
-1, /* color_umark 35 */
-1, /* color_value 36 */
-1, /* color_vbox 37 */
-1, /* color_vtrans 38 */
-1, /* color_x 39 */
-1, /* color_xfill 40 */
-1, /* color_0 41 */
-1, /* color_1 42 */
-1, /* color_ufill 43 */
-1, /* color_u 44 */
-1, /* color_wfill 45 */
-1, /* color_w 46 */
-1, /* color_dashfill 47 */
-1, /* color_dash 48 */
-1, /* color_white 49 */
-1, /* color_black 50 */
-1, /* color_ltgray 51 */
-1, /* color_normal 52 */
-1, /* color_mdgray 53 */
-1, /* color_dkgray 54 */
-1, /* color_dkblue 55 */


/*
 * currenttime.c
 */
0, /* is_vcd 56 */
0, /* partial_vcd */
1, /* use_maxtime_display 57 */
0, /* use_frequency_delta 58 */
NULL, /* max_or_marker_label_currenttime_c_1 59 */
NULL, /* base_or_curtime_label_currenttime_c_1 60 */
0, /* cached_currenttimeval_currenttime_c_1 61 */
0, /* currenttime 62 */
0, /* max_time 63 */
-1, /* min_time 64 */
~0, /* display_grid 65 */
1, /* time_scale 66 */
'n', /* time_dimension 67 */
0, /* maxtimewid_currenttime_c_1 69 */
0, /* curtimewid_currenttime_c_1 70 */
0, /* maxtext_currenttime_c_1 71 */
0, /* curtext_currenttime_c_1 72 */
1, /* time_trunc_val_currenttime_c_1 76 */
0, /* use_full_precision 77 */


/*
 * debug.c
 */
NULL, /* alloc2_chain */
0, /* outstanding */
NULL, /* atoi_cont_ptr 78 */
0, /* disable_tooltips 79 */


/*
 * entry.c
 */
0, /* window_entry_c_1 80 */
0, /* entry_entry_c_1 81 */
NULL, /* entrybox_text 82 */
0, /* cleanup_entry_c_1 83 */


/*
 * fetchbuttons.c
 */
100, /* fetchwindow 84 */


/*
 * fgetdynamic.c
 */
0, /* fgetmalloc_len 85 */


/*
 * file.c
 */
0, /* fs_file_c_1 86 */
NULL, /* fileselbox_text 87 */
0, /* filesel_ok 88 */
0, /* cleanup_file_c_2 89 */
0, /* bad_cleanup_file_c1 */


/*
 * fonts.c
 */
NULL, /* fontname_signals 90 */
NULL, /* fontname_waves 91 */


/*
 * ghw.c
 */
0, /* nxp_ghw_c_1 93 */
NULL, /* sym_head_ghw_c_1 94 */
NULL,
0, /* sym_which_ghw_c_1 95 */
NULL, /* gwt_ghw_c_1 96 */
NULL, /* gwt_corr_ghw_c_1 97 */
1, /* xlat_1164_ghw_c_1 98 */
0, /* is_ghw 99 */
NULL, /* asbuf */
0, /* nbr_sig_ref_ghw_c_1 101 */
0, /* num_glitches_ghw_c_1 102 */
0, /*  num_glitch_regions_ghw_c_1 */
{0, 0 }, /* dummy_en_ghw_c_1 103 */
0, /* fac_name_ghw_c_1 104 */
0, /* fac_name_len_ghw_c_1 105 */
0, /* fac_name_max_ghw_c_1 106 */
0, /* last_fac_ghw_c_1 107 */
0, /* warned_ghw_c_1 108 */


/*
 * help.c
 */
0, /* helpbox_is_active 110 */
0, /* text_help_c_1 111 */
0, /* vscrollbar_help_c_1 112 */
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
{0}, /* iter_help_c_1 113 */
0, /* bold_tag_help_c_1 114 */
#endif
0, /* window_help_c_2 115 */


/*
 * hiersearch.c
 */
1, /* hier_grouping 116 */
0, /* window_hiersearch_c_3 117 */
0, /* entry_main_hiersearch_c_1 118 */
0, /* clist_hiersearch_c_1 119 */
0, /* bundle_direction_hiersearch_c_1 120 */
0, /* cleanup_hiersearch_c_3 121 */
0, /* num_rows_hiersearch_c_1 122 */
0, /* selected_rows_hiersearch_c_1 123 */
0, /* window1_hiersearch_c_1 124 */
0, /* entry_hiersearch_c_2 125 */
NULL, /* entrybox_text_local_hiersearch_c_1 126 */
NULL, /* cleanup_e_hiersearch_c_1 127 */
NULL, /* h_selectedtree_hiersearch_c_1 128 */
NULL, /* current_tree_hiersearch_c_1 129 */
NULL, /* treechain_hiersearch_c_1 130 */
0, /* is_active_hiersearch_c_1 131 */


/* 
 * logfile.c
 */
NULL, /* fontname_logfile 133 */
NULL, /* font_logfile_c_1 134 */
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
{0}, /* iter_logfile_c_2 135 */
NULL, /* bold_tag_logfile_c_2 136 */
NULL, /* mono_tag_logfile_c_1 137 */
NULL, /* size_tag_logfile_c_1 138 */
#endif


/*
 * lx2.c
 */
LXT2_IS_INACTIVE, /* is_lx2 139 */
NULL, /* lx2_lx2_c_1 140 */
0, /* first_cycle_lx2_c_1 141 */
0, /* last_cycle */
0, /* total_cycles */
NULL, /* lx2_table_lx2_c_1 142 */
NULL, /* mvlfacs_lx2_c_1 143 */
0, /* busycnt_lx2_c_1 144 */


/*
 * lxt.c
 */
NULL, /* mm_lxt_mmap_addr */
0, /* mm_lxt_mmap_len */
#if defined __MINGW32__ || defined _MSC_VER
NULL, /* HANDLE hIn */
NULL, /* HANDLE hInMap */
NULL, /* char *win_fname = NULL; */
#endif
0, /* fpos_lxt_c_1 145 */
0, /* is_lxt 146 */
0, /* lxt_clock_compress_to_z 147 */
NULL, /* mm_lxt_c_1 148 */
NULL, /* mmcache_lxt_c_1 */
0, /* version_lxt_c_1 149 */
NULL, /* mvlfacs_lxt_c_2 150 */
0, /* first_cycle_lxt_c_2 151 */
0, /* last_cycle */
0, /* total_cycles */
0, /* maxchange_lxt_c_1 152 */
0, /* maxindex */
0, /* f_len_lxt_c_1 153 */
NULL, /* positional_information_lxt_c_1 154 */
NULL, /* time_information 155 */
0, /* change_field_offset_lxt_c_1 156 */
0, /* facname_offset_lxt_c_1 157 */
0, /* facgeometry_offset_lxt_c_1 158 */
0, /* time_table_offset_lxt_c_1 159 */
0, /* time_table_offset64_lxt_c_1 160 */
0, /* sync_table_offset_lxt_c_1 161 */
0, /* initial_value_offset_lxt_c_1 162 */
0, /* timescale_offset_lxt_c_1 163 */
0, /* double_test_offset_lxt_c_1 164 */
0, /* zdictionary_offset_lxt_c_1 165 */
0, /* zfacname_predec_size_lxt_c_1 166 */
0, /* zfacname_size_lxt_c_1 167 */
0, /* zfacgeometry_size_lxt_c_1 168 */
0, /* zsync_table_size_lxt_c_1 169 */
0, /* ztime_table_size_lxt_c_1 170 */
0, /* zchg_predec_size_lxt_c_1 171 */
0, /* zchg_size_lxt_c_1 172 */
0, /* zdictionary_predec_size_lxt_c_1 173 */
AN_X, /* initial_value_lxt_c_1 174 */
0, /* dict_num_entries_lxt_c_1 175 */
0, /* dict_string_mem_required_lxt_c_1 176 */
0, /* dict_16_offset_lxt_c_1 177 */
0, /* dict_24_offset_lxt_c_1 178 */
0, /* dict_32_offset_lxt_c_1 179 */
0, /* dict_width_lxt_c_1 180 */
NULL, /* dict_string_mem_array_lxt_c_1 181 */
0, /* exclude_offset_lxt_c_1 182 */
NULL, /* lt_buf_lxt_c_1 183 */
0, /* lt_len_lxt_c_1 184 */
0, /* fd_lxt_c_1 185 */
{0,0,0,0,0,0,0,0}, /* double_mask_lxt_c_1 186 */
0, /* double_is_native_lxt_c_1 187 */
0, /* max_compare_time_tc_lxt_c_2 189 */
0, /* max_compare_pos_tc_lxt_c_2 */


/* 
 * main.c
 */
NULL, /* loaded_file_name */
NULL, /* unoptimized_vcd_file_name  */
NULL, /* skip_start */
NULL, /* skip_end */
NULL, /* indirect_fname */
NO_FILE, /* loaded_file_type */
NULL, /* whoami 190 */
NULL, /* logfile 191 */
NULL, /* stems_name 192 */
WAVE_ANNO_NONE, /* stems_type 193 */
NULL, /* aet_name 194 */
NULL, /* anno_ctx 195 */
NULL, /* dual_ctx 196 */
0, /* dual_id 197 */
0, /* dual_attach_id_main_c_1 198 */
0, /* dual_race_lock 199 */
NULL, /* mainwindow 200 */
NULL, /* signalwindow 201 */
NULL, /* wavewindow 202 */
NULL, /* toppanedwindow 203 */
NULL, /* sstpane 204 */
NULL, /* expanderwindow 205 */
0, /* disable_window_manager 206 */
1, /* paned_pack_semantics 207 */
0, /* zoom_was_explicitly_set 208 */
800, /* initial_window_x 209 */
400, /* initial_window_y */
-1, /* initial_window_width */
-1, /* initial_window_height 210 */
0, /* xy_ignore_main_c_1 211 */
0, /* optimize_vcd 212 */
1, /* num_cpus 213 */
-1, /* initial_window_xpos 214 */
-1, /* initial_window_ypos 214 */
0, /* initial_window_set_valid 215 */
-1, /* initial_window_xpos_set 216 */
-1, /* initial_window_ypos_set */
0, /* initial_window_get_valid 217 */
-1, /* initial_window_xpos_get 218 */
-1, /* initial_window_ypos_get 218 */
0, /* xpos_delta 219 */
0, /* ypos_delta 219 */
0, /* use_scrollbar_only 220 */
0, /* force_toolbars 221 */
0, /* hide_sst 222 */
1, /* sst_expanded 223 */
0, /* socket_xid 224 */
0, /* disable_menus 225 */
NULL, /* ftext_main_main_c_1 226 */


/*
 * markerbox.c
 */
NULL, /* window_markerbox_c_4 231 */
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, /* entries_markerbox_c_1 232 */
NULL, /* cleanup_markerbox_c_4 233 */
0, /* dirty_markerbox_c_1 234 */
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, /* shadow_markers_markerbox_c_1 235 */


/*
 * menu.c
 */ 
0, /* enable_fast_exit 236 */
NULL, /* script_handle 237 */
0, /* ignore_savefile_pos 238 */
0, /* ignore_savefile_size 239 */
NULL, /* item_factory_menu_c_1 241 */
NULL, /* regexp_string_menu_c_1 242 */
NULL, /* trace_to_alias_menu_c_1 243 */
NULL, /* showchangeall_menu_c_1 244 */
NULL, /* filesel_newviewer_menu_c_1 245 */
NULL, /* filesel_logfile_menu_c_1 246 */
NULL, /* filesel_writesave 247 */
0, /* save_success_menu_c_1 248 */
NULL, /* filesel_vcd_writesave 249 */
NULL, /* filesel_lxt_writesave 250 */
0, /* lock_menu_c_1 251 */
0, /* lock_menu_c_2 252 */
NULL, /* buf_menu_c_1 253 128 */

/*
 * mouseover.c
 */
1, /* disable_mouseover 254 */
NULL, /* mouseover_mouseover_c_1 255 */
NULL, /* mo_area_mouseover_c_1 256 */
NULL, /* mo_pixmap_mouseover_c_1 257 */
NULL, /* mo_dk_gray_mouseover_c_1 258 */
NULL, /* mo_black_mouseover_c_1 259 */
0, /* mo_width_mouseover_c_1 260 */
0, /* mo_height_mouseover_c_1 260 */


/*
 * pagebuttons.c
 */
1.0, /* page_divisor 261 */


/*
 * pixmaps.c
 */
NULL, /* larrow_pixmap 263 */
NULL, /* larrow_mask 264 */
NULL, /* rarrow_pixmap 266 */
NULL, /* rarrow_mask 267 */
NULL, /* zoomin_pixmap 269 */
NULL, /* zoomin_mask 270 */
NULL, /* zoomout_pixmap 272 */
NULL, /* zoomout_mask 273 */
NULL, /* zoomfit_pixmap 275 */
NULL, /* zoomfit_mask 276 */
NULL, /* zoomundo_pixmap 278 */
NULL, /* zoomundo_mask 279 */
NULL, /* zoom_larrow_pixmap 281 */
NULL, /* zoom_larrow_mask 282 */
NULL, /* zoom_rarrow_pixmap 284 */
NULL, /* zoom_rarrow_mask 285 */
NULL, /* prev_page_pixmap 287 */
NULL, /* prev_page_mask 288 */
NULL, /* next_page_pixmap 290 */
NULL, /* next_page_mask 291 */
NULL, /* wave_info_pixmap 293 */
NULL, /* wave_info_mask 294 */
NULL, /* wave_alert_pixmap 296 */
NULL, /* wave_alert_mask 297 */


/*
 * print.c
 */
72, /* inch_print_c_1 298 */
1.0, /* ps_chwidth_print_c_1 299 */
0, /* ybound_print_c_1 300 */
0, /* pr_signal_fill_width_print_c_1 301 */
0, /* ps_nummaxchars_print_c_1 302 */
1, /* ps_fullpage 303 */
66, /* ps_maxveclen 304 */
0, /* liney_max 305 */


/*
 * ptranslate.c
 */
0, /* current_translate_proc 308 */
0, /* current_filter_ptranslate_c_1 309 */
0, /* num_proc_filters 310 */
NULL, /* procsel_filter 311 */
NULL, /* proc_filter 312 */
0, /* is_active_ptranslate_c_2 313 */
NULL, /* fcurr_ptranslate_c_1 314 */
NULL, /* window_ptranslate_c_5 315 */
NULL, /* clist_ptranslate_c_2 316 */


/*
 * rc.c
 */
0, /* rc_line_no 318 */
1, /* possibly_use_rc_defaults 319 */


/*
 * regex.c
 */
NULL, /* preg_regex_c_1 321 */
NULL, /* regex_ok_regex_c_1 322 */


/*
 * renderopt.c
 */
0, /* is_active_renderopt_c_3 323 */
0, /* window_renderopt_c_6 324 */
NULL, /* filesel_print_ps_renderopt_c_1 325 */
NULL, /* filesel_print_mif_renderopt_c_1 326 */
{0,0}, /* target_mutex_renderopt_c_1 328 */
{0,0,0,0,0}, /* page_mutex_renderopt_c_1 330 */
{0,0}, /* render_mutex_renderopt_c_1 332 */
0, /* page_size_type_renderopt_c_1 333 */


/*
 * search.c
 */
{NULL,NULL,NULL,NULL,NULL}, /* menuitem_search */
NULL, /* window1_search_c_2 340 */
NULL, /* entry_a_search_c_1 341 */
NULL, /* entrybox_text_local_search_c_2 342 */
NULL, /* cleanup_e_search_c_2 343 */
NULL, /* pdata 344 */
0, /* is_active_search_c_4 345 */
0, /* is_insert_running_search_c_1 346 */
0, /* is_replace_running_search_c_1 347 */
0, /* is_append_running_search_c_1 348 */
0, /* is_searching_running_search_c_1 349 */
{0,0,0,0,0}, /* regex_mutex_search_c_1 352 */
0, /* regex_which_search_c_1 353 */
NULL, /* window_search_c_7 354 */
NULL, /* entry_search_c_3 355 */
NULL, /* clist_search_c_3 356 */
NULL, /* searchbox_text_search_c_1 358 */
0, /* bundle_direction_search_c_2 359 */
NULL, /* cleanup_search_c_5 360 */
0, /* num_rows_search_c_2 361 */
0, /* selected_rows_search_c_2 362 */


/*
 * showchange.c
 */
NULL, /* button1_showchange_c_1 363 */
NULL, /* button1_showchange_c_2 363 */
NULL, /* button1_showchange_c_3 363 */
NULL, /* button1_showchange_c_4 363 */
NULL, /* button1_showchange_c_5 363 */
NULL, /* button1_showchange_c_6 363 */
NULL, /* toggle1_showchange_c_1 364 */
NULL, /* toggle2_showchange_c_1 364 */
NULL, /* toggle3_showchange_c_1 364 */
NULL, /* toggle4_showchange_c_1 364 */
NULL, /* window_showchange_c_8 365 */
NULL, /* cleanup_showchange_c_6 366 */
NULL, /* tcache_showchange_c_1 367 */
0, /* flags_showchange_c_1 368 */


/* 
 * signalwindow.c
 */
NULL, /* signalarea 369 */
NULL, /* signalfont 370 */
NULL, /* signalpixmap 371 */
0, /* max_signal_name_pixel_width 372 */
0, /* signal_pixmap_width 373 */
0, /* signal_fill_width 374 */
0, /* old_signal_fill_width 375 */
0, /* old_signal_fill_height */
1, /* fontheight 376 */
0, /* dnd_state 377 */
NULL, /* hscroll_signalwindow_c_1 378 */
NULL, /* signal_hslider 379 */
0, /* cachedhiflag_signalwindow_c_1 380 */
-1, /* cachedwhich_signalwindow_c_1 381 */
NULL, /* cachedtrace 382 */
NULL, /* shift_click_trace 383 */
0, /* trtarget_signalwindow_c_1 384 */


/*
 * simplereq.c
 */
NULL, /* window_simplereq_c_9 385 */
NULL, /* cleanup 386 */


/*
 * splash.c
 */ 
1, /* splash_disable 387 */
NULL, /* wave_splash_pixmap 389 */
NULL, /* wave_splash_mask 390 */
NULL, /* splash_splash_c_1 391 */
NULL, /* darea_splash_c_1 392 */
NULL, /* gt_splash_c_1 393 */
0, /* timeout_tag 394 */
0, /* load_complete_splash_c_1 395 */
2, /* cnt_splash_c_1 396 */
0, /* prev_bar_x_splash_c_1 397 */


/*
 * status.c
 */
NULL, /* text_status_c_2 398 */
NULL, /* vscrollbar_status_c_2 399 */
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
{0}, /* iter_status_c_3 400 */
NULL, /* bold_tag_status_c_3 401 */
#endif


/*
 * strace.c
 */
NULL, /* timearray 402 */
0, /* timearray_size 403 */
NULL, /* ptr_mark_count_label_strace_c_1 404 */
NULL, /* straces 405 */
NULL, /* shadow_straces 406 */
NULL, /* strace_defer_free_head 407 */
NULL, /* window_strace_c_10 408 */
NULL, /* cleanup_strace_c_7 409 */
{0,0,0,0,0,0}, /* logical_mutex 412 */
{0,0,0,0,0,0}, /* shadow_logical_mutex 413 */
0, /* shadow_active 414 */
0, /* shadow_type 415 */
NULL, /* shadow_string 416 */
0, /* mark_idx_start 417 */
0, /* mark_idx_end 418 */
0, /* shadow_mark_idx_start 419 */
0, /* shadow_mark_idx_end 420 */
NULL, /* mprintf_buff_head 423 */
NULL, /* mprintf_buff_current */


/*
 * symbol.c
 */
NULL, /* sym 424 */
NULL, /* facs 425 */
0, /* facs_are_sorted 426 */
0, /* numfacs 427 */
0, /* regions 428 */
0, /* longestname 429 */
NULL, /* firstnode 430 */
NULL, /* curnode 431 */
0, /* hashcache 432 */


/*
 * timeentry.c
 */
NULL, /* from_entry 433 */
NULL, /* to_entry */


/*
 * translate.c
 */
0, /* current_translate_file 434 */
0, /* current_filter_translate_c_2 435 */
0, /* num_file_filters 436 */
NULL, /* filesel_filter 437 */
NULL, /* xl_file_filter 438 */
0, /* is_active_translate_c_5 439 */
NULL, /* fcurr_translate_c_2 440 */
NULL, /* window_translate_c_11 441 */
NULL, /* clist_translate_c_4 442 */


/*
 * tree.c
 */
NULL, /* treeroot 443 */
NULL, /* module_tree_c_1 444 */
0, /* module_len_tree_c_1 445 */
NULL, /* terminals_tchain_tree_c_1 446 */
'.', /* hier_delimeter 447 */
0, /* hier_was_explicitly_set 448 */
0x00, /* alt_hier_delimeter 449 */
1, /* fast_tree_sort 450 */
NULL, /* facs2_tree_c_1 451 */
0, /* facs2_pos_tree_c_1 452 */


/*
 * treesearch_gtk1.c
 */
NULL, /* GtkWidget *window1_treesearch_gtk1_c;  */
NULL, /* GtkWidget *entry_a_treesearch_gtk1_c;  */
NULL, /* char *entrybox_text_local_treesearch_gtk1_c;  */
NULL, /* void (*cleanup_e_treesearch_gtk1_c)(); */
NULL, /* struct tree *selectedtree_treesearch_gtk1_c;  */
0, /* int is_active_treesearch_gtk1_c;  */
NULL, /* GtkWidget *window_treesearch_gtk1_c;  */
NULL, /* GtkWidget *tree_treesearch_gtk1_c; */
0, /* char bundle_direction_treesearch_gtk1_c;  */
NULL, /* void (*cleanup_treesearch_gtk1_c)(); */


/*
 * treesearch_gtk2.c
 */
NULL, /* treesearch_gtk2_window_vbox */
NULL, /* selected_hierarchy_name */
NULL, /* gtk2_tree_frame */
NULL, /* filter_entry */
NULL, /* any_tree_node */
NULL, /* open_tree_nodes */
0, /* autoname_bundles 453 */
NULL, /* window1_treesearch_gtk2_c_3 454 */
NULL, /* entry_a_treesearch_gtk2_c_2 455 */
NULL, /* entrybox_text_local_treesearch_gtk2_c_3 456 */
NULL, /* cleanup_e_treesearch_gtk2_c_3 457 */
NULL, /* sig_root_treesearch_gtk2_c_1 458 */
NULL, /* filter_str_treesearch_gtk2_c_1 459 */
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
NULL, /* sig_store_treesearch_gtk2_c_1 460 */
NULL, /* sig_selection_treesearch_gtk2_c_1 461 */
#endif
0, /* is_active_treesearch_gtk2_c_6 462 */
NULL, /* ctree_main 463 */
NULL, /* afl_treesearch_gtk2_c_1 464 */
NULL, /* window_treesearch_gtk2_c_12 465 */
NULL, /* tree_treesearch_gtk2_c_1 466 */
0, /* bundle_direction_treesearch_gtk2_c_3 467 */
NULL, /* cleanup_treesearch_gtk2_c_8 468 */
0, /* pre_import_treesearch_gtk2_c_1 469 */
{0,0,0,NULL,NULL,NULL,0}, /* tcache_treesearch_gtk2_c_2 470 */
0, /* dnd_tgt_on_signalarea_treesearch_gtk2_c_1 471 */


/*
 * vcd.c
 */
NULL, /* vcd_jmp_buf */
-1, /* vcd_warning_filesize 472 */
1, /* autocoalesce 473 */
0, /* autocoalesce_reversal */
-1, /* vcd_explicit_zero_subscripts 474 */
0, /* convert_to_reals 475 */
1, /* atomic_vectors 476 */
0, /* make_vcd_save_file 477 */
0, /* vcd_preserve_glitches 478 */
NULL, /* vcd_save_handle 479 */
NULL, /* vcd_handle_vcd_c_1 480 */
0, /* vcd_is_compressed_vcd_c_1 481 */
0, /* vcdbyteno_vcd_c_1 482 */
0, /* error_count_vcd_c_1 483 */
0, /* header_over_vcd_c_1 484 */
0, /* dumping_off_vcd_c_1 485 */
-1, /* start_time_vcd_c_1 486 */
-1, /* end_time_vcd_c_1 487 */
-1, /* current_time_vcd_c_1 488 */
0, /* num_glitches_vcd_c_2 489 */
0, /* num_glitch_regions_vcd_c_2 490 */
{0, 0}, /* vcd_hier_delimeter 491 */
NULL, /* pv_vcd_c_1 492 */
NULL, /* rootv_vcd_c_1 */
NULL, /* vcdbuf_vcd_c_1 493 */
NULL, /* vst */
NULL, /* vend */
0, /* escaped_names_found_vcd_c_1 494 */
NULL, /* slistroot 495 */
NULL, /* slistcurr */
NULL, /* slisthier 496 */
0, /* slisthier_len 497x */
1024, /* T_MAX_STR_vcd_c_1 499 */
NULL, /* yytext_vcd_c_1 500 */
0, /* yylen_vcd_c_1 501 */
0, /* yylen_cache */
NULL, /* vcdsymroot_vcd_c_1 502 */
NULL, /* vcdsymcurr */
NULL, /* sorted_vcd_c_1 503 */
NULL, /* indexed_vcd_c_1 504 */
0, /* numsyms_vcd_c_1 505 */
NULL, /* he_curr_vcd_c_1 506 */
NULL, /* he_fini */
NULL, /* queuedevents_vcd_c_1 507 */
~0, /* vcd_minid_vcd_c_1 508 */
0, /* vcd_maxid_vcd_c_1 509 */
0, /* err_vcd_c_1 510 */
0, /* vcd_fsiz_vcd_c_1 511 */
NULL, /* varsplit_vcd_c_1 512 */
NULL, /* varsplitcurr */
0, /* var_prevch_vcd_c_1 513 */


/*
 * vcd_partial.c
 */
0, /* vcdbyteno_vcd_partial_c_2 516 */
0, /* error_count_vcd_partial_c_2 517 */
0, /* header_over_vcd_partial_c_2 518 */
0, /* dumping_off_vcd_partial_c_2 519 */
-1, /* start_time_vcd_partial_c_2 520 */
-1, /* end_time_vcd_partial_c_2 521 */
-1, /* current_time_vcd_partial_c_2 522 */
0, /* num_glitches_vcd_partial_c_3 523 */
0, /* num_glitch_regions_vcd_partial_c_3 524 */
NULL, /* pv_vcd_partial_c_2 525 */
NULL, /* rootv */
NULL, /* vcdbuf_vcd_partial_c_2 526 */
NULL, /* vst */
NULL, /* vend */
NULL, /* consume_ptr_vcd_partial_c_1 527 */
NULL, /* buf_vcd_partial_c_2 528 */
100000, /* consume_countdown_vcd_partial_c_1 529 */
1024, /* T_MAX_STR_vcd_partial_c_2 531 */
NULL, /* yytext_vcd_partial_c_2 532 */
0, /* yylen_vcd_partial_c_2 533 */
0, /* yylen_cache */
NULL, /* vcdsymroot_vcd_partial_c_2 534 */
NULL, /* vcdsymcurr */
NULL, /* sorted_vcd_partial_c_2 535 */
NULL, /* indexed_vcd_partial_c_2 536 */
0, /* numsyms_vcd_partial_c_2 538 */
NULL, /* queuedevents_vcd_partial_c_2 539 */
~0, /* vcd_minid_vcd_partial_c_2 540 */
0, /* vcd_maxid_vcd_partial_c_2 541 */
0, /* err_vcd_partial_c_2 542 */
NULL, /* varsplit_vcd_partial_c_2 543 */
NULL, /* vsplitcurr */
0, /* var_prevch_vcd_partial_c_2 544 */
0, /* timeset_vcd_partial_c_1 547 */


/*
 * vcd_recoder.c
 */
NULL, /* time_vlist_vcd_recoder_c_1 548 */
0, /* time_vlist_count_vcd_recoder_c_1 549 */
NULL, /* vcd_handle_vcd_recoder_c_2 550 */
0, /* vcd_is_compressed_vcd_recoder_c_2 551 */
0, /* vcdbyteno_vcd_recoder_c_3 552 */
0, /* error_count_vcd_recoder_c_3 553 */
0, /* header_over_vcd_recoder_c_3 554 */
0, /* dumping_off_vcd_recoder_c_3 555 */
-1, /* start_time_vcd_recoder_c_3 556 */
-1, /* end_time_vcd_recoder_c_3 557 */
-1, /* current_time_vcd_recoder_c_3 558 */
0, /* num_glitches_vcd_recoder_c_4 559 */
0, /* num_glitch_regions_vcd_recoder_c_4 560 */
NULL, /* pv_vcd_recoder_c_3 561 */
NULL, /* rootv */
NULL, /* vcdbuf_vcd_recoder_c_3 562 */
NULL, /* vst */
NULL, /* vend */
1024, /* T_MAX_STR_vcd_recoder_c_3 564 */
NULL, /* yytext_vcd_recoder_c_3 565 */
0, /* yylen_vcd_recoder_c_3 566 */
0, /* yylen_cache */
NULL, /* vcdsymroot_vcd_recoder_c_3 567 */
NULL, /* vcdsymcurr */
NULL, /* sorted_vcd_recoder_c_3 568 */
NULL, /* indexed_vcd_recoder_c_3 569 */
0, /* numsyms_vcd_recoder_c_3 570 */
~0, /* vcd_minid_vcd_recoder_c_3 571 */
0, /* vcd_maxid_vcd_recoder_c_3 572 */
0, /* err_vcd_recoder_c_3 573 */
0, /* vcd_fsiz_vcd_recoder_c_2 574 */
NULL, /* varsplit_vcd_recoder_c_3 575 */
NULL, /* vsplitcurr */
0, /* var_prevch_vcd_recoder_c_3 576 */


/*
 * vcd_saver.c
 */
NULL, /* f_vcd_saver_c_1 579 */
{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}, /* buf_vcd_saver_c_3 580 */
NULL, /* hp_vcd_saver_c_1 581 */
NULL, /* nhold_vcd_saver_c_1 582 */


/*
 * vlist.c
 */
4, /* vlist_compression_depth 583 */


/*
 * vzt.c
 */
NULL, /* vzt_vzt_c_1 584 */
0, /* first_cycle_vzt_c_3 585 */
0, /* last_cycle */
0, /* total_cycles */
NULL, /* vzt_table_vzt_c_1 586 */
NULL, /* mvlfacs_vzt_c_3 587 */
0, /* busycnt_vzt_c_2 588 */


/*
 * wavewindow.c
 */
0, /* m1x_wavewindow_c_1 589 */
0, /* m2x */
1, /* signalwindow_width_dirty 590 */
1, /* enable_ghost_marker 591 */
1, /* enable_horiz_grid 592 */
1, /* enable_vert_grid 593 */
0, /* use_big_fonts 594 */
0, /* use_nonprop_fonts */
~0, /* do_resize_signals 595 */
0, /* constant_marker_update 596 */
0, /* use_roundcaps 597 */
~0, /* show_base 598 */
~0, /* wave_scrolling 599 */
4, /* vector_padding 600 */
0, /* in_button_press_wavewindow_c_1 601 */
0, /* left_justify_sigs 602 */
0, /* zoom_pow10_snap 603 */
0, /* cursor_snap 604 */
-1.0, /* old_wvalue 605 */
NULL, /* blackout_regions 606 */
0, /* zoom 607 */
1, /* scale */
1, /* nsperframe */
1, /* pixelsperframe 608 */
1.0, /* hashstep 609 */
-1, /* prevtim_wavewindow_c_1 610 */
1.0, /* pxns 611 */
1.0, /* nspx */
2.0, /* zoombase 612 */
NULL, /* topmost_trace 613 */
1, /* waveheight 614 */
0, /* wavecrosspiec */
1, /* wavewidth 615 */
NULL, /* wavefont 616 */
NULL, /* wavefont_smaller 617 */
NULL, /* wavearea 618 */
NULL, /* vscroll_wavewindow_c_1 619 */
NULL, /* hscroll_wavewindow_c_2 620 */
NULL, /* wavepixmap_wavewindow_c_1 621 */
NULL, /* wave_vslider 622 */
NULL, /* wave_hslider */
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, /* named_markers 623 */
0, /* made_gc_contexts_wavewindow_c_1 624 */
NULL, /* gc_back_wavewindow_c_1 625 */
NULL, /* gc_baseline_wavewindow_c_1 626 */
NULL, /* gc_grid_wavewindow_c_1 627 */
NULL, /* gc_time_wavewindow_c_1 628 */
NULL, /* gc_timeb_wavewindow_c_1 629 */
NULL, /* gc_value_wavewindow_c_1 630 */
NULL, /* gc_low_wavewindow_c_1 631 */
NULL, /* gc_high_wavewindow_c_1 632 */
NULL, /* gc_trans_wavewindow_c_1 633 */
NULL, /* gc_mid_wavewindow_c_1 634 */
NULL, /* gc_xfill_wavewindow_c_1 635 */
NULL, /* gc_x_wavewindow_c_1 636 */
NULL, /* gc_vbox_wavewindow_c_1 637 */
NULL, /* gc_vtrans_wavewindow_c_1 638 */
NULL, /* gc_mark_wavewindow_c_1 639 */
NULL, /* gc_umark_wavewindow_c_1 640 */
NULL, /* gc_0_wavewindow_c_1 641 */
NULL, /* gc_1_wavewindow_c_1 642 */
NULL, /* gc_ufill_wavewindow_c_1 643 */
NULL, /* gc_u_wavewindow_c_1 644 */
NULL, /* gc_wfill_wavewindow_c_1 645 */
NULL, /* gc_w_wavewindow_c_1 646 */
NULL, /* gc_dashfill_wavewindow_c_1 647 */
NULL, /* gc_dash_wavewindow_c_1 648 */
0, /* made_sgc_contexts_wavewindow_c_1 649 */
NULL, /* gc_white 650 */
NULL, /* gc_black 651 */
NULL, /* gc_ltgray 652 */
NULL, /* gc_normal 653 */
NULL, /* gc_mdgray 654 */
NULL, /* gc_dkgray 655 */
NULL, /* gc_dkblue 656 */
0, /* fill_in_smaller_rgb_areas_wavewindow_c_1 659 */


/*
 * zoombuttons.c
 */
1, /* do_zoom_center 660 */
0, /* do_initial_zoom_fit 661 */
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


/*
 * reload from old into the new context
 */
void reload_into_new_context(void)
{
  FILE *statefile;
  struct Global *new_globals, *setjmp_globals;
  int i;
  gint tree_frame_x = -1, tree_frame_y = -1;
  gdouble tree_vadj_value = 0.0;
  gdouble tree_hadj_value = 0.0;
  int fix_from_time = 0, fix_to_time = 0;
  TimeType from_time, to_time;
  char timestr[32];
  struct stringchain_t *hier_head = NULL, *hier_curr = NULL;
  int load_was_success = 0;
  int reload_fail_delay = 1;


 if(GLOBALS->text_status_c_2)
	{
	gtk_grab_add(GLOBALS->text_status_c_2);
	while (gtk_events_pending()) gtk_main_iteration();
	gtk_grab_remove(GLOBALS->text_status_c_2);
	}

 printf("GTKWAVE | Reloading waveform...\n");

 /* Save state to file */
 statefile = fopen(".temp","wb");
 if(statefile == NULL) {
   fprintf(stderr, "Failed to reload file.\n");
   return;
 }

 write_save_helper(statefile);
 fclose(statefile);

 /* save off size of tree frame if active */
#if WAVE_USE_GTK2
 if(GLOBALS->gtk2_tree_frame)
	{
	GtkCList *cl = GTK_CLIST(GLOBALS->ctree_main);
	GtkAdjustment *vadj = gtk_clist_get_vadjustment(cl);
	GtkAdjustment *hadj = gtk_clist_get_hadjustment(cl);

	tree_vadj_value = vadj->value;
	tree_hadj_value = hadj->value;

	tree_frame_x = (GLOBALS->gtk2_tree_frame)->allocation.width;
	tree_frame_y = (GLOBALS->gtk2_tree_frame)->allocation.height;
	}
#endif

 /* Kill any open processes */
 remove_all_proc_filters();

 /* Instantiate new global status */
 new_globals = initialize_globals();
 
 /* Time to copy over state */
 /* Marker positions */
 for(i = 0; i < 26; i++) {
   new_globals->named_markers[i] = GLOBALS->named_markers[i];
 }


 /* Default colors, X contexts, pixmaps, drawables, etc */

 new_globals->signalarea = GLOBALS->signalarea;
 new_globals->wavearea = GLOBALS->wavearea;
 new_globals->wavepixmap_wavewindow_c_1 = GLOBALS->wavepixmap_wavewindow_c_1;
 new_globals->signalpixmap = GLOBALS->signalpixmap;
 new_globals->wave_splash_pixmap = GLOBALS->wave_splash_pixmap;
 new_globals->wave_splash_mask = GLOBALS->wave_splash_mask;

 new_globals->gc_white = GLOBALS->gc_white;
 new_globals->gc_black = GLOBALS->gc_black;
 new_globals->gc_ltgray = GLOBALS->gc_ltgray;
 new_globals->gc_normal = GLOBALS->gc_normal;
 new_globals->gc_mdgray = GLOBALS->gc_mdgray;
 new_globals->gc_dkgray = GLOBALS->gc_dkgray;
 new_globals->gc_dkblue = GLOBALS->gc_dkblue;
 new_globals->made_sgc_contexts_wavewindow_c_1 = GLOBALS->made_sgc_contexts_wavewindow_c_1;

 new_globals->gc_back_wavewindow_c_1 = GLOBALS->gc_back_wavewindow_c_1;
 new_globals->gc_baseline_wavewindow_c_1 = GLOBALS->gc_baseline_wavewindow_c_1;
 new_globals->gc_grid_wavewindow_c_1 = GLOBALS->gc_grid_wavewindow_c_1;
 new_globals->gc_time_wavewindow_c_1 = GLOBALS->gc_time_wavewindow_c_1;
 new_globals->gc_timeb_wavewindow_c_1 = GLOBALS->gc_timeb_wavewindow_c_1;
 new_globals->gc_value_wavewindow_c_1 = GLOBALS->gc_value_wavewindow_c_1;
 new_globals->gc_low_wavewindow_c_1 = GLOBALS->gc_low_wavewindow_c_1;
 new_globals->gc_high_wavewindow_c_1 = GLOBALS->gc_high_wavewindow_c_1;
 new_globals->gc_trans_wavewindow_c_1 = GLOBALS->gc_trans_wavewindow_c_1;
 new_globals->gc_mid_wavewindow_c_1 = GLOBALS->gc_mid_wavewindow_c_1;
 new_globals->gc_xfill_wavewindow_c_1 = GLOBALS->gc_xfill_wavewindow_c_1;
 new_globals->gc_x_wavewindow_c_1 = GLOBALS->gc_x_wavewindow_c_1;
 new_globals->gc_vbox_wavewindow_c_1 = GLOBALS->gc_vbox_wavewindow_c_1;
 new_globals->gc_vtrans_wavewindow_c_1 = GLOBALS->gc_vtrans_wavewindow_c_1;
 new_globals->gc_mark_wavewindow_c_1 = GLOBALS->gc_mark_wavewindow_c_1;
 new_globals->gc_umark_wavewindow_c_1 = GLOBALS->gc_umark_wavewindow_c_1;
 new_globals->gc_0_wavewindow_c_1 = GLOBALS->gc_0_wavewindow_c_1;
 new_globals->gc_1_wavewindow_c_1 = GLOBALS->gc_1_wavewindow_c_1;
 new_globals->gc_ufill_wavewindow_c_1 = GLOBALS->gc_ufill_wavewindow_c_1;
 new_globals->gc_u_wavewindow_c_1 = GLOBALS->gc_u_wavewindow_c_1;
 new_globals->gc_wfill_wavewindow_c_1 = GLOBALS->gc_wfill_wavewindow_c_1;
 new_globals->gc_w_wavewindow_c_1 = GLOBALS->gc_w_wavewindow_c_1;
 new_globals->gc_dashfill_wavewindow_c_1 = GLOBALS->gc_dashfill_wavewindow_c_1;
 new_globals->gc_dash_wavewindow_c_1 = GLOBALS->gc_dash_wavewindow_c_1;
 new_globals->made_gc_contexts_wavewindow_c_1 = GLOBALS->made_gc_contexts_wavewindow_c_1;


 new_globals->mainwindow = GLOBALS->mainwindow;
 new_globals->signalwindow = GLOBALS->signalwindow; 
 new_globals->wavewindow = GLOBALS->wavewindow;
 new_globals->toppanedwindow = GLOBALS->toppanedwindow;
 new_globals->sstpane = GLOBALS->sstpane;
 new_globals->expanderwindow = GLOBALS->expanderwindow;
 new_globals->signal_hslider = GLOBALS->signal_hslider;
 new_globals->wave_vslider = GLOBALS->wave_vslider;
 new_globals->wave_hslider = GLOBALS->wave_hslider;
 new_globals->signalfont = GLOBALS->signalfont;
 new_globals->wavefont = GLOBALS->wavefont;
 new_globals->max_or_marker_label_currenttime_c_1 = GLOBALS->max_or_marker_label_currenttime_c_1;
 new_globals->maxtext_currenttime_c_1 = (char *) calloc_2_into_context(new_globals,1,40);
 memcpy(new_globals->maxtext_currenttime_c_1, GLOBALS->maxtext_currenttime_c_1,40); 
 new_globals->maxtimewid_currenttime_c_1 = GLOBALS->maxtimewid_currenttime_c_1;
 new_globals->curtext_currenttime_c_1 = (char *) calloc_2_into_context(new_globals,1,40);
 memcpy(new_globals->curtext_currenttime_c_1, GLOBALS->curtext_currenttime_c_1, 40);
 new_globals->base_or_curtime_label_currenttime_c_1 = GLOBALS->base_or_curtime_label_currenttime_c_1;
 new_globals->curtimewid_currenttime_c_1 = GLOBALS->curtimewid_currenttime_c_1;
 new_globals->from_entry = GLOBALS->from_entry;
 new_globals->to_entry = GLOBALS->to_entry;

 /* busy.c */
 new_globals->busycursor_busy_c_1 = GLOBALS->busycursor_busy_c_1;
 new_globals->busy_busy_c_1 = GLOBALS->busy_busy_c_1;

 /* pixmaps.c */
 new_globals->larrow_pixmap = GLOBALS->larrow_pixmap;
 new_globals->rarrow_pixmap = GLOBALS->rarrow_pixmap;
 new_globals->zoomout_pixmap = GLOBALS->zoomout_pixmap;
 new_globals->zoomin_pixmap = GLOBALS->zoomin_pixmap;
 new_globals->zoomfit_pixmap = GLOBALS->zoomfit_pixmap;
 new_globals->zoomundo_pixmap = GLOBALS->zoomundo_pixmap;
 new_globals->zoom_larrow_pixmap = GLOBALS->zoom_larrow_pixmap;
 new_globals->zoom_rarrow_pixmap = GLOBALS->zoom_rarrow_pixmap;
 new_globals->prev_page_pixmap = GLOBALS->prev_page_pixmap;
 new_globals->next_page_pixmap = GLOBALS->next_page_pixmap;
 new_globals->wave_info_pixmap = GLOBALS->wave_info_pixmap;
 new_globals->wave_alert_pixmap = GLOBALS->wave_alert_pixmap;

 /* rc.c */
 new_globals->ignore_savefile_pos = 1; /* to keep window from resizing/jumping */
 new_globals->ignore_savefile_size = 1; /* to keep window from resizing/jumping */

 new_globals->color_back = GLOBALS->color_back;
 new_globals->color_baseline = GLOBALS->color_baseline;
 new_globals->color_grid = GLOBALS->color_grid;
 new_globals->color_high = GLOBALS->color_high;
 new_globals->color_low = GLOBALS->color_low;
 new_globals->color_1 = GLOBALS->color_1;
 new_globals->color_0 = GLOBALS->color_0;
 new_globals->color_mark = GLOBALS->color_mark;
 new_globals->color_mid = GLOBALS->color_mid;
 new_globals->color_time = GLOBALS->color_time;
 new_globals->color_timeb = GLOBALS->color_timeb;
 new_globals->color_trans = GLOBALS->color_trans;
 new_globals->color_umark = GLOBALS->color_umark;
 new_globals->color_value = GLOBALS->color_value;
 new_globals->color_vbox = GLOBALS->color_vbox;
 new_globals->color_vtrans = GLOBALS->color_vtrans;
 new_globals->color_x = GLOBALS->color_x;
 new_globals->color_xfill = GLOBALS->color_xfill;
 new_globals->color_u = GLOBALS->color_u;
 new_globals->color_ufill = GLOBALS->color_ufill;
 new_globals->color_w = GLOBALS->color_w;
 new_globals->color_wfill = GLOBALS->color_wfill;
 new_globals->color_dash = GLOBALS->color_dash;
 new_globals->color_dashfill = GLOBALS->color_dashfill;
 new_globals->color_white = GLOBALS->color_white;
 new_globals->color_black = GLOBALS->color_black;
 new_globals->color_ltgray = GLOBALS->color_ltgray;
 new_globals->color_normal = GLOBALS->color_normal;
 new_globals->color_mdgray = GLOBALS->color_mdgray;
 new_globals->color_dkgray = GLOBALS->color_dkgray;
 new_globals->color_dkblue = GLOBALS->color_dkblue;

 new_globals->atomic_vectors = GLOBALS->atomic_vectors;
 new_globals->autoname_bundles = GLOBALS->autoname_bundles;
 new_globals->autocoalesce = GLOBALS->autocoalesce;
 new_globals->autocoalesce_reversal = GLOBALS->autocoalesce_reversal;
 new_globals->constant_marker_update = GLOBALS->constant_marker_update;
 new_globals->convert_to_reals = GLOBALS->convert_to_reals;
 new_globals->disable_mouseover = GLOBALS->disable_mouseover;
 new_globals->disable_tooltips = GLOBALS->disable_tooltips;
 new_globals->do_initial_zoom_fit = GLOBALS->do_initial_zoom_fit;
 new_globals->do_resize_signals = GLOBALS->do_resize_signals;
 new_globals->enable_fast_exit = GLOBALS->enable_fast_exit;
 new_globals->enable_ghost_marker = GLOBALS->enable_ghost_marker;
 new_globals->enable_horiz_grid = GLOBALS->enable_horiz_grid;
 new_globals->make_vcd_save_file = GLOBALS->make_vcd_save_file;
 new_globals->enable_vert_grid = GLOBALS->enable_vert_grid;
 new_globals->force_toolbars = GLOBALS->force_toolbars;
 new_globals->hide_sst = GLOBALS->hide_sst;
 new_globals->sst_expanded = GLOBALS->sst_expanded;
 new_globals->hier_grouping = GLOBALS->hier_grouping;
 new_globals->hier_max_level = GLOBALS->hier_max_level;
 new_globals->paned_pack_semantics = GLOBALS->paned_pack_semantics;
 new_globals->left_justify_sigs = GLOBALS->left_justify_sigs;
 new_globals->lxt_clock_compress_to_z = GLOBALS->lxt_clock_compress_to_z;
 new_globals->ps_maxveclen = GLOBALS->ps_maxveclen;
 new_globals->show_base = GLOBALS->show_base;
 new_globals->display_grid = GLOBALS->display_grid;
 new_globals->use_big_fonts = GLOBALS->use_big_fonts;
 new_globals->use_full_precision = GLOBALS->use_full_precision;
 new_globals->use_frequency_delta = GLOBALS->use_frequency_delta;
 new_globals->use_maxtime_display = GLOBALS->use_maxtime_display;
 new_globals->use_nonprop_fonts = GLOBALS->use_nonprop_fonts;
 new_globals->use_roundcaps = GLOBALS->use_roundcaps;
 new_globals->use_scrollbar_only = GLOBALS->use_scrollbar_only;
 new_globals->vcd_explicit_zero_subscripts = GLOBALS->vcd_explicit_zero_subscripts;
 new_globals->vcd_preserve_glitches = GLOBALS->vcd_preserve_glitches;
 new_globals->vcd_warning_filesize = GLOBALS->vcd_warning_filesize;
 new_globals->vector_padding = GLOBALS->vector_padding;
 new_globals->vlist_compression_depth = GLOBALS->vlist_compression_depth;
 new_globals->wave_scrolling = GLOBALS->wave_scrolling;
 new_globals->do_zoom_center = GLOBALS->do_zoom_center;
 new_globals->zoom_pow10_snap = GLOBALS->zoom_pow10_snap;
 new_globals->alt_hier_delimeter = GLOBALS->alt_hier_delimeter;
 new_globals->cursor_snap = GLOBALS->cursor_snap;
 new_globals->hier_delimeter = GLOBALS->hier_delimeter;
 new_globals->hier_was_explicitly_set = GLOBALS->hier_was_explicitly_set;
 new_globals->page_divisor = GLOBALS->page_divisor;
 new_globals->ps_maxveclen = GLOBALS->ps_maxveclen;
 new_globals->vector_padding = GLOBALS->vector_padding;
 new_globals->vlist_compression_depth = GLOBALS->vlist_compression_depth;
 new_globals->zoombase = GLOBALS->zoombase;
 new_globals->splash_disable = 1; /* to disable splash for reload */


 if(GLOBALS->fontname_logfile)
	{
  	new_globals->fontname_logfile = calloc_2_into_context(new_globals, 1, strlen(GLOBALS->fontname_logfile) + 1);
  	strcpy(new_globals->fontname_logfile, GLOBALS->fontname_logfile);
	}

 if(GLOBALS->fontname_signals)
	{
  	new_globals->fontname_signals = calloc_2_into_context(new_globals, 1, strlen(GLOBALS->fontname_signals) + 1);
  	strcpy(new_globals->fontname_signals, GLOBALS->fontname_signals);
	}

 if(GLOBALS->fontname_waves)
	{
  	new_globals->fontname_waves = calloc_2_into_context(new_globals, 1, strlen(GLOBALS->fontname_waves) + 1);
  	strcpy(new_globals->fontname_waves, GLOBALS->fontname_waves);
	}

 /* lxt2.c / vzt.c / ae2.c */
 if(GLOBALS->skip_start)
	{
  	new_globals->skip_start = calloc_2_into_context(new_globals, 1, strlen(GLOBALS->skip_start) + 1);
  	strcpy(new_globals->skip_start, GLOBALS->skip_start);
	}
 if(GLOBALS->skip_end)
	{
  	new_globals->skip_end = calloc_2_into_context(new_globals, 1, strlen(GLOBALS->skip_end) + 1);
  	strcpy(new_globals->skip_end, GLOBALS->skip_end);
	}
 if(GLOBALS->indirect_fname)
	{
  	new_globals->indirect_fname = calloc_2_into_context(new_globals, 1, strlen(GLOBALS->indirect_fname) + 1);
  	strcpy(new_globals->indirect_fname, GLOBALS->indirect_fname);
	}

 /* main.c */
 new_globals->optimize_vcd = GLOBALS->optimize_vcd;

 /* menu.c */
 new_globals->item_factory_menu_c_1 = GLOBALS->item_factory_menu_c_1;

 /* status.c */
 new_globals->text_status_c_2 = GLOBALS->text_status_c_2;
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
 memcpy(&new_globals->iter_status_c_3, &GLOBALS->iter_status_c_3, sizeof(GtkTextIter));
#endif

 /* treesearch_gtk2.c */
 new_globals->treesearch_gtk2_window_vbox = GLOBALS->treesearch_gtk2_window_vbox;
 new_globals->window_treesearch_gtk2_c_12 = GLOBALS->window_treesearch_gtk2_c_12;

 if(GLOBALS->filter_str_treesearch_gtk2_c_1)
	{
	new_globals->filter_str_treesearch_gtk2_c_1 = calloc_2_into_context(new_globals, 1, strlen(GLOBALS->filter_str_treesearch_gtk2_c_1) + 1);
	strcpy(new_globals->filter_str_treesearch_gtk2_c_1, GLOBALS->filter_str_treesearch_gtk2_c_1);
	}
  if(GLOBALS->selected_hierarchy_name)
	{
	new_globals->selected_hierarchy_name = calloc_2_into_context(new_globals, 1, strlen(GLOBALS->selected_hierarchy_name) + 1);
	strcpy(new_globals->selected_hierarchy_name, GLOBALS->selected_hierarchy_name);
	}

 /* timeentry.c */
 new_globals->from_entry = GLOBALS->from_entry;
 new_globals->to_entry = GLOBALS->to_entry;

 if(GLOBALS->tims.first != GLOBALS->min_time)
	{
	fix_from_time = 1;
	from_time = GLOBALS->tims.first;
	}

 if(GLOBALS->tims.last != GLOBALS->max_time)
	{
	fix_to_time = 1;
	to_time = GLOBALS->tims.last;
	}

 /* twinwave stuff */
 new_globals->dual_attach_id_main_c_1 = GLOBALS->dual_attach_id_main_c_1;
 new_globals->dual_id = GLOBALS->dual_id;
 new_globals->socket_xid = GLOBALS->socket_xid;
 new_globals->dual_ctx = GLOBALS->dual_ctx;
  
 /* Times struct */
 memcpy(&(new_globals->tims), &(GLOBALS->tims), sizeof(Times));

 /* File name and type */
 new_globals->loaded_file_type = GLOBALS->loaded_file_type;
 new_globals->loaded_file_name = calloc_2_into_context(new_globals,1,strlen(GLOBALS->loaded_file_name) + 1);  
 strcpy(new_globals->loaded_file_name, GLOBALS->loaded_file_name);

 if(new_globals->optimize_vcd) {
   new_globals->unoptimized_vcd_file_name = calloc_2_into_context(new_globals,1,strlen(GLOBALS->unoptimized_vcd_file_name) + 1);  
   strcpy(new_globals->unoptimized_vcd_file_name, GLOBALS->unoptimized_vcd_file_name);
 }


 /* deallocate any loader-related stuff */
 switch(GLOBALS->loaded_file_type) {
   case LXT_FILE:
                if(GLOBALS->mm_lxt_mmap_addr)
                        {
                        munmap(GLOBALS->mm_lxt_mmap_addr, GLOBALS->mm_lxt_mmap_len);
                        }
                break;
        
   case LX2_FILE: lxt2_rd_close(GLOBALS->lx2_lx2_c_1); break;
   case VZT_FILE: vzt_rd_close(GLOBALS->vzt_vzt_c_1); break;
   case AE2_FILE: 
#ifdef AET2_IS_PRESENT
	ae2_read_close(GLOBALS->ae2); fclose(GLOBALS->ae2_f); 
#endif
	break;
  
   case GHW_FILE:
   case VCD_FILE:
   case VCD_RECODER_FILE: /* do nothing */ break;
 }

 /* window destruction (of windows that aren't the parent window) */
 if(GLOBALS->window_entry_c_1)
        {
        gtk_grab_remove(GLOBALS->window_entry_c_1);
        gtk_widget_destroy(GLOBALS->window_entry_c_1);
        GLOBALS->window_entry_c_1 = NULL;
        }

 if(GLOBALS->window_help_c_2)
        {
        gtk_widget_destroy(GLOBALS->window_help_c_2);
        GLOBALS->window_help_c_2 = NULL;
        }
                         
 if(GLOBALS->window1_hiersearch_c_1)
        {
        gtk_grab_remove(GLOBALS->window1_hiersearch_c_1);
        gtk_widget_destroy(GLOBALS->window1_hiersearch_c_1);
        GLOBALS->window1_hiersearch_c_1 = NULL;
        }
 
 if(GLOBALS->window_hiersearch_c_3)
        {
	struct treechain *tc = GLOBALS->treechain_hiersearch_c_1;
	while(tc)
		{
		if(!hier_curr)
			{
			hier_head = hier_curr = calloc_2_into_context(new_globals,1,sizeof(struct stringchain_t));
			}
			else
			{
			hier_curr->next = calloc_2_into_context(new_globals,1,sizeof(struct stringchain_t));
			hier_curr = hier_curr->next;
			}

		hier_curr->name = calloc_2_into_context(new_globals,1,strlen(tc->label->name) + 1);
		strcpy(hier_curr->name, tc->label->name);

		tc = tc->next;
		}

	new_globals->window_hiersearch_c_3 = GLOBALS->window_hiersearch_c_3;
	new_globals->entry_main_hiersearch_c_1 = GLOBALS->entry_main_hiersearch_c_1;
	new_globals->clist_hiersearch_c_1 = GLOBALS->clist_hiersearch_c_1;
	new_globals->bundle_direction_hiersearch_c_1 = GLOBALS->bundle_direction_hiersearch_c_1;
	new_globals->cleanup_hiersearch_c_3 = GLOBALS->cleanup_hiersearch_c_3;
	new_globals->is_active_hiersearch_c_1 = GLOBALS->is_active_hiersearch_c_1;
        }
 
 if(GLOBALS->window_markerbox_c_4)
        {
        gtk_grab_remove(GLOBALS->window_markerbox_c_4);
        gtk_widget_destroy(GLOBALS->window_markerbox_c_4);
        GLOBALS->window_markerbox_c_4 = NULL;
        }
         
 if(GLOBALS->mouseover_mouseover_c_1)
        {
        gtk_widget_destroy(GLOBALS->mouseover_mouseover_c_1); GLOBALS->mouseover_mouseover_c_1 = NULL;
        gdk_pixmap_unref(GLOBALS->mo_pixmap_mouseover_c_1);   GLOBALS->mo_pixmap_mouseover_c_1 = NULL;
        }
         
 if(GLOBALS->window_ptranslate_c_5)
        {
        gtk_widget_destroy(GLOBALS->window_ptranslate_c_5);
        GLOBALS->window_ptranslate_c_5 = NULL;
        }
        
 if(GLOBALS->window_renderopt_c_6)
        {
        gtk_widget_destroy(GLOBALS->window_renderopt_c_6);
        GLOBALS->window_ptranslate_c_5 = NULL;
        }

 if(GLOBALS->window1_search_c_2)
        {
        gtk_grab_remove(GLOBALS->window1_search_c_2);
        gtk_widget_destroy(GLOBALS->window1_search_c_2);
        GLOBALS->window1_search_c_2 = NULL;
        }
        
 if(GLOBALS->window_search_c_7)
        {
	int i;

	new_globals->pdata = calloc_2_into_context(new_globals, 1, sizeof(SearchProgressData));
	memcpy(new_globals->pdata, GLOBALS->pdata, sizeof(SearchProgressData));

	new_globals->is_active_search_c_4 = GLOBALS->is_active_search_c_4;
	new_globals->regex_which_search_c_1 = GLOBALS->regex_which_search_c_1;
	new_globals->window_search_c_7 = GLOBALS->window_search_c_7;
	new_globals->entry_search_c_3 = GLOBALS->entry_search_c_3;
	new_globals->clist_search_c_3 = GLOBALS->clist_search_c_3;
	if(GLOBALS->searchbox_text_search_c_1)
		{
  		new_globals->searchbox_text_search_c_1 = calloc_2_into_context(new_globals, 1, strlen(GLOBALS->searchbox_text_search_c_1) + 1);
  		strcpy(new_globals->searchbox_text_search_c_1, GLOBALS->searchbox_text_search_c_1);
		}

	for(i=0;i<5;i++)
		{
		new_globals->menuitem_search[i] = GLOBALS->menuitem_search[i];
		new_globals->regex_mutex_search_c_1[i] = GLOBALS->regex_mutex_search_c_1[i];
		}
        }
        
 if(GLOBALS->window_showchange_c_8)
        {
        gtk_grab_remove(GLOBALS->window_showchange_c_8);
        gtk_widget_destroy(GLOBALS->window_showchange_c_8);
        GLOBALS->window_showchange_c_8 = NULL;
        }
         
 if(GLOBALS->window_simplereq_c_9)
        {
        gtk_grab_remove(GLOBALS->window_simplereq_c_9);
        gtk_widget_destroy(GLOBALS->window_simplereq_c_9);
        GLOBALS->window_simplereq_c_9 = NULL; 
        }

 if(GLOBALS->window_strace_c_10)
        {
        gtk_widget_destroy(GLOBALS->window_strace_c_10);
        GLOBALS->window_strace_c_10 = NULL;
        }
         
 if(GLOBALS->window_translate_c_11)
        {
        gtk_widget_destroy(GLOBALS->window_translate_c_11);
        GLOBALS->window_translate_c_11 = NULL;
        }
         
 if(GLOBALS->window_treesearch_gtk1_c)
	{
        gtk_widget_destroy(GLOBALS->window_treesearch_gtk1_c);
        GLOBALS->window_treesearch_gtk1_c = NULL;
	}    
     
 if(GLOBALS->window1_treesearch_gtk1_c)
        {
        gtk_grab_remove(GLOBALS->window1_treesearch_gtk1_c);
        gtk_widget_destroy(GLOBALS->window1_treesearch_gtk1_c);
        GLOBALS->window1_treesearch_gtk1_c = NULL;
        }

 if(GLOBALS->window1_treesearch_gtk2_c_3)
        {
        gtk_grab_remove(GLOBALS->window1_treesearch_gtk2_c_3);
        gtk_widget_destroy(GLOBALS->window1_treesearch_gtk2_c_3);
        GLOBALS->window1_treesearch_gtk2_c_3 = NULL;
        }

 /* Free the context */
 free_outstanding();

 /* Free the old globals struct, memsetting it to zero in the hope of forcing crashes. */
 memset(GLOBALS, 0, sizeof(struct Global));
 free(GLOBALS);

 /* Set the GLOBALS pointer to the newly allocated struct. */
 GLOBALS = new_globals;

 /* Initialize new variables */
 GLOBALS->sym=(struct symbol **)calloc_2(SYMPRIME,sizeof(struct symbol *));

 init_proctrans_data();
 load_all_fonts();

 for(;;)
	{
 /* Check to see if we need to reload a vcd file */
#if !defined _MSC_VER && !defined __MINGW32__
	if(GLOBALS->optimize_vcd) 
		{
	   	optimize_vcd_file();
	 	}
#endif
	/* Load new file from disk, no reload on partial vcd or vcd from stdin. */
	switch(GLOBALS->loaded_file_type) 
		{
   		case LX2_FILE: 
			lx2_main(GLOBALS->loaded_file_name,GLOBALS->skip_start,GLOBALS->skip_end);
			load_was_success = (GLOBALS->lx2_lx2_c_1 != NULL);
			break;

   		case VZT_FILE: 
			vzt_main(GLOBALS->loaded_file_name,GLOBALS->skip_start,GLOBALS->skip_end); 
			load_was_success = (GLOBALS->vzt_vzt_c_1 != NULL);
			break;

   		case AE2_FILE: 
			ae2_main(GLOBALS->loaded_file_name,GLOBALS->skip_start,GLOBALS->skip_end,GLOBALS->indirect_fname);
			load_was_success = (GLOBALS->ae2 != NULL);
			break;

   		case GHW_FILE: 
			load_was_success = (ghw_main(GLOBALS->loaded_file_name) != 0);
			break;

		case LXT_FILE:
   		case VCD_FILE: 
		case VCD_RECODER_FILE: 
			GLOBALS->vcd_jmp_buf = calloc(1, sizeof(jmp_buf));
			setjmp_globals =  calloc(1,sizeof(struct Global));      /* allocate yet another copy of viewer context */
			memcpy(setjmp_globals, GLOBALS, sizeof(struct Global));	/* clone */
			GLOBALS->alloc2_chain = NULL;				/* will merge this in after load if successful */
			GLOBALS->outstanding = 0;				/* zero out count of chunks in this ctx */

			if(!setjmp(*(GLOBALS->vcd_jmp_buf)))			/* loader exception handling */
				{
				void **t, **t2;

				switch(GLOBALS->loaded_file_type)		/* on fail, longjmp called in these loaders */
					{			
			   		case LXT_FILE: lxt_main(GLOBALS->loaded_file_name); break;
			   		case VCD_FILE: vcd_main(GLOBALS->loaded_file_name); break;
					case VCD_RECODER_FILE: vcd_recoder_main(GLOBALS->loaded_file_name); break;
					}
				
				t = (void **)setjmp_globals->alloc2_chain;
				while(t)
					{
					t2 = (void **) *(t+1);
					if(t2)
						{
						t = t2;
						}
						else
						{
						*(t+1) = GLOBALS->alloc2_chain;
						if(GLOBALS->alloc2_chain)
							{
							t2 = (void **)GLOBALS->alloc2_chain;
							*(t2+0) = t;
							}
						GLOBALS->alloc2_chain = setjmp_globals->alloc2_chain;
						GLOBALS->outstanding += setjmp_globals->outstanding;
						break;					
						}
					}
				free(GLOBALS->vcd_jmp_buf); GLOBALS->vcd_jmp_buf = NULL;
				free(setjmp_globals); setjmp_globals = NULL;

				load_was_success = 1;
				}
				else
				{
				free(GLOBALS->vcd_jmp_buf); GLOBALS->vcd_jmp_buf = NULL;
				if(GLOBALS->vcd_handle_vcd_c_1) { fclose(GLOBALS->vcd_handle_vcd_c_1); GLOBALS->vcd_handle_vcd_c_1 = NULL; }
				if(GLOBALS->vcd_handle_vcd_recoder_c_2) { fclose(GLOBALS->vcd_handle_vcd_recoder_c_2); GLOBALS->vcd_handle_vcd_recoder_c_2 =NULL; }
				if(GLOBALS->mm_lxt_mmap_addr)
					{                        
					munmap(GLOBALS->mm_lxt_mmap_addr, GLOBALS->mm_lxt_mmap_len); 
					GLOBALS->mm_lxt_mmap_addr = NULL;
					}                        
				free_outstanding(); /* free anything allocated in loader ctx */

				memcpy(GLOBALS, setjmp_globals, sizeof(struct Global)); /* copy over old ctx */
				free(setjmp_globals);					/* remove cached old ctx */
				/* now try again, jump through recovery sequence below */
				}
			break;
 		} 

	if(load_was_success)
		{
		break;
		}
		else
		{
		/* recovery sequence */
		printf("GTKWAVE | Reload failure, reattempt in %d second%s...\n", reload_fail_delay, (reload_fail_delay==1) ? "" : "s");
		fflush(stdout);
		sleep(reload_fail_delay);
		switch(reload_fail_delay)
			{
			case 1: reload_fail_delay = 2; break;
			case 2: reload_fail_delay = 5; break;
			case 5: reload_fail_delay = 10; break;
			case 10: reload_fail_delay = 30; break;
			case 30: reload_fail_delay = 60; break;
			default: break;
			}
		}
	}

 /* Setup timings we probably need to redraw here */
 GLOBALS->tims.last=GLOBALS->max_time;
 GLOBALS->tims.first=GLOBALS->min_time;

 if(fix_from_time)
	{
	if((from_time >= GLOBALS->min_time) && (from_time <= GLOBALS->max_time))
		{
		GLOBALS->tims.first = from_time;
		}
	}

 if(fix_to_time)
	{
	if((to_time >= GLOBALS->min_time) && (to_time <= GLOBALS->max_time) && (to_time > GLOBALS->tims.first))
		{
		GLOBALS->tims.last = to_time;
		}
	}

 if(GLOBALS->tims.start < GLOBALS->tims.first) {
   GLOBALS->tims.start = GLOBALS->tims.first;
 }

 if(GLOBALS->tims.end > GLOBALS->tims.last) {
   GLOBALS->tims.end = GLOBALS->tims.last;
 }

 if(GLOBALS->tims.marker < GLOBALS->tims.first) {
   GLOBALS->tims.marker = GLOBALS->tims.first;
 }

 if(GLOBALS->tims.marker > GLOBALS->tims.last) {
   GLOBALS->tims.marker = GLOBALS->tims.last;
 }

 if(GLOBALS->tims.prevmarker < GLOBALS->tims.first) {
   GLOBALS->tims.prevmarker = GLOBALS->tims.first;
 }

 if(GLOBALS->tims.prevmarker > GLOBALS->tims.last) {
   GLOBALS->tims.prevmarker = GLOBALS->tims.last;
 }

 if(GLOBALS->tims.laststart < GLOBALS->tims.first) {
   GLOBALS->tims.laststart = GLOBALS->tims.first;
 }

 if(GLOBALS->tims.laststart > GLOBALS->tims.last) {
   GLOBALS->tims.laststart = GLOBALS->tims.last;
 } 

 reformat_time(timestr, GLOBALS->tims.first, GLOBALS->time_dimension);
 gtk_entry_set_text(GTK_ENTRY(GLOBALS->from_entry),timestr);
 reformat_time(timestr, GLOBALS->tims.last, GLOBALS->time_dimension);
 gtk_entry_set_text(GTK_ENTRY(GLOBALS->to_entry),timestr);


 /* Change SST - if it exists */
 /* XXX need to destroy/free the old tree widgets. */
 #if GTK_CHECK_VERSION(2,4,0)
 if(!GLOBALS->hide_sst) {
   gtk_widget_hide(GLOBALS->expanderwindow);
   gtk_container_remove(GTK_CONTAINER(GLOBALS->expanderwindow), GLOBALS->sstpane);
   GLOBALS->sstpane = treeboxframe("SST", GTK_SIGNAL_FUNC(mkmenu_treesearch_cleanup));
   gtk_container_add(GTK_CONTAINER(GLOBALS->expanderwindow), GLOBALS->sstpane);
   gtk_widget_show(GLOBALS->expanderwindow);
 }
 #endif
 #if WAVE_USE_GTK2 
 if(GLOBALS->window_treesearch_gtk2_c_12)
	{
   	gtk_container_remove(GTK_CONTAINER(GLOBALS->window_treesearch_gtk2_c_12), GLOBALS->treesearch_gtk2_window_vbox);
   	treebox(NULL, GTK_SIGNAL_FUNC(mkmenu_treesearch_cleanup), GLOBALS->window_treesearch_gtk2_c_12);
	}

   if((GLOBALS->filter_str_treesearch_gtk2_c_1) && (GLOBALS->filter_entry))
	{
        gtk_entry_set_text(GTK_ENTRY(GLOBALS->filter_entry), GLOBALS->filter_str_treesearch_gtk2_c_1);
	wave_regex_compile(GLOBALS->filter_str_treesearch_gtk2_c_1, WAVE_REGEX_TREE);
	}
 #endif

 /* Reload state from file */
 read_save_helper(".temp"); 

 /* unlink temp */
 unlink(".temp");


 /* part 2 of SST (which needs to be done after the tree is expanded from loading the savefile...) */
 #if WAVE_USE_GTK2
 if((!GLOBALS->hide_sst)||(GLOBALS->window_treesearch_gtk2_c_12)) {
   GtkCList *cl = GTK_CLIST(GLOBALS->ctree_main);
   char *hiername_cache = NULL;	/* some strange race condition side effect in gtk selecting/deselecting blows this out so cache it */

   if(GLOBALS->selected_hierarchy_name)
	{
	hiername_cache = strdup_2(GLOBALS->selected_hierarchy_name);
   	select_tree_node(GLOBALS->selected_hierarchy_name);
	}

   if(tree_vadj_value != 0.0)
	{
	GtkAdjustment *vadj = gtk_clist_get_vadjustment(cl);

	if((tree_vadj_value >= vadj->lower) && (tree_vadj_value <= vadj->upper))
		{ 
		vadj->value = tree_vadj_value;
		gtk_clist_set_vadjustment(cl, vadj);

		gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(vadj)), "changed");
		gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(vadj)), "value_changed"); 
		}
	}

   if(tree_hadj_value != 0.0)
	{
	GtkAdjustment *hadj = gtk_clist_get_hadjustment(cl);

	if((tree_hadj_value >= hadj->lower) && (tree_hadj_value <= hadj->upper))
		{ 
		hadj->value = tree_hadj_value;
		gtk_clist_set_hadjustment(cl, hadj);

		gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(hadj)), "changed");
		gtk_signal_emit_by_name (GTK_OBJECT (GTK_ADJUSTMENT(hadj)), "value_changed"); 
		}
	}

    if(hiername_cache)
	{
	if(GLOBALS->selected_hierarchy_name)
		{
		free_2(GLOBALS->selected_hierarchy_name);
		}
	GLOBALS->selected_hierarchy_name = hiername_cache;
	}


   if(tree_frame_y != -1)
	{
	/* this doesn't work...this sets the *minimum* size */
	/* gtk_widget_set_size_request(GLOBALS->gtk2_tree_frame, -1, tree_frame_y); */
	}

 }
 #endif

 /* part 2 of search (which needs to be done after the new dumpfile is loaded) */
 if(GLOBALS->window_search_c_7)
        {
	search_enter_callback(GLOBALS->entry_search_c_3, NULL);
	}

 /* part 2 of hier search (which needs to be done after the new dumpfile is loaded) */
 if(GLOBALS->window_hiersearch_c_3)
        {
	if(!hier_curr)
        	{
        	GLOBALS->current_tree_hiersearch_c_1=GLOBALS->treeroot;
        	GLOBALS->h_selectedtree_hiersearch_c_1=NULL;
        	}
		else
		{
		struct tree *t = GLOBALS->treeroot;
		hier_curr = hier_head;

		while((hier_curr)&&(t))
			{
			if(!strcmp(hier_curr->name, t->name))
				{
			        if(t->child)
			                {
			                struct treechain *tc, *tc2;

			                tc=GLOBALS->treechain_hiersearch_c_1;
			                if(tc)
			                        {
			                        while(tc->next) tc=tc->next;

			                        tc2=calloc_2(1,sizeof(struct treechain));
			                        tc2->label=t;
			                        tc2->tree=GLOBALS->current_tree_hiersearch_c_1;
			                        tc->next=tc2;
			                        }
			                        else
			                        {
			                        GLOBALS->treechain_hiersearch_c_1=calloc_2(1,sizeof(struct treechain));
			                        GLOBALS->treechain_hiersearch_c_1->tree=GLOBALS->current_tree_hiersearch_c_1;
			                        GLOBALS->treechain_hiersearch_c_1->label=t;
			                        }
			
			                GLOBALS->current_tree_hiersearch_c_1=t->child;
			                }

				t = t->child;
				hier_curr = hier_curr->next;
				continue;
				}
			t = t->next;
			}

		hier_curr = hier_head;

		while(hier_head)
			{
			hier_head = hier_curr->next;
			free_2(hier_curr->name);
			free_2(hier_curr);
			hier_curr = hier_head;			
			}		
		}

	refresh_hier_tree(GLOBALS->current_tree_hiersearch_c_1);
	}

 printf("GTKWAVE | ...waveform reloaded\n");
}
