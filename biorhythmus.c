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
#include <math.h>

#include "biorhythmus-math.h"
#include "biorhythmus-chart.h"
#include "biorhythmus-fileview.h"
#include "biorhythmus-cli.h"

void
biorhythmus_gui_help_info_dialog (GtkMenuItem *eintrag, gpointer user_data)
{
	static const gchar *authors[] = {"Gabriel Mainberger <gabisoft@freesurf.ch>", NULL};
	gtk_show_about_dialog (NULL, "authors", authors, "program-name", "Biorhythmus", "title", "Funny and useless :)", "version", "0.12", "copyright", "Copyright © 2003-2013 Gabriel Mainberger", NULL);
}

void
biorhythmus_gui_cli_output (GtkMenuItem *item, gpointer user_data)
{
    BiorhythmusCli *cli=(BiorhythmusCli*)user_data;

	biorhythmus_cli_output (cli);
}

void
biorhythmus_gui_option_physical (GtkMenuItem *entry, gpointer user_data)
{
	biorhythmus_chart_set_option_physical (user_data, gtk_check_menu_item_get_active ((GtkCheckMenuItem *)entry));
}

void
biorhythmus_gui_option_emotional (GtkMenuItem *entry, gpointer user_data)
{
	biorhythmus_chart_set_option_emotional (user_data, gtk_check_menu_item_get_active ((GtkCheckMenuItem *)entry));
}

void
biorhythmus_gui_option_intellectual (GtkMenuItem *entry, gpointer user_data)
{
	biorhythmus_chart_set_option_intellectual (user_data, gtk_check_menu_item_get_active ((GtkCheckMenuItem *)entry));
}

void
biorhythmus_gui_option_total (GtkMenuItem *entry, gpointer user_data)
{
	biorhythmus_chart_set_option_total (user_data, gtk_check_menu_item_get_active ((GtkCheckMenuItem *)entry));
}

void
biorhythmus_gui_calendar_change_chart (GtkCalendar *calendar, gpointer user_data)
{
	gint year, month, day;

	gtk_calendar_get_date (calendar, &year, &month, &day);
	biorhythmus_chart_set_active_date (user_data, day, month+1, year);
}

void
biorhythmus_gui_calendar_change_cli (GtkCalendar *calendar, gpointer user_data)
{
	gint year, month, day;
	gtk_calendar_get_date (calendar, &year, &month, &day);

	biorhythmus_cli_set_active_date (user_data, day, month+1, year);
}

void
biorhythmus_birthday_changed_chart (BiorhythmusFileView *file_view, gpointer user_data)
{
	gint day, month, year;

	biorhythmus_file_view_get_date (file_view, &day, &month, &year);
	biorhythmus_chart_set_birthday (user_data, day, month, year);
}

void
biorhythmus_birthday_changed_cli (BiorhythmusFileView *file_view, gpointer user_data)
{
	gint day, month, year;

	biorhythmus_file_view_get_date (file_view, &day, &month, &year);
	biorhythmus_cli_set_birthday (user_data, day, month, year);
};

biorhythmus_gui_delete_event (GtkWindow *widget, GdkEvent event, gpointer daten)
{
	return FALSE;
}

void
biorhythmus_gui_close (GtkWidget *widget, gpointer user_data)
{
	gtk_main_quit ();
}

void
biorhythmus_gui_open (GtkWidget *widget, gpointer user_data)
{
	BiorhythmusFileView *file_view = (BiorhythmusFileView*)user_data;
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
		biorhythmus_file_view_load_from_file(file_view, filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void
biorhythmus_gui_save (GtkWidget *widget, gpointer user_data)
{
	BiorhythmusFileView *file_view = (BiorhythmusFileView*)user_data;

	biorhythmus_file_view_save_to_file (file_view);
}

void
biorhythmus_gui_save_as (GtkWidget *widget, gpointer user_data)
{
	BiorhythmusFileView *file_view = (BiorhythmusFileView*)user_data;
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
biorhythmus_gui_menubar_init (GtkWindow *window, GtkMenuBar *menu, BiorhythmusChart *chart, BiorhythmusFileView *file_view, BiorhythmusCli *cli)
{
	GtkAccelGroup *accel;
	GtkWidget *file_menu_item, *file_open_menu_item, *file_save_menu_item, *file_close_menu_item, *option_menu_item, *console, *help_menu_item, *help_info_menu_item;
	GtkWidget *option_physical, *option_emotional, *option_intellectual, *option_total;
	GtkMenu *file_menu, *option_menu, *help_menu;

	/* AccelGroup */
	accel = g_object_new (GTK_TYPE_ACCEL_GROUP, NULL);
	gtk_window_add_accel_group (GTK_WINDOW(window), accel);

	/* menu -> file */
	file_menu_item = gtk_menu_item_new_with_mnemonic ("_File");
	file_menu = g_object_new (GTK_TYPE_MENU, NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL(menu), file_menu_item);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(file_menu_item), GTK_WIDGET(file_menu));

	/* menu -> file -> file open */
	file_open_menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN, NULL);
	g_signal_connect(file_open_menu_item, "activate", G_CALLBACK(biorhythmus_gui_open), file_view);
	gtk_menu_shell_append (GTK_MENU_SHELL(file_menu), file_open_menu_item);

	/* menu -> file -> file save */
	file_save_menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE, NULL);
	g_signal_connect(file_save_menu_item, "activate", G_CALLBACK(biorhythmus_gui_save), file_view);
	gtk_menu_shell_append (GTK_MENU_SHELL(file_menu), file_save_menu_item);

	/* menu -> file -> file save */
	file_save_menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE_AS, NULL);
	g_signal_connect(file_save_menu_item, "activate", G_CALLBACK(biorhythmus_gui_save_as), file_view);
	gtk_menu_shell_append (GTK_MENU_SHELL(file_menu), file_save_menu_item);

	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), gtk_separator_menu_item_new());

	/* menu -> file -> file close */
	file_close_menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_CLOSE, accel);
	g_signal_connect (file_close_menu_item, "activate", G_CALLBACK(biorhythmus_gui_close), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL(file_menu), file_close_menu_item);

	
	/* menu -> option */
	option_menu = g_object_new(GTK_TYPE_MENU, NULL);

	option_physical = gtk_check_menu_item_new_with_mnemonic ("_Physical");
	gtk_check_menu_item_set_active ((GtkCheckMenuItem*)option_physical, TRUE);
	g_signal_connect (option_physical, "activate", G_CALLBACK(biorhythmus_gui_option_physical), chart);
	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_physical);

	option_emotional = gtk_check_menu_item_new_with_mnemonic ("_Emotional");
	gtk_check_menu_item_set_active ((GtkCheckMenuItem*)option_emotional, TRUE);
	g_signal_connect (option_emotional, "activate", G_CALLBACK(biorhythmus_gui_option_emotional), chart);
	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_emotional);

	option_intellectual = gtk_check_menu_item_new_with_mnemonic ("_Intellectual");
	gtk_check_menu_item_set_active ((GtkCheckMenuItem*)option_intellectual, TRUE);
 	g_signal_connect (option_intellectual, "activate", G_CALLBACK(biorhythmus_gui_option_intellectual), chart);
	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_intellectual);

	option_total = gtk_check_menu_item_new_with_mnemonic("_Total");
	gtk_check_menu_item_set_active((GtkCheckMenuItem*)option_total, TRUE);
	g_signal_connect(option_total, "activate", G_CALLBACK(biorhythmus_gui_option_total), chart);
	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_total);

	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), gtk_separator_menu_item_new());

	console = gtk_menu_item_new_with_mnemonic("_Console");
	g_signal_connect(console, "activate", G_CALLBACK(biorhythmus_gui_cli_output), cli);
	gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), console);

	/* menu -> option */
	option_menu_item = gtk_menu_item_new_with_mnemonic("_Options");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), option_menu_item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(option_menu_item), GTK_WIDGET(option_menu));

	/* menu -> help -> .. */
	help_info_menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, accel);
	g_signal_connect(help_info_menu_item, "activate", G_CALLBACK(biorhythmus_gui_help_info_dialog), NULL);

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
	GtkWidget *chart;
	GtkWidget *file_view;
	GtkWidget *calendar;
	GtkVBox *vbox;
	GtkTreeIter iter;
	GtkTreeView *view;
	GtkTreeViewColumn *column0, *column1;
	BiorhythmusCli *cli;

	/* Init type system as soon as possible */
	g_type_init ();

	gtk_init (&argc, &argv);

	cli = biorhythmus_cli_new ();
	chart = biorhythmus_chart_new ();

	file_view = biorhythmus_file_view_new ();
	g_signal_connect (G_OBJECT (file_view), "date-changed", G_CALLBACK (biorhythmus_birthday_changed_chart), chart);
	g_signal_connect (G_OBJECT (file_view), "date-changed", G_CALLBACK (biorhythmus_birthday_changed_cli), cli);

	window = g_object_new (GTK_TYPE_WINDOW, "title", "Biorhythmus", "default-width", 800, "default-height", 600, NULL);
	g_signal_connect (window, "delete-event", G_CALLBACK(biorhythmus_gui_delete_event), NULL);
	g_signal_connect (window, "destroy", G_CALLBACK(biorhythmus_gui_close), NULL);

	menu = g_object_new(GTK_TYPE_MENU_BAR, NULL);

	/* VBOX */
	vbox = g_object_new(GTK_TYPE_VBOX, NULL);

	biorhythmus_gui_menubar_init(window, menu, BIORHYTHMUS_CHART(chart), BIORHYTHMUS_FILE_VIEW(file_view), cli);
	
	/* Add Menubar to vbox */
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(menu), FALSE, TRUE, 0);

	/* Calendar */
	calendar = gtk_calendar_new();
	gtk_calendar_set_display_options((GtkCalendar*)calendar, GTK_CALENDAR_SHOW_HEADING|GTK_CALENDAR_SHOW_DAY_NAMES);
	g_signal_connect(G_OBJECT(calendar), "day-selected", G_CALLBACK(biorhythmus_gui_calendar_change_chart), chart);
	g_signal_connect(G_OBJECT(calendar), "day-selected", G_CALLBACK(biorhythmus_gui_calendar_change_cli), cli);

  /* Paned */
	list_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_add (GTK_CONTAINER(list_scrolled_window), GTK_WIDGET(file_view));

#ifdef GTK2
	GtkWidget *hpaned = gtk_hpaned_new ();
#else
	GtkWidget *hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
#endif
	gtk_paned_pack1 (GTK_PANED (hpaned), calendar, FALSE, FALSE);
	gtk_paned_pack2 (GTK_PANED (hpaned), list_scrolled_window, TRUE, TRUE);

#ifdef GTK2
	GtkWidget *vpaned = gtk_vpaned_new ();
#else
	GtkWidget *vpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
#endif
	gtk_paned_pack1 (GTK_PANED (vpaned), chart, TRUE, TRUE);
	gtk_paned_pack2 (GTK_PANED (vpaned), hpaned, FALSE, FALSE);

	gtk_box_pack_start (GTK_BOX(vbox), GTK_WIDGET(vpaned), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(vbox));
	gtk_widget_show_all(GTK_WIDGET(window));

	gtk_main();

	return (0);
}

/* ex:set ts=4 noet: */
