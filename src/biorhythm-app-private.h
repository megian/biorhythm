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

void            _biorhythm_app_file_new_activate        (GtkWidget *widget,
                                                         BiorhythmFileView *file_view);

void            _biorhythm_app_file_save_activate       (GtkWidget *widget,
                                                         BiorhythmFileView *file_view);

void            _biorhythm_app_file_save_as_activate    (GtkWidget *widget,
                                                         BiorhythmFileView *file_view);

void            _biorhythm_app_print_activate           (GtkWidget *widget,
                                                         BiorhythmChart *chart);

void            _biorhythm_app_option_physical_activate (GtkCheckMenuItem *menu_item,
                                                         BiorhythmChart *chart);

void            _biorhythm_app_option_emotional_activate (GtkCheckMenuItem *menu_item,
                                                          BiorhythmChart *chart);

void            _biorhythm_app_option_intellectual_activate (GtkCheckMenuItem *menu_item,
                                                             BiorhythmChart *chart);

void            _biorhythm_app_option_total_activate    (GtkCheckMenuItem *menu_item,
                                                         BiorhythmChart *chart);

void            _biorhythm_app_console_activate         (GtkMenuItem *menu_item,
                                                         BiorhythmCli *cli);

void            _biorhythm_app_about_activated          (GtkMenuItem *menu_item,
                                                         gpointer user_data);

void            _biorhythm_app_calendar_changed_chart   (GtkCalendar *calendar,
                                                         BiorhythmChart *chart);

void            _biorhythm_app_calendar_changed_cli     (GtkCalendar *calendar,
                                                         BiorhythmCli *cli);

void            _biorhythm_app_file_view_birthday_changed_chart (BiorhythmFileView *file_view,
                                                                 BiorhythmChart *chart);

void            _biorhythm_app_file_view_name_changed_chart (BiorhythmFileView *file_view,
                                                             BiorhythmChart *chart);

void            _biorhythm_app_file_view_birthday_changed_cli (BiorhythmFileView *file_view,
                                                               BiorhythmCli *cli);

void            _biorhythm_app_window_destroy           (GtkWidget *widget,
                                                         gpointer user_data);

void            _biorhythm_app_menubar_check_menu_item  (GtkMenu *menu,
                                                         gchar *caption,
                                                         void *callback_function,
                                                         void *object_pointer);

void            _biorhythm_app_menubar_mnemonic_menu_item (GtkMenu *menu,
                                                           gchar *caption,
                                                           void *callback_function,
                                                           void *object_pointer);

GtkMenu         *_biorhythm_app_menubar_sub_menu        (GtkMenuBar *menu,
                                                         gchar *caption);

void            _biorhythm_gui_menubar_init             (GtkApplication *app,
                                                         GtkMenuBar *menu,
                                                         BiorhythmChart *chart,
                                                         BiorhythmFileView *file_view,
                                                         BiorhythmCli *cli);

static void     _biorhythm_app_activate                 (GApplication *application);

G_END_DECLS

#endif /* __BIORHYTHM_APP_PRIVATE_H__ */

/* ex:set ts=8 noet: */
