/* -*-coding: utf-8;-*- */

/* biorhythm-app.h
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

#ifndef BIORHYTHM_APP_H
#define BIORHYTHM_APP_H

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "biorhythm-chart.h"
#include "biorhythm-cli.h"
#include "biorhythm-fileview.h"
#include "biorhythm-math.h"
#include "biorhythm-print.h"
#include "config.h"

G_BEGIN_DECLS

#define BIORHYTHM_TYPE_APP (biorhythm_app_get_type ())

G_DECLARE_DERIVABLE_TYPE (BiorhythmApp, biorhythm_app, BIORHYTHM, APP, GtkApplication)

struct _BiorhythmAppClass
{
	GtkApplicationClass parent_class;
};

G_END_DECLS

#endif /* BIORHYTHM_APP_H */

/* ex:set ts=8 noet: */
