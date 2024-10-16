/* -*-coding: utf-8;-*- */

/* biorhythm-fileview.c
 * This file is part of Biorhythm
 * Copyright (C) 2003-2024, Gabriel Mainberger
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

typedef struct
{
	GtkWidget *parent_widget;

	GtkWidget *columnview;
	GListStore *liststore;
	gchar *filename;

	gint month;
	gint year;
	gint day;

	gchar *name;
	gboolean saved;
} BiorhythmFileViewPrivate;

struct _BiorhythmFileView
{
	GtkWidget parent_instance;

	BiorhythmFileViewPrivate *priv;
};

enum
{
	DATE_CHANGED,
	NAME_CHANGED,
	LAST_SIGNAL
};

typedef struct
{
	GListStore *liststore;
	GtkListItem *listitem;
} RemoveRowCallbackData;

static guint biorhythm_file_view_signals[LAST_SIGNAL] = { 0 };

static void biorhythm_file_view_dispose (GObject *object);
static void biorhythm_file_view_list_store_clear (GListStore *liststore);
static void biorhythm_file_view_list_store_add_empty_row (GListStore *liststore);
static void biorhythm_file_view_add_new_row_if_required (GtkText *self, GListStore *liststore);
static void biorhythm_file_view_remove_row (GtkText *self, RemoveRowCallbackData *data);
static void biorhythm_file_view_setup_column_binding (GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void biorhythm_file_view_bind_name_column_binding (GtkSignalListItemFactory *factory, GtkListItem *listitem, BiorhythmFileView *file_view);
static void biorhythm_file_view_bind_birthday_column_binding (GtkSignalListItemFactory *factory, GtkListItem *listitem, BiorhythmFileView *file_view);
static void biorhythm_file_view_unbind_column_binding (GtkListItemFactory *factory, GtkListItem *listitem);
static void biorhythm_file_view_append_person (GListStore *liststore, const char *name, const char *birthday);
static void biorhythm_file_view_on_date_changed (GtkColumnView *self, guint position, BiorhythmFileView *file_view);
static void biorhythm_file_view_text_callback_update_view (GtkText *self, BiorhythmFileView *file_view);
static void biorhythm_file_view_text_callback_changed (GtkText *self, BiorhythmFileView *file_view);
static void biorhythm_file_view_mark_date_cell_if_invalid_date (GtkText *self);
static void biorhythm_file_view_read_file_each_element (JsonArray *array, guint index_, JsonNode *element_node, gpointer user_data);
static int biorhythm_file_view_read_file (BiorhythmFileView *file_view);
static void biorhythm_file_view_write_file_each_person (JsonBuilder *builder, GListStore *liststore);
static gboolean biorhythm_file_view_write_file (BiorhythmFileView *file_view);
static GtkWidget *biorhythm_file_view_close_dialog ();
static void biorhythm_file_view_new_file_on_response (GtkDialog *dialog, int response_id, BiorhythmFileView *file_view);
static void biorhythm_file_view_load_from_file_on_response (GtkDialog *dialog, int response_id, BiorhythmFileView *file_view);

G_DEFINE_TYPE_WITH_CODE (BiorhythmFileView, biorhythm_file_view, GTK_TYPE_WIDGET, G_ADD_PRIVATE (BiorhythmFileView))

/****************************************
 *                 Class                *
 ****************************************/

