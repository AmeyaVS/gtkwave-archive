/* 
 * Copyright (c) Tony Bybell 1999-2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include <gtk/gtk.h>
#include <string.h>
#include "splay.h"
#include "vlex.h"
#include "jrb.h"
#include "wavelink.h"

void bwlogbox(char *title, int width, ds_Tree *t, int display_mode);

extern ds_Tree *flattened_mod_list_root;

/* only for use locally */
struct wave_logfile_lines_t
{
struct wave_logfile_lines_t *next;
int line_no, tok;
char *text;
};

struct logfile_context_t
{
ds_Tree *which;
int display_mode;
int width;
};

struct text_find_t
{
struct text_find_t *next;
GtkWidget *text, *window;
struct logfile_context_t *ctx;
};


struct text_find_t *text_root = NULL;
char *fontname_logfile = NULL;

/* Add some text to our text widget - this is a callback that is invoked
when our window is realized. We could also force our window to be
realized with gtk_widget_realize, but it would have to be part of
a hierarchy first */

static GdkFont *font = NULL;

#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
static GtkTextIter iter;
static GtkTextTag *bold_tag = NULL;
static GtkTextTag *dgray_tag = NULL, *lgray_tag = NULL;
static GtkTextTag *blue_tag = NULL, *fwht_tag = NULL;
static GtkTextTag *mono_tag = NULL;
static GtkTextTag *size_tag = NULL;
#endif


void log_text(GtkWidget *text, GdkFont *font, char *str)
{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_buffer_insert_with_tags (GTK_TEXT_VIEW (text)->buffer, &iter,  
                                 str, -1, mono_tag, size_tag, NULL);
#else
gtk_text_insert (GTK_TEXT (text), font, &text->style->black, NULL, str, -1);
#endif
}

void log_text_bold(GtkWidget *text, GdkFont *font, char *str)
{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_buffer_insert_with_tags (GTK_TEXT_VIEW (text)->buffer, &iter,  
                                 str, -1, bold_tag, mono_tag, size_tag, fwht_tag, blue_tag, NULL);
#else
gtk_text_insert (GTK_TEXT (text), font, &text->style->fg[GTK_STATE_SELECTED], &text->style->bg[GTK_STATE_SELECTED], str, -1);
#endif
}

void log_text_active(GtkWidget *text, GdkFont *font, char *str)
{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_buffer_insert_with_tags (GTK_TEXT_VIEW (text)->buffer, &iter,
                                 str, -1, dgray_tag, mono_tag, size_tag, NULL);
#else
gtk_text_insert (GTK_TEXT (text), font, &text->style->fg[GTK_STATE_ACTIVE], &text->style->bg[GTK_STATE_ACTIVE], str, -1);
#endif
}

void log_text_prelight(GtkWidget *text, GdkFont *font, char *str)
{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_buffer_insert_with_tags (GTK_TEXT_VIEW (text)->buffer, &iter,
                                 str, -1, lgray_tag, mono_tag, size_tag, NULL);
#else
gtk_text_insert (GTK_TEXT (text), font, &text->style->fg[GTK_STATE_PRELIGHT], &text->style->bg[GTK_STATE_PRELIGHT], str, -1);
#endif
}

static void
log_realize_text (GtkWidget *text, gpointer data)
{
/* nothing for now */
}


static int is_identifier(char ch)
{
int rc = ((ch>='a')&&(ch<='z')) || ((ch>='A')&&(ch<='Z')) || ((ch>='0')&&(ch<='9')) || (ch == '_') || (ch == '$');

return(rc);
}


