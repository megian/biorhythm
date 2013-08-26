/* -*-coding: utf-8;-*- */

/* biorhythmus-print.c
 * This file is part of Biorhythmus
 * Copyright (C) 2003-2013, Gabriel Mainberger
 *
 * Some parts of this code is taken from EOG (GPL 2).
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

#include "biorhythmus-print.h"

#define BIORHYTHMUS_PRINT_SETTINGS_FILE "biorhythmus-print-settings.ini"
#define BIORHYTHMUS_PAGE_SETUP_GROUP "Page Setup"
#define BIORHYTHMUS_PRINT_SETTINGS_GROUP "Print Settings"

typedef struct {
	BiorhythmusChart *chart;
	gdouble scale_factor;
} BiorhythmusPrintData;

static gchar *dot_dir = NULL;

static gboolean
biorhythmus_util_ensure_dir_exists (const char *dir)
{
	if (g_file_test (dir, G_FILE_TEST_IS_DIR))
		return TRUE;

	if (g_mkdir_with_parents (dir, 0700) == 0)
		return TRUE;

	g_warning ("Failed to create directory %s", dir);
	return FALSE;
}

const gchar *
biorhythmus_util_dot_dir ()
{
	if (dot_dir == NULL)
	{
		gboolean exists;

		dot_dir = g_build_filename (g_get_user_config_dir (), "biorhythmus", NULL);

		exists = biorhythmus_util_ensure_dir_exists (dot_dir);
		if (G_UNLIKELY (!exists))
		{
			static gboolean printed_warning = FALSE;

			if (!printed_warning)
			{
				g_warning ("Biorhythmus could not save some of your preferences in its settings directory due to a file with the same name (%s) blocking its creation. Please remove that file, or move it away.", dot_dir);
				printed_warning = TRUE;
			}
			g_free (dot_dir);
			dot_dir = NULL;
			return NULL;
		}
	}

	return dot_dir;
}

static void
biorhythmus_print_draw_page (GtkPrintOperation *operation, GtkPrintContext *context, gint page_nr, gpointer user_data)
{
	cairo_t *cr;
	gdouble scale_factor;
	gdouble p_width, p_height;
	BiorhythmusPrintData *data;
	GtkPageSetup *page_setup;

	data = (BiorhythmusPrintData *) user_data;

	cr = gtk_print_context_get_cairo_context (context);

	page_setup = gtk_print_context_get_page_setup (context);
	p_width =  gtk_page_setup_get_page_width (page_setup, GTK_UNIT_POINTS);
	p_height = gtk_page_setup_get_page_height (page_setup, GTK_UNIT_POINTS);

	scale_factor = data->scale_factor / 100;
	cairo_scale (cr, scale_factor, scale_factor);

  	biorhythmus_chart_draw_cairo (data->chart, cr, p_height, p_width);
}

static void
biorhythmus_print_end_print (GtkPrintOperation *operation, GtkPrintContext *context, gpointer user_data)
{
	BiorhythmusPrintData *data = (BiorhythmusPrintData*) user_data;

	g_slice_free (BiorhythmusPrintData, data);
}

GtkPrintOperation *
biorhythmus_print_operation_new (BiorhythmusChart *chart)
{
	GtkPrintOperation *print;
//	GtkPageSetup *page_setup;
	BiorhythmusPrintData *data;

	print = gtk_print_operation_new ();
//	page_setup = gtk_page_setup_new ();

	data = g_slice_new0 (BiorhythmusPrintData);

	data->chart = chart;
	data->scale_factor = 100;

//	gtk_page_setup_set_orientation (page_setup, GTK_PAGE_ORIENTATION_LANDSCAPE);

	gtk_print_operation_set_print_settings (print, biorhythmus_print_get_print_settings ());
	gtk_print_operation_set_default_page_setup (print, biorhythmus_print_get_page_setup ());
	gtk_print_operation_set_n_pages (print, 1);
	gtk_print_operation_set_job_name (print, "Print Biorhythmus");
	gtk_print_operation_set_embed_page_setup (print, FALSE);

	g_signal_connect (print, "draw_page", G_CALLBACK (biorhythmus_print_draw_page), data);
	g_signal_connect (print, "end-print", G_CALLBACK (biorhythmus_print_end_print), data);

	return print;
}

static GKeyFile *
biorhythmus_print_get_key_file (void)
{
	GKeyFile *key_file;
	GError *error = NULL;
	gchar *filename;
	GFile *file;
	const gchar *dot_dir = biorhythmus_util_dot_dir ();

	filename = g_build_filename (dot_dir, BIORHYTHMUS_PRINT_SETTINGS_FILE, NULL);

	file = g_file_new_for_path (filename);
	key_file = g_key_file_new ();

	if (g_file_query_exists (file, NULL)) {
		g_key_file_load_from_file (key_file, filename,
					   G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS,
					   &error);
		if (error) {
			g_warning ("Error loading print settings file: %s", error->message);
			g_error_free (error);
			g_object_unref (file);
			g_free (filename);
			g_key_file_free (key_file);
			return NULL;
		}
	}

	g_object_unref (file);
	g_free (filename);

	return key_file;
}

GtkPageSetup *
biorhythmus_print_get_page_setup ()
{
	GtkPageSetup *page_setup;
	GKeyFile *key_file;
	GError *error = NULL;

	key_file = biorhythmus_print_get_key_file ();

	if (key_file && g_key_file_has_group (key_file, BIORHYTHMUS_PAGE_SETUP_GROUP))
		page_setup = gtk_page_setup_new_from_key_file (key_file, BIORHYTHMUS_PAGE_SETUP_GROUP, &error);
	else
		page_setup = gtk_page_setup_new ();
	
	if (error)
	{
		page_setup = gtk_page_setup_new ();

		g_warning ("Error loading print settings file: %s", error->message);
		g_error_free (error);
	}

	if (key_file)
		g_key_file_free (key_file);

	gtk_page_setup_set_orientation (page_setup, GTK_PAGE_ORIENTATION_LANDSCAPE);	

	return page_setup;
}

static void
biorhythmus_print_save_key_file (GKeyFile *key_file)
{
	gchar *filename;
	gchar *data;
	GError *error = NULL;
	const gchar *dot_dir = biorhythmus_util_dot_dir ();

	filename = g_build_filename (dot_dir, BIORHYTHMUS_PRINT_SETTINGS_FILE, NULL);

	data = g_key_file_to_data (key_file, NULL, NULL);

	g_file_set_contents (filename, data, -1, &error);

	if (error) {
		g_warning ("Error saving print settings file: %s", error->message);
		g_error_free (error);
	}

	g_free (filename);
	g_free (data);
}

void
biorhythmus_print_set_page_setup (GtkPageSetup *page_setup)
{
	GKeyFile *key_file;

	key_file = biorhythmus_print_get_key_file ();

	if (key_file == NULL) {
		key_file = g_key_file_new ();
	}

	gtk_page_setup_to_key_file (page_setup, key_file, BIORHYTHMUS_PAGE_SETUP_GROUP);
	biorhythmus_print_save_key_file (key_file);

	g_key_file_free (key_file);
}

GtkPrintSettings *
biorhythmus_print_get_print_settings (void)
{
	GtkPrintSettings *print_settings;
	GError *error = NULL;
	GKeyFile *key_file;

	key_file = biorhythmus_print_get_key_file ();

	if (key_file && g_key_file_has_group (key_file, BIORHYTHMUS_PRINT_SETTINGS_GROUP))
		print_settings = gtk_print_settings_new_from_key_file (key_file, BIORHYTHMUS_PRINT_SETTINGS_GROUP, &error);
	else
		print_settings = gtk_print_settings_new ();

	if (error)
	{
		print_settings = gtk_print_settings_new ();

		g_warning ("Error loading print settings file: %s", error->message);
		g_error_free (error);
	}

	if (key_file)
		g_key_file_free (key_file);

	return print_settings;
}

void
biorhythmus_print_set_print_settings (GtkPrintSettings *print_settings)
{
	GKeyFile *key_file;

	key_file = biorhythmus_print_get_key_file ();

	if (key_file == NULL)
		key_file = g_key_file_new ();

	/* Override copies settings as we don't want to save
	 * this for the next time eog is used. */
	gtk_print_settings_set_n_copies (print_settings, 1);

	/* Unset output-uri as it would break setting output-base-name */
	gtk_print_settings_set (print_settings, GTK_PRINT_SETTINGS_OUTPUT_URI, NULL);
	gtk_print_settings_to_key_file (print_settings, key_file, BIORHYTHMUS_PRINT_SETTINGS_GROUP);
	biorhythmus_print_save_key_file (key_file);

	g_key_file_free (key_file);
}

/* ex:set ts=4 noet: */