static void
biorhythm_file_view_class_init (BiorhythmFileViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	object_class->dispose = biorhythm_file_view_dispose;

	// https://blog.gtk.org/2020/04/27/custom-widgets-in-gtk-4-layout/
	gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BOX_LAYOUT);

	biorhythm_file_view_signals[DATE_CHANGED] = g_signal_new ("date-changed", G_TYPE_FROM_CLASS (klass),
															  G_SIGNAL_RUN_FIRST,
															  0,
															  NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	biorhythm_file_view_signals[NAME_CHANGED] = g_signal_new ("name-changed", G_TYPE_FROM_CLASS (klass),
															  G_SIGNAL_RUN_FIRST,
															  0,
															  NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

static void
biorhythm_file_view_dispose (GObject *object)
{
	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (BIORHYTHM_FILE_VIEW (object));

	g_clear_pointer (&priv->columnview, gtk_widget_unparent);
}

static void
biorhythm_file_view_init (BiorhythmFileView *file_view)
{
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);
	priv->filename = NULL;
	priv->name = NULL;
	priv->saved = TRUE;

	priv->columnview = gtk_column_view_new (NULL);
	gtk_widget_set_parent (priv->columnview, GTK_WIDGET (file_view));
	gtk_column_view_set_single_click_activate (GTK_COLUMN_VIEW (priv->columnview), TRUE);
	gtk_column_view_set_show_column_separators (GTK_COLUMN_VIEW (priv->columnview), TRUE);
	g_signal_connect (priv->columnview, "activate", G_CALLBACK (biorhythm_file_view_on_date_changed), file_view);

	priv->liststore = g_list_store_new (BIORHYTHM_TYPE_PERSON);
	biorhythm_file_view_list_store_clear (priv->liststore);
	biorhythm_file_view_list_store_add_empty_row (priv->liststore);

	GtkSorter *sorter = g_object_ref (gtk_column_view_get_sorter (GTK_COLUMN_VIEW (priv->columnview)));
	GtkSortListModel *model = gtk_sort_list_model_new (G_LIST_MODEL (priv->liststore), sorter);
	GtkSingleSelection *selection = gtk_single_selection_new (G_LIST_MODEL (model));
	gtk_single_selection_set_autoselect (selection, TRUE);
	gtk_column_view_set_model (GTK_COLUMN_VIEW (priv->columnview), GTK_SELECTION_MODEL (selection));

	// Factory setup and bind for column Name
	GtkListItemFactory *factory = gtk_signal_list_item_factory_new ();
	g_signal_connect (factory, "setup", G_CALLBACK (biorhythm_file_view_setup_column_binding), NULL);
	g_signal_connect (factory, "bind", G_CALLBACK (biorhythm_file_view_bind_name_column_binding), file_view);
	g_signal_connect (factory, "unbind", G_CALLBACK (biorhythm_file_view_unbind_column_binding), NULL);
	GtkColumnViewColumn *column = gtk_column_view_column_new ("Name", factory);
	gtk_column_view_append_column (GTK_COLUMN_VIEW (priv->columnview), column);

	// Factory setup and bind for column Birthday
	factory = gtk_signal_list_item_factory_new ();
	g_signal_connect (factory, "setup", G_CALLBACK (biorhythm_file_view_setup_column_binding), NULL);
	g_signal_connect (factory, "bind", G_CALLBACK (biorhythm_file_view_bind_birthday_column_binding), file_view);
	g_signal_connect (factory, "unbind", G_CALLBACK (biorhythm_file_view_unbind_column_binding), NULL);
	column = gtk_column_view_column_new ("Birthday", factory);
	gtk_column_view_column_set_expand (GTK_COLUMN_VIEW_COLUMN (column), TRUE);
	gtk_column_view_append_column (GTK_COLUMN_VIEW (priv->columnview), column);

	GtkCssProvider *cssProvider = gtk_css_provider_new ();
	gtk_css_provider_load_from_path (cssProvider, "theme.css");
	gtk_style_context_add_provider_for_display (gdk_display_get_default (), GTK_STYLE_PROVIDER (cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

BiorhythmFileView *
biorhythm_file_view_new ()
{
	return g_object_new (BIORHYTHM_TYPE_FILE_VIEW, NULL);
}

static void
biorhythm_file_view_list_store_clear (GListStore *liststore)
{
	g_return_if_fail (G_IS_LIST_STORE (liststore));

	g_list_store_remove_all (liststore);
}

static void
biorhythm_file_view_list_store_add_empty_row (GListStore *liststore)
{
	g_return_if_fail (G_IS_LIST_STORE (liststore));

	g_list_store_append (liststore, biorhythm_person_new ("", ""));
}

static void
biorhythm_file_view_add_new_row_if_required (GtkText *self, GListStore *liststore)
{
	guint n_items = g_list_model_get_n_items (G_LIST_MODEL (liststore));
	if (n_items == 0)
	{
		return;
	}

	BiorhythmPerson *item = g_list_model_get_item (G_LIST_MODEL (liststore), n_items - 1);

	g_return_if_fail (item != NULL);

	if (((g_strcmp0 (item->name, "") != 0) || (g_strcmp0 (item->birthday, "") != 0)))
	{
		g_list_store_append (liststore, biorhythm_person_new ("", ""));
	}
}

static void
biorhythm_file_view_remove_row (GtkText *self, RemoveRowCallbackData *data)
{
	guint position = gtk_list_item_get_position (GTK_LIST_ITEM (data->listitem));
	guint n_items = g_list_model_get_n_items (G_LIST_MODEL (data->liststore));
	if (n_items <= 1)
	{
		return;
	}

	if (position == GTK_INVALID_LIST_POSITION)
	{
		g_print ("biorhythm_file_view_remove_row: GTK_INVALID_LIST_POSITION\n");
		return;
	}

	BiorhythmPerson *item = g_list_model_get_item (G_LIST_MODEL (data->liststore), position);
	if (((g_strcmp0 (item->name, "") == 0) && (g_strcmp0 (item->birthday, "") == 0)))
	{
		// g_print ("Release: %i %p %s %s\n", position, item, item->name, item->birthday);
		g_list_store_remove (data->liststore, position);
	}
}

static void
biorhythm_file_view_setup_column_binding (GtkSignalListItemFactory *factory, GtkListItem *listitem)
{
	GtkWidget *text = gtk_text_new ();
	gtk_list_item_set_child (GTK_LIST_ITEM (listitem), GTK_WIDGET (text));
	gtk_editable_set_alignment (GTK_EDITABLE (text), 0.0);
}

static void
biorhythm_file_view_bind_name_column_binding (GtkSignalListItemFactory *factory,
											  GtkListItem *listitem,
											  BiorhythmFileView *file_view)
{
	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	GtkWidget *text = gtk_list_item_get_child (listitem);
	GtkEntryBuffer *buffer = gtk_text_get_buffer (GTK_TEXT (text));
	BiorhythmPerson *item = BIORHYTHM_PERSON (gtk_list_item_get_item (GTK_LIST_ITEM (listitem)));
	GBinding *bind;

	// g_print ("%i\n", gtk_list_item_get_position (GTK_LIST_ITEM (listitem)));

	gtk_editable_set_text (GTK_EDITABLE (text), item->name);
	gtk_editable_set_position (GTK_EDITABLE (text), 0);

	bind = g_object_bind_property (buffer, "text", item, "name", G_BINDING_DEFAULT);
	g_object_set_data (G_OBJECT (listitem), "bind", bind);

	RemoveRowCallbackData *data_for_biorhythm_file_view_remove_row = g_new0 (RemoveRowCallbackData, 1);
	data_for_biorhythm_file_view_remove_row->liststore = priv->liststore;
	data_for_biorhythm_file_view_remove_row->listitem = listitem;

	g_signal_connect (text, "changed", G_CALLBACK (biorhythm_file_view_remove_row), data_for_biorhythm_file_view_remove_row);
	g_signal_connect (text, "changed", G_CALLBACK (biorhythm_file_view_add_new_row_if_required), priv->liststore);
	g_signal_connect (text, "changed", G_CALLBACK (biorhythm_file_view_text_callback_changed), file_view);
	// TODO: It works but it's executed to to often unneccessary
	g_signal_connect (text, "changed", G_CALLBACK (biorhythm_file_view_text_callback_update_view), file_view);

	// TODO: Free the variable without a segfault
	// g_free (data_for_biorhythm_file_view_remove_row);
}

static void
biorhythm_file_view_bind_birthday_column_binding (GtkSignalListItemFactory *factory,
												  GtkListItem *listitem,
												  BiorhythmFileView *file_view)
{
	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	GtkWidget *text = gtk_list_item_get_child (listitem);
	GtkEntryBuffer *buffer = gtk_text_get_buffer (GTK_TEXT (text));
	BiorhythmPerson *item = BIORHYTHM_PERSON (gtk_list_item_get_item (GTK_LIST_ITEM (listitem)));
	GBinding *bind;

	gtk_editable_set_text (GTK_EDITABLE (text), item->birthday);
	gtk_editable_set_position (GTK_EDITABLE (text), 0);

	bind = g_object_bind_property (buffer, "text", item, "birthday",
								   G_BINDING_DEFAULT);
	g_object_set_data (G_OBJECT (listitem), "bind", bind);

	RemoveRowCallbackData *data_for_biorhythm_file_view_remove_row = g_new0 (RemoveRowCallbackData, 1);
	data_for_biorhythm_file_view_remove_row->liststore = priv->liststore;
	data_for_biorhythm_file_view_remove_row->listitem = listitem;

	g_signal_connect (text, "changed", G_CALLBACK (biorhythm_file_view_remove_row), data_for_biorhythm_file_view_remove_row);
	g_signal_connect (text, "changed", G_CALLBACK (biorhythm_file_view_add_new_row_if_required), priv->liststore);
	g_signal_connect (text, "changed", G_CALLBACK (biorhythm_file_view_mark_date_cell_if_invalid_date), NULL);
	g_signal_connect (text, "changed", G_CALLBACK (biorhythm_file_view_text_callback_changed), file_view);
	// TODO: It works but it's executed to to often unneccessary
	g_signal_connect (text, "changed", G_CALLBACK (biorhythm_file_view_text_callback_update_view), file_view);

	// TODO: Free the variable without a segfault
	// g_free (data_for_biorhythm_file_view_remove_row);
}

static void
biorhythm_file_view_unbind_column_binding (GtkListItemFactory *factory, GtkListItem *listitem)
{
	GBinding *bind = G_BINDING (g_object_get_data (G_OBJECT (listitem), "bind"));

	if (bind)
	{
		g_binding_unbind (bind);
	}
	g_object_set_data (G_OBJECT (listitem), "bind", NULL);
}

static void
biorhythm_file_view_append_person (GListStore *liststore, const char *name, const char *birthday)
{
	g_return_if_fail (G_IS_LIST_STORE (liststore));

	guint n_items = g_list_model_get_n_items (G_LIST_MODEL (liststore));

	g_list_store_insert (liststore, n_items,
						 biorhythm_person_new (name, birthday));
}

static void
biorhythm_file_view_on_date_changed (GtkColumnView *self, guint position, BiorhythmFileView *file_view)
{
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	GtkSelectionModel *selection = gtk_column_view_get_model (self);
	BiorhythmPerson *item = gtk_single_selection_get_selected_item (GTK_SINGLE_SELECTION (selection));
	gchar *str_name;
	gchar *str_birthday;
	GDate *date;

	if (selection == NULL)
	{
		return;
	}

	date = g_date_new ();
	g_date_set_parse (date, item->birthday);

	if (g_date_valid (date))
	{
		priv->day = g_date_get_day (date);
		priv->month = g_date_get_month (date);
		priv->year = g_date_get_year (date);

		priv->name = g_strdup (item->name);

		g_signal_emit_by_name (G_OBJECT (file_view), "date-changed", NULL);
		g_signal_emit_by_name (G_OBJECT (file_view), "name-changed", NULL);
	}

	g_date_free (date);
}

static void
biorhythm_file_view_text_callback_update_view (GtkText *self, BiorhythmFileView *file_view)
{
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	g_signal_emit_by_name (G_OBJECT (priv->columnview), "activate");
}

static void
biorhythm_file_view_text_callback_changed (GtkText *self, BiorhythmFileView *file_view)
{
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	priv->saved = FALSE;
}

static void
biorhythm_file_view_mark_date_cell_if_invalid_date (GtkText *self)
{
	GDate *date;
	GtkEntryBuffer *buffer = gtk_text_get_buffer (GTK_TEXT (self));

	date = g_date_new ();
	g_date_set_parse (date, gtk_entry_buffer_get_text (buffer));

	if (g_date_valid (date))
	{
		gtk_widget_remove_css_class (GTK_WIDGET (self), "error");
	}
	else
	{
		gtk_widget_add_css_class (GTK_WIDGET (self), "error");
	}

	g_date_free (date);
}

static void
biorhythm_file_view_read_file_each_element (JsonArray *array, guint index_, JsonNode *element_node, gpointer user_data)
{
	g_return_if_fail (G_IS_LIST_STORE (user_data));

	JsonObject *element;
	JsonNode *name;
	JsonNode *birthday;
	GListStore *liststore = G_LIST_STORE (user_data);

	element = json_node_get_object (element_node);
	name = json_object_get_member (element, "Name");
	birthday = json_object_get_member (element, "Birthday");

	biorhythm_file_view_append_person (liststore, json_node_dup_string (name), json_node_dup_string (birthday));
}

static int
biorhythm_file_view_read_file (BiorhythmFileView *file_view)
{
	g_return_val_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view), 0);

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);
	JsonParser *parser;
	JsonNode *root;
	JsonNode *result;
	JsonObject *element;
	GError *error;
	JsonArray *array;

	parser = json_parser_new ();
	error = NULL;
	json_parser_load_from_file (parser, priv->filename, &error);
	if (error)
	{
		g_print ("Unable to parse `%s': %s\n", priv->filename, error->message);
		g_error_free (error);
		g_object_unref (parser);
		return 1;
	}

	root = json_parser_get_root (parser);
	element = json_node_get_object (root);
	result = json_object_get_member (element, "Persons");
	array = json_node_get_array (result);
	json_array_foreach_element (array, biorhythm_file_view_read_file_each_element, priv->liststore);

	g_object_unref (parser);

	return 0;
}

static void
biorhythm_file_view_write_file_each_person (JsonBuilder *builder, GListStore *liststore)
{
	guint n_items = g_list_model_get_n_items (G_LIST_MODEL (liststore));
	guint position;
	BiorhythmPerson *item;

	for (position = 0; position < n_items; position++)
	{
		item = g_list_model_get_item (G_LIST_MODEL (liststore), position);

		if ((g_strcmp0 (item->name, "") != 0) && (g_strcmp0 (item->birthday, "") != 0))
		{
			json_builder_begin_object (builder);
			json_builder_set_member_name (builder, "Name");
			json_builder_add_string_value (builder, item->name);
			json_builder_set_member_name (builder, "Birthday");
			json_builder_add_string_value (builder, item->birthday);
			json_builder_end_object (builder);
		}
	}
}

static gboolean
biorhythm_file_view_write_file (BiorhythmFileView *file_view)
{
	g_return_val_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view), FALSE);

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);
	GError *error;
	JsonBuilder *builder = json_builder_new ();

	json_builder_begin_object (builder);

	json_builder_set_member_name (builder, "Persons");

	json_builder_begin_array (builder);
	biorhythm_file_view_write_file_each_person (builder, priv->liststore);
	json_builder_end_array (builder);

	json_builder_end_object (builder);

	JsonGenerator *gen = json_generator_new ();
	JsonNode *root = json_builder_get_root (builder);
	json_generator_set_root (gen, root);

	error = NULL;
	json_generator_to_file (gen, priv->filename, &error);

	json_node_free (root);
	g_object_unref (gen);
	g_object_unref (builder);

	if (error)
	{
		g_print ("Unable to write `%s': %s\n", priv->filename, error->message);
		g_error_free (error);
		return FALSE;
	}

	return TRUE;
}

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

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	if (day != NULL)
	{
		*day = priv->day;
	}

	if (month != NULL)
	{
		*month = priv->month;
	}

	if (year != NULL)
	{
		*year = priv->year;
	}
}

