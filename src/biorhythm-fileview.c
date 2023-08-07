/* -*-coding: utf-8;-*- */

/* biorhythm-fileview.c
 * This file is part of Biorhythm
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

#include "biorhythm-fileview.h"

void biorhythm_file_view_list_store_clear (GtkListStore *list_store);
void biorhythm_file_view_list_store_add_empty_row (GtkListStore *list_store);
void biorhythm_file_view_read_file_each_element (JsonArray *array, guint index_, JsonNode *element_node, gpointer user_data);
int biorhythm_file_view_read_file (BiorhythmFileView *file_view);
void biorhythm_file_view_write_file_each_person (JsonBuilder *builder, GtkListStore *list_store);
gboolean biorhythm_file_view_write_file (BiorhythmFileView *file_view);
void biorhythm_file_view_on_date_changed (GtkTreeView *tree_view, BiorhythmFileView *file_view);
gboolean biorhythm_file_view_check_other_cells_empty (GtkListStore *list_store, GtkTreeIter *iter, gint except_column);
void biorhythm_file_view_add_row_if_needed (GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text);
gboolean biorhythm_file_view_delete_row_if_needed (GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text);
void biorhythm_file_view_change_cell_text (GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text);
void biorhythm_file_view_textrenderer_callback_update_view (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data);
void biorhythm_file_view_textrenderer_callback_name_edited (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data);
void biorhythm_file_view_textrenderer_callback_birthday_edited_list (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data);
void biorhythm_file_view_textrenderer_callback_edited (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data);
void biorhythm_file_view_textrenderer_mark_empty_cell_green (GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter);
void biorhythm_file_view_textrenderer_mark_date_cell_if_invalid_date (GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter);
void biorhythm_file_view_textrenderer_callback_name (GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data);
void biorhythm_file_view_textrenderer_callback_date (GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data);
gboolean biorhythm_file_view_close_dialog ();
gboolean biorhythm_file_view_close_file_dialog (BiorhythmFileView *file_view);

typedef struct
{
	GtkWidget *parent_widget;

	GtkListStore *list_store;
	gchar *filename;

	GtkCellRenderer *text_renderer_name;
	GtkCellRenderer *text_renderer_birthday;

	gint month;
	gint year;
	gint day;

	gchar *name;
	gboolean saved;
} BiorhythmFileViewPrivate;

struct _BiorhythmFileView
{
	GtkTreeView parent_instance;

	BiorhythmFileViewPrivate *priv;
};

enum
{
    VIEW_COLUMN_NAME = 0,
	VIEW_COLUMN_BIRTHDAY,
	VIEW_COLUMN_NUM_COLS
};

enum {
    DATE_CHANGED,
	NAME_CHANGED,
	LAST_SIGNAL
};

static guint biorhythm_file_view_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE_WITH_PRIVATE (BiorhythmFileView, biorhythm_file_view, GTK_TYPE_TREE_VIEW)

/****************************************
 *                 Class                *
 ****************************************/

