/* -*-coding: utf-8;-*- */

/* Biorhythmus
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

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "biorhythmus-math.h"
#include "biorhythmus-chart.h"
#include "biorhythmus-fileview.h"
#include "biorhythmus-cli.h"

void
biorhythmus_gui_on_file_open_activate (GtkWidget *widget, BiorhythmusFileView *file_view)
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
		biorhythmus_file_view_load_from_file(file_view, filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void
biorhythmus_gui_on_file_save_activate (GtkWidget *widget, BiorhythmusFileView *file_view)
{
	biorhythmus_file_view_save_to_file (file_view);
}

void
biorhythmus_gui_on_file_save_as_activate (GtkWidget *widget, BiorhythmusFileView *file_view)
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
		biorhythmus_file_view_save_to_new_file (file_view, filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void
biorhythmus_gui_on_option_physical_activate (GtkCheckMenuItem *menu_item, BiorhythmusChart *chart)
{
	biorhythmus_chart_set_option_physical (chart, gtk_check_menu_item_get_active (menu_item));
}

void
biorhythmus_gui_on_option_emotional_activate (GtkCheckMenuItem *menu_item, BiorhythmusChart *chart)
{
	biorhythmus_chart_set_option_emotional (chart, gtk_check_menu_item_get_active (menu_item));
}

void
biorhythmus_gui_on_option_intellectual_activate (GtkCheckMenuItem *menu_item, BiorhythmusChart *chart)
{
	biorhythmus_chart_set_option_intellectual (chart, gtk_check_menu_item_get_active (menu_item));
}

void
biorhythmus_gui_on_option_total_activate (GtkCheckMenuItem *menu_item, BiorhythmusChart *chart)
{
	biorhythmus_chart_set_option_total (chart, gtk_check_menu_item_get_active (menu_item));
}

void
biorhythmus_gui_on_console_activate (GtkMenuItem *menu_item, BiorhythmusCli *cli)
{
	biorhythmus_cli_output (cli);
}

void
biorhythmus_gui_on_help_info_activate (GtkMenuItem *menu_item, gpointer user_data)
{
	static const gchar *authors[] = {"Gabriel Mainberger <gabisoft@freesurf.ch>", NULL};
	gtk_show_about_dialog (NULL, "authors", authors, "program-name", "Biorhythmus", "title", "Funny and useless :)", "version", "0.12", "copyright", "Copyright © 2003-2013 Gabriel Mainberger", NULL);
}

void
biorhythmus_gui_on_calendar_changed_chart (GtkCalendar *calendar, BiorhythmusChart *chart)
{
	gint year, month, day;

	gtk_calendar_get_date (calendar, &year, &month, &day);
	biorhythmus_chart_set_active_date (chart, day, month+1, year);
}

void
biorhythmus_gui_on_calendar_changed_cli (GtkCalendar *calendar, BiorhythmusCli *cli)
{
	gint year, month, day;
	gtk_calendar_get_date (calendar, &year, &month, &day);

	biorhythmus_cli_set_active_date (cli, day, month+1, year);
}

void
biorhythmus_gui_on_file_view_birthday_changed_chart (BiorhythmusFileView *file_view, BiorhythmusChart *chart)
{
	gint day, month, year;

	biorhythmus_file_view_get_date (file_view, &day, &month, &year);
	biorhythmus_chart_set_birthday (chart, day, month, year);
}

void
biorhythmus_gui_on_file_view_name_changed_chart (BiorhythmusFileView *file_view, BiorhythmusChart *chart)
{
	biorhythmus_chart_set_name (chart, biorhythmus_file_view_get_name (file_view));
}

void
biorhythmus_gui_on_file_view_birthday_changed_cli (BiorhythmusFileView *file_view, BiorhythmusCli *cli)
{
	gint day, month, year;

	biorhythmus_file_view_get_date (file_view, &day, &month, &year);
	biorhythmus_cli_set_birthday (cli, day, month, year);
};

gboolean
biorhythmus_gui_on_delete_event (GtkWindow *widget, GdkEvent event, gpointer user_data)
{
	return FALSE;
}

void
biorhythmus_gui_on_window_destroy (GtkWidget *widget, gpointer user_data)
{
	gtk_main_quit ();
}

void
biorhythmus_gui_menubar_check_menu_item (GtkMenu *menu, gchar *caption, void *callback_function, void *object_pointer)
{
	GtkWidget *menu_item = gtk_check_menu_item_new_with_mnemonic (_(caption));
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menu_item), TRUE);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (callback_function), object_pointer);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
}

void
biorhythmus_gui_menubar_image_menu_item (GtkMenu *menu, GtkAccelGroup *accel, gchar *stock_id, void *callback_function, void *object_pointer)
{
	GtkWidget *menu_item = gtk_image_menu_item_new_from_stock (stock_id, accel);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (callback_function), object_pointer);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
}

void
biorhythmus_gui_menubar_mnemonic_menu_item (GtkMenu *menu, gchar *caption, void *callback_function, void *object_pointer)
{
	GtkWidget *menu_item = gtk_menu_item_new_with_mnemonic (_(caption));
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (callback_function), object_pointer);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
}

GtkMenu*
biorhythmus_gui_menubar_sub_menu (GtkMenuBar *menu, gchar *caption)
{
	GtkMenu *sub_menu = g_object_new (GTK_TYPE_MENU, NULL);
	GtkWidget *menu_item = menu_item = gtk_menu_item_new_with_mnemonic (_(caption));
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(menu_item), GTK_WIDGET (sub_menu));
	return (sub_menu);
}

void
biorhythmus_gui_menubar_init (GtkWindow *window, GtkMenuBar *menu, BiorhythmusChart *chart, BiorhythmusFileView *file_view, BiorhythmusCli *cli)
{
	GtkAccelGroup *accel;
	GtkWidget *menu_item;
	GtkMenu *sub_menu;

	/* AccelGroup */
	accel = g_object_new (GTK_TYPE_ACCEL_GROUP, NULL);
	gtk_window_add_accel_group (GTK_WINDOW (window), accel);

	/* FILE MENU */
	sub_menu = biorhythmus_gui_menubar_sub_menu (menu, "_File");

	biorhythmus_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_OPEN, biorhythmus_gui_on_file_open_activate, file_view);
	biorhythmus_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_SAVE, biorhythmus_gui_on_file_save_activate, file_view);
	biorhythmus_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_SAVE_AS, biorhythmus_gui_on_file_save_as_activate, file_view);

	gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu), gtk_separator_menu_item_new ());

	biorhythmus_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_CLOSE, biorhythmus_gui_on_window_destroy, NULL);
	
	/* OPTION MENU */
	sub_menu = biorhythmus_gui_menubar_sub_menu (menu, "_Options");

	biorhythmus_gui_menubar_check_menu_item (sub_menu, "_Physical", biorhythmus_gui_on_option_physical_activate, chart);
	biorhythmus_gui_menubar_check_menu_item (sub_menu, "_Emotional", biorhythmus_gui_on_option_emotional_activate, chart);
	biorhythmus_gui_menubar_check_menu_item (sub_menu, "_Intellectual", biorhythmus_gui_on_option_intellectual_activate, chart);
	biorhythmus_gui_menubar_check_menu_item (sub_menu, "_Total", biorhythmus_gui_on_option_total_activate, chart);
	
	gtk_menu_shell_append (GTK_MENU_SHELL (sub_menu), gtk_separator_menu_item_new ());

	biorhythmus_gui_menubar_mnemonic_menu_item (sub_menu, "_Console", biorhythmus_gui_on_console_activate, cli);

	/* HELP MENU */
	sub_menu = biorhythmus_gui_menubar_sub_menu (menu, "_Help");

	biorhythmus_gui_menubar_image_menu_item (sub_menu, accel, GTK_STOCK_HELP, biorhythmus_gui_on_help_info_activate, NULL);
}

