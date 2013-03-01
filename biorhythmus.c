/* -*-coding: utf-8;-*- */
/* Biorhythmus
   Copyright (C) 2003-2013 by Gabriel Mainberger

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2 of the License,
   or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA

*/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <json-glib/json-glib.h>
#include <math.h>

struct bio_date
{
	guint day;
	guint month;
	guint year;
};

struct bio_date bio_birthday;
struct bio_date bio_viewdata;

//GDate *new_birthday;
//GDate *new_viewdata;

enum
{
	PERSON_VIEW_COLUMN_NAME = 0,
	PERSON_VIEW_COLUMN_BIRTHDAY,
	PERSON_VIEW_COLUMN_NUM_COLS
};

GtkWidget *option_physical, *option_emotional, *option_intellectual, *option_total, *dates, *map;
GtkStatusbar *status;

gint
bio_bioday (gint days_of_life, gint bio_cycle_days)
{
	gint rd;
	gdouble pi2, result;

	pi2 = (gfloat)(6.2831853); // 2 * (PI) 3.141592654 = 6.2831853
	rd = (gint)(days_of_life - (bio_cycle_days * floor(days_of_life / bio_cycle_days)));
	result = (gfloat)(sin(rd * pi2 / bio_cycle_days)); // calculate
	return((gint)floor(100 * result + 0.5));
}

gint
bio_bioday_graphic (gint x, gint days_of_life, gint bio_cycle_days, gint half_height, gint day_pix, gint margin)
{
	gdouble pi2, calcsin, ri;

	pi2 = (gfloat)(6.2831853); // 2 * (PI) 3.141592654 = 6.2831853
	ri = (days_of_life - (bio_cycle_days * floor(days_of_life / bio_cycle_days)));
	calcsin = sin((x - margin + (day_pix * ri)) * pi2 / (day_pix * bio_cycle_days));
	return((gint)(floor(half_height - ((half_height-margin) * calcsin)) + 0.5));
}

gint
bio_setpositiv (gint i)
{
	if (i < 0)
		return(i * -1);
	else
		return(i);
}

gint
bio_setpositivgraphic (gint i, gint half_height)
{
	if (i > half_height)
		return (half_height - (i - half_height));
	else
		return (i);
}

gint
bio_biodaytotal (gint days_of_life)
{
	gint result_physical, result_emotional, result_intellectual;

	result_physical = bio_setpositiv (bio_bioday (days_of_life, 23));
	result_emotional = bio_setpositiv (bio_bioday (days_of_life, 28));
	result_intellectual = bio_setpositiv (bio_bioday (days_of_life, 33));

	return((gint)((result_physical + result_emotional + result_intellectual) / 3));
}

gint
bio_bioday_graphic_total (gint x, gint days_of_life, gint half_height, gint day_pix, gint margin)
{
	gint result_physical, result_emotional, result_intellectual;
	
	result_physical = bio_setpositivgraphic(bio_bioday_graphic (x, days_of_life, 23, half_height, day_pix, margin), half_height);
	result_emotional = bio_setpositivgraphic(bio_bioday_graphic (x, days_of_life, 28, half_height, day_pix, margin), half_height);
	result_intellectual = bio_setpositivgraphic(bio_bioday_graphic (x, days_of_life, 33, half_height, day_pix, margin), half_height);

	return((gint)((result_physical + result_emotional + result_intellectual) / 3));
}

glong
bio_daysto (gint day, gint month, gint year)
{
	gint result=day;

	month--;
	// Month
	while (month)
	{
 		result += g_date_get_days_in_month(month, year);
		month--;
	}

	year--;
	// Years
	while (year)
	{
		if(year%4 == 0 && ( year%100 != 0 || year%400 == 0))
			result += 366;
		else
			result += 365;

		year--;
	}

	return(result);
}

gint
bio_daysoflife (struct bio_date date_selection, struct bio_date date_birthday)
{
	return (gint)(bio_daysto(date_selection.day, date_selection.month, date_selection.year)-bio_daysto(date_birthday.day, date_birthday.month, date_birthday.year));
}