static char *hexify(char *s)
{
int len = strlen(s);

if(len < 4) 
	{
	char *s2 = malloc(len+1+1);
	int idx;

	s2[0]='b';
	for(idx = 0; idx < len; idx++)
		{
		s2[idx+1] = toupper(s[idx]);
		}
	s2[idx+1] = 0;

	free(s);
	return(s2);
	}
	else
	{
	int skip = len & 3;
	int siz = ((len + 3) / 4) + 1;
	char *sorig = s;
	char *s2 = calloc(1, siz);
	int idx;
	char *pnt = s2;
	char arr[4];

	while(*s)
		{
		char isx, isz;
		int val;

		if(!skip)
			{
			arr[0] = toupper(*(s++));
			arr[1] = toupper(*(s++));
			arr[2] = toupper(*(s++));
			arr[3] = toupper(*(s++));
			}
			else
			{
			int j = 3;
			for(idx = skip-1; idx>=0; idx--)
				{
				arr[j] = toupper(s[idx]);
				j--;
				}
			for(idx = j; idx >= 0; idx--)
				{
				arr[idx] = ((arr[j+1] == 'X') || (arr[j+1] == 'Z')) ? arr[j+1] : '0';
				}

			s+=skip;
			skip = 0;
			}

		isx = isz = 0;
		val = 0;
		for(idx=0; idx<4; idx++)
			{
			val <<= 1;
			if(arr[idx] == '0') continue;
			if(arr[idx] == '1') { val |= 1; continue; }
			if(arr[idx] == 'Z') { isz++; continue; }
			isx++;
			}

		if(isx)
			{
			*(pnt++) = (isx==4) ? 'X' : 'x';	
			}
		else
		if(isz)
			{
			*(pnt++) = (isz==4) ? 'Z' : 'z';	
			}
		else
			{
			*(pnt++) = "0123456789ABCDEF"[val];
			}
		}

	free(sorig);
	return(s2);
	}
}


/* lxt2 iteration handling... (lxt2 doesn't have a direct "value at" function) */
static JRB lx2vals = NULL;

static void lx2_iter_fn(struct lxt2_rd_trace **lt, lxtint64_t *pnt_time, lxtint32_t *pnt_facidx, char **pnt_value)
{
if(*pnt_time <= anno_ctx->marker)
	{
	JRB node = jrb_find_int(lx2vals, *pnt_facidx);
	Jval jv;

	if(node)
		{
		free(node->val.s);
		node->val.s = strdup(*pnt_value);
		}
		else
		{
		jv.s = strdup(*pnt_value);
		jrb_insert_int(lx2vals, *pnt_facidx, jv);
		}
	}
}


static void
import_doubleclick(GtkWidget *text, char *s)
{
struct text_find_t *t = text_root;
char *s2;
ds_Tree *ft = flattened_mod_list_root;

while(t)
	{
	if(text == t->text)
		{
		break;
		}
	t = t->next;
	}

if(!t) return;

s2 = malloc(strlen(t->ctx->which->fullname) + 1 + strlen(s) + 1);
sprintf(s2, "%s.%s", t->ctx->which->fullname, s);

while(ft)
	{
	if(!strcmp(s2, ft->fullname))
		{
		bwlogbox(ft->fullname, 640 + 8*8, ft, 0);
		break;
		}

	ft = ft->next_flat;
	}

free(s2);
}


