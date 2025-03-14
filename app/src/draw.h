#ifndef DRAW_H
#define DRAW_H

#include <gtk/gtk.h>
#include "io.h"

typedef struct app_data {
	file_data* fd;
	GtkEntryBuffer* iter_buffer;
	GtkEntryBuffer* wcss_buffer;
	GtkWidget* iter_button;
} app_data;

void drag_update(GtkGestureDrag* drag, gdouble x, gdouble y, gpointer user_data);
void drag_end(GtkGestureDrag* drag, gdouble x, gdouble y, gpointer user_data);
gboolean on_scroll(GtkEventControllerScroll* controller, gdouble dx, gdouble dy, gpointer user_data);
void on_iterate(GtkButton* button, gpointer user_data);
void on_reset_data(GtkButton* button, gpointer user_data);
void grid_size_change(GtkSpinButton* spin_button, gpointer user_data);
void on_animate_toggle(GtkCheckButton* button, gpointer user_data);
void on_iter_speed_change(GtkSpinButton* spin_button, gpointer user_data);

gboolean trigger_redraw(gpointer data);
void init_gl(GtkGLArea* area, gpointer user_data);
void render_gl(GtkGLArea* area, GdkGLContext* ctx, gpointer data);
void destroy_gl(GtkGLArea* area);

#endif