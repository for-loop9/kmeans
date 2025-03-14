#include "draw.h"
#include <epoxy/gl.h>
#include "renderer.h"

renderer* r;

float dx = 0;
float dy = 0;
float lx = 0, ly = 0;
float sc = 0;

float scx = 0;
float scy = 0;
float scs = 13;
int total_iters = 0;

int grd = 4;
file_data fd_copy;
file_data original;

struct timespec ptime;
float dt;
float et;
float interval = 0.5f;

bool animate = false;

void iterate(app_data* adata) {
    float* mins = malloc(adata->fd->k * sizeof(float));
    for (int i = 0; i < adata->fd->n; i++) {
        float min_v = FLT_MAX;
        int min_i = -1;

        for (int j = 0; j < adata->fd->k; j++) {
            float d0 = adata->fd->data_points[i].x - adata->fd->centroids[j].x;
            float d1 = adata->fd->data_points[i].y - adata->fd->centroids[j].y;
            mins[j] = d0 * d0 + d1 * d1;

            if (mins[j] < min_v) {
                min_i = j;
                min_v = mins[j];
            }
        }

        adata->fd->data_points[i].cluster = min_i;
        adata->fd->data_points[i].c.r = adata->fd->centroids[min_i].c.r * adata->fd->centroids[min_i].c.r;
        adata->fd->data_points[i].c.g = adata->fd->centroids[min_i].c.g * adata->fd->centroids[min_i].c.g;
        adata->fd->data_points[i].c.b = adata->fd->centroids[min_i].c.b * adata->fd->centroids[min_i].c.b;
    }
    free(mins);

    bool moved = false;

    for (int i = 0; i < adata->fd->k; i++) {
        float mean_x = 0, mean_y = 0;
        int total = 0;
        for (int j = 0; j < adata->fd->n; j++) {
            if (adata->fd->data_points[j].cluster == i) {
                mean_x += adata->fd->data_points[j].x;
                mean_y += adata->fd->data_points[j].y;
                total++;
            }
        }

        if (!(fabsf(adata->fd->centroids[i].x - mean_x / total) < 1e-9 && fabsf(adata->fd->centroids[i].y - mean_y / total) < 1e-9)) {
            moved = true;
        }

        adata->fd->centroids[i].x = mean_x / total;
        adata->fd->centroids[i].y = mean_y / total;
    }

    if (moved) {
        total_iters++;
        char buff[32] = {};
        sprintf(buff, "%d iterations", total_iters);
        gtk_entry_buffer_set_text(adata->iter_buffer, buff, strlen(buff));

        float wcss = 0;
        for (int i = 0; i < adata->fd->n; i++) {
            point* pt = adata->fd->data_points + i;
            point* ct = adata->fd->centroids + pt->cluster;

            float d0 = pt->x - ct->x;
            float d1 = pt->y - ct->y;

            wcss += d0 * d0 + d1 * d1;
        }

        sprintf(buff, "wcss = %.2f", wcss);
        gtk_entry_buffer_set_text(adata->wcss_buffer, buff, strlen(buff));
        return;
    }
}

void on_animate_toggle(GtkCheckButton* button, gpointer user_data) {
    gboolean active = gtk_check_button_get_active(button);
    app_data* adata = (app_data*) user_data;
    if (active) {
        animate = true;
        gtk_widget_set_sensitive(adata->iter_button, false);
    } else {
        animate = false;
        gtk_widget_set_sensitive(adata->iter_button, true);
    }
}

void drag_update(GtkGestureDrag* drag, gdouble x, gdouble y, gpointer user_data) {
    if (gtk_gesture_is_active(GTK_GESTURE(drag))) {
        dx = x - lx;
        dy = y - ly;
        lx = x;
        ly = y;
    }
}

