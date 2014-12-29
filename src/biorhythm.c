/* -*-coding: utf-8;-*- */

/* Biorhythm
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

#include <biorhythm.h>

void
biorhythm_gui_on_file_new_activate (GtkWidget *widget, BiorhythmFileView *file_view)
{
	biorhythm_file_view_new_file (file_view);
}

void
biorhythm_gui_on_file_open_activate (GtkWidget *widget, BiorhythmFileView *file_view)
{
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new (_("Open File"),
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
		biorhythm_file_view_load_from_file(file_view, filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void
biorhythm_gui_on_file_save_activate (GtkWidget *widget, BiorhythmFileView *file_view)
{
	biorhythm_file_view_save_to_file (file_view);
}

void
biorhythm_gui_on_file_save_as_activate (GtkWidget *widget, BiorhythmFileView *file_view)
{
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new (_("Save File"),
						NULL,
						GTK_FILE_CHOOSER_ACTION_SAVE,
						GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						GTK_STOCK_SAVE,
						GTK_RESPONSE_ACCEPT,
						NULL);

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
biorhythm_gui_on_print_activate (GtkWidget *widget, BiorhythmChart *chart)
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
biorhythm_gui_on_option_physical_activate (GtkCheckMenuItem *menu_item, BiorhythmChart *chart)
{
	biorhythm_chart_set_option_physical (chart, gtk_check_menu_item_get_active (menu_item));
}

void
biorhythm_gui_on_option_emotional_activate (GtkCheckMenuItem *menu_item, BiorhythmChart *chart)
{
	biorhythm_chart_set_option_emotional (chart, gtk_check_menu_item_get_active (menu_item));
}

void
biorhythm_gui_on_option_intellectual_activate (GtkCheckMenuItem *menu_item, BiorhythmChart *chart)
{
	biorhythm_chart_set_option_intellectual (chart, gtk_check_menu_item_get_active (menu_item));
}

void
biorhythm_gui_on_option_total_activate (GtkCheckMenuItem *menu_item, BiorhythmChart *chart)
{
	biorhythm_chart_set_option_total (chart, gtk_check_menu_item_get_active (menu_item));
}

void
biorhythm_gui_on_console_activate (GtkMenuItem *menu_item, BiorhythmCli *cli)
{
	biorhythm_cli_output (cli);
}

void
biorhythm_gui_on_help_info_activate (GtkMenuItem *menu_item, gpointer user_data)
{
	static const gchar *authors[] = {"Gabriel Mainberger <gabisoft@freesurf.ch>", NULL};
	gtk_show_about_dialog (NULL, "authors", authors, "program-name", "Biorhythm", "title", "Funny and useless :)", "version", VERSION, "copyright", "Copyright © 2003-2013 Gabriel Mainberger", NULL);
}

void
biorhythm_gui_on_calendar_changed_chart (GtkCalendar *calendar, BiorhythmChart *chart)
{
	guint year, month, day;

	gtk_calendar_get_date (calendar, &year, &month, &day);
	biorhythm_chart_set_active_date (chart, day, month+1, year);
}

void
biorhythm_gui_on_calendar_changed_cli (GtkCalendar *calendar, BiorhythmCli *cli)
{
	guint year, month, day;
	gtk_calendar_get_date (calendar, &year, &month, &day);

	biorhythm_cli_set_active_date (cli, day, month+1, year);
}

void
biorhythm_gui_on_file_view_birthday_changed_chart (BiorhythmFileView *file_view, BiorhythmChart *chart)
{
	guint day, month, year;

	biorhythm_file_view_get_date (file_view, &day, &month, &year);
	biorhythm_chart_set_birthday (chart, day, month, year);
}

void
biorhythm_gui_on_file_view_name_changed_chart (BiorhythmFileView *file_view, BiorhythmChart *chart)
{
	biorhythm_chart_set_name (chart, biorhythm_file_view_get_name (file_view));
}

void
biorhythm_gui_on_file_view_birthday_changed_cli (BiorhythmFileView *file_view, BiorhythmCli *cli)
{
	guint day, month, year;

	biorhythm_file_view_get_date (file_view, &day, &month, &year);
	biorhythm_cli_set_birthday (cli, day, month, year);
};

gboolean
biorhythm_gui_on_delete_event (GtkWindow *widget, GdkEvent event, gpointer user_data)
{
	return FALSE;
}

void
biorhythm_gui_on_window_destroy (GtkWidget *widget, gpointer user_data)
{
	gtk_main_quit ();
}

void
biorhythm_gui_menubar_check_menu_item (GtkMenu *menu, gchar *caption, void *callback_function, void *object_pointer)
{
	GtkWidget *menu_item = gtk_check_menu_item_new_with_mnemonic (_(caption));
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menu_item), TRUE);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (callback_function), object_pointer);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
}

void
biorhythm_gui_menubar_image_menu_item (GtkMenu *menu, GtkAccelGroup *accel, gchar *stock_id, void *callback_function, void *object_pointer)
{
	GtkWidget *menu_item = gtk_image_menu_item_new_from_stock (stock_id, accel);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (callback_function), object_pointer);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
}

void
biorhythm_gui_menubar_mnemonic_menu_item (GtkMenu *menu, gchar *caption, void *callback_function, void *object_pointer)
{
	GtkWidget *menu_item = gtk_menu_item_new_with_mnemonic (_(caption));
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (callback_function), object_pointer);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
}

GtkMenu*
biorhythm_gui_menubar_sub_menu (GtkMenuBar *menu, gchar *caption)
{
	GtkMenu *sub_menu = g_object_new (GTK_TYPE_MENU, NULL);
	GtkWidget *menu_item = menu_item = gtk_menu_item_new_with_mnemonic (_(caption));
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(menu_item), GTK_WIDGET (sub_menu));
	return (sub_menu);
}

void
biorhythm_gui_menubar_init (GtkWindow *window, GtkMenuBar *menu, BiorhythmChart *chart, BiorhythmFileView *file_view, BiorhythmCli *cli)
{
	GtkAccelGroup *accel;
	GtkMenu *sub_menu;

	/* AccelGroup */
	accel = g_object_new (GTK_TYPE_ACCEL_GROUP, NULL);
	gtk_window_add_accel_group (GTK_WINDOW (window), accel);

	/* FILE MENU */
	sub_menu = biorhythm_gui_menubar_sub_menu (menu, _("_File"));

	biorhythm_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_NEW, biorhythm_gui_on_file_new_activate, file_view);
	biorhythm_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_OPEN, biorhythm_gui_on_file_open_activate, file_view);
	biorhythm_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_SAVE, biorhythm_gui_on_file_save_activate, file_view);
	biorhythm_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_SAVE_AS, biorhythm_gui_on_file_save_as_activate, file_view);

	gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu), gtk_separator_menu_item_new ());

	biorhythm_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_PRINT, biorhythm_gui_on_print_activate, chart);

	gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu), gtk_separator_menu_item_new ());

	biorhythm_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_CLOSE, biorhythm_gui_on_window_destroy, NULL);
	
	/* OPTION MENU */
	sub_menu = biorhythm_gui_menubar_sub_menu (menu, _("_Options"));

	biorhythm_gui_menubar_check_menu_item (sub_menu, _("_Physical"), biorhythm_gui_on_option_physical_activate, chart);
	biorhythm_gui_menubar_check_menu_item (sub_menu, _("_Emotional"), biorhythm_gui_on_option_emotional_activate, chart);
	biorhythm_gui_menubar_check_menu_item (sub_menu, _("_Intellectual"), biorhythm_gui_on_option_intellectual_activate, chart);
	biorhythm_gui_menubar_check_menu_item (sub_menu, _("_Total"), biorhythm_gui_on_option_total_activate, chart);
	
	gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu), gtk_separator_menu_item_new ());

	biorhythm_gui_menubar_mnemonic_menu_item (sub_menu, _("_Console"), biorhythm_gui_on_console_activate, cli);

	/* HELP MENU */
	sub_menu = biorhythm_gui_menubar_sub_menu (menu, _("_Help"));

	biorhythm_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_HELP, biorhythm_gui_on_help_info_activate, NULL);
}

