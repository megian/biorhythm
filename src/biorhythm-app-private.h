/*
 * biorhythm-app-private.h
 * This file is part of Biorhythm
 * Copyright (C) 2003-2023, Gabriel Mainberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __BIORHYTHM_APP_PRIVATE_H__
#define __BIORHYTHM_APP_PRIVATE_H__

#include "biorhythm-app.h"

G_BEGIN_DECLS

BiorhythmFileView *_biorhythm_app_get_file_view         (BiorhythmApp *app);

BiorhythmFileView *_biorhythm_app_set_file_view         (BiorhythmApp *app,
                                                         BiorhythmFileView *file_view);

BiorhythmChart *_biorhythm_app_get_chart                (BiorhythmApp *app);

BiorhythmChart *_biorhythm_app_set_chart                (BiorhythmApp *app,
                                                         BiorhythmChart *chart);

BiorhythmCli    *_biorhythm_app_get_cli                 (BiorhythmApp *app);

BiorhythmCli    *_biorhythm_app_set_cli                 (BiorhythmApp *app,
                                                         BiorhythmCli *cli);

static void     _biorhythm_app_file_new_activated       (GSimpleAction *action,
                                                         GVariant *param,
                                                         gpointer user_data);

static void     _biorhythm_app_file_open_activated      (GSimpleAction *action,
                                                         GVariant *param,
                                                         gpointer user_data);

static void     _biorhythm_app_file_save_activated      (GSimpleAction *action,
                                                         GVariant *param,
                                                         gpointer user_data);

static void     _biorhythm_app_file_save_as_activated   (GSimpleAction *action,
                                                         GVariant *param,
                                                         gpointer user_data);

static void     _biorhythm_app_print_activated          (GSimpleAction *action,
                                                         GVariant *param,
                                                         gpointer user_data);

static void     _biorhythm_app_option_physical_changed     (GSimpleAction *action,
                                                         GVariant *state,
                                                         gpointer user_data);

static void     _biorhythm_app_option_emotional_changed     (GSimpleAction *action,
                                                         GVariant *state,
                                                         gpointer user_data);

static void     _biorhythm_app_option_intellectual_changed     (GSimpleAction *action,
                                                         GVariant *state,
                                                         gpointer user_data);

static void     _biorhythm_app_option_total_changed     (GSimpleAction *action,
                                                         GVariant *state,
                                                         gpointer user_data);

static void     _biorhythm_app_console_activated        (GSimpleAction *action,
                                                         GVariant *param,
                                                        gpointer user_data);

static void     _biorhythm_app_about_activated          (GSimpleAction *action,
                                                         GVariant *param,
                                                         gpointer user_data);

static void     _biorhythm_app_calendar_changed_chart   (GtkCalendar *calendar,
                                                         BiorhythmChart *chart);

static void     _biorhythm_app_calendar_changed_cli     (GtkCalendar *calendar,
                                                         BiorhythmCli *cli);

static void     _biorhythm_app_file_view_birthday_changed_chart (BiorhythmFileView *file_view,
                                                                 BiorhythmChart *chart);

static void     _biorhythm_app_file_view_name_changed_chart (BiorhythmFileView *file_view,
                                                             BiorhythmChart *chart);

static void     _biorhythm_app_file_view_birthday_changed_cli (BiorhythmFileView *file_view,
                                                               BiorhythmCli *cli);

static void     _biorhythm_app_quit_activated (GSimpleAction *action,
                                               GVariant *param,
                                               gpointer app);

static void     _biorhythm_app_startup (GApplication *application);

static GtkWidget *_biorhythm_app_create_open_dialog_button (void);

static void     _biorhythm_app_add_new_button (GtkHeaderBar *bar);

static void     _biorhythm_app_add_save_button (GtkHeaderBar *bar);

static GtkWidget *_biorhythm_app_hamburger_create ();

static GMenu *_biorhythm_app_hamburger_create_section_main ();

static GMenu *_biorhythm_app_hamburger_create_section_options ();

static GMenu *_biorhythm_app_hamburger_create_section_about ();

static GtkWidget *_biorhythm_app_create_headerbar (GApplication *app,
                                                   GtkWindow *window);

static void     _biorhythm_app_quit_activated           (GSimpleAction *action,
                                                         GVariant *param,
                                                         gpointer app);

static void     _biorhythm_app_activate                 (GApplication *application);

BiorhythmFileView *
_biorhythm_app_get_file_view (BiorhythmApp *app);

G_END_DECLS

#endif /* __BIORHYTHM_APP_PRIVATE_H__ */

/* ex:set ts=8 noet: */