void
bio_cli_output (GtkMenuItem *eintrag, gpointer foo)
{
	gint daysoflife;

	daysoflife = bio_daysoflife (bio_viewdata, bio_birthday);
	g_print ("Age in days: %d\n\n", daysoflife);

	g_print ("Physical: %d\n", bio_bioday (daysoflife, 23));
	g_print ("Emotional: %d\n", bio_bioday (daysoflife, 28));
	g_print ("Intellectual: %d\n", bio_bioday (daysoflife, 33));

	g_print ("\nTotal: %d\n", bio_biodaytotal (daysoflife));
}

void
bio_gui_help_info_dialog (GtkMenuItem *eintrag, gpointer user_data)
{
	static const gchar *authors[] = {"Gabriel Mainberger <gabisoft@freesurf.ch>", NULL};
	gtk_show_about_dialog (NULL, "authors", authors, "program-name", "Biorhythmus", "title", "Funny and useless :)", "version", "0.10", "copyright", "Copyright (c) 2003-2013 Gabriel Mainberger", NULL);
}

void
bio_gui_draw_chart_curves (cairo_t *cr, double red, double green, double blue, gint margin, gint days_in_month, gint day_pix, gint days_of_life, gint cycle_day, gint half_height)
{
	gint i = margin;

	if ((bio_birthday.month == bio_viewdata.month) && (bio_birthday.year == bio_viewdata.year))
		i = margin + ((bio_birthday.day - 1) * day_pix);
	else if (days_of_life < 0)
		return;

	cairo_set_source_rgb (cr, red, green, blue);

	// Draw curves
	while (i <= margin + ((days_in_month - 1) * day_pix))
	{
		if (cycle_day == 0)
			cairo_line_to (cr, i, bio_bioday_graphic_total (i, days_of_life, half_height, day_pix, margin));
		else
			cairo_line_to (cr, i, bio_bioday_graphic (i, days_of_life, cycle_day, half_height, day_pix, margin));

		i++;
	}

	cairo_stroke (cr);
}

void
bio_gui_draw_chart_lines (cairo_t *cr, gint margin, gint half_height, gint days_in_month, gint day_pix)
{
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_line_width (cr, 0.5);
	cairo_move_to (cr, margin, half_height);
	cairo_line_to (cr, margin + ((days_in_month - 1) * day_pix), half_height);
	cairo_stroke (cr);
}

void
bio_gui_draw_chart_current_day_line(cairo_t *cr, gint margin, gint full_height, gint days_in_month, gint day_pix, gint current_day)
{
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_line_width (cr, 0.5);
	cairo_move_to (cr, margin + (day_pix * (current_day - 1)), 0);
	cairo_line_to (cr, margin + (day_pix * (current_day - 1)), full_height);
	cairo_stroke (cr);
}

void
bio_gui_draw_chart_day_lines(cairo_t *cr, gint margin, gint half_height, gint full_height, gint days_in_month, gint day_pix)
{
	gint i, h100;
	gchar *s;

	h100 = (gint)(floor (full_height / 100) + 1);
	for (i = 0;i < days_in_month;i++)
	{
		s = g_strdup_printf ("%d", i + 1);
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size (cr, 10);
		cairo_move_to (cr, margin + (i * day_pix) - 4, half_height + h100 + 10);
		cairo_show_text (cr, s);
		g_free (s);

		cairo_set_source_rgb (cr, 0, 0, 0);
		cairo_move_to (cr, margin + (i * day_pix), half_height - h100);
		cairo_line_to (cr, margin + (i * day_pix), half_height + h100);
	}
	cairo_stroke (cr);
}