int
main (int argc, char **argv)
{
	GtkWindow *window;
	GtkMenuBar *menu;
	BiorhythmCli *cli;
	GtkWidget *chart;
	GtkWidget *calendar;
	GtkWidget *file_view;

<<<<<<< HEAD
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
=======
	/* Init type system as soon as possible */
#ifdef GTK2
	g_type_init ();
#endif

	bindtextdomain (PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (PACKAGE, "UTF-8");
	textdomain (PACKAGE);
>>>>>>> 3e6b98efa264855fa0ef2750c084faea799ce4ea

	/* Init GTK */
	gtk_init (&argc, &argv);

	/* Init Objects */
	menu = g_object_new (GTK_TYPE_MENU_BAR, NULL);

	cli = biorhythm_cli_new ();

	chart = biorhythm_chart_new ();

	calendar = gtk_calendar_new ();
	gtk_calendar_set_display_options (GTK_CALENDAR (calendar), GTK_CALENDAR_SHOW_HEADING|GTK_CALENDAR_SHOW_DAY_NAMES);
	g_signal_connect (G_OBJECT (calendar), "day-selected", G_CALLBACK (biorhythm_gui_on_calendar_changed_chart), chart);
	g_signal_connect (G_OBJECT (calendar), "day-selected", G_CALLBACK (biorhythm_gui_on_calendar_changed_cli), cli);

	file_view = biorhythm_file_view_new ();
	g_signal_connect (G_OBJECT (file_view), "date-changed", G_CALLBACK (biorhythm_gui_on_file_view_birthday_changed_chart), chart);
	g_signal_connect (G_OBJECT (file_view), "date-changed", G_CALLBACK (biorhythm_gui_on_file_view_birthday_changed_cli), cli);
	g_signal_connect (G_OBJECT (file_view), "name-changed", G_CALLBACK (biorhythm_gui_on_file_view_name_changed_chart), chart);
	GtkWidget *file_view_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_add (GTK_CONTAINER (file_view_scrolled_window), GTK_WIDGET (file_view));

	window = g_object_new (GTK_TYPE_WINDOW, "title", "Biorhythm", "default-width", 800, "default-height", 600, NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (biorhythm_gui_on_delete_event), NULL);
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (biorhythm_gui_on_window_destroy), NULL);

	/* Init Menu */
	biorhythm_gui_menubar_init (window, menu, BIORHYTHM_CHART (chart), BIORHYTHM_FILE_VIEW (file_view), cli);

	/* GUI Layout */
	GtkVBox *vbox = g_object_new (GTK_TYPE_VBOX, NULL);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (menu), FALSE, TRUE, 0);

	GtkWidget *hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);

	gtk_paned_pack1 (GTK_PANED (hpaned), calendar, FALSE, FALSE);
	gtk_paned_pack2 (GTK_PANED (hpaned), file_view_scrolled_window, TRUE, TRUE);

	GtkWidget *vpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);

	gtk_paned_pack1 (GTK_PANED (vpaned), chart, TRUE, TRUE);
	gtk_paned_pack2 (GTK_PANED (vpaned), hpaned, FALSE, FALSE);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (vpaned), TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (vbox));
	gtk_widget_show_all (GTK_WIDGET (window));

	gtk_main ();

	return (0);
}

/* ex:set ts=4 noet: */