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
	BiorhythmFileView *file_view;
	BiorhythmChart    *chart;
	BiorhythmCli	  *cli;
} BiorhythmAppPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (BiorhythmApp, biorhythm_app, GTK_TYPE_APPLICATION)

static void
biorhthm_app_dispose (GObject *object)
{
	BiorhythmAppPrivate *priv;

	priv = biorhythm_app_get_instance_private (BIORHYTHM_APP (object));

	g_clear_object (&priv->file_view);
	g_clear_object (&priv->chart);
	g_clear_object (&priv->cli);

	G_OBJECT_CLASS (biorhythm_app_parent_class)->dispose (object);
}

static GActionEntry app_entries[] =
{
	{ "new", _biorhythm_app_file_new_activated, NULL, NULL, NULL },
	{ "open", _biorhythm_app_file_open_activated, NULL, NULL, NULL },
	{ "save", _biorhythm_app_file_save_activated, NULL, NULL, NULL },
	{ "save-as", _biorhythm_app_file_save_as_activated, NULL, NULL, NULL },
	{ "print", _biorhythm_app_print_activated, NULL, NULL, NULL },
	{ "option-physical", NULL, NULL, "true", _biorhythm_app_option_physical_changed },
	{ "option-emotional", NULL, NULL, "true", _biorhythm_app_option_emotional_changed },
	{ "option-intellectual", NULL, NULL, "true", _biorhythm_app_option_intellectual_changed },
	{ "option-total", NULL, NULL, "true", _biorhythm_app_option_total_changed },
	{ "cli", _biorhythm_app_console_activated, NULL, NULL, NULL },
	{ "about", _biorhythm_app_about_activated, NULL, NULL, NULL },
	{ "quit", _biorhythm_app_quit_activated, NULL, NULL, NULL }
};

BiorhythmFileView *
_biorhythm_app_get_file_view (BiorhythmApp *app)
{
	BiorhythmAppPrivate *priv;

	g_return_val_if_fail (BIORHYTHM_IS_APP (app), NULL);

	priv = biorhythm_app_get_instance_private (app);

	return priv->file_view;
}

BiorhythmFileView *
_biorhythm_app_set_file_view (BiorhythmApp *app, BiorhythmFileView *file_view)
{
	BiorhythmAppPrivate *priv;

	g_return_val_if_fail (BIORHYTHM_IS_APP (app), NULL);
	g_return_val_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view), NULL);

	priv = biorhythm_app_get_instance_private (app);

	priv->file_view = file_view;
}

BiorhythmChart *
_biorhythm_app_get_chart (BiorhythmApp *app)
{
	BiorhythmAppPrivate *priv;

	g_return_val_if_fail (BIORHYTHM_IS_APP (app), NULL);

	priv = biorhythm_app_get_instance_private (app);

	return priv->chart;
}

BiorhythmChart *
_biorhythm_app_set_chart (BiorhythmApp *app, BiorhythmChart *chart)
{
	BiorhythmAppPrivate *priv;

	g_return_val_if_fail (BIORHYTHM_IS_APP (app), NULL);
	g_return_val_if_fail (BIORHYTHM_IS_CHART (chart), NULL);

	priv = biorhythm_app_get_instance_private (app);

	priv->chart = chart;
}

BiorhythmCli *
_biorhythm_app_get_cli (BiorhythmApp *app)
{
	BiorhythmAppPrivate *priv;

	g_return_val_if_fail (BIORHYTHM_IS_APP (app), NULL);

	priv = biorhythm_app_get_instance_private (app);

	return priv->cli;
}

BiorhythmCli *
_biorhythm_app_set_cli (BiorhythmApp *app, BiorhythmCli *cli)
{
	BiorhythmAppPrivate *priv;

	g_return_val_if_fail (BIORHYTHM_IS_APP (app), NULL);
	g_return_val_if_fail (BIORHYTHM_IS_CLI (cli), NULL);

	priv = biorhythm_app_get_instance_private (app);

	priv->cli = cli;
}

static void
_biorhythm_app_file_new_activated (GSimpleAction *action, GVariant *param, gpointer user_data)
{
	BiorhythmApp *app;

	app = BIORHYTHM_APP(user_data);
	biorhythm_file_view_new_file (_biorhythm_app_get_file_view (app));
}

static void
_biorhythm_app_file_open_response (GtkDialog *dialog, int response, gpointer user_data)
{
	BiorhythmFileView *file_view = user_data;

	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	if (response == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

		g_autoptr(GFile) file = gtk_file_chooser_get_file (chooser);
		biorhythm_file_view_load_from_file(file_view, g_file_get_parse_name (file));
	}

	gtk_window_destroy (GTK_WINDOW (dialog));
}

static void
_biorhythm_app_file_open_activated (GSimpleAction *action, GVariant *param, gpointer user_data)
{
	BiorhythmApp *app;
	GtkWindow *window;
	BiorhythmFileView *file_view;
	GtkWidget *dialog;

	app = BIORHYTHM_APP(user_data);
	window = GTK_WINDOW (gtk_application_get_active_window (GTK_APPLICATION (app)));
	file_view = _biorhythm_app_get_file_view (app);

	dialog = gtk_file_chooser_dialog_new (_("Open File"),
						window,
						GTK_FILE_CHOOSER_ACTION_OPEN,
						_("_Cancel"), GTK_RESPONSE_CANCEL,
						_("_Open"), GTK_RESPONSE_ACCEPT,
						NULL);

	gtk_window_present (GTK_WINDOW (dialog));

	g_signal_connect (dialog, "response",
                    G_CALLBACK (_biorhythm_app_file_open_response),
                    file_view);
}

static void
_biorhythm_app_file_save_activated (GSimpleAction *action, GVariant *param, gpointer user_data)
{
	BiorhythmApp *app;
	BiorhythmFileView *file_view;

	app = BIORHYTHM_APP(user_data);
	file_view = _biorhythm_app_get_file_view (app);

	if (biorhythm_file_view_get_filename (file_view) == NULL)
	    _biorhythm_app_file_save_as_activated (action, param, user_data);
	else
		biorhythm_file_view_save_to_file (file_view);
}

static void
_biorhythm_app_file_save_as_response (GtkDialog *dialog, int response, gpointer user_data)
{
	BiorhythmFileView *file_view = user_data;

	g_return_if_fail (BIORHYTHM_IS_FILE_VIEW (file_view));

	if (response == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

		g_autoptr(GFile) file = gtk_file_chooser_get_file (chooser);
		biorhythm_file_view_save_to_new_file (file_view, g_file_get_parse_name (file));
    }

	gtk_window_destroy (GTK_WINDOW (dialog));
}

static void
_biorhythm_app_file_save_as_activated (GSimpleAction *action, GVariant *param, gpointer user_data)
{
	BiorhythmApp *app;
	GtkWindow *window;
	BiorhythmFileView *file_view;
	GtkWidget *dialog;
	GtkFileChooser *chooser;

	app = BIORHYTHM_APP(user_data);
	window = GTK_WINDOW (gtk_application_get_active_window (GTK_APPLICATION (app)));
	file_view = _biorhythm_app_get_file_view (app);

	dialog = gtk_file_chooser_dialog_new (_("Save File"),
						window,
						GTK_FILE_CHOOSER_ACTION_SAVE,
						_("_Cancel"), GTK_RESPONSE_CANCEL,
						_("_Save"), GTK_RESPONSE_ACCEPT,
						NULL);

	gtk_window_present (GTK_WINDOW (dialog));

	g_signal_connect (dialog, "response",
                    G_CALLBACK (_biorhythm_app_file_save_as_response),
                    file_view);
}

static void
_biorhythm_app_print_activated (GSimpleAction *action, GVariant *param, gpointer user_data)
{
	BiorhythmApp *app;
	GtkWindow *window;
	BiorhythmChart *chart;
	GtkWidget *dialog;
	GError *error = NULL;
	GtkPrintOperation *print;
	GtkPrintOperationResult res;

	app = BIORHYTHM_APP(user_data);
	window = GTK_WINDOW (gtk_application_get_active_window (GTK_APPLICATION (app)));
	chart = _biorhythm_app_get_chart (app);
	print = biorhythm_print_operation_new (chart);

	res = gtk_print_operation_run (print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, window, &error);

    if (res == GTK_PRINT_OPERATION_RESULT_ERROR)
	{
                dialog = gtk_message_dialog_new (window,
                                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                                 GTK_MESSAGE_ERROR,
                                                 GTK_BUTTONS_CLOSE,
                                                 _("Error printing file:\n%s"),
                                                 error->message);
                g_signal_connect (dialog, "response",
                                  G_CALLBACK (gtk_window_destroy), NULL);
                gtk_window_present (GTK_WINDOW (dialog));
                g_error_free (error);
	}
}

static void
_biorhythm_app_option_physical_changed (GSimpleAction *action, GVariant *state, gpointer user_data)
{
	BiorhythmApp *app;
	BiorhythmChart *chart;

	g_simple_action_set_state (action, state);

	app = BIORHYTHM_APP(user_data);
	chart = _biorhythm_app_get_chart (app);
	biorhythm_chart_set_option_physical (chart, g_variant_get_boolean (state));
}

static void
_biorhythm_app_option_emotional_changed (GSimpleAction *action, GVariant *state, gpointer user_data)
{
	BiorhythmApp *app;
	BiorhythmChart *chart;

	g_simple_action_set_state (action, state);

	app = BIORHYTHM_APP(user_data);
	chart = _biorhythm_app_get_chart (app);
	biorhythm_chart_set_option_emotional (chart, g_variant_get_boolean (state));
}

static void
_biorhythm_app_option_intellectual_changed (GSimpleAction *action, GVariant *state, gpointer user_data)
{
	BiorhythmApp *app;
	BiorhythmChart *chart;

	g_simple_action_set_state (action, state);

	app = BIORHYTHM_APP(user_data);
	chart = _biorhythm_app_get_chart (app);
	biorhythm_chart_set_option_intellectual (chart, g_variant_get_boolean (state));
}

static void
_biorhythm_app_option_total_changed (GSimpleAction *action, GVariant *state, gpointer user_data)
{
	BiorhythmApp *app;
	BiorhythmChart *chart;

	g_simple_action_set_state (action, state);

	app = BIORHYTHM_APP(user_data);
	chart = _biorhythm_app_get_chart (app);
	biorhythm_chart_set_option_total (chart, g_variant_get_boolean (state));
}

static void
_biorhythm_app_console_activated (GSimpleAction *action, GVariant *param, gpointer user_data)
{
	BiorhythmApp *app;
	BiorhythmCli *cli;

	app = BIORHYTHM_APP(user_data);
	cli = _biorhythm_app_get_cli (app);
	biorhythm_cli_output (cli);
}

static void
_biorhythm_app_about_activated (GSimpleAction *action, GVariant *param, gpointer user_data)
{
	static const gchar *authors[] = {"Gabriel Mainberger <gabisoft@freesurf.ch>", NULL};
	GtkApplication *app;
	GtkWindow *window;

	app = GTK_APPLICATION (user_data);
	window = GTK_WINDOW (gtk_application_get_active_window (app));
	gtk_show_about_dialog (window, "authors", authors, "program-name", "Biorhythm", "title", "Funny and useless :)", "version", PACKAGE_VERSION, "copyright", "Copyright © 2003-2023 Gabriel Mainberger", NULL);
}

static void
_biorhythm_app_calendar_changed_chart (GtkCalendar *calendar, BiorhythmChart *chart)
{
	gint year, month, day;
	GDateTime *t;

	t = gtk_calendar_get_date (calendar);
	day = g_date_time_get_day_of_month(t);
	month = g_date_time_get_month(t);
	year = g_date_time_get_year(t);

	biorhythm_chart_set_active_date (chart, day, month+1, year);
}

static void
_biorhythm_app_calendar_changed_cli (GtkCalendar *calendar, BiorhythmCli *cli)
{
	gint year, month, day;
	GDateTime *t;

	t = gtk_calendar_get_date (calendar);
	day = g_date_time_get_day_of_month(t);
	month = g_date_time_get_month(t);
	year = g_date_time_get_year(t);

	biorhythm_cli_set_active_date (cli, day, month+1, year);
}