void
bio_gui_update_statusbar (gint days_of_life)
{
	gchar *s;

	s = g_strdup_printf ("Date: %d.%d.%d / Birthday: %d.%d.%d / Days: %d / Physical: %d / Emotional: %d / Intellectual: %d / Total: %d", bio_viewdata.day, bio_viewdata.month,
	bio_viewdata.year,
	bio_birthday.day, bio_birthday.month, bio_birthday.year, days_of_life, bio_bioday(days_of_life, 23), bio_bioday(days_of_life, 28), bio_bioday(days_of_life, 33),
	bio_biodaytotal (days_of_life));
	gtk_statusbar_push (status, 0, s);
	g_free (s);
}

gboolean
bio_gui_draw_chart (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	gint i, days_of_life, days_in_month, day_pix, full_height, half_height, full_width, h100, margin;
	cairo_t *cr;
	GdkWindow *mywindow;

	mywindow = gtk_widget_get_window (widget);
	cr = gdk_cairo_create (mywindow);

	// Inizialize
	margin = 15;

#ifdef GTK2
	full_height = widget->allocation.height;
	full_width = widget->allocation.width;
#else
	full_height = gtk_widget_get_allocated_height (widget);
	full_width = gtk_widget_get_allocated_width (widget);
#endif

	gtk_calendar_get_date ((GtkCalendar*)dates, &bio_viewdata.year, &bio_viewdata.month, &bio_viewdata.day);
	//g_date_set_dmy(new_viewdata, bio_viewdata.day, bio_viewdata.month+1, bio_viewdata.year);
	bio_viewdata.month++;

	// Set Parameter
	days_in_month = g_date_get_days_in_month (bio_viewdata.month, bio_viewdata.year);
	days_of_life = bio_daysoflife (bio_viewdata, bio_birthday);
	//g_print("Daysoflife: %i %i (daysinmonth) %i\n", daysoflife, g_date_days_between(new_birthday, new_viewdata));
	day_pix = full_width / days_in_month;
	half_height = full_height / 2;

	bio_gui_draw_chart_lines (cr, margin, half_height, days_in_month, day_pix);
	bio_gui_draw_chart_current_day_line (cr, margin, full_height, days_in_month, day_pix, bio_viewdata.day);
	bio_gui_draw_chart_day_lines (cr, margin, half_height, full_height, days_in_month, day_pix);

	// Physical
	if (gtk_check_menu_item_get_active ((GtkCheckMenuItem*)option_physical))
		bio_gui_draw_chart_curves (cr, 1, 0, 0, margin, days_in_month, day_pix, days_of_life, 23, half_height);

	// Emotional
	if (gtk_check_menu_item_get_active ((GtkCheckMenuItem*)option_emotional))
		bio_gui_draw_chart_curves (cr, 0, 1, 0, margin, days_in_month, day_pix, days_of_life, 28, half_height);

	// Intellectual
	if (gtk_check_menu_item_get_active ((GtkCheckMenuItem*)option_intellectual))
		bio_gui_draw_chart_curves (cr, 0, 0, 1, margin, days_in_month, day_pix, days_of_life, 33, half_height);

	if (gtk_check_menu_item_get_active ((GtkCheckMenuItem*)option_total))
		bio_gui_draw_chart_curves (cr, 0, 0, 0, margin, days_in_month, day_pix, days_of_life, 0, half_height);

	cairo_stroke (cr);
	cairo_destroy (cr);

	bio_gui_update_statusbar (days_of_life);

	return TRUE;
}

void
bio_gui_refresh_chart (GtkMenuItem *entry, gpointer user_data)
{
	gtk_widget_queue_resize (user_data);
}

void
bio_gui_calendar_change (GtkCalendar *calendar, gpointer user_data)
{
	gtk_widget_queue_resize (map);
}

void
bio_read_file_each_person (JsonArray *array, guint index_, JsonNode *element_node, gpointer user_data)
{
	JsonObject *element;
	JsonNode *name, *birthday;
	GtkTreeIter iter;
	GtkListStore *list_store = (GtkListStore*)user_data;

	element = json_node_get_object (element_node);
	name = json_object_get_member (element, "Name");
	birthday = json_object_get_member (element, "Birthday");

	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, PERSON_VIEW_COLUMN_NAME, json_node_dup_string(name), PERSON_VIEW_COLUMN_BIRTHDAY, json_node_dup_string(birthday), -1);
}

