/* -*-coding: utf-8;-*- */
/* biorhythmus-chart.c
   This file is part of biorhythmus

   Copyright (C) 2003-2013 by Gabriel Mainberger

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2 of the License,
   or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA

*/

#include "biorhythmus-chart.h"

struct bio_date
{
    guint day;
	guint month;
	guint year;
};

struct _BiorhythmusChartPrivate
{
	GtkWidget *parent_widget;

	struct bio_date birthday;
	struct bio_date active_date;

	gboolean option_physical;
	gboolean option_emotional;
	gboolean option_intellectual;
	gboolean option_total;
};

#define BIORHYTHMUS_CHART_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), BIORHYTHMUS_TYPE_CHART, BiorhythmusChartPrivate));

G_DEFINE_TYPE (BiorhythmusChart, biorhythmus_chart, G_TYPE_OBJECT)

void
biorhythmus_chart_draw_curves (BiorhythmusChart *self, cairo_t *cr, double red, double green, double blue, gint margin, gint days_in_month, gint day_pix, gint days_of_life, gint cycle_day, gint half_height)
{
	gint i = margin;

	if ((self->priv->birthday.month == self->priv->active_date.month) && (self->priv->birthday.year == self->priv->active_date.year))
		i = margin + ((self->priv->birthday.day - 1) * day_pix);
	else if (days_of_life < 0)
		return;

	cairo_set_source_rgb (cr, red, green, blue);

	// Draw curves
	while (i <= margin + ((days_in_month - 1) * day_pix))
	{
		if (cycle_day == 0)
			cairo_line_to (cr, i, bio_bioday_graphic_total (i, days_of_life, half_height, day_pix, margin));
		else
			cairo_line_to (cr, i, bio_bioday_graphic (i, days_of_life, cycle_day, half_height, day_pix, margin));

		i++;
	}

	cairo_stroke (cr);
}

void
biorhythmus_chart_draw_lines (cairo_t *cr, gint margin, gint half_height, gint days_in_month, gint day_pix)
{
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_line_width (cr, 0.5);
	cairo_move_to (cr, margin, half_height);
	cairo_line_to (cr, margin + ((days_in_month - 1) * day_pix), half_height);
	cairo_stroke (cr);
}

void
biorhythmus_chart_draw_current_day_line(cairo_t *cr, gint margin, gint full_height, gint days_in_month, gint day_pix, gint current_day)
{
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_line_width (cr, 0.5);
	cairo_move_to (cr, margin + (day_pix * (current_day - 1)), 0);
	cairo_line_to (cr, margin + (day_pix * (current_day - 1)), full_height);
	cairo_stroke (cr);
}

void
biorhythmus_chart_draw_day_lines(cairo_t *cr, gint margin, gint half_height, gint full_height, gint days_in_month, gint day_pix)
{
	gint i, h100;
	gchar *s;

	h100 = (gint)(floor (full_height / 100) + 1);
	for (i = 0;i < days_in_month;i++)
	{
		s = g_strdup_printf ("%d", i + 1);
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size (cr, 10);
		cairo_move_to (cr, margin + (i * day_pix) - 4, half_height + h100 + 10);
		cairo_show_text (cr, s);
		g_free (s);

		cairo_set_source_rgb (cr, 0, 0, 0);
		cairo_move_to (cr, margin + (i * day_pix), half_height - h100);
		cairo_line_to (cr, margin + (i * day_pix), half_height + h100);
	}
	cairo_stroke (cr);
}