static void
_biorhythm_app_file_view_birthday_changed_chart (BiorhythmFileView *file_view, BiorhythmChart *chart)
{
	guint day, month, year;

	biorhythm_file_view_get_date (file_view, &day, &month, &year);
	biorhythm_chart_set_birthday (chart, day, month, year);
}

static void
_biorhythm_app_file_view_name_changed_chart (BiorhythmFileView *file_view, BiorhythmChart *chart)
{
	biorhythm_chart_set_name (chart, biorhythm_file_view_get_name (file_view));
}

static void
_biorhythm_app_file_view_birthday_changed_cli (BiorhythmFileView *file_view, BiorhythmCli *cli)
{
	guint day, month, year;

	biorhythm_file_view_get_date (file_view, &day, &month, &year);
	biorhythm_cli_set_birthday (cli, day, month, year);
};

static void
_biorhythm_app_quit_activated (GSimpleAction *action, GVariant *param, gpointer app)
{
	g_application_quit (G_APPLICATION (app));
}

static void
_biorhythm_app_startup (GApplication *application)
{
	BiorhythmAppPrivate *priv;

	priv = biorhythm_app_get_instance_private (BIORHYTHM_APP (application));

	G_APPLICATION_CLASS (biorhythm_app_parent_class)->startup (application);
	_biorhythm_app_set_file_view(BIORHYTHM_APP (application), biorhythm_file_view_new ());
	_biorhythm_app_set_chart (BIORHYTHM_APP (application), biorhythm_chart_new ());
	_biorhythm_app_set_cli (BIORHYTHM_APP (application), biorhythm_cli_new ());
}

/* Header bar */

static GtkWidget *
_biorhythm_app_create_open_dialog_button (void)
{
    GtkWidget *button;

    button = gtk_button_new_with_mnemonic (_("_Open"));
    gtk_widget_set_tooltip_text (button, _("Open a file"));
    gtk_actionable_set_action_name (GTK_ACTIONABLE (button), "win.open");

    return button;
}

static void
_biorhythm_app_add_new_button (GtkHeaderBar *bar)
{
	GtkWidget *button;

	button = gtk_button_new_from_icon_name ("tab-new-symbolic");
	gtk_widget_set_tooltip_text (button, _("Create a new document"));
	gtk_actionable_set_action_name (GTK_ACTIONABLE (button), "win.new");

	gtk_header_bar_pack_start (GTK_HEADER_BAR (bar), button);
}

static void
_biorhythm_app_add_save_button (GtkHeaderBar *bar)
{
	GtkWidget *button;

	button = gtk_button_new_with_mnemonic (_("_Save"));
	gtk_widget_set_tooltip_text (button, _("Save the current file"));
	gtk_actionable_set_action_name (GTK_ACTIONABLE (button), "win.save");

	gtk_header_bar_pack_end (GTK_HEADER_BAR (bar), button);
}

static GMenu *
_biorhythm_app_hamburger_create_section_main ()
{
	GMenu *menu = g_menu_new ();

	g_menu_append (menu, _("_Save as"), "win.save-as");
	g_menu_append (menu, _("_Print"), "win.print");
	g_menu_append (menu, _("_Console"), "win.cli");

	return menu;
}

static GMenu *
_biorhythm_app_hamburger_create_section_options ()
{
	GMenu *menu = g_menu_new ();

	g_menu_append (menu, _("_Physical"), "win.option-physical");
	g_menu_append (menu, _("_Emotional"), "win.option-emotional");
	g_menu_append (menu, _("_Intellectual"), "win.option-intellectual");
	g_menu_append (menu, _("_Total"), "win.option-total");

	return menu;
}

static GMenu *
_biorhythm_app_hamburger_create_section_about ()
{
	GMenu *menu = g_menu_new ();

	g_menu_append (menu, _("_About"), "win.about");

	return menu;
}