int
bio_read_file_person (GtkListStore *list_store, gchar *filename)
{
	JsonParser *parser;
	JsonNode *root, *result;
	JsonObject *element;
	GError *error;
	JsonArray *array;

	g_print ("%s %p\n", filename, list_store);
	parser = json_parser_new ();
	error = NULL;
	json_parser_load_from_file (parser, filename, &error);
	if (error)
	{
		g_print ("Unable to parse `%s': %s\n", filename, error->message);
		g_error_free (error);
    	g_object_unref (parser);
		return 1;
	}

	root = json_parser_get_root (parser);
	element = json_node_get_object (root);
	result = json_object_get_member (element, "Persons");
	array = json_node_get_array (result);
	json_array_foreach_element (array, bio_read_file_each_person, list_store);

	g_object_unref (parser);

	return 0;
}

void
bio_gui_open_file (GtkListStore *list_store)
{
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Open File",
						NULL,
						GTK_FILE_CHOOSER_ACTION_OPEN,
						GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						GTK_STOCK_OPEN,
						GTK_RESPONSE_ACCEPT,
						NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		bio_read_file_person(list_store, filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void
bio_write_file_each_person (JsonBuilder *builder, GtkListStore *list_store)
{
	gboolean valid;
	GtkTreeIter iter;
	gchar *cell_text_name, *cell_text_birthday;

	valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL(list_store), &iter);
	while ( valid )
	{
		gtk_tree_model_get (GTK_TREE_MODEL(list_store), &iter, PERSON_VIEW_COLUMN_NAME, &cell_text_name, PERSON_VIEW_COLUMN_BIRTHDAY, &cell_text_birthday, -1);

		if((g_strcmp0(cell_text_name, "") != 0) && (g_strcmp0(cell_text_birthday, "") != 0))
		{
			json_builder_begin_object (builder);
			json_builder_set_member_name (builder, "Name");
			json_builder_add_string_value (builder, cell_text_name);
			json_builder_set_member_name (builder, "Birthday");
			json_builder_add_string_value (builder, cell_text_birthday);
			json_builder_end_object (builder);
		}

		valid = gtk_tree_model_iter_next (GTK_TREE_MODEL(list_store), &iter);
	}
}

int
bio_write_file_person (GtkListStore *list_store)
{
	GError *error;

	JsonBuilder *builder = json_builder_new ();
	json_builder_begin_object (builder);

	json_builder_set_member_name (builder, "Persons");

	json_builder_begin_array (builder);
	bio_write_file_each_person (builder, list_store);
	json_builder_end_array (builder);

	json_builder_end_object (builder);

	JsonGenerator *gen = json_generator_new ();
	JsonNode *root = json_builder_get_root (builder);
	json_generator_set_root (gen, root);

	error = NULL;
	json_generator_to_file (gen, "default.bio", &error);

	json_node_free (root);
	g_object_unref (gen);
	g_object_unref (builder);

	if (error)
	{
		g_print ("Unable to write `%s': %s\n", "default.bio", error->message);
		g_error_free (error);
		return 1;
	}

	return 0;
}

void
bio_gui_persons_clear (GtkListStore *list_store)
{
	gtk_list_store_clear (list_store);
}

void
bio_gui_persons_add_empty_row (GtkListStore *list_store)
{
	GtkTreeIter iter;

	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, PERSON_VIEW_COLUMN_NAME, "", PERSON_VIEW_COLUMN_BIRTHDAY, "", -1);
}

