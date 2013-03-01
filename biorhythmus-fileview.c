/* -*-coding: utf-8;-*- */

/* biorhythmus-fileview.c
 * This file is part of Biorhythmus
 * Copyright (C) 2003-2013, Gabriel Mainberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "biorhythmus-fileview.h"

struct _BiorhythmusFileViewPrivate
{
	GtkWidget *parent_widget;

	GtkListStore *list_store;
	gchar *filename;

	GtkCellRenderer *text_renderer_name;
	GtkCellRenderer *text_renderer_birthday;

	gint month;
    gint year;
	gint day;
};

enum
{
    VIEW_COLUMN_NAME = 0,
	VIEW_COLUMN_BIRTHDAY,
	VIEW_COLUMN_NUM_COLS
};

enum {
    DATE_CHANGED,
	LAST_SIGNAL
};

static guint biorhythmus_file_view_signals[LAST_SIGNAL] = { 0 };

#define BIORHYTHMUS_FILE_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), BIORHYTHMUS_TYPE_FILE_VIEW, BiorhythmusFileViewPrivate));

G_DEFINE_TYPE (BiorhythmusFileView, biorhythmus_file_view, GTK_TYPE_TREE_VIEW)

void
biorhythmus_file_view_list_store_clear (GtkListStore *list_store)
{
	gtk_list_store_clear (list_store);
}

void
biorhythmus_file_view_list_store_add_empty_row (GtkListStore *list_store)
{
	GtkTreeIter iter;

	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, VIEW_COLUMN_NAME, "", VIEW_COLUMN_BIRTHDAY, "", -1);
}

void
biorhythmus_file_view_read_file_each_element (JsonArray *array, guint index_, JsonNode *element_node, gpointer user_data)
{
	JsonObject *element;
	JsonNode *name, *birthday;
	GtkTreeIter iter;
	GtkListStore *list_store = (GtkListStore*)user_data;

	element = json_node_get_object (element_node);
	name = json_object_get_member (element, "Name");
	birthday = json_object_get_member (element, "Birthday");

	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, VIEW_COLUMN_NAME, json_node_dup_string(name), VIEW_COLUMN_BIRTHDAY, json_node_dup_string(birthday), -1);
}

int
biorhythmus_file_view_read_file (BiorhythmusFileView *self)
{
	JsonParser *parser;
	JsonNode *root, *result;
	JsonObject *element;
	GError *error;
	JsonArray *array;

	parser = json_parser_new ();
	error = NULL;
	json_parser_load_from_file (parser, self->priv->filename, &error);
	if (error)
	{
		g_print ("Unable to parse `%s': %s\n", self->priv->filename, error->message);
		g_error_free (error);
    	g_object_unref (parser);
		return 1;
	}

	root = json_parser_get_root (parser);
	element = json_node_get_object (root);
	result = json_object_get_member (element, "Persons");
	array = json_node_get_array (result);
	json_array_foreach_element (array, biorhythmus_file_view_read_file_each_element, self->priv->list_store);

	g_object_unref (parser);

	return 0;
}

int biorhythmus_file_view_load_from_file (BiorhythmusFileView *self, gchar *filename)
{
    self->priv->filename = g_strdup (filename);
	
	biorhythmus_file_view_list_store_clear (self->priv->list_store);
    biorhythmus_file_view_read_file (self);
    biorhythmus_file_view_list_store_add_empty_row (self->priv->list_store);
}

void
biorhythmus_file_view_write_file_each_person (JsonBuilder *builder, GtkListStore *list_store)
{
	gboolean valid;
	GtkTreeIter iter;
	gchar *cell_text_name, *cell_text_birthday;

	valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL(list_store), &iter);
	while ( valid )
	{
		gtk_tree_model_get (GTK_TREE_MODEL(list_store), &iter, VIEW_COLUMN_NAME, &cell_text_name, VIEW_COLUMN_BIRTHDAY, &cell_text_birthday, -1);

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
biorhythmus_file_view_write_file (BiorhythmusFileView *self)
{
	GError *error;

	JsonBuilder *builder = json_builder_new ();
	json_builder_begin_object (builder);

	json_builder_set_member_name (builder, "Persons");

	json_builder_begin_array (builder);
	biorhythmus_file_view_write_file_each_person (builder, self->priv->list_store);
	json_builder_end_array (builder);

	json_builder_end_object (builder);

	JsonGenerator *gen = json_generator_new ();
	JsonNode *root = json_builder_get_root (builder);
	json_generator_set_root (gen, root);

	error = NULL;
	json_generator_to_file (gen, self->priv->filename, &error);

	json_node_free (root);
	g_object_unref (gen);
	g_object_unref (builder);

	if (error)
	{
		g_print ("Unable to write `%s': %s\n", self->priv->filename, error->message);
		g_error_free (error);
		return 1;
	}

	return 0;
}

int
biorhythmus_file_view_save_to_new_file (BiorhythmusFileView *self, gchar *filename)
{
	self->priv->filename = g_strdup (filename);

	biorhythmus_file_view_write_file (self);
}

int
biorhythmus_file_view_save_to_file (BiorhythmusFileView *self)
{
	biorhythmus_file_view_write_file (self);
}

void
biorhythmus_file_view_date_changed(GtkTreeView *tree_view, gpointer user_data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *str;
	GDate *date;
	BiorhythmusFileView *self = (BiorhythmusFileView*)user_data;

	selection = gtk_tree_view_get_selection (tree_view);
	if(gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, VIEW_COLUMN_BIRTHDAY, &str, -1);

		date = g_date_new ();
		g_date_set_parse (date, str);

		if(g_date_valid(date))
		{
			self->priv->day = g_date_get_day (date);
			self->priv->month = g_date_get_month (date);
			self->priv->year = g_date_get_year (date);

			g_signal_emit_by_name((gpointer)user_data, "date-changed", date);
		}

		g_free (str);
		g_date_free (date);
	}

	//bio_gui_update_statusbar ();
}

gboolean
biorhythmus_file_view_check_other_cells_empty (GtkListStore *list_store, GtkTreeIter *iter, gint except_column)
{
	gint i;
	gchar *cell_text;
	gboolean status=FALSE;

	for (i = 0 ; i < VIEW_COLUMN_NUM_COLS ; i++)
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
biorhythmus_file_view_add_row_if_needed (GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
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
				if (biorhythmus_file_view_check_other_cells_empty (list_store, &iter, list_store_column))
					biorhythmus_file_view_list_store_add_empty_row (list_store);
			}
		}
	}
}

gboolean
biorhythmus_file_view_delete_row_if_needed (GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
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
				if (biorhythmus_file_view_check_other_cells_empty (list_store, &iter, list_store_column))
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
biorhythmus_file_view_change_cell_text (GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
{
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(list_store), &iter, path_string) == TRUE)
		gtk_list_store_set (GTK_LIST_STORE(list_store), &iter, list_store_column, new_text, -1);
}

void
biorhythmus_file_view_textrenderer_callback_name_edited (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
	biorhythmus_file_view_add_row_if_needed (user_data, path_string, VIEW_COLUMN_NAME, new_text);

	if (biorhythmus_file_view_delete_row_if_needed (user_data, path_string, VIEW_COLUMN_NAME, new_text) == FALSE)
		biorhythmus_file_view_change_cell_text (user_data, path_string, VIEW_COLUMN_NAME, new_text);
}

void
biorhythmus_file_view_textrenderer_callback_birthday_edited_list (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
	biorhythmus_file_view_add_row_if_needed (user_data, path_string, VIEW_COLUMN_NAME, new_text);

	if (biorhythmus_file_view_delete_row_if_needed (user_data, path_string, VIEW_COLUMN_BIRTHDAY, new_text) == FALSE)
		biorhythmus_file_view_change_cell_text (user_data, path_string, VIEW_COLUMN_BIRTHDAY, new_text);
}

void
biorhythmus_file_view_textrenderer_callback_birthday_edited_view (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
	g_signal_emit_by_name (G_OBJECT(user_data), "cursor-changed");
}

void
biorhythmus_file_view_textrenderer_mark_empty_cell_green (GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter)
{
	gchar *str;

	gtk_tree_model_get (tree_model, iter, VIEW_COLUMN_NAME, &str, -1);

	// Is cell empty? Mark it light green!
	if (g_strcmp0 (str, "") == 0)
		g_object_set (cell, "background", "lightgreen", "background-set", TRUE, NULL);
	else
		g_object_set (cell, "background-set", FALSE, NULL); /* print this normal */

	g_free(str);
}