static GtkWidget *
_biorhythm_app_create_headerbar (GApplication *app, GtkWindow *window)
{
	GtkWidget *headerbar;
	GtkMenuButton *hamburger_button;

	headerbar = gtk_header_bar_new ();

	GMenu *menu = g_menu_new ();

	g_menu_append_section (menu, NULL, G_MENU_MODEL (_biorhythm_app_hamburger_create_section_main ()));
	g_menu_append_section (menu, NULL, G_MENU_MODEL (_biorhythm_app_hamburger_create_section_options ()));
	g_menu_append_section (menu, NULL, G_MENU_MODEL (_biorhythm_app_hamburger_create_section_about ()));

	hamburger_button = GTK_MENU_BUTTON (gtk_menu_button_new ());
	gtk_menu_button_set_icon_name (hamburger_button, "open-menu-symbolic");
	gtk_menu_button_set_menu_model (hamburger_button, G_MENU_MODEL (menu));

	gtk_header_bar_pack_start (GTK_HEADER_BAR (headerbar), _biorhythm_app_create_open_dialog_button ());
	gtk_header_bar_pack_end (GTK_HEADER_BAR (headerbar), GTK_WIDGET (hamburger_button));

	_biorhythm_app_add_new_button (GTK_HEADER_BAR (headerbar));
	_biorhythm_app_add_save_button (GTK_HEADER_BAR (headerbar));

	return headerbar;
}

static void
_biorhythm_app_activate (GApplication *application)
{
	GtkWindow *window;
	GActionGroup *actions;
	BiorhythmCli *cli;
	BiorhythmChart *chart;
	GtkWidget *calendar;
	BiorhythmFileView *file_view;

	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

    /* Window */
	window = GTK_WINDOW (gtk_application_window_new (GTK_APPLICATION (application)));
	gtk_window_set_title (GTK_WINDOW (window), "Biorhythm");
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);

	/* Header Bar */
	gtk_window_set_titlebar (GTK_WINDOW (window), _biorhythm_app_create_headerbar(application, window));

	/* Init Objects */
	cli = _biorhythm_app_get_cli(BIORHYTHM_APP (application));

	chart = _biorhythm_app_get_chart(BIORHYTHM_APP (application));
	gtk_drawing_area_set_content_height (GTK_DRAWING_AREA (chart), 300);

	calendar = gtk_calendar_new ();
	g_signal_connect (G_OBJECT (calendar), "day-selected", G_CALLBACK (_biorhythm_app_calendar_changed_chart), chart);
	g_signal_connect (G_OBJECT (calendar), "day-selected", G_CALLBACK (_biorhythm_app_calendar_changed_cli), cli);

	file_view = _biorhythm_app_get_file_view(BIORHYTHM_APP (application));
	g_signal_connect (G_OBJECT (file_view), "date-changed", G_CALLBACK (_biorhythm_app_file_view_birthday_changed_chart), chart);
	g_signal_connect (G_OBJECT (file_view), "date-changed", G_CALLBACK (_biorhythm_app_file_view_birthday_changed_cli), cli);
	g_signal_connect (G_OBJECT (file_view), "name-changed", G_CALLBACK (_biorhythm_app_file_view_name_changed_chart), chart);
	GtkWidget *file_view_scrolled_window = gtk_scrolled_window_new ();
	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (file_view_scrolled_window), GTK_WIDGET (file_view));

	/* Paned */
	GtkWidget *hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_paned_set_start_child (GTK_PANED (hpaned), calendar);
	gtk_paned_set_end_child  (GTK_PANED (hpaned), file_view_scrolled_window);

	GtkWidget *vpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
	gtk_paned_set_start_child (GTK_PANED (vpaned), GTK_WIDGET (chart));
	gtk_paned_set_end_child (GTK_PANED (vpaned), hpaned);

    /* Actions */
    actions = (GActionGroup*)g_simple_action_group_new ();
    g_action_map_add_action_entries (G_ACTION_MAP (actions),
                                     app_entries, G_N_ELEMENTS (app_entries),
                                     application);
	gtk_widget_insert_action_group (GTK_WIDGET (window), "win", actions);

	gtk_window_set_child (window, vpaned);
	gtk_window_present (window);
}

static void
biorhythm_app_class_init (BiorhythmAppClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

	G_APPLICATION_CLASS (app_class)->startup = _biorhythm_app_startup;
	G_APPLICATION_CLASS (app_class)->activate = _biorhythm_app_activate;
}

static void
biorhythm_app_init (BiorhythmApp *app)
{
}