void
biorhythmus_chart_draw (BiorhythmusChart *self)
{
	gint i, days_of_life, days_in_month, day_pix, full_height, half_height, full_width, h100, margin;
	cairo_t *cr;
	GdkWindow *mywindow;

	mywindow = gtk_widget_get_window (self->priv->parent_widget);
	cr = gdk_cairo_create (mywindow);

	// Inizialize
	margin = 15;

#ifdef GTK2
	full_height = self->priv->parent_widget->allocation.height;
	full_width = self->priv->parent_widget->allocation.width;
#else
	full_height = gtk_widget_get_allocated_height (self->priv->parent_widget);
	full_width = gtk_widget_get_allocated_width (self->priv->parent_widget);
#endif

	// Set Parameter
	days_in_month = g_date_get_days_in_month (self->priv->active_date.month, self->priv->active_date.year);
	days_of_life = bio_daysoflife (self->priv->active_date, self->priv->birthday);
	day_pix = full_width / days_in_month;
	half_height = full_height / 2;

	biorhythmus_chart_draw_lines (cr, margin, half_height, days_in_month, day_pix);
	biorhythmus_chart_draw_current_day_line (cr, margin, full_height, days_in_month, day_pix, self->priv->active_date.day);
	biorhythmus_chart_draw_day_lines (cr, margin, half_height, full_height, days_in_month, day_pix);

	// Physical
	if (self->priv->option_physical == TRUE)
		biorhythmus_chart_draw_curves (self, cr, 1, 0, 0, margin, days_in_month, day_pix, days_of_life, 23, half_height);

	// Emotional
	if (self->priv->option_emotional == TRUE)
		biorhythmus_chart_draw_curves (self, cr, 0, 1, 0, margin, days_in_month, day_pix, days_of_life, 28, half_height);

	// Intellectual
	if (self->priv->option_intellectual == TRUE)
		biorhythmus_chart_draw_curves (self, cr, 0, 0, 1, margin, days_in_month, day_pix, days_of_life, 33, half_height);

	if (self->priv->option_total == TRUE)
		biorhythmus_chart_draw_curves (self, cr, 0, 0, 0, margin, days_in_month, day_pix, days_of_life, 0, half_height);

	cairo_stroke (cr);
	cairo_destroy (cr);

	//bio_gui_update_statusbar (days_of_life);
}

void
biorhythmus_chart_set_birthday (BiorhythmusChart *self, gint day, gint month, gint year)
{
	self->priv->birthday.day = day;
	self->priv->birthday.month = month;
	self->priv->birthday.year = year;
}

void
biorhythmus_chart_set_active_date (BiorhythmusChart *self, gint day, gint month, gint year)
{
    self->priv->active_date.day = day;
	self->priv->active_date.month = month;
	self->priv->active_date.year = year;
}

void
biorhythmus_chart_get_private_variables (BiorhythmusChart *self)
{
	g_print ("Birthday: %i.%i.%i\n", self->priv->birthday.day, self->priv->birthday.month, self->priv->birthday.year);
	g_print ("Active Date: %i.%i.%i\n", self->priv->active_date.day, self->priv->active_date.month, self->priv->active_date.year);

	g_print ("Physical %i\n", self->priv->option_physical);
}

void
biorhythmus_chart_set_option_physical (BiorhythmusChart *self, gboolean state)
{
	self->priv->option_physical = state;
}

gboolean
biorhythmus_chart_get_option_physical (BiorhythmusChart *self)
{
	return self->priv->option_physical;
}

void
biorhythmus_chart_set_option_emotional (BiorhythmusChart *self, gboolean state)
{
	self->priv->option_emotional = state;
}

gboolean
biorhythmus_chart_get_option_emotional (BiorhythmusChart *self)
{
	return self->priv->option_emotional;
}

void
biorhythmus_chart_set_option_intellectual (BiorhythmusChart *self, gboolean state)
{
	self->priv->option_intellectual = state;
}

gboolean
biorhythmus_chart_get_option_intellectual (BiorhythmusChart *self)
{
	return self->priv->option_intellectual;
}

void
biorhythmus_chart_set_option_total (BiorhythmusChart *self, gboolean state)
{
	self->priv->option_total = state;
}

gboolean
biorhythmus_chart_get_option_total (BiorhythmusChart *self)
{
	return self->priv->option_total;
}

static void
biorhythmus_chart_class_init (BiorhythmusChartClass *klass)
{
  g_type_class_add_private (klass, sizeof (BiorhythmusChartPrivate));
}

static void
biorhythmus_chart_init (BiorhythmusChart *self)
{
	BiorhythmusChartPrivate *priv;

	self->priv = priv = BIORHYTHMUS_CHART_GET_PRIVATE (self);

	priv->option_physical = TRUE;
	priv->option_emotional = TRUE;
	priv->option_intellectual = TRUE;
	priv->option_total = TRUE;
}

BiorhythmusChart*
biorhythmus_chart_new (GtkWidget *parent_widget)
{
	BiorhythmusChart *self;

	g_return_val_if_fail (GTK_IS_WIDGET (parent_widget), NULL);

	self = g_object_new (BIORHYTHMUS_TYPE_CHART, NULL);

	self->priv->parent_widget = parent_widget;

	return (self);

	//return GTK_WIDGET (g_object_new (BIORHYTHMUS_TYPE_CHART, NULL));
}

/* ex:set ts=4 noet: */
