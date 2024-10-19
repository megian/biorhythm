/* -*-coding: utf-8;-*- */

/* biorhythm-chart.h
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

#ifndef __BIORHYTHM_CHART_H__
#define __BIORHYTHM_CHART_H__

#include <math.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "biorhythm-math.h"

G_BEGIN_DECLS

#define BIORHYTHM_TYPE_CHART (biorhythm_chart_get_type ())
G_DECLARE_FINAL_TYPE (BiorhythmChart, biorhythm_chart, BIORHYTHM, CHART, GtkDrawingArea)

struct _BiorhythmChartClass
{
	GtkDrawingAreaClass parent_class;
};

BiorhythmChart *biorhythm_chart_new ();

void biorhythm_chart_set_birthday (BiorhythmChart *chart, gint day, gint month, gint year);
void biorhythm_chart_set_active_date (BiorhythmChart *chart, gint day, gint month, gint year);
void biorhythm_chart_set_name (BiorhythmChart *chart, gchar *s);

void biorhythm_chart_set_option_physical (BiorhythmChart *chart, gboolean state);
gboolean biorhythm_chart_get_option_physical (BiorhythmChart *chart);
void biorhythm_chart_set_option_emotional (BiorhythmChart *chart, gboolean state);
gboolean biorhythm_chart_get_option_emotional (BiorhythmChart *chart);
void biorhythm_chart_set_option_intellectual (BiorhythmChart *chart, gboolean state);
gboolean biorhythm_chart_get_option_intellectual (BiorhythmChart *chart);
void biorhythm_chart_set_option_total (BiorhythmChart *chart, gboolean state);
gboolean biorhythm_chart_get_option_total (BiorhythmChart *chart);

void biorhythm_chart_draw_cairo (BiorhythmChart *chart, cairo_t *cr, gint full_height, gint full_width);

G_END_DECLS

#endif /* __BIORHYTHM_CHART_H__ */

/* ex:set ts=4 noet: */