void
bio_gui_persons_date_changed(GtkTreeView *tree_view, gpointer user_data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *str;
	GDate *date;

	selection = gtk_tree_view_get_selection (tree_view);
	if(gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, PERSON_VIEW_COLUMN_BIRTHDAY, &str, -1);

		date = g_date_new ();
		g_date_set_parse (date, str);

		if(g_date_valid(date))
		{
			bio_birthday.day = g_date_get_day (date);
			bio_birthday.month = g_date_get_month (date);
			bio_birthday.year = g_date_get_year (date);

			//g_date_set_parse (new_birthday, str);
		}

		g_free (str);
		g_date_free (date);
	}

	gtk_widget_queue_resize (map);
}

gboolean
bio_gui_persons_check_other_cells_empty (GtkListStore *list_store, GtkTreeIter *iter, gint except_column)
{
	gint i;
	gchar *cell_text;
	gboolean status=FALSE;

	for (i = 0 ; i < PERSON_VIEW_COLUMN_NUM_COLS ; i++)
	{
		if (i != except_column)
		{
			gtk_tree_model_get(GTK_TREE_MODEL(list_store), iter, i, &cell_text, -1);

			if (cell_text != NULL)
			{
				if (g_strcmp0(cell_text, "") == 0)
					status = status || TRUE;
			}
		}
	}
	return (status);
}

void
bio_gui_persons_add_row_if_needed (GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
{
	GtkTreeIter iter;
	gchar *cell_text;

	if (gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(list_store), &iter, path_string))
	{
		gtk_tree_model_get (GTK_TREE_MODEL(list_store), &iter, list_store_column, &cell_text, -1);

		if (cell_text != NULL)
		{
			// Check if the new text is not "" and the old was empty
			if ((g_strcmp0(new_text, "") != 0) && (g_strcmp0(cell_text, "") == 0))
			{
				if (bio_gui_persons_check_other_cells_empty (list_store, &iter, list_store_column))
					bio_gui_persons_add_empty_row (list_store);
			}
		}
	}
}