static gboolean
button_release_event (GtkWidget *text, GdkEventButton *event)
{
gchar *sel;

#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
GtkTextIter start;
GtkTextIter end;

if (gtk_text_buffer_get_selection_bounds (GTK_TEXT_VIEW(text)->buffer,
                                         &start, &end))
       {
       if(gtk_text_iter_compare (&start, &end) < 0)
               {
               sel = gtk_text_buffer_get_text(GTK_TEXT_VIEW(text)->buffer,
                                              &start, &end, FALSE);

		if(sel)
			{
			if(strlen(sel))
			{
			int i, len = strlen(sel);
			char *sel2;
			char ch;

			for(i=0;i<len;i++)
				{
				if(!is_identifier(sel[i])) goto bail;
				}

			while(gtk_text_iter_backward_char(&start))
				{
		               	sel2 = gtk_text_buffer_get_text(GTK_TEXT_VIEW(text)->buffer,
                                              &start, &end, FALSE);
				if(!sel2) break;
				ch = *sel2;
				g_free(sel2);
				if(!is_identifier(ch))
					{
					gtk_text_iter_forward_char(&start);
					break;
					}
				}

			gtk_text_iter_backward_char(&end);
			for(;;)
				{
				gtk_text_iter_forward_char(&end);
		               	sel2 = gtk_text_buffer_get_text(GTK_TEXT_VIEW(text)->buffer,
                                              	&start, &end, FALSE);
				if(!sel2) break;
				ch = *(sel2 + strlen(sel2) - 1);
				g_free(sel2);
				if(!is_identifier(ch))
					{
					gtk_text_iter_backward_char(&end);
					break;
					}
				}

		 	sel2 = gtk_text_buffer_get_text(GTK_TEXT_VIEW(text)->buffer,
						&start, &end, FALSE);


			/* oec->set_selection(oe, lft, rgh); */

			import_doubleclick(text, sel2);
			g_free(sel2);
			}
bail:			g_free(sel);
			}
		}
	}

#else

#ifndef WAVE_USE_GTK2
GtkEditable *oe = GTK_EDITABLE(&GTK_TEXT(text)->editable);
GtkTextClass *tc = (GtkTextClass *) ((GtkObject*) (GTK_OBJECT(text)))->klass;
GtkEditableClass *oec = &tc->parent_class;
#else
GtkOldEditable *oe = GTK_OLD_EDITABLE(&GTK_TEXT(text)->old_editable);
GtkOldEditableClass *oec = GTK_OLD_EDITABLE_GET_CLASS(oe);
#endif

if(oe->has_selection)
	{
	if(oec->get_chars)
		{
	 	sel = oec->get_chars(oe, oe->selection_start_pos, oe->selection_end_pos);
	
		if(sel)
			{
			if(strlen(sel))
			{
			int lft = oe->selection_start_pos, rgh = oe->selection_end_pos;
			int i, len = strlen(sel);
			char *sel2;
			char ch;

			for(i=0;i<len;i++)
				{
				if(!is_identifier(sel[i])) goto bail;
				}

			while(lft>=0)
				{
				sel2 = oec->get_chars(oe, lft-1, lft-1+1);
				if(!sel2) break;
				ch = *sel2;
				g_free(sel2);
				if(!is_identifier(ch))
					{
					break;
					}
				lft--;				
				}

			rgh--;
			for(;;)
				{
				rgh++;
				sel2 = oec->get_chars(oe, rgh, rgh+1);
				if(!sel2) break;
				ch = *sel2;
				g_free(sel2);
				if(!is_identifier(ch))
					{
					break;
					}
				}

		 	sel2 = oec->get_chars(oe, lft, rgh);
			oec->set_selection(oe, lft, rgh);

			import_doubleclick(text, sel2);
			g_free(sel2);
			}
bail:			g_free(sel);
			}
		}
	}

#endif

return(FALSE); /* call remaining handlers... */
}
   
/* Create a scrolled text area that displays a "message" */
static GtkWidget *create_log_text (GtkWidget **textpnt)
{
GtkWidget *text;
GtkWidget *table;
GtkWidget *vscrollbar;

/* Create a table to hold the text widget and scrollbars */
table = gtk_table_new (1, 16, FALSE);

/* Put a text widget in the upper left hand corner. Note the use of
* GTK_SHRINK in the y direction */
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
text = gtk_text_view_new ();
gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW (text)), &iter);
bold_tag = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (text)->buffer, "bold",
                                      	"weight", PANGO_WEIGHT_BOLD, NULL);
dgray_tag = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (text)->buffer, "dk_gray_background",
			      "background", "dark gray", NULL);
lgray_tag = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (text)->buffer, "lt_gray_background",
			      "background", "light gray", NULL);
fwht_tag = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (text)->buffer, "white_foreground",
			      "foreground", "white", NULL);
blue_tag = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (text)->buffer, "blue_background",
			      "background", "blue", NULL);
mono_tag = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (text)->buffer, "monospace", 
					"family", "monospace", NULL);
size_tag = gtk_text_buffer_create_tag (GTK_TEXT_VIEW (text)->buffer, "fsiz",
					"size", 10 * PANGO_SCALE, NULL);
#else                                  
text = gtk_text_new (NULL, NULL);
#endif
*textpnt = text;
gtk_table_attach (GTK_TABLE (table), text, 0, 14, 0, 1,
                        GTK_FILL | GTK_EXPAND,
                        GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
gtk_widget_set_usize(GTK_WIDGET(text), 100, 100);
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)  
gtk_text_view_set_editable(GTK_TEXT_VIEW(text), FALSE); 
#else
gtk_text_set_editable(GTK_TEXT(text), FALSE);
#endif
gtk_widget_show (text);