static void
biorhythm_file_view_class_init (BiorhythmFileViewClass *klass)
{
	biorhythm_file_view_signals[DATE_CHANGED] =
				g_signal_new ("date-changed", G_TYPE_FROM_CLASS (klass),
				G_SIGNAL_RUN_FIRST,
				0,
				NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	biorhythm_file_view_signals[NAME_CHANGED] =
				g_signal_new ("name-changed", G_TYPE_FROM_CLASS (klass),
				G_SIGNAL_RUN_FIRST,
				0,
				NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

static void
biorhythm_file_view_init (BiorhythmFileView *file_view)
{
	BiorhythmFileViewPrivate *priv;

	file_view->priv = priv = biorhythm_file_view_get_instance_private (file_view);

	priv->name = NULL;

	priv->saved = TRUE;

	priv->list_store = gtk_list_store_new (VIEW_COLUMN_NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);

	biorhythm_file_view_list_store_clear (priv->list_store);
	biorhythm_file_view_list_store_add_empty_row (priv->list_store);

	g_object_set (GTK_TREE_VIEW (file_view), "model", priv->list_store,
										"headers-clickable", TRUE,
										"reorderable", TRUE,
										"enable-search", TRUE,
										"search-column", 0,
										"enable-grid-lines", GTK_TREE_VIEW_GRID_LINES_BOTH,
										"rubber-banding", FALSE,
										NULL);

	priv->text_renderer_name = gtk_cell_renderer_text_new ();
	g_object_set (priv->text_renderer_name, "editable", TRUE, NULL);
	g_signal_connect (G_OBJECT (priv->text_renderer_name), "edited", G_CALLBACK (biorhythm_file_view_textrenderer_callback_name_edited), priv->list_store);
	g_signal_connect (G_OBJECT (priv->text_renderer_name), "edited", G_CALLBACK (biorhythm_file_view_textrenderer_callback_edited), file_view);
	g_signal_connect (G_OBJECT (priv->text_renderer_name), "edited", G_CALLBACK (biorhythm_file_view_textrenderer_callback_update_view), file_view);
	GtkTreeViewColumn *column_name = gtk_tree_view_column_new_with_attributes (_("Name"), priv->text_renderer_name,
																				"text", VIEW_COLUMN_NAME,
																				NULL);
	g_object_set(column_name, "resizable", TRUE,
							"clickable", TRUE,
							"reorderable", TRUE,
							NULL);
	gtk_tree_view_column_set_cell_data_func (column_name, priv->text_renderer_name, biorhythm_file_view_textrenderer_callback_name, NULL, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (file_view), column_name);

	priv->text_renderer_birthday = gtk_cell_renderer_text_new ();
	g_object_set (priv->text_renderer_birthday, "editable", TRUE, NULL);
	g_signal_connect (priv->text_renderer_birthday, "edited", G_CALLBACK (biorhythm_file_view_textrenderer_callback_birthday_edited_list), priv->list_store);
	g_signal_connect (priv->text_renderer_birthday, "edited", G_CALLBACK (biorhythm_file_view_textrenderer_callback_edited), file_view);
	g_signal_connect (priv->text_renderer_birthday, "edited", G_CALLBACK (biorhythm_file_view_textrenderer_callback_update_view), file_view);
	GtkTreeViewColumn *column_birthday = gtk_tree_view_column_new_with_attributes (_("Birthday"), priv->text_renderer_birthday,
																					"text", VIEW_COLUMN_BIRTHDAY,
																					NULL);
	g_object_set (column_birthday, "resizable", TRUE,
									"clickable", TRUE,
									"reorderable", TRUE,
									NULL);
	gtk_tree_view_column_set_cell_data_func (column_birthday, priv->text_renderer_birthday, biorhythm_file_view_textrenderer_callback_date, NULL, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (file_view), column_birthday);

	g_signal_connect (G_OBJECT(file_view), "cursor-changed", G_CALLBACK (biorhythm_file_view_on_date_changed), file_view);
}

GtkWidget*
biorhythm_file_view_new ()
{
	return GTK_WIDGET (g_object_new (BIORHYTHM_TYPE_FILE_VIEW, NULL));
}

/****************************************
 *           Private Functions          *
 ****************************************/

void
biorhythm_file_view_list_store_clear (GtkListStore *list_store)
{
	gtk_list_store_clear (list_store);
}

void
biorhythm_file_view_list_store_add_empty_row (GtkListStore *list_store)
{
	GtkTreeIter iter;

	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, VIEW_COLUMN_NAME, "", VIEW_COLUMN_BIRTHDAY, "", -1);
}

void
biorhythm_file_view_read_file_each_element (JsonArray *array, guint index_, JsonNode *element_node, gpointer user_data)
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
biorhythm_file_view_read_file (BiorhythmFileView *file_view)
{
	JsonParser *parser;
	JsonNode *root, *result;
	JsonObject *element;
	GError *error;
	JsonArray *array;

	parser = json_parser_new ();
	error = NULL;
	json_parser_load_from_file (parser, file_view->priv->filename, &error);
	if (error)
	{
		g_print ("Unable to parse `%s': %s\n", file_view->priv->filename, error->message);
		g_error_free (error);
    		g_object_unref (parser);
		return 1;
	}

	root = json_parser_get_root (parser);
	element = json_node_get_object (root);
	result = json_object_get_member (element, "Persons");
	array = json_node_get_array (result);
	json_array_foreach_element (array, biorhythm_file_view_read_file_each_element, file_view->priv->list_store);

	g_object_unref (parser);

	return 0;
}

void
biorhythm_file_view_write_file_each_person (JsonBuilder *builder, GtkListStore *list_store)
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

gboolean
biorhythm_file_view_write_file (BiorhythmFileView *file_view)
{
	GError *error;

	JsonBuilder *builder = json_builder_new ();
	json_builder_begin_object (builder);

	json_builder_set_member_name (builder, "Persons");

	json_builder_begin_array (builder);
	biorhythm_file_view_write_file_each_person (builder, file_view->priv->list_store);
	json_builder_end_array (builder);

	json_builder_end_object (builder);

	JsonGenerator *gen = json_generator_new ();
	JsonNode *root = json_builder_get_root (builder);
	json_generator_set_root (gen, root);

	error = NULL;
	json_generator_to_file (gen, file_view->priv->filename, &error);

	json_node_free (root);
	g_object_unref (gen);
	g_object_unref (builder);

	if (error)
	{
		g_print ("Unable to write `%s': %s\n", file_view->priv->filename, error->message);
		g_error_free (error);
		return FALSE;
	}

	return TRUE;
}

void
biorhythm_file_view_on_date_changed (GtkTreeView *tree_view, BiorhythmFileView *file_view)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *str_name, *str_birthday;
	GDate *date;

	selection = gtk_tree_view_get_selection (tree_view);
	if (selection == NULL)
		return;

	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, VIEW_COLUMN_NAME, &str_name, VIEW_COLUMN_BIRTHDAY, &str_birthday, -1);

		date = g_date_new ();
		g_date_set_parse (date, str_birthday);

		if (g_date_valid (date))
		{
			file_view->priv->day = g_date_get_day (date);
			file_view->priv->month = g_date_get_month (date);
			file_view->priv->year = g_date_get_year (date);

			file_view->priv->name = g_strdup (str_name);

			g_signal_emit_by_name(G_OBJECT (file_view), "date-changed", date); //FIXME: Shouln't it be NULL?
			g_signal_emit_by_name(G_OBJECT (file_view), "name-changed", NULL);
		}

		g_free (str_name);
		g_free (str_birthday);
		g_date_free (date);
	}
}