gboolean
bio_gui_persons_delete_row_if_needed(GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
{
	GtkTreeIter iter;
	gchar *cell_text;

	if (gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(list_store), &iter, path_string))
	{
		gtk_tree_model_get (GTK_TREE_MODEL(list_store), &iter, list_store_column, &cell_text, -1);

		if (cell_text != NULL)
		{
			// Check if the new text is "" and the old was not empty
			if ((g_strcmp0(new_text, "") == 0) && (g_strcmp0(cell_text, "") != 0))
			{
				if (bio_gui_persons_check_other_cells_empty (list_store, &iter, list_store_column))
				{
					gtk_list_store_remove(GTK_LIST_STORE(list_store), &iter);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void
bio_gui_persons_change_cell_text (GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
{
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(list_store), &iter, path_string) == TRUE)
		gtk_list_store_set (GTK_LIST_STORE(list_store), &iter, list_store_column, new_text, -1);
}

void
bio_gui_persons_textrenderer_callback_name_edited (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
	bio_gui_persons_add_row_if_needed (user_data, path_string, PERSON_VIEW_COLUMN_NAME, new_text);

	if (bio_gui_persons_delete_row_if_needed (user_data, path_string, PERSON_VIEW_COLUMN_NAME, new_text) == FALSE)
		bio_gui_persons_change_cell_text (user_data, path_string, PERSON_VIEW_COLUMN_NAME, new_text);
}

void
bio_gui_persons_textrenderer_callback_birthday_edited_list (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
	bio_gui_persons_add_row_if_needed (user_data, path_string, PERSON_VIEW_COLUMN_NAME, new_text);

	if (bio_gui_persons_delete_row_if_needed (user_data, path_string, PERSON_VIEW_COLUMN_BIRTHDAY, new_text) == FALSE)
		bio_gui_persons_change_cell_text (user_data, path_string, PERSON_VIEW_COLUMN_BIRTHDAY, new_text);
}

void
bio_gui_persons_textrenderer_callback_birthday_edited_view(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
	g_signal_emit_by_name (G_OBJECT(user_data), "cursor-changed");
}

void
bio_gui_persons_textrenderer_mark_empty_cell_green(GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter)
{
	gchar *str;

	gtk_tree_model_get (tree_model, iter, PERSON_VIEW_COLUMN_NAME, &str, -1);

	// Is cell empty? Mark it light green!
	if (g_strcmp0 (str, "") == 0)
		g_object_set (cell, "background", "lightgreen", "background-set", TRUE, NULL);
	else
		g_object_set (cell, "background-set", FALSE, NULL); /* print this normal */

	g_free(str);
}

void
bio_gui_persons_textrenderer_mark_date_cell_if_invalid_date(GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter)
{
	gchar *str;
	GDate *date;

	gtk_tree_model_get (tree_model, iter, PERSON_VIEW_COLUMN_BIRTHDAY, &str, -1);

	// Is the date valid?
	date = g_date_new ();
	g_date_set_parse (date, str);
	if (g_date_valid (date))
 		g_object_set (cell, "foreground-set", FALSE, NULL); /* print this normal */
	else
    	g_object_set (cell, "foreground", "red", "foreground-set", TRUE, NULL);

	g_free (str);
	g_date_free (date);
}

void
bio_gui_persons_textrenderer_callback_name (GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
	bio_gui_persons_textrenderer_mark_empty_cell_green (cell, tree_model, iter);
}

void
bio_gui_persons_textrenderer_callback_date(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
	bio_gui_persons_textrenderer_mark_empty_cell_green (cell, tree_model, iter);
	bio_gui_persons_textrenderer_mark_date_cell_if_invalid_date (cell, tree_model, iter);
}

GtkTreeView*
bio_gui_persons (GtkListStore *list)
{
	GtkCellRenderer *TextRendererName, *textrenderer_birthday;
	GtkTreeViewColumn *column0, *column1;
	GtkTreeView *view;

	view = g_object_new (GTK_TYPE_TREE_VIEW, "model", list, "rules-hint", TRUE, "headers-clickable", TRUE, "reorderable", TRUE, "enable-search", TRUE, "search-column", 0,
  "enable-grid-lines", GTK_TREE_VIEW_GRID_LINES_BOTH, "rubber-banding", FALSE, NULL);
	TextRendererName = gtk_cell_renderer_text_new ();
	g_object_set (TextRendererName, "editable", TRUE, NULL);
	g_signal_connect (TextRendererName, "edited", (GCallback) bio_gui_persons_textrenderer_callback_name_edited, list);
	column0 = gtk_tree_view_column_new_with_attributes ("Name", TextRendererName, "text", PERSON_VIEW_COLUMN_NAME, NULL);
	g_object_set(column0, "resizable", TRUE, "clickable", TRUE, "reorderable", TRUE, NULL);
	gtk_tree_view_column_set_cell_data_func (column0, TextRendererName, bio_gui_persons_textrenderer_callback_name, NULL, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(view), column0);

	textrenderer_birthday = gtk_cell_renderer_text_new ();
	g_object_set (textrenderer_birthday, "editable", TRUE, NULL);
	g_signal_connect (textrenderer_birthday, "edited", (GCallback) bio_gui_persons_textrenderer_callback_birthday_edited_list, list);
	g_signal_connect (textrenderer_birthday, "edited", (GCallback) bio_gui_persons_textrenderer_callback_birthday_edited_view, view);
	column1 = gtk_tree_view_column_new_with_attributes ("Birthday", textrenderer_birthday, "text", PERSON_VIEW_COLUMN_BIRTHDAY, NULL);
	g_object_set (column1, "resizable", TRUE, "clickable", TRUE, "reorderable", TRUE, NULL);
	gtk_tree_view_column_set_cell_data_func (column1, textrenderer_birthday, bio_gui_persons_textrenderer_callback_date, NULL, NULL);
	gtk_tree_view_append_column (view, column1);

	g_signal_connect (view, "cursor-changed", G_CALLBACK(bio_gui_persons_date_changed), NULL);

	return (view);
}

gint
bio_gui_delete_event (GtkWindow *widget, GdkEvent event, gpointer daten)
{
	return FALSE;
}

void
bio_gui_close (GtkWidget *widget, gpointer user_data)
{
	gtk_main_quit ();
}

void
bio_gui_open (GtkWidget *widget, gpointer user_data)
{
	GtkListStore *list_store=(GtkListStore*)user_data;

    bio_gui_persons_clear (list_store);
	bio_gui_open_file (list_store);
    bio_gui_persons_add_empty_row (list_store);
}

void
bio_gui_save (GtkWidget *widget, gpointer user_data)
{
	GtkListStore *list_store=(GtkListStore*)user_data;

	bio_write_file_person (list_store);
}

void
bio_gui_menubar_init (GtkWindow *window, GtkMenuBar *menu, GtkListStore *list)
{
	GtkAccelGroup *accel;
	GtkWidget *file_menu_item, *file_open_menu_item, *file_save_menu_item, *file_close_menu_item, *option_menu_item, *console, *help_menu_item, *help_info_menu_item;
	GtkMenu *file_menu, *option_menu, *help_menu;

	/* AccelGroup */
	accel = g_object_new (GTK_TYPE_ACCEL_GROUP, NULL);
	gtk_window_add_accel_group (GTK_WINDOW(window), accel);

	/* menu -> file -> file open */
	file_open_menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN, NULL);
	g_signal_connect(file_open_menu_item, "activate", G_CALLBACK(bio_gui_open), list);

	/* menu -> file -> file save */
	file_save_menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE, NULL);
	g_signal_connect(file_save_menu_item, "activate", G_CALLBACK(bio_gui_save), list);

	/* menu -> file -> file close */
	file_close_menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_CLOSE, accel);
	g_signal_connect (file_close_menu_item, "activate", G_CALLBACK(bio_gui_close), NULL);

	/* menu -> file */
	file_menu = g_object_new (GTK_TYPE_MENU, NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL(file_menu), file_open_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL(file_menu), file_save_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL(file_menu), file_close_menu_item);

	file_menu_item = gtk_menu_item_new_with_mnemonic ("_File");
	gtk_menu_shell_append (GTK_MENU_SHELL(menu), file_menu_item);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(file_menu_item), GTK_WIDGET(file_menu));

	/* menu -> option -> ... */
	option_physical = gtk_check_menu_item_new_with_mnemonic ("_Physical");
	gtk_check_menu_item_set_active ((GtkCheckMenuItem*)option_physical, TRUE);
	g_signal_connect (option_physical, "activate", G_CALLBACK(bio_gui_refresh_chart), map);

	option_emotional = gtk_check_menu_item_new_with_mnemonic ("_Emotional");
	gtk_check_menu_item_set_active ((GtkCheckMenuItem*)option_emotional, TRUE);
	g_signal_connect (option_emotional, "activate", G_CALLBACK(bio_gui_refresh_chart), map);

	option_intellectual = gtk_check_menu_item_new_with_mnemonic ("_Intellectual");
	gtk_check_menu_item_set_active ((GtkCheckMenuItem*)option_intellectual, TRUE);
 	g_signal_connect (option_intellectual, "activate", G_CALLBACK(bio_gui_refresh_chart), map);

	option_total = gtk_check_menu_item_new_with_mnemonic("_Total");
	gtk_check_menu_item_set_active((GtkCheckMenuItem*)option_total, TRUE);
	g_signal_connect(option_total, "activate", G_CALLBACK(bio_gui_refresh_chart), map);

	console = gtk_menu_item_new_with_mnemonic("_Console");
	g_signal_connect(console, "activate", G_CALLBACK(bio_cli_output), NULL);

	/* menu -> option */
	option_menu = g_object_new(GTK_TYPE_MENU, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_physical);
	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_emotional);
	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_intellectual);
	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_total);
	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), console);

	option_menu_item = gtk_menu_item_new_with_mnemonic("_Options");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), option_menu_item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(option_menu_item), GTK_WIDGET(option_menu));

	/* menu -> help -> .. */
	help_info_menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, accel);
	g_signal_connect(help_info_menu_item, "activate", G_CALLBACK(bio_gui_help_info_dialog), NULL);

	/* menu -> help */
	help_menu = g_object_new(GTK_TYPE_MENU, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), help_info_menu_item);
	help_menu_item = gtk_menu_item_new_with_mnemonic(_("_Help"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), help_menu_item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_menu_item), GTK_WIDGET(help_menu));
}

