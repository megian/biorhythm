/* -*-coding: utf-8;-*- */

/* biorhythmus-print.h
 * This file is part of Biorhythmus
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

#ifndef __BIORHYTHMUS_PRINT_H__
#define __BIORHYTHMUS_PRINT_H__

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "biorhythmus-chart.h"

G_BEGIN_DECLS

GtkPrintOperation* biorhythmus_print_operation_new ();
GtkPageSetup* biorhythmus_print_get_page_setup (void);
void biorhythmus_print_set_page_setup (GtkPageSetup *page_setup);
GtkPrintSettings* biorhythmus_print_get_print_settings (void);
void biorhythmus_print_set_print_settings (GtkPrintSettings *print_settings);

G_END_DECLS

#endif /* __BIORHYTHMUS_PRINT_H__ */
