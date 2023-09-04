/* -*-coding: utf-8;-*- */

/* biorhythm-app.c
 * This file is part of Biorhythm
 * Copyright (C) 2003-2023, Gabriel Mainberger
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

#include "config.h"

#include "biorhythm-app.h"
#include "biorhythm-app-private.h"

typedef struct
{
	GtkMenuBar        *menu;
} BiorhythmAppPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (BiorhythmApp, biorhythm_app, GTK_TYPE_APPLICATION)

void
_biorhythm_app_file_new_activate (GtkWidget *widget, BiorhythmFileView *file_view)
{
	biorhythm_file_view_new_file (file_view);
}

void
_biorhythm_app_file_open_activate (GtkWidget *widget, BiorhythmFileView *file_view)
{
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new (_("Open File"),
						NULL,
						GTK_FILE_CHOOSER_ACTION_OPEN,
						_("_Cancel"), GTK_RESPONSE_CANCEL,
						_("_Open"), GTK_RESPONSE_ACCEPT,
						NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		biorhythm_file_view_load_from_file(file_view, filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void
_biorhythm_app_file_save_activate (GtkWidget *widget, BiorhythmFileView *file_view)
{
	biorhythm_file_view_save_to_file (file_view);
}

void
_biorhythm_app_file_save_as_activate (GtkWidget *widget, BiorhythmFileView *file_view)
{
	GtkWidget *dialog;
	GtkFileChooser *chooser;

	dialog = gtk_file_chooser_dialog_new (_("Save File"),
						NULL,
						GTK_FILE_CHOOSER_ACTION_SAVE,
						_("_Cancel"), GTK_RESPONSE_CANCEL,
						_("_Save"), GTK_RESPONSE_ACCEPT,
						NULL);

	chooser = GTK_FILE_CHOOSER (dialog);
	gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		biorhythm_file_view_save_to_new_file (file_view, filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void
_biorhythm_app_print_activate (GtkWidget *widget, BiorhythmChart *chart)
{
	GtkWidget *dialog;
	GError *error = NULL;
	GtkPrintOperation *print;
	GtkPrintOperationResult res;

	print = biorhythm_print_operation_new (chart);

	res = gtk_print_operation_run (print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, NULL, &error);

    if (res == GTK_PRINT_OPERATION_RESULT_ERROR)
	{
                dialog = gtk_message_dialog_new (NULL,
                                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                                 GTK_MESSAGE_ERROR,
                                                 GTK_BUTTONS_CLOSE,
                                                 _("Error printing file:\n%s"),
                                                 error->message);
                g_signal_connect (dialog, "response",
                                  G_CALLBACK (gtk_widget_destroy), NULL);
                gtk_widget_show (dialog);
                g_error_free (error);
	}
}

void
_biorhythm_app_option_physical_activate (GtkCheckMenuItem *menu_item, BiorhythmChart *chart)
{
	biorhythm_chart_set_option_physical (chart, gtk_check_menu_item_get_active (menu_item));
}

void
_biorhythm_app_option_emotional_activate (GtkCheckMenuItem *menu_item, BiorhythmChart *chart)
{
	biorhythm_chart_set_option_emotional (chart, gtk_check_menu_item_get_active (menu_item));
}

void
_biorhythm_app_option_intellectual_activate (GtkCheckMenuItem *menu_item, BiorhythmChart *chart)
{
	biorhythm_chart_set_option_intellectual (chart, gtk_check_menu_item_get_active (menu_item));
}

void
_biorhythm_app_option_total_activate (GtkCheckMenuItem *menu_item, BiorhythmChart *chart)
{
	biorhythm_chart_set_option_total (chart, gtk_check_menu_item_get_active (menu_item));
}

void
_biorhythm_app_console_activate (GtkMenuItem *menu_item, BiorhythmCli *cli)
{
	biorhythm_cli_output (cli);
}

void
_biorhythm_app_about_activated (GtkMenuItem *menu_item, gpointer user_data)
{
	static const gchar *authors[] = {"Gabriel Mainberger <gabisoft@freesurf.ch>", NULL};
	gtk_show_about_dialog (NULL, "authors", authors, "program-name", "Biorhythm", "title", "Funny and useless :)", "version", PACKAGE_VERSION, "copyright", "Copyright © 2003-2023 Gabriel Mainberger", NULL);
}

void
_biorhythm_app_calendar_changed_chart (GtkCalendar *calendar, BiorhythmChart *chart)
{
	guint year, month, day;

	gtk_calendar_get_date (calendar, &year, &month, &day);
	biorhythm_chart_set_active_date (chart, day, month+1, year);
}

void
_biorhythm_app_calendar_changed_cli (GtkCalendar *calendar, BiorhythmCli *cli)
{
	guint year, month, day;
	gtk_calendar_get_date (calendar, &year, &month, &day);

	biorhythm_cli_set_active_date (cli, day, month+1, year);
}

void
_biorhythm_app_file_view_birthday_changed_chart (BiorhythmFileView *file_view, BiorhythmChart *chart)
{
	guint day, month, year;

	biorhythm_file_view_get_date (file_view, &day, &month, &year);
	biorhythm_chart_set_birthday (chart, day, month, year);
}

void
_biorhythm_app_file_view_name_changed_chart (BiorhythmFileView *file_view, BiorhythmChart *chart)
{
	biorhythm_chart_set_name (chart, biorhythm_file_view_get_name (file_view));
}

void
_biorhythm_app_file_view_birthday_changed_cli (BiorhythmFileView *file_view, BiorhythmCli *cli)
{
	guint day, month, year;

	biorhythm_file_view_get_date (file_view, &day, &month, &year);
	biorhythm_cli_set_birthday (cli, day, month, year);
};

void
_biorhythm_app_window_destroy (GtkWidget *widget, gpointer user_data)
{
	g_application_quit (user_data);
}

void
_biorhythm_app_menubar_check_menu_item (GtkMenu *menu, gchar *caption, void *callback_function, void *object_pointer)
{
	GtkWidget *menu_item = gtk_check_menu_item_new_with_mnemonic (_(caption));
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menu_item), TRUE);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (callback_function), object_pointer);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
}

void
_biorhythm_app_menubar_mnemonic_menu_item (GtkMenu *menu, gchar *caption, void *callback_function, void *object_pointer)
{
	GtkWidget *menu_item = gtk_menu_item_new_with_mnemonic (_(caption));
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (callback_function), object_pointer);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
}

GtkMenu*
_biorhythm_app_menubar_sub_menu (GtkMenuBar *menu, gchar *caption)
{
	GtkMenu *sub_menu = g_object_new (GTK_TYPE_MENU, NULL);
	GtkWidget *menu_item = menu_item = gtk_menu_item_new_with_mnemonic (_(caption));
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(menu_item), GTK_WIDGET (sub_menu));
	return (sub_menu);
}

void
_biorhythm_gui_menubar_init (GtkApplication *app, GtkMenuBar *menu, BiorhythmChart *chart, BiorhythmFileView *file_view, BiorhythmCli *cli)
{
	GtkMenu *sub_menu;

	/* FILE MENU */
	sub_menu = _biorhythm_app_menubar_sub_menu (menu, _("_File"));

	_biorhythm_app_menubar_mnemonic_menu_item (sub_menu, _("_New"), _biorhythm_app_file_new_activate, file_view);
	_biorhythm_app_menubar_mnemonic_menu_item (sub_menu, _("_Open"), _biorhythm_app_file_open_activate, file_view);
	_biorhythm_app_menubar_mnemonic_menu_item (sub_menu, _("_Save"), _biorhythm_app_file_save_activate, file_view);
	_biorhythm_app_menubar_mnemonic_menu_item (sub_menu, _("_Save as"), _biorhythm_app_file_save_as_activate, file_view);

	gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu), gtk_separator_menu_item_new ());

	_biorhythm_app_menubar_mnemonic_menu_item (sub_menu, _("_Print"), _biorhythm_app_print_activate, chart);

	gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu), gtk_separator_menu_item_new ());

	_biorhythm_app_menubar_mnemonic_menu_item (sub_menu, _("_Close"), _biorhythm_app_window_destroy, app);

	/* OPTION MENU */
	sub_menu = _biorhythm_app_menubar_sub_menu (menu, _("_Options"));

	_biorhythm_app_menubar_check_menu_item (sub_menu, _("_Physical"), _biorhythm_app_option_physical_activate, chart);
	_biorhythm_app_menubar_check_menu_item (sub_menu, _("_Emotional"), _biorhythm_app_option_emotional_activate, chart);
	_biorhythm_app_menubar_check_menu_item (sub_menu, _("_Intellectual"), _biorhythm_app_option_intellectual_activate, chart);
	_biorhythm_app_menubar_check_menu_item (sub_menu, _("_Total"), _biorhythm_app_option_total_activate, chart);

	gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu), gtk_separator_menu_item_new ());

	_biorhythm_app_menubar_mnemonic_menu_item (sub_menu, _("_Console"), _biorhythm_app_console_activate, cli);

	/* HELP MENU */
	sub_menu = _biorhythm_app_menubar_sub_menu (menu, _("_Help"));

	_biorhythm_app_menubar_mnemonic_menu_item (sub_menu, _("_About"), _biorhythm_app_about_activated, NULL);
}