int
main (int argc, char **argv)
{
	GtkWindow *window;
	GtkMenuBar *menu;
	GtkWidget *list_scrolled_window;
	GtkVBox *vbox;
	GtkListStore *list;
	GtkTreeIter iter;
	GtkTreeView *view;
	GtkTreeViewColumn *column0, *column1;

	/* Init type system as soon as possible */
	g_type_init ();

	list = gtk_list_store_new (PERSON_VIEW_COLUMN_NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);
	bio_gui_persons_clear (list);
	bio_gui_persons_add_empty_row (list);

	view = bio_gui_persons (list);

	//new_birthday = g_date_new_dmy(1, 11, 2012);
	//new_viewdata = g_date_new_dmy(1, 1, 1970);

	bio_birthday.day = 1;
	bio_birthday.month = 11;
	bio_birthday.year = 2012;

	gtk_init (&argc, &argv);

	window = g_object_new(GTK_TYPE_WINDOW, "title", "Biorhythmus", "default-width", 400, "default-height", 400, NULL);
	g_signal_connect(window, "delete-event", G_CALLBACK(bio_gui_delete_event), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(bio_gui_close), list);

	menu = g_object_new(GTK_TYPE_MENU_BAR, NULL);
	status = g_object_new(GTK_TYPE_STATUSBAR, NULL);

	/* VBOX */
	vbox = g_object_new(GTK_TYPE_VBOX, NULL);

	/* MAP */
	map = gtk_drawing_area_new();

	bio_gui_menubar_init(window, menu, list);
	
	/* Add Menubar to vbox */
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(menu), FALSE, TRUE, 0);

	/* Add MAP */