gchar *
biorhythm_file_view_get_filename (BiorhythmFileView *file_view)
{
	g_return_val_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view), NULL);

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	return priv->filename;
}

gchar *
biorhythm_file_view_get_name (BiorhythmFileView *file_view)
{
	g_return_val_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view), NULL);

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	return priv->name;
}

static GtkWidget *
biorhythm_file_view_close_dialog ()
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new (NULL,
									 GTK_DIALOG_DESTROY_WITH_PARENT,
									 GTK_MESSAGE_QUESTION,
									 GTK_BUTTONS_YES_NO,
									 _ ("There are unsaved changes, do you like to continue?"));

	gtk_window_present (GTK_WINDOW (dialog));

	return dialog;
}

static void
biorhythm_file_view_new_file_on_response (GtkDialog *dialog, int response_id, BiorhythmFileView *file_view)
{
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	if (response_id == GTK_RESPONSE_YES)
	{
		biorhythm_file_view_list_store_clear (priv->liststore);
		biorhythm_file_view_list_store_add_empty_row (priv->liststore);
		priv->saved = FALSE;
	}

	gtk_window_destroy (GTK_WINDOW (dialog));
}

void
biorhythm_file_view_new_file (BiorhythmFileView *file_view)
{
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	GtkWidget *dialog = biorhythm_file_view_close_dialog ();

	g_signal_connect (dialog, "response",
					  G_CALLBACK (biorhythm_file_view_new_file_on_response),
					  file_view);
}

