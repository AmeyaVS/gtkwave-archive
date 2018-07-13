#ifndef WAVE_GTK23COMPAT_H
#define WAVE_GTK23COMPAT_H

/* this is for any future gtk2/gtk3 compatibility */

#ifdef MAC_INTEGRATION
/* #undef WAVE_ALLOW_QUARTZ_FLUSH_WORKAROUND */
#endif

#if GTK_CHECK_VERSION(3,0,0)
/* workaround for wave_vslider not rendering properly on startup */
#define WAVE_ALLOW_GTK3_VSLIDER_WORKAROUND

/* workaround for gtk warnings "How does the code know the size to allocate?" */
#define WAVE_GTK3_SIZE_ALLOCATE_WORKAROUND

#define WAVE_GTK3_HIERSEARCH_DEBOUNCE
#define WAVE_GTK3_MENU_SEPARATOR
#endif

#if GTK_CHECK_VERSION(3,0,0)
/* triggers "How does the code know the size to allocate?" warnings when treeview is expanded, so currently is disabled */
/* #define WAVE_ALLOW_GTK3_GRID */
#endif

#define WAVE_GTKIFE(a,b,c,d,e) {a,b,c,d,e,NULL}
#define WAVE_GDK_GET_POINTER(a,b,c,bi,ci,d)  gdk_window_get_pointer(a,bi,ci,d)
#define WAVE_GDK_GET_POINTER_COPY x=xi; y=yi;

#define WAVE_GTK_SFUNCAST(x) ((void (*)(GtkWidget *, gpointer))(x))

/* doesn't work in gtk 2 or 3 */
#undef WAVE_ALLOW_SLIDER_ZOOM

/* gtk3->4 deprecated */

#if GTK_CHECK_VERSION(3,0,0)

#define YYY_GTK_TREE_VIEW GTK_SCROLLABLE
#define YYY_gtk_tree_view_get_vadjustment gtk_scrollable_get_vadjustment
#define YYY_gtk_tree_view_get_hadjustment gtk_scrollable_get_hadjustment
#define YYY_gtk_tree_view_set_vadjustment gtk_scrollable_set_vadjustment
#define YYY_gtk_tree_view_set_hadjustment gtk_scrollable_set_hadjustment

#define YYY_GTK_TEXT_VIEW GTK_SCROLLABLE
#define YYY_gtk_text_view_get_vadjustment gtk_scrollable_get_vadjustment

GtkWidget *XXX_gtk_hbox_new (gboolean homogeneous, gint spacing);
GtkWidget *XXX_gtk_vbox_new (gboolean homogeneous, gint spacing);

#define YYY_gtk_hpaned_new(a) gtk_paned_new(GTK_ORIENTATION_HORIZONTAL)
#define YYY_gtk_vpaned_new(a) gtk_paned_new(GTK_ORIENTATION_VERTICAL)

GtkWidget *XXX_gtk_hseparator_new (void);

#define YYY_gtk_hscrollbar_new(a) gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, a)
#define YYY_gtk_vscrollbar_new(a) gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, a)

#else

#define YYY_GTK_TREE_VIEW GTK_TREE_VIEW
#define YYY_gtk_tree_view_get_vadjustment gtk_tree_view_get_vadjustment
#define YYY_gtk_tree_view_get_hadjustment gtk_tree_view_get_hadjustment
#define YYY_gtk_tree_view_set_vadjustment gtk_tree_view_set_vadjustment
#define YYY_gtk_tree_view_set_hadjustment gtk_tree_view_set_hadjustment

#define YYY_GTK_TEXT_VIEW GTK_TEXT_VIEW
#define YYY_gtk_text_view_get_vadjustment gtk_text_view_get_vadjustment

#define XXX_gtk_hbox_new(a, b) gtk_hbox_new((a), (b))
#define XXX_gtk_vbox_new(a, b) gtk_vbox_new((a), (b))

#define YYY_gtk_hpaned_new(a) gtk_hpaned_new()
#define YYY_gtk_vpaned_new(a) gtk_vpaned_new()

#define XXX_gtk_hseparator_new gtk_hseparator_new

#define YYY_gtk_hscrollbar_new(a) gtk_hscrollbar_new(a)
#define YYY_gtk_vscrollbar_new(a) gtk_vscrollbar_new(a)

#endif


#ifdef WAVE_ALLOW_GTK3_GRID

GtkWidget *XXX_gtk_table_new (guint rows,
               guint columns,
               gboolean homogeneous);

void XXX_gtk_table_attach (GtkGrid *table,
                  GtkWidget *child,
                  guint left_attach,
                  guint right_attach,
                  guint top_attach,
                  guint bottom_attach,
                  GtkAttachOptions xoptions,
                  GtkAttachOptions yoptions,
                  guint xpadding,
                  guint ypadding);

#define XXX_GTK_TABLE GTK_GRID

#else

#define XXX_gtk_table_new gtk_table_new
#define XXX_gtk_table_attach gtk_table_attach
#define XXX_GTK_TABLE GTK_TABLE

#endif

#endif