void
biorhythmus_file_view_textrenderer_mark_date_cell_if_invalid_date (GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter)
{
	gchar *str;
	GDate *date;

	gtk_tree_model_get (tree_model, iter, VIEW_COLUMN_BIRTHDAY, &str, -1);

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
biorhythmus_file_view_textrenderer_callback_name (GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
	biorhythmus_file_view_textrenderer_mark_empty_cell_green (cell, tree_model, iter);
}

void
bio_gui_persons_textrenderer_callback_date(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
	biorhythmus_file_view_textrenderer_mark_empty_cell_green (cell, tree_model, iter);
	biorhythmus_file_view_textrenderer_mark_date_cell_if_invalid_date (cell, tree_model, iter);
}

/**
 * biorhythmus_file_view_get_date:
 * @calendar: a #GtkCalendar
 * @year: (out) (allow-none): location to store the year as a decimal
 *     number (e.g. 2011), or %NULL
 * @month: (out) (allow-none): location to store the month number
 *     (between 1 and 12), or %NULL
 * @day: (out) (allow-none): location to store the day number (between
 *     1 and 31), or %NULL
 *
 * Obtains the selected date from a #GtkCalendar.
*/

void
biorhythmus_file_view_get_date (BiorhythmusFileView *file_view, guint *day, guint *month, guint *year)
{
	BiorhythmusFileViewPrivate *priv;
	
	g_return_if_fail (BIORHYTHMUS_IS_FILE_VIEW (file_view));

	priv = file_view->priv;

	if (day)
		*day = priv->day;

	if (month)
		*month = priv->month;

	if (year)
		*year = priv->year;
}

static void
biorhythmus_file_view_class_init (BiorhythmusFileViewClass *klass)
{
	biorhythmus_file_view_signals[DATE_CHANGED] = 
				g_signal_new("date-changed", G_TYPE_FROM_CLASS(klass),
				G_SIGNAL_RUN_FIRST,
				G_STRUCT_OFFSET(BiorhythmusFileViewClass, date_changed),
	//			NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);
				NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	g_type_class_add_private (klass, sizeof (BiorhythmusFileViewPrivate));
}

static void
biorhythmus_file_view_init (BiorhythmusFileView *self)
{
	BiorhythmusFileViewPrivate *priv;

	self->priv = priv = BIORHYTHMUS_FILE_VIEW_GET_PRIVATE (self);
    
	priv->list_store = gtk_list_store_new (VIEW_COLUMN_NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);

	biorhythmus_file_view_list_store_clear (priv->list_store);
	biorhythmus_file_view_list_store_add_empty_row (priv->list_store);

	g_object_set (GTK_TREE_VIEW (self), "model", priv->list_store, 
										"rules-hint", TRUE, 
										"headers-clickable", TRUE, 
										"reorderable", TRUE, 
										"enable-search", TRUE, 
										"search-column", 0,
										"enable-grid-lines", GTK_TREE_VIEW_GRID_LINES_BOTH, 
										"rubber-banding", FALSE,
										NULL);

// gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(priv->treestore));

	priv->text_renderer_name = gtk_cell_renderer_text_new ();
	g_object_set (priv->text_renderer_name, "editable", TRUE, NULL);
	g_signal_connect (priv->text_renderer_name, "edited", (GCallback) biorhythmus_file_view_textrenderer_callback_name_edited, priv->list_store);
	GtkTreeViewColumn *column_name = gtk_tree_view_column_new_with_attributes ("Name", priv->text_renderer_name, 
																				"text", VIEW_COLUMN_NAME,
																				NULL);
	g_object_set(column_name, "resizable", TRUE, 
							"clickable", TRUE, 
							"reorderable", TRUE, 
							NULL);
	gtk_tree_view_column_set_cell_data_func (column_name, priv->text_renderer_name, biorhythmus_file_view_textrenderer_callback_name, NULL, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (self), column_name);

	priv->text_renderer_birthday = gtk_cell_renderer_text_new ();
	g_object_set (priv->text_renderer_birthday, "editable", TRUE, NULL);
	g_signal_connect (priv->text_renderer_birthday, "edited", (GCallback) biorhythmus_file_view_textrenderer_callback_birthday_edited_list, priv->list_store);
	g_signal_connect (priv->text_renderer_birthday, "edited", (GCallback) biorhythmus_file_view_textrenderer_callback_birthday_edited_view, self);
	GtkTreeViewColumn *column_birthday = gtk_tree_view_column_new_with_attributes ("Birthday", priv->text_renderer_birthday, 
																					"text", VIEW_COLUMN_BIRTHDAY,
																					NULL);
	g_object_set (column_birthday, "resizable", TRUE,
									"clickable", TRUE,
									"reorderable", TRUE,
									NULL);
	gtk_tree_view_column_set_cell_data_func (column_birthday, priv->text_renderer_birthday, bio_gui_persons_textrenderer_callback_date, NULL, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (self), column_birthday);

	g_signal_connect (GTK_TREE_VIEW (self), "cursor-changed", G_CALLBACK(biorhythmus_file_view_date_changed), self);
}

GtkWidget*
biorhythmus_file_view_new ()
{
	//g_return_val_if_fail (GTK_IS_WIDGET (parent_widget), NULL);

	return GTK_WIDGET (g_object_new (BIORHYTHMUS_TYPE_FILE_VIEW, NULL));
}

/* ex:set ts=4 noet: */