/* And a VScrollbar in the upper right */
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
{
GtkTextViewClass *tc = (GtkTextViewClass*)GTK_OBJECT_GET_CLASS(GTK_OBJECT(text));
 
tc->set_scroll_adjustments(GTK_TEXT_VIEW (text), NULL, NULL);
vscrollbar = gtk_vscrollbar_new (GTK_TEXT_VIEW (text)->vadjustment);  
}
#else                                  
vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
#endif
gtk_table_attach (GTK_TABLE (table), vscrollbar, 15, 16, 0, 1,
                        GTK_FILL, GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
gtk_widget_show (vscrollbar);

/* Add a handler to put a message in the text widget when it is realized */
gtk_signal_connect (GTK_OBJECT (text), "realize",
                        GTK_SIGNAL_FUNC (log_realize_text), NULL);

gtk_signal_connect(GTK_OBJECT(text), "button_release_event",
                       GTK_SIGNAL_FUNC(button_release_event), NULL);

#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_CHAR);
#else
gtk_text_set_word_wrap(GTK_TEXT(text), FALSE);
gtk_text_set_line_wrap(GTK_TEXT(text), TRUE);
#endif
return(table);
}
   
/***********************************************************************************/

static void ok_callback(GtkWidget *widget, struct logfile_context_t *ctx)
{
  bwlogbox(ctx->which->fullname, ctx->width, ctx->which, (ctx->display_mode == 0));
}

static void destroy_callback(GtkWidget *widget, gpointer dummy)
{
struct text_find_t *t = text_root, *tprev = NULL;
struct logfile_context_t *ctx = NULL;

while(t)
	{
	if(t->window==widget)
		{
		if(tprev)	/* prune out struct text_find_t */
			{
			tprev->next = t->next;
			ctx = t->ctx;
			free(t);
			break;
			}
			else
			{
			text_root = t->next;
			ctx = t->ctx;
			free(t);
			break;
			}
		}

	tprev = t;
	t = t->next;
	}

gtk_widget_destroy(widget);
if(ctx) free(ctx);
}

void bwlogbox(char *title, int width, ds_Tree *t, int display_mode)
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox, *button1;
    GtkWidget *label, *separator;
    GtkWidget *ctext;
    GtkWidget *text;
    FILE *handle;
    int lx;
    int lx_module_line = 0;
    int lx_module_line_locked = 0;
    int lx_endmodule_line_locked = 0;

    struct wave_logfile_lines_t *wlog_head=NULL, *wlog_curr=NULL;
    int wlog_size = 0;
    int line_no;
    int s_line_find = -1, e_line_find = -1;
    struct logfile_context_t *ctx;
    struct text_find_t *text_curr;

    char *default_text = t->filename;
    char *design_unit = t->item;
    int s_line = t->s_line;
    int e_line = t->e_line;

    handle = fopen(default_text, "rb");
    if(!handle)
	{
	fprintf(stderr, "Could not open logfile '%s'\n", default_text);
	return;
	}
    fclose(handle);

#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
/* nothing */
#else
    if(!font) 
	{
	if(fontname_logfile)
		{
		font=gdk_font_load(fontname_logfile);
		}

	if(!font)
		{
#ifndef __CYGWIN__
		font=gdk_font_load("-*-courier-*-r-*-*-10-*-*-*-*-*-*-*");
#else
		font=gdk_font_load("-misc-fixed-*-*-*-*-10-*-*-*-*-*-*-*");
#endif
		}
	}
