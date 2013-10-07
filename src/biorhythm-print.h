/* -*-coding: utf-8;-*- */

/* biorhythm-print.h
 * This file is part of Biorhythm
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

#ifndef __BIORHYTHM_PRINT_H__
#define __BIORHYTHM_PRINT_H__

#include <gtk/gtk.h>
#include "biorhythm-chart.h"

G_BEGIN_DECLS

GtkPrintOperation* biorhythm_print_operation_new ();

G_END_DECLS

#endif /* __BIORHYTHM_PRINT_H__ */
