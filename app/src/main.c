#include <gtk/gtk.h>
#include "draw.h"

app_data adata;

static void activate(GtkApplication* app, gpointer user_data) {
	GtkWidget* window;
	GtkWidget* paned;
	GtkWidget* sidebar;
	GtkWidget* draw_area;
	GtkGesture* dragger;
	GtkEventController* scroll_controller;
	GtkWidget* iterate_button;
	GtkWidget* grid_spin_button;
	GtkWidget* tot_iterations_box;
	GtkWidget* wcss_box;
	GtkWidget* auto_iter_toggle;
	GtkWidget* animate_speed;
	GtkWidget* reset_button;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "kmeans");
	gtk_window_set_default_size(GTK_WINDOW(window), 1400, (6 / 10.0f) * 1400);

	paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_window_set_child(GTK_WINDOW(window), paned);

	sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_widget_set_margin_start(sidebar, 10);
	gtk_widget_set_margin_top(sidebar, 10);
	gtk_widget_set_margin_bottom(sidebar, 10);
	gtk_widget_set_margin_end(sidebar, 10);
	gtk_paned_set_start_child(GTK_PANED(paned), sidebar);
	gtk_paned_set_position(GTK_PANED(paned), 200);

	auto_iter_toggle = gtk_check_button_new_with_label("animate");
	reset_button = gtk_button_new_with_label("reset");

	tot_iterations_box = gtk_entry_new();
	wcss_box = gtk_entry_new();
	gtk_editable_set_editable(GTK_EDITABLE(tot_iterations_box), FALSE);
	gtk_editable_set_editable(GTK_EDITABLE(wcss_box), FALSE);
	GtkEntryBuffer* buf = adata.iter_buffer = gtk_entry_get_buffer(GTK_ENTRY(tot_iterations_box));
	GtkEntryBuffer* wcss_buf = adata.wcss_buffer = gtk_entry_get_buffer(GTK_ENTRY(wcss_box));
	gtk_entry_buffer_set_text(buf, "0 iterations", -1);
	gtk_entry_buffer_set_text(wcss_buf, "wcss = 0.00", -1);


	iterate_button = gtk_button_new_with_label("step");
	grid_spin_button = gtk_spin_button_new_with_range(1, 10, 1);
	animate_speed = gtk_spin_button_new_with_range(0.25f, 2, 0.25f);

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(grid_spin_button), 4);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(animate_speed), 0.5f);

	adata.fd = (file_data*) user_data;
	adata.iter_button = iterate_button;

	g_signal_connect(iterate_button, "clicked", G_CALLBACK(on_iterate), &adata);
	g_signal_connect(reset_button, "clicked", G_CALLBACK(on_reset_data), &adata);

	gtk_box_append(GTK_BOX(sidebar), iterate_button);
	gtk_box_append(GTK_BOX(sidebar), grid_spin_button);
	gtk_box_append(GTK_BOX(sidebar), tot_iterations_box);
	gtk_box_append(GTK_BOX(sidebar), wcss_box);
	gtk_box_append(GTK_BOX(sidebar), auto_iter_toggle);
	gtk_box_append(GTK_BOX(sidebar), animate_speed);
	gtk_box_append(GTK_BOX(sidebar), reset_button);

	dragger = gtk_gesture_drag_new();
	scroll_controller = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
	draw_area = gtk_gl_area_new();

	g_signal_connect(auto_iter_toggle, "toggled", G_CALLBACK(on_animate_toggle), &adata);
	g_signal_connect(draw_area, "realize", G_CALLBACK(init_gl), &adata);
	g_signal_connect(draw_area, "render", G_CALLBACK(render_gl), &adata);
	g_signal_connect(draw_area, "unrealize", G_CALLBACK(destroy_gl), NULL);
	g_signal_connect(dragger, "drag-update", G_CALLBACK(drag_update), NULL);
	g_signal_connect(dragger, "drag-end", G_CALLBACK(drag_end), NULL);
	g_signal_connect(scroll_controller, "scroll", G_CALLBACK(on_scroll), NULL);
	g_signal_connect(grid_spin_button, "value-changed", G_CALLBACK(grid_size_change), NULL);
	g_signal_connect(animate_speed, "value-changed", G_CALLBACK(on_iter_speed_change), NULL);

	gtk_widget_add_controller(draw_area, GTK_EVENT_CONTROLLER(dragger));
	gtk_widget_add_controller(draw_area, GTK_EVENT_CONTROLLER(scroll_controller));

	g_timeout_add(10, trigger_redraw, draw_area);

	gtk_paned_set_end_child(GTK_PANED(paned), draw_area);

	gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char** argv) {
	file_data fd = read_file_data("input.txt");

	GtkApplication* app;
	int status;

	app = gtk_application_new("org.gtk.kmeans", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), &fd);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	free(fd.centroids);
	free(fd.data_points);

	return status;
}