#ifdef GTK2
	g_signal_connect(G_OBJECT(map), "expose_event", G_CALLBACK(bio_gui_draw_chart), NULL);
#else
	g_signal_connect(G_OBJECT(map), "draw", G_CALLBACK(bio_gui_draw_chart), NULL);
#endif

	/* Calendar */
	dates = gtk_calendar_new();
	gtk_calendar_set_display_options((GtkCalendar*)dates, GTK_CALENDAR_SHOW_HEADING|GTK_CALENDAR_SHOW_DAY_NAMES);
	g_signal_connect(G_OBJECT(dates), "day-selected", G_CALLBACK(bio_gui_calendar_change), NULL);
	g_signal_connect(G_OBJECT(dates), "month-changed", G_CALLBACK(bio_gui_calendar_change), NULL);

  /* Paned */
	list_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_add (GTK_CONTAINER(list_scrolled_window), GTK_WIDGET(view));

#ifdef GTK2
	GtkWidget *hpaned = gtk_hpaned_new ();
#else
	GtkWidget *hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
#endif
	gtk_paned_pack1 (GTK_PANED (hpaned), dates, FALSE, FALSE);
	gtk_paned_pack2 (GTK_PANED (hpaned), list_scrolled_window, TRUE, TRUE);

#ifdef GTK2
	GtkWidget *vpaned = gtk_vpaned_new ();
#else
	GtkWidget *vpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
#endif
	gtk_paned_pack1 (GTK_PANED (vpaned), map, TRUE, TRUE);
	gtk_paned_pack2 (GTK_PANED (vpaned), hpaned, FALSE, FALSE);

	gtk_box_pack_start (GTK_BOX(vbox), GTK_WIDGET(vpaned), TRUE, TRUE, 0);

	/* Statusbar */
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(status), FALSE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(vbox));
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();

	return (0);
}

/* ex:set ts=4 noet: */