#endif

    /* create a new nonmodal window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if(fontname_logfile)
	{
    	gtk_widget_set_usize( GTK_WIDGET (window), width*1.8, 640);
	}
	else
	{
    	gtk_widget_set_usize( GTK_WIDGET (window), width, 640);
	}
    gtk_window_set_title(GTK_WINDOW (window), title);


    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_widget_show (vbox);

    label=gtk_label_new(default_text);
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);
    gtk_widget_show (separator);

    ctext=create_log_text(&text);
    gtk_box_pack_start (GTK_BOX (vbox), ctext, TRUE, TRUE, 0);
    gtk_widget_show (ctext);

    separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);
    gtk_widget_show (separator);

    hbox = gtk_hbox_new (FALSE, 1);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
    gtk_widget_show (hbox);

    ctx = (struct logfile_context_t *)calloc(1, sizeof(struct logfile_context_t));
    ctx->which = t;
    ctx->display_mode = display_mode;
    ctx->width = width;

    button1 = gtk_button_new_with_label (display_mode ? "View Design Unit Only": "View Full File");
    gtk_widget_set_usize(button1, 100, -1);
    gtk_signal_connect(GTK_OBJECT (button1), "clicked",
                               GTK_SIGNAL_FUNC(ok_callback),
                               ctx);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbox), button1);  
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);
    gtk_signal_connect_object (GTK_OBJECT (button1),
                                "realize",
                             (GtkSignalFunc) gtk_widget_grab_default,
                             GTK_OBJECT (button1));

    gtk_signal_connect(GTK_OBJECT (window), "delete_event",
                       (GtkSignalFunc) destroy_callback, NULL);

    gtk_widget_show(window);


    v_preproc_name = default_text;
    while((lx = yylex()))
	{
	char *pnt = yytext;
	struct wave_logfile_lines_t *w;
	line_no = my_yylineno;

	if(!lx_module_line_locked)
		{
		if(lx==V_MODULE)
			{
			lx_module_line = line_no;
			}
		else
		if((lx==V_ID)&&(lx_module_line))
			{
			if(!strcmp(pnt, design_unit))
				{
				lx_module_line_locked = 1;
				s_line_find = lx_module_line;
				}
			}
		else
		if((lx != V_WS)&&(lx_module_line))
			{
			lx_module_line = 0;
			}
		}
	else
		{
		if((lx==V_ENDMODULE)&&(!lx_endmodule_line_locked))
			{
			e_line_find = line_no;
			lx_endmodule_line_locked = 1;
			}
		}

	w = calloc(1, sizeof(struct wave_logfile_lines_t));
	w->line_no = line_no;
	w->tok = lx;
	wlog_size += yyleng;
	w->text = malloc(yyleng + 1);
	strcpy(w->text, pnt);
	if(!wlog_curr) { wlog_head = wlog_curr = w; } else { wlog_curr->next = w; wlog_curr = w; }
	}

    log_text(text, NULL, "Design unit ");
    log_text_bold(text, NULL, design_unit);
	{
	char buf[128];

	s_line = s_line_find > 0 ? s_line_find : s_line;
	e_line = e_line_find > 0 ? e_line_find : e_line;

	sprintf(buf, " occupies lines %d - %d.\n", s_line, e_line);
	log_text(text, NULL, buf);        
	if(anno_ctx)
		{
		sprintf(buf, "Marker time for '%s' is %s.\n", anno_ctx->aet_name, 
			anno_ctx->marker_set ? anno_ctx->time_string: "not set");		
		log_text(text, NULL, buf);        
		}

	log_text(text, NULL, "\n");
	}

    if(wlog_curr)
	{
	struct wave_logfile_lines_t *w = wlog_head;
	struct wave_logfile_lines_t *wt;
	char *pnt = malloc(wlog_size + 1);
	char *pnt2 = pnt;
	JRB varnames = make_jrb();
	JRB node;
	int numvars = 0;

	/* build up list of potential variables in this module */
	if(!display_mode)
	while(w)
		{
		if(w->tok == V_ID)
			{
			if((w->line_no >= s_line) && (w->line_no <= e_line))
				{
				if(strcmp(w->text, design_unit)) /* filter out design unit name */
					{
					node = jrb_find_str(varnames, w->text);

					if(!node)
						{
						Jval dummy;
						dummy.v = NULL;
						jrb_insert_str(varnames, strdup(w->text), dummy);
						numvars++;
						}
					}
				}
			}
		w = w->next;
		}

	if((anno_ctx)&&(anno_ctx->marker_set))
		{
		int resolved = 0;

		if(vzt)
			{
			int numfacs=vzt_rd_get_num_facs(vzt);
			int i;
			int tlen;
			char *pfx = malloc((tlen=strlen(title))+1+1);
			
			strcpy(pfx, title);
			strcat(pfx+tlen, ".");
			tlen++;

			jrb_traverse(node, varnames) { node->val.i = -1; }

			for(i=0;i<numfacs;i++)
				{
				char *fnam = vzt_rd_get_facname(vzt, i);

				if(!strncmp(fnam, pfx, tlen))
					{
					if(!strchr(fnam+tlen, '.'))
						{
						jrb_traverse(node, varnames)
							{
							int mat = 0;

							if(node->val.i < 0)
								{
								if(!strcmp(fnam+tlen, node->key.s))
									{
									mat = 1;
									if(vzt->flags[i] & VZT_RD_SYM_F_ALIAS)
										{
										node->val.i = vzt_rd_get_alias_root(vzt, i);
										}
										else
										{
										node->val.i = i;
										}
									}
								}
								else /* bitblasted */
								{
								if(!strcmp(fnam+tlen, node->key.s))
									{
									struct jrb_chain *jvc = node->jval_chain;

									mat = 1;

									if(jvc) { 
										while(jvc->next) jvc = jvc->next;
										jvc->next = calloc(1, sizeof(struct jrb_chain));
										jvc = jvc->next;
										}
										else
										{ 
										jvc = calloc(1, sizeof(struct jrb_chain));
										node->jval_chain = jvc;
										}

									if(vzt->flags[i] & VZT_RD_SYM_F_ALIAS)
										{
										jvc->val.i = vzt_rd_get_alias_root(vzt, i);
										}
										else
										{
										jvc->val.i = i;
										}
									}
								}

							if(mat)
								{
								if(i==(numfacs-1))
									{
									resolved++;
									}
									else
									{
									char *fnam2 = vzt_rd_get_facname(vzt, i+1);
									if(strcmp(fnam, fnam2))
										{
										resolved++;
										if(resolved == numvars) goto resolved_vzt;
										}
									}
								}

							}

						}
					}
				}
resolved_vzt:		free(pfx);

			jrb_traverse(node, varnames)
				{
				if(node->val.i >= 0)
					{
					char *rc = vzt_rd_value(vzt, anno_ctx->marker, node->val.i);
					struct jrb_chain *jvc = node->jval_chain;
					char first_char = rc ? rc[0] : '?';

					if(!jvc)
						{
						if(rc)
							{
							node->val.v = hexify(strdup(rc));
							}
							else
							{
							node->val.v = NULL;
							}
						}
						else
						{
						char *rc2;
						int len = rc ? strlen(rc) : 0;
						int iter = 1;

						while(jvc)
							{
							rc = vzt_rd_value(vzt, anno_ctx->marker, jvc->val.i);
							len+=(rc ? strlen(rc) : 0);
							iter++;
							jvc = jvc->next;
							}

						if(iter==len)
							{
							int pos = 1;
							jvc = node->jval_chain;
							rc2 = calloc(1, len+1);
							rc2[0] = first_char;
	
							while(jvc)
								{
								char *rc = vzt_rd_value(vzt, anno_ctx->marker, jvc->val.i);
								rc2[pos++] = *rc;
								jvc = jvc->next;
								}		

							node->val.v = hexify(strdup(rc2));
							free(rc2);
							}
							else
							{
							node->val.v = NULL;
							}
						}
					}
					else
					{
					node->val.v = NULL;
					}
				}
			}
/******/
		else if(lx2)
			{
			int numfacs=lxt2_rd_get_num_facs(lx2);
			int i;
			int tlen;
			char *pfx = malloc((tlen=strlen(title))+1+1);
			
			strcpy(pfx, title);
			strcat(pfx+tlen, ".");
			tlen++;

			lxt2_rd_clr_fac_process_mask_all(lx2);
			jrb_traverse(node, varnames) { node->val.i = -1; }

			for(i=0;i<numfacs;i++)
				{
				char *fnam = lxt2_rd_get_facname(lx2, i);

				if(!strncmp(fnam, pfx, tlen))
					{
					if(!strchr(fnam+tlen, '.'))
						{
						jrb_traverse(node, varnames)
							{
							int mat = 0;

							if(node->val.i < 0)
								{
								if(!strcmp(fnam+tlen, node->key.s))
									{
									if(lx2->flags[i] & LXT2_RD_SYM_F_ALIAS)
										{
										node->val.i = lxt2_rd_get_alias_root(lx2, i);
										}
										else
										{
										node->val.i = i;
										}

									lxt2_rd_set_fac_process_mask(lx2, node->val.i);
									}
								}
								else /* bitblasted */
								{
                                                                if(!strcmp(fnam+tlen, node->key.s))
                                                                        {
                                                                        struct jrb_chain *jvc = node->jval_chain;
                                                        
                                                                        mat = 1;
                                                                
                                                                        if(jvc) {
                                                                                while(jvc->next) jvc = jvc->next;
                                                                                jvc->next = calloc(1, sizeof(struct jrb_chain));
                                                                                jvc = jvc->next;
                                                                                }
                                                                                else
                                                                                {
                                                                                jvc = calloc(1, sizeof(struct jrb_chain));
                                                                                node->jval_chain = jvc;
                                                                                }

									if(lx2->flags[i] & LXT2_RD_SYM_F_ALIAS)
										{
										jvc->val.i = lxt2_rd_get_alias_root(lx2, i);
                                                                                }
                                                                                else
                                                                                {
                                                                                jvc->val.i = i;
                                                                                }

									lxt2_rd_set_fac_process_mask(lx2, jvc->val.i);
                                                                        }
								}

                                                       if(mat)
                                                                {
                                                                if(i==(numfacs-1))
                                                                        {
                                                                        resolved++;
                                                                        }
                                                                        else
                                                                        {
                                                                        char *fnam2 = lxt2_rd_get_facname(lx2, i+1);
                                                                        if(strcmp(fnam, fnam2))
                                                                                {
                                                                                resolved++;
                                                                                if(resolved == numvars) goto resolved_lxt2;
                                                                                }
                                                                        }
								}
							}
						}
					}
				}
resolved_lxt2:		free(pfx);

			lx2vals = make_jrb();
			lxt2_rd_unlimit_time_range(lx2);
			lxt2_rd_limit_time_range(lx2, anno_ctx->marker, anno_ctx->marker);
			lxt2_rd_iter_blocks(lx2, lx2_iter_fn, NULL);

			jrb_traverse(node, varnames)
				{
				struct jrb_chain *jvc = node->jval_chain;

				if(node->val.i >= 0)
					{
					JRB srch = jrb_find_int(lx2vals, node->val.i);
					char *rc = srch ? srch->val.s : NULL;
					char first_char = rc ? rc[0] : '?';

					if(!jvc)
						{
						if(rc)
							{
							node->val.v = hexify(strdup(rc));
							}
							else
							{
							node->val.v = NULL;
							}
						}
						else
						{
                                                char *rc2;
                                                int len = rc ? strlen(rc) : 0;
                                                int iter = 1;
                                                         
                                                while(jvc)  
                                                        {
							srch = jrb_find_int(lx2vals, jvc->val.i);
							rc = srch ? srch->val.s : NULL;
                                                        len+=(rc ? strlen(rc) : 0);
                                                        iter++;
                                                        jvc = jvc->next;
                                                        }

                                                if(iter==len)
                                                        {
                                                        int pos = 1;
                                                        jvc = node->jval_chain;
                                                        rc2 = calloc(1, len+1);
                                                        rc2[0] = first_char;
                         
                                                        while(jvc)
                                                                {
								srch = jrb_find_int(lx2vals, jvc->val.i);
								rc = srch->val.s;
                                                                rc2[pos++] = *rc;
                                                                jvc = jvc->next; 
                                                                }
    
                                                        node->val.v = hexify(strdup(rc2));
                                                        free(rc2);               
                                                        }
                                                        else
                                                        {               
                                                        node->val.v = NULL;
							}						
						}
					}
					else
					{
					node->val.v = NULL;
					}
				}

			jrb_traverse(node, lx2vals)
				{
				if(node->val.s) free(node->val.s);
				}
			jrb_free_tree(lx2vals);
			lx2vals = NULL;
			}
/******/
#ifdef AET2_IS_PRESENT
		else if(ae2)
			{
			int numfacs=ae2_read_num_symbols(ae2);
			int i;
			int tlen;
			char *pfx = malloc((tlen=strlen(title))+1+1);
			char *tstr;
			int attempt = 0;
			
			strcpy(pfx, title);
			strcat(pfx+tlen, ".");
			tlen++;

			jrb_traverse(node, varnames) { node->val.i = -1; }

retry_ae2:		for(i=0;i<numfacs;i++)
				{
				char bf[65537];
				char *fnam = bf;

				ae2_read_symbol_name(ae2, i, bf);

				if(!strncmp(fnam, pfx, tlen))
					{
					if(!strchr(fnam+tlen, '.'))
						{
						jrb_traverse(node, varnames)
							{
							if(node->val.i < 0)
								{
								/* note, ae2 is never bitblasted */
								if(!strcmp(fnam+tlen, node->key.s))
									{
									node->val.i = i;
									resolved++;
									if(resolved == numvars) goto resolved_ae2;
									}
								}
							}

						}
					}
				}

			free(pfx);

			/* prune off one level of hierarchy... */
			tstr = strchr(title, '.');
			if(tstr)
				{
				if((!attempt)&&(!resolved))
					{
					pfx = malloc((tlen=strlen(tstr+1))+1+1);
					strcpy(pfx, tstr+1);
					strcat(pfx+tlen, ".");
					tlen++;
					attempt++;
					goto retry_ae2;
					}
					else
					{
					pfx = malloc(1); /* dummy */
					}
				}
				else	/* try name as top level sig */
				{
				pfx = malloc(1); /* dummy */

				if(!resolved)
				for(i=0;i<numfacs;i++)
					{
					char bf[65537];
					char *fnam = bf;
	
					ae2_read_symbol_name(ae2, i, bf);
					jrb_traverse(node, varnames)
						{
						if(node->val.i < 0)
							{
							if(!strcmp(fnam, node->key.s))
								{
								node->val.i = i;
								resolved++;
								if(resolved == numvars) goto resolved_ae2;
								}
							}
						}
					}				
				}

resolved_ae2:		free(pfx);

			jrb_traverse(node, varnames)
				{
				if(node->val.i >= 0)
					{
					char rc[MAXFACLEN+1];
					FACREF fr;

					fr.s = node->val.i;
					fr.row = 0;
					fr.row_high = 0;
					fr.offset = 0;
					fr.length = ae2_read_symbol_length(ae2, fr.s);
					ae2_read_value(ae2, &fr, anno_ctx->marker, rc);

					node->val.v = rc[0] ? hexify(strdup(rc)) : NULL;
					}
					else
					{
					node->val.v = NULL;
					}
				}
			}
#endif
/******/

		if(resolved > 0)
			{
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
#else
			gtk_text_freeze(GTK_TEXT(text));
#endif
			w = wlog_head;
			while(w)
				{
				if((w->line_no >= s_line) && (w->line_no <= e_line))
					{
					if(w->tok == V_ID)
						{
						if(strcmp(w->text, design_unit)) /* filter out design unit name */
							{
							node = jrb_find_str(varnames, w->text);
							if((node)&&(node->val.v))
								{
								log_text(text, font, w->text);
								log_text_bold(text, font, "[");
								log_text_bold(text, font, node->val.v);
								log_text_bold(text, font, "]");
								goto iter_free;
								}
							}
						}

					switch(w->tok)
						{
						case V_CMT:	log_text_active(text, font, w->text); break;

						case V_STRING:	
						case V_PREPROC:	
						case V_PREPROC_WS:	
						case V_MACRO:	log_text_prelight(text, font, w->text); break;

						default:	log_text(text, font, w->text); break;
						}
					}

iter_free:			free(w->text);
				wt = w;
				w = w->next;
				free(wt);
				}
			free(pnt);
			wlog_head = wlog_curr = NULL;
#if defined(WAVE_USE_GTK2) && !defined(GTK_ENABLE_BROKEN)
#else
			gtk_text_thaw(GTK_TEXT(text));
#endif
			goto free_vars;
			}

		}


	w = wlog_head;
	while(w)
		{
		if((display_mode)||((w->line_no >= s_line) && (w->line_no <= e_line)))
			{
			int len = strlen(w->text);
			memcpy(pnt2, w->text, len);
			pnt2 += len;
			}

		free(w->text);
		wt = w;
		w = w->next;
		free(wt);
		}
	wlog_head = wlog_curr = NULL;
	*pnt2 = 0;
	log_text(text, font, pnt);
	free(pnt);

free_vars:
	/* free up variables list */
	if(!display_mode)
		{
		jrb_traverse(node, varnames)
			{
			if(node->val.v) free(node->val.v);
			free(node->key.s);
			}
		}

	jrb_traverse(node, varnames)
		{
		struct jrb_chain *jvc = node->jval_chain;
		while(jvc)
			{
			struct jrb_chain *jvcn = jvc->next;
			free(jvc);
			jvc = jvcn;
			}
		}

	jrb_free_tree(varnames);
	varnames = NULL;

	/* insert context for destroy */
        text_curr = (struct text_find_t *)calloc(1, sizeof(struct text_find_t));
	text_curr->window = window;
	text_curr->text = text;
	text_curr->ctx = ctx;
	text_curr->next = text_root;
	text_root = text_curr;
	}
}