static void
_biorhythm_app_activate (GApplication *application)
{
	GtkWidget *window;
	GtkMenuBar *menu;
	BiorhythmCli *cli;
	GtkWidget *chart;
	GtkWidget *calendar;
	GtkWidget *file_view;

	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	/* Init Objects */
	menu = g_object_new (GTK_TYPE_MENU_BAR, NULL);

	cli = biorhythm_cli_new ();

	chart = biorhythm_chart_new ();

	calendar = gtk_calendar_new ();
	gtk_calendar_set_display_options (GTK_CALENDAR (calendar), GTK_CALENDAR_SHOW_HEADING|GTK_CALENDAR_SHOW_DAY_NAMES);
	g_signal_connect (G_OBJECT (calendar), "day-selected", G_CALLBACK (_biorhythm_app_calendar_changed_chart), chart);
	g_signal_connect (G_OBJECT (calendar), "day-selected", G_CALLBACK (_biorhythm_app_calendar_changed_cli), cli);

	file_view = biorhythm_file_view_new ();
	g_signal_connect (G_OBJECT (file_view), "date-changed", G_CALLBACK (_biorhythm_app_file_view_birthday_changed_chart), chart);
	g_signal_connect (G_OBJECT (file_view), "date-changed", G_CALLBACK (_biorhythm_app_file_view_birthday_changed_cli), cli);
	g_signal_connect (G_OBJECT (file_view), "name-changed", G_CALLBACK (_biorhythm_app_file_view_name_changed_chart), chart);
	GtkWidget *file_view_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_add (GTK_CONTAINER (file_view_scrolled_window), GTK_WIDGET (file_view));

	window = gtk_application_window_new (GTK_APPLICATION (application));
	gtk_window_set_title (GTK_WINDOW (window), "Biorhythm");
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (_biorhythm_app_window_destroy), GTK_APPLICATION (application));

	/* Init Menu */
	_biorhythm_gui_menubar_init (GTK_APPLICATION (application), menu, BIORHYTHM_CHART (chart), BIORHYTHM_FILE_VIEW (file_view), cli);

	/* Paned */
	GtkWidget *hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_paned_pack1 (GTK_PANED (hpaned), calendar, FALSE, FALSE);
	gtk_paned_pack2 (GTK_PANED (hpaned), file_view_scrolled_window, TRUE, TRUE);

	GtkWidget *vpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
	gtk_paned_pack1 (GTK_PANED (vpaned), chart, TRUE, TRUE);
	gtk_paned_pack2 (GTK_PANED (vpaned), hpaned, FALSE, FALSE);

	/* GUI Layout */
	GtkWidget *grid = gtk_grid_new ();
	gtk_grid_attach (GTK_GRID (grid), GTK_WIDGET (menu), 0, 0, 1, 1);
	gtk_widget_set_hexpand (vpaned, TRUE);
	gtk_widget_set_vexpand (vpaned, TRUE);
	gtk_grid_attach (GTK_GRID (grid), GTK_WIDGET (vpaned), 0, 1, 1, 1);

	gtk_container_add (GTK_CONTAINER (window), grid);
	gtk_widget_show_all (window);
}

static void
biorhythm_app_class_init (BiorhythmAppClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

	app_class->activate = _biorhythm_app_activate;
}

static void
biorhythm_app_init (BiorhythmApp *app)
{
}