gboolean
biorhythm_file_view_check_other_cells_empty (GtkListStore *list_store, GtkTreeIter *iter, gint except_column)
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
biorhythm_file_view_add_row_if_needed (GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
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
				if (biorhythm_file_view_check_other_cells_empty (list_store, &iter, list_store_column))
					biorhythm_file_view_list_store_add_empty_row (list_store);
			}
		}
	}
}

gboolean
biorhythm_file_view_delete_row_if_needed (GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
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
				if (biorhythm_file_view_check_other_cells_empty (list_store, &iter, list_store_column))
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
biorhythm_file_view_change_cell_text (GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
{
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(list_store), &iter, path_string) == TRUE)
		gtk_list_store_set (GTK_LIST_STORE(list_store), &iter, list_store_column, new_text, -1);
}

void
biorhythm_file_view_textrenderer_callback_name_edited (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
	biorhythm_file_view_add_row_if_needed (user_data, path_string, VIEW_COLUMN_NAME, new_text);

	if (biorhythm_file_view_delete_row_if_needed (user_data, path_string, VIEW_COLUMN_NAME, new_text) == FALSE)
		biorhythm_file_view_change_cell_text (user_data, path_string, VIEW_COLUMN_NAME, new_text);
}

void
biorhythm_file_view_textrenderer_callback_birthday_edited_list (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
	biorhythm_file_view_add_row_if_needed (user_data, path_string, VIEW_COLUMN_NAME, new_text);

	if (biorhythm_file_view_delete_row_if_needed (user_data, path_string, VIEW_COLUMN_BIRTHDAY, new_text) == FALSE)
		biorhythm_file_view_change_cell_text (user_data, path_string, VIEW_COLUMN_BIRTHDAY, new_text);
}