static void
biorhythm_file_view_load_from_file_on_response (GtkDialog *dialog, int response_id, BiorhythmFileView *file_view)
{
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	int result;
	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	if (response_id == GTK_RESPONSE_YES)
	{
		biorhythm_file_view_list_store_clear (priv->liststore);
		result = biorhythm_file_view_read_file (file_view);
		biorhythm_file_view_list_store_add_empty_row (priv->liststore);
		priv->saved = result;
	}

	gtk_window_destroy (GTK_WINDOW (dialog));
}

void
biorhythm_file_view_load_from_file (BiorhythmFileView *file_view, gchar *filename)
{
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));
	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	priv->filename = g_strdup (filename);

	GtkWidget *dialog = biorhythm_file_view_close_dialog ();

	g_signal_connect (dialog, "response",
					  G_CALLBACK (biorhythm_file_view_load_from_file_on_response),
					  file_view);
}

void
biorhythm_file_view_save_to_new_file (BiorhythmFileView *file_view, gchar *filename)
{
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	priv->filename = g_strdup (filename);

	gboolean result = biorhythm_file_view_write_file (file_view);
	priv->saved = result;
}

void
biorhythm_file_view_save_to_file (BiorhythmFileView *file_view)
{
	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	BiorhythmFileViewPrivate *priv = biorhythm_file_view_get_instance_private (file_view);

	gboolean result = biorhythm_file_view_write_file (file_view);
	priv->saved = result;
}

/* ex:set ts=4 noet: */
