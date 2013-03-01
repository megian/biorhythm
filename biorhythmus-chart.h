/* -*-coding: utf-8;-*- */
/* biorhythmus-chart.h
 * This file is part of biorhythmus
 *
 * Copyright (C) 2003-2013 by Gabriel Mainberger
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA

*/

#ifndef __BIORHYTHMUS_CHART_H__
#define __BIORHYTHMUS_CHART_H__

#include <math.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS
	
#define BIORHYTHMUS_TYPE_CHART					(biorhythmus_chart_get_type ())
#define BIORHYTHMUS_CHART(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), BIORHYTHMUS_TYPE_CHART, BiorhythmusChart))
#define BIORHYTHMUS_IS_CHART(obj)				(G_TYPE_CHECK_INSTANCE_TYPE ((obj), BIORHYTHMUS_TYPE_CHART))
#define BIORHYTHMUS_CHART_CLASS(klass)			(G_TYPE_CHECK_CLASS_CAST ((klass), BIORHYTHMUS_TYPE_CHART, BiorhythmusChartClass))
#define BIORHYTHMUS_IS_CHART_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), BIORHYTHMUS_TYPE_CHART))
#define BIORHYTHMUS_CHART_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), BIORHYTHMUS_TYPE_CHART, BiorhythmusChartClass))

typedef struct _BiorhythmusChart        BiorhythmusChart;
typedef struct _BiorhythmusChartClass   BiorhythmusChartClass;
typedef struct _BiorhythmusChartPrivate	BiorhythmusChartPrivate;

struct _BiorhythmusChart
{
	GObject parent_instance;

	BiorhythmusChartPrivate *priv;
};

struct _BiorhythmusChartClass
{
	GObjectClass parent_class;
};

/* used by BIORHYTHMUS_TYPE_CHART */
GType biorhythmus_chart_get_type (void) G_GNUC_CONST;

BiorhythmusChart* biorhythmus_chart_new(GtkWidget *widget);
void biorhythmus_chart_draw (BiorhythmusChart *self);

void biorhythmus_chart_set_birthday (BiorhythmusChart *self, gint day, gint month, gint year);
void biorhythmus_chart_set_active_date (BiorhythmusChart *self, gint day, gint month, gint year);

void biorhythmus_chart_get_private_variables (BiorhythmusChart *self);

void biorhythmus_chart_set_option_physical (BiorhythmusChart *self, gboolean state);
gboolean biorhythmus_chart_get_option_physical (BiorhythmusChart *self);
void biorhythmus_chart_set_option_emotional (BiorhythmusChart *self, gboolean state);
gboolean biorhythmus_chart_get_option_emotional (BiorhythmusChart *self);
void biorhythmus_chart_set_option_intellectual (BiorhythmusChart *self, gboolean state);
gboolean biorhythmus_chart_get_option_intellectual (BiorhythmusChart *self);
void biorhythmus_chart_set_option_total (BiorhythmusChart *self, gboolean state);
gboolean biorhythmus_chart_get_option_total (BiorhythmusChart *self);

G_END_DECLS

#endif /* __BIORHYTHMUS_CHART_H__ */

/* ex:set ts=4 noet: */