void
biorhythm_file_view_textrenderer_callback_update_view (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
	g_signal_emit_by_name (G_OBJECT(user_data), "cursor-changed");
}

void
biorhythm_file_view_textrenderer_callback_edited (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
	BiorhythmFileView *file_view = user_data;

	file_view->priv->saved = FALSE;
}

void
biorhythm_file_view_textrenderer_mark_empty_cell_green (GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter)
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
biorhythm_file_view_textrenderer_mark_date_cell_if_invalid_date (GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter)
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
biorhythm_file_view_textrenderer_callback_name (GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
	biorhythm_file_view_textrenderer_mark_empty_cell_green (cell, tree_model, iter);
}

void
biorhythm_file_view_textrenderer_callback_date (GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
	biorhythm_file_view_textrenderer_mark_empty_cell_green (cell, tree_model, iter);
	biorhythm_file_view_textrenderer_mark_date_cell_if_invalid_date (cell, tree_model, iter);
}

gboolean
biorhythm_file_view_close_dialog ()
{
	GtkWidget *dialog;
	int result;

	dialog = gtk_message_dialog_new (NULL,
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					_("There are unsaved changes, do you like to continue?"));

	result = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	return result == GTK_RESPONSE_YES;
}

gboolean
biorhythm_file_view_close_file_dialog (BiorhythmFileView *file_view)
{
	if (file_view->priv->saved == FALSE)
		return biorhythm_file_view_close_dialog ();
	else
		return TRUE;
}

/****************************************
 *              Public API              *
 ****************************************/

/**
 * biorhythm_file_view_get_date:
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
biorhythm_file_view_get_date (BiorhythmFileView *file_view, guint *day, guint *month, guint *year)
{
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	BiorhythmFileViewPrivate *priv = file_view->priv;

	if (day)
		*day = priv->day;

	if (month)
		*month = priv->month;

	if (year)
		*year = priv->year;
}

gchar *
biorhythm_file_view_get_name (BiorhythmFileView *file_view)
{
	g_return_val_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view), NULL);

	BiorhythmFileViewPrivate *priv = file_view->priv;

	return priv->name;
}

gboolean
biorhythm_file_view_new_file (BiorhythmFileView *file_view)
{
	g_return_val_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view), FALSE);

	if (biorhythm_file_view_close_file_dialog (file_view) == TRUE)
	{
		biorhythm_file_view_list_store_clear (file_view->priv->list_store);
		biorhythm_file_view_list_store_add_empty_row (file_view->priv->list_store);
		file_view->priv->saved = TRUE;
	}

	return TRUE;
}

gboolean
biorhythm_file_view_load_from_file (BiorhythmFileView *file_view, gchar *filename)
{
	gboolean result = FALSE;

	g_return_val_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view), FALSE);

	file_view->priv->filename = g_strdup (filename);

	if (biorhythm_file_view_close_file_dialog (file_view) == TRUE)
	{
		biorhythm_file_view_list_store_clear (file_view->priv->list_store);
		result = biorhythm_file_view_read_file (file_view);
		biorhythm_file_view_list_store_add_empty_row (file_view->priv->list_store);
		file_view->priv->saved = TRUE;
	}

	return result;
}

gboolean
biorhythm_file_view_save_to_new_file (BiorhythmFileView *file_view, gchar *filename)
{
	gboolean result;

	g_return_val_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view), FALSE);

	file_view->priv->filename = g_strdup (filename);

	result = biorhythm_file_view_write_file (file_view);
	file_view->priv->saved = result;
	return result;
}

gboolean
biorhythm_file_view_save_to_file (BiorhythmFileView *file_view)
{
	gboolean result;

	g_return_val_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view), FALSE);

	result = biorhythm_file_view_write_file (file_view);
	file_view->priv->saved = result;
	return result;
}

/* ex:set ts=4 noet: */