int
main (int argc, char **argv)
{
	GtkWindow *window;
	GtkMenuBar *menu;
	BiorhythmusCli *cli;
	GtkWidget *chart;
	GtkWidget *calendar;
	GtkWidget *file_view;

	/* Init type system as soon as possible */
	g_type_init ();

	/* Init GTK */
	gtk_init (&argc, &argv);

	/* Init Objects */
	menu = g_object_new (GTK_TYPE_MENU_BAR, NULL);

	cli = biorhythmus_cli_new ();

	chart = biorhythmus_chart_new ();

	calendar = gtk_calendar_new ();
	gtk_calendar_set_display_options (GTK_CALENDAR (calendar), GTK_CALENDAR_SHOW_HEADING|GTK_CALENDAR_SHOW_DAY_NAMES);
	g_signal_connect (G_OBJECT (calendar), "day-selected", G_CALLBACK (biorhythmus_gui_on_calendar_changed_chart), chart);
	g_signal_connect (G_OBJECT (calendar), "day-selected", G_CALLBACK (biorhythmus_gui_on_calendar_changed_cli), cli);

	file_view = biorhythmus_file_view_new ();
	g_signal_connect (G_OBJECT (file_view), "date-changed", G_CALLBACK (biorhythmus_gui_on_file_view_birthday_changed_chart), chart);
	g_signal_connect (G_OBJECT (file_view), "date-changed", G_CALLBACK (biorhythmus_gui_on_file_view_birthday_changed_cli), cli);
	g_signal_connect (G_OBJECT (file_view), "name-changed", G_CALLBACK (biorhythmus_gui_on_file_view_name_changed_chart), chart);
	GtkWidget *file_view_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_add (GTK_CONTAINER (file_view_scrolled_window), GTK_WIDGET (file_view));

	window = g_object_new (GTK_TYPE_WINDOW, "title", "Biorhythmus", "default-width", 800, "default-height", 600, NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (biorhythmus_gui_on_delete_event), NULL);
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (biorhythmus_gui_on_window_destroy), NULL);

	/* Init Menu */
	biorhythmus_gui_menubar_init (window, menu, BIORHYTHMUS_CHART (chart), BIORHYTHMUS_FILE_VIEW (file_view), cli);

	/* GUI Layout */
	GtkVBox *vbox = g_object_new (GTK_TYPE_VBOX, NULL);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (menu), FALSE, TRUE, 0);

#ifdef GTK2
	GtkWidget *hpaned = gtk_hpaned_new ();
#else
	GtkWidget *hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
#endif

	gtk_paned_pack1 (GTK_PANED (hpaned), calendar, FALSE, FALSE);
	gtk_paned_pack2 (GTK_PANED (hpaned), file_view_scrolled_window, TRUE, TRUE);

#ifdef GTK2
	GtkWidget *vpaned = gtk_vpaned_new ();
#else
	GtkWidget *vpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
#endif

	gtk_paned_pack1 (GTK_PANED (vpaned), chart, TRUE, TRUE);
	gtk_paned_pack2 (GTK_PANED (vpaned), hpaned, FALSE, FALSE);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (vpaned), TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (vbox));
	gtk_widget_show_all (GTK_WIDGET (window));

	gtk_main ();

	return (0);
}

/* ex:set ts=4 noet: */