void drag_end(GtkGestureDrag* drag, gdouble x, gdouble y, gpointer user_data) {
    lx = 0;
    ly = 0;
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

gboolean on_scroll(GtkEventControllerScroll* controller, gdouble dx, gdouble dy, gpointer user_data) {
    sc = dy;
    return GDK_EVENT_STOP;
}

void on_iterate(GtkButton* button, gpointer user_data) {
    iterate((app_data*) user_data);
}

void on_reset_data(GtkButton* button, gpointer user_data) {
    app_data* adata = (app_data*) user_data;
    for (int i = 0; i < original.n; i++) {
        adata->fd->data_points[i] = original.data_points[i];
    }
    for (int i = 0; i < original.k; i++) {
        adata->fd->centroids[i] = original.centroids[i];
    }
    total_iters = 0;
    et = 0;
    gtk_entry_buffer_set_text(adata->iter_buffer, "0 iterations", -1);
    gtk_entry_buffer_set_text(adata->wcss_buffer, "wcss = 0.00", -1);
}

void grid_size_change(GtkSpinButton* spin_button, gpointer user_data) {
    grd = gtk_spin_button_get_value_as_int(spin_button);
}

void on_iter_speed_change(GtkSpinButton* spin_button, gpointer user_data) {
    interval = (float) gtk_spin_button_get_value(spin_button);
}

gboolean trigger_redraw(gpointer data) {
    if (GTK_IS_GL_AREA(data)) {
        GtkGLArea* area = GTK_GL_AREA(data);
        gtk_gl_area_queue_render(area);
    }
    
    return G_SOURCE_CONTINUE;
}

void init_gl(GtkGLArea* area, gpointer user_data) {
    app_data* adata = (app_data*) user_data;
    fd_copy = copy_file_data(adata->fd);
    original = copy_file_data(adata->fd);

    gtk_gl_area_make_current(area);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    r = renderer_create(1024, 128, 128, 128);

    clock_gettime(CLOCK_REALTIME, &ptime);
}

void transition(file_data* fd, float dt) {
    for (int i = 0; i < fd->k; i++) {
        fd_copy.centroids[i].x = lerp(fd_copy.centroids[i].x, fd->centroids[i].x, 0.25f);
        fd_copy.centroids[i].y = lerp(fd_copy.centroids[i].y, fd->centroids[i].y, 0.25f);
    }

    for (int i = 0; i < fd->n; i++) {
        fd_copy.data_points[i].cluster = fd->data_points[i].cluster;
        fd_copy.data_points[i].c.r = lerp(fd_copy.data_points[i].c.r, fd->data_points[i].c.r, 0.25f);
        fd_copy.data_points[i].c.g = lerp(fd_copy.data_points[i].c.g, fd->data_points[i].c.g, 0.25f);
        fd_copy.data_points[i].c.b = lerp(fd_copy.data_points[i].c.b, fd->data_points[i].c.b, 0.25f);
    }
}

void render_gl(GtkGLArea* area, GdkGLContext* ctx, gpointer data) {
    struct timespec tnow;
    clock_gettime(CLOCK_REALTIME, &tnow);
    double tnow_sec = (double) tnow.tv_sec + tnow.tv_nsec / 1e9;
    double ptime_sec = (double) ptime.tv_sec + ptime.tv_nsec / 1e9;
    dt = (float) (tnow_sec - ptime_sec);
    ptime = tnow;

    app_data* adata = (app_data*) data;

    #if DARK_THEME
    glClearColor(0.1f, 0.1f, 0.1f, 1);
    #else
    glClearColor(1, 1, 1, 1);
    #endif
    glClear(GL_COLOR_BUFFER_BIT);

    r->global.width = gtk_widget_get_width(GTK_WIDGET(area));
    r->global.height = gtk_widget_get_height(GTK_WIDGET(area));
    
    scs -= sc * 1.4f;
    scs = fmaxf(10, fminf(scs, 60));

    scx -= dx / r->global.cs;
    scy += dy / r->global.cs;

    r->global.cx = lerp(r->global.cx, scx, 0.25f);
    r->global.cy = lerp(r->global.cy, scy, 0.25f);
    r->global.cs = lerp(r->global.cs, scs, 0.25f);
    sc = 0;
    dx = 0;
    dy = 0;

    glViewport(0, 0, r->global.width, r->global.height);

    if (animate) {
        et += dt;
        if (et >= interval) {
            iterate(adata);
            et = 0;
        }
    } else et = 0;

    for (int i = 1; i < 100; i += 1) {
        #if DARK_THEME
        renderer_push_line(r, &(line) { .x0 = i * grd, .y0 = 10000, .x1 = i * grd, .y1 = -10000, .c = { 0.15f, 0.15f, 0.15f } });
        renderer_push_line(r, &(line) { .x0 = -i * grd, .y0 = 10000, .x1 = -i * grd, .y1 = -10000, .c = { 0.15f, 0.15f, 0.15f } });
        renderer_push_line(r, &(line) { .x0 = -10000, .y0 = i * grd, .x1 = 10000, .y1 = i * grd, .c = { 0.15f, 0.15f, 0.15f } });
        renderer_push_line(r, &(line) { .x0 = -10000, .y0 = -i * grd, .x1 = 10000, .y1 = -i * grd, .c = { 0.15f, 0.15f, 0.15f } });
        #else
        renderer_push_line(r, &(line) { .x0 = i * grd, .y0 = 10000, .x1 = i * grd, .y1 = -10000, .c = { 0.95f, 0.95f, 0.95f } });
        renderer_push_line(r, &(line) { .x0 = -i * grd, .y0 = 10000, .x1 = -i * grd, .y1 = -10000, .c = { 0.95f, 0.95f, 0.95f } });
        renderer_push_line(r, &(line) { .x0 = -10000, .y0 = i * grd, .x1 = 10000, .y1 = i * grd, .c = { 0.95f, 0.95f, 0.95f } });
        renderer_push_line(r, &(line) { .x0 = -10000, .y0 = -i * grd, .x1 = 10000, .y1 = -i * grd, .c = { 0.95f, 0.95f, 0.95f } });
        #endif
    }

    #if DARK_THEME
    renderer_push_line(r, &(line) { .x0 = -10000, .y0 = 0, .x1 = 10000, .y1 = 0, .c = { 0.3f, 0.2f, 0.2f } });
    renderer_push_line(r, &(line) { .x0 = 0, .y0 = -10000, .x1 = 0, .y1 = 10000, .c = { 0.2f, 0.3f, 0.2f } });
    #else
    renderer_push_line(r, &(line) { .x0 = -10000, .y0 = 0, .x1 = 10000, .y1 = 0, .c = { 0.9f, 0.7f, 0.7f } });
    renderer_push_line(r, &(line) { .x0 = 0, .y0 = -10000, .x1 = 0, .y1 = 10000, .c = { 0.7f, 0.9f, 0.7f } });
    #endif

    transition(adata->fd, dt);

    for (int i = 0; i < adata->fd->n; i++) {
        point* pt = fd_copy.data_points + i;
        renderer_push_di(r, &(di) { .x = pt->x, pt->y, .r = 12 / r->global.cs, .c = pt->c });
        if (pt->cluster != -1) {
            #if DARK_THEME
            renderer_push_line(r, &(line) {
                .x0 = pt->x, .y0 = pt->y,
                .x1 = fd_copy.centroids[pt->cluster].x,
                .y1 = fd_copy.centroids[pt->cluster].y,
                .c = { 0.3f * pt->c.r, 0.3f * pt->c.g, 0.3f * pt->c.b } });
            #else
            renderer_push_line(r, &(line) {
                .x0 = pt->x, .y0 = pt->y,
                .x1 = fd_copy.centroids[pt->cluster].x,
                .y1 = fd_copy.centroids[pt->cluster].y,
                .c = { 0.8f, 0.8f, 0.8f } });
            #endif
        }
    }

    for (int i = 0; i < adata->fd->k; i++) {
        point* pt = fd_copy.centroids + i;

        renderer_push_circle(r, &(circle) { .x = pt->x, .y = pt->y, .r = 18 / r->global.cs, .c = { 0, 0, 0 } });
        renderer_push_circle(r, &(circle) { .x = pt->x, .y = pt->y, .r = 15 / r->global.cs, .c = pt->c });
    }

    renderer_flush(r);
}

void destroy_gl(GtkGLArea* area) {
    renderer_destroy(r);

    free(original.data_points);
    free(original.centroids);

    free(fd_copy.data_points);
    free(fd_copy.centroids);
}
