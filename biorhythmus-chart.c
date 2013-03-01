/* -*-coding: utf-8;-*- */

/* biorhythmus-chart.c
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

#include "biorhythmus-chart.h"

#ifdef GTK2
static gboolean
biorhythmus_chart_draw (GtkWidget *widget, GdkEventExpose *event, gpointer data);
#else
void
biorhythmus_chart_draw (GtkWidget *widget, cairo_t *cr);
#endif

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

G_DEFINE_TYPE (BiorhythmusChart, biorhythmus_chart, GTK_TYPE_DRAWING_AREA)

/****************************************
 *                 Class                *
 ****************************************/

static void
biorhythmus_chart_class_init (BiorhythmusChartClass *klass)
{
	GtkWidgetClass *widget_class;

	g_type_class_add_private (klass, sizeof (BiorhythmusChartPrivate));
	
	widget_class = GTK_WIDGET_CLASS (klass);

#ifdef GTK2
	widget_class->expose_event = (void*)biorhythmus_chart_draw;
#else
	widget_class->draw = (void*)biorhythmus_chart_draw;
#endif
}

void
biorhythmus_chart_set_current_date (struct bio_date *date)
{
	GTimeVal time_val;
	GDate *current_date;

	g_get_current_time (&time_val);
	current_date = g_date_new ();
	g_date_set_time_val (current_date, &time_val);

	date->day = g_date_get_day (current_date);
	date->month = g_date_get_month (current_date);
	date->year = g_date_get_year (current_date);

	g_date_free (current_date);
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

	biorhythmus_chart_set_current_date (&priv->active_date);
	biorhythmus_chart_set_current_date (&priv->birthday);
}

GtkWidget*
biorhythmus_chart_new ()
{
	return GTK_WIDGET (g_object_new (BIORHYTHMUS_TYPE_CHART, NULL));
}

/****************************************
 *           Private Functions          *
 ****************************************/

void
biorhythmus_chart_draw_curves (BiorhythmusChartPrivate *priv, cairo_t *cr, double red, double green, double blue, gint margin, gint days_in_month, gint day_pix, gint days_of_life, gint cycle_day, gint half_height)
{
	gint i = margin;

	if ((priv->birthday.month == priv->active_date.month) && (priv->birthday.year == priv->active_date.year))
	{
		i = margin + ((priv->birthday.day - 1) * day_pix);
		days_of_life = (priv->birthday.day - 1)*-1;
	}
	else if (days_of_life < 0)
		return;

	cairo_set_source_rgb (cr, red, green, blue);

	// Draw curves
	while (i <= margin + ((days_in_month - 1) * day_pix))
	{
		if (cycle_day == 0)
			cairo_line_to (cr, i, biorhythmus_math_bioday_graphic_total (i, days_of_life, half_height, day_pix, margin));
		else
			cairo_line_to (cr, i, biorhythmus_math_bioday_graphic (i, days_of_life, cycle_day, half_height, day_pix, margin));

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
biorhythmus_chart_caption (BiorhythmusChartPrivate *priv, cairo_t *cr, gint margin, gint full_height)
{
	gchar *s;
	gint days_of_life;

	days_of_life = biorhythmus_math_daysoflife (priv->active_date, priv->birthday);

	s = g_strdup_printf ("Date: %d.%d.%d    Birthday: %d.%d.%d    Days: %d    Physical: %d    Emotional: %d    Intellectual: %d    Total: %d",
		priv->active_date.day, priv->active_date.month,	priv->active_date.year,
		priv->birthday.day, priv->birthday.month, priv->birthday.year,
		days_of_life, biorhythmus_math_bioday(days_of_life, BIORHYTHMUS_DAYS_PHYSICAL), biorhythmus_math_bioday(days_of_life, BIORHYTHMUS_DAYS_EMOTIONAL), biorhythmus_math_bioday(days_of_life,
		BIORHYTHMUS_DAYS_INTELLECTUAL),
		biorhythmus_math_bioday_total (days_of_life));

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 11);
	cairo_move_to (cr, margin, full_height-margin);
	cairo_show_text (cr, s);

	g_free (s);
	cairo_stroke (cr);
}

#ifdef GTK2
static gboolean
biorhythmus_chart_draw (GtkWidget *widget, GdkEventExpose *event, gpointer data)
#else
void
biorhythmus_chart_draw (GtkWidget *widget, cairo_t *cr)
#endif
{
	gint i, days_of_life, days_in_month, day_pix, full_height, half_height, full_width, h100, margin;

#ifdef GTK2
	cairo_t *cr;
	GdkWindow *window;
#endif

	BiorhythmusChartPrivate *priv;

	priv = BIORHYTHMUS_CHART_GET_PRIVATE (widget);

#ifdef GTK2
	window = gtk_widget_get_window (widget);
	if ((cr = gdk_cairo_create (window))==NULL)
	{
		cairo_destroy (cr);
		return (FALSE);
	}
#endif

	// Inizialize
	margin = 15;

#ifdef GTK2
	full_height = widget->allocation.height;
	full_width = widget->allocation.width;
#else
	full_height = gtk_widget_get_allocated_height (widget);
	full_width = gtk_widget_get_allocated_width (widget);
#endif

	// Caption
	biorhythmus_chart_caption (priv, cr, margin, full_height);
	full_height = full_height - (2*margin);

	// Set Parameter
	days_in_month = g_date_get_days_in_month (priv->active_date.month, priv->active_date.year);
	days_of_life = biorhythmus_math_daysoflife (priv->active_date, priv->birthday);
	day_pix = full_width / days_in_month;
	half_height = full_height / 2;

	biorhythmus_chart_draw_lines (cr, margin, half_height, days_in_month, day_pix);
	biorhythmus_chart_draw_current_day_line (cr, margin, full_height, days_in_month, day_pix, priv->active_date.day);
	biorhythmus_chart_draw_day_lines (cr, margin, half_height, full_height, days_in_month, day_pix);

	// Physical
	if (priv->option_physical == TRUE)
		biorhythmus_chart_draw_curves (priv, cr, 1, 0, 0, margin, days_in_month, day_pix, days_of_life, BIORHYTHMUS_DAYS_PHYSICAL, half_height);

	// Emotional
	if (priv->option_emotional == TRUE)
		biorhythmus_chart_draw_curves (priv, cr, 0, 1, 0, margin, days_in_month, day_pix, days_of_life, BIORHYTHMUS_DAYS_EMOTIONAL, half_height);

	// Intellectual
	if (priv->option_intellectual == TRUE)
		biorhythmus_chart_draw_curves (priv, cr, 0, 0, 1, margin, days_in_month, day_pix, days_of_life, BIORHYTHMUS_DAYS_INTELLECTUAL, half_height);

	if (priv->option_total == TRUE)
		biorhythmus_chart_draw_curves (priv, cr, 0, 0, 0, margin, days_in_month, day_pix, days_of_life, 0, half_height);

	cairo_stroke (cr);

#ifdef GTK2
	cairo_destroy (cr);
	return (FALSE);
#endif
}

/****************************************
 *              Public API              *
 ****************************************/

gboolean
biorhythmus_chart_set_birthday (BiorhythmusChart *chart, gint day, gint month, gint year)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	chart->priv->birthday.day = day;
	chart->priv->birthday.month = month;
	chart->priv->birthday.year = year;

	gtk_widget_queue_resize (GTK_WIDGET (chart));

	return TRUE;
}

gboolean
biorhythmus_chart_set_active_date (BiorhythmusChart *chart, gint day, gint month, gint year)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	chart->priv->active_date.day = day;
	chart->priv->active_date.month = month;
	chart->priv->active_date.year = year;

	gtk_widget_queue_resize (GTK_WIDGET (chart));

	return TRUE;
}

gboolean
biorhythmus_chart_set_option_physical (BiorhythmusChart *chart, gboolean state)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	chart->priv->option_physical = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));

	return TRUE;
}

gboolean
biorhythmus_chart_get_option_physical (BiorhythmusChart *chart)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	return chart->priv->option_physical;
}

gboolean
biorhythmus_chart_set_option_emotional (BiorhythmusChart *chart, gboolean state)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	chart->priv->option_emotional = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));

	return TRUE;
}

gboolean
biorhythmus_chart_get_option_emotional (BiorhythmusChart *chart)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	return chart->priv->option_emotional;
}

gboolean
biorhythmus_chart_set_option_intellectual (BiorhythmusChart *chart, gboolean state)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	chart->priv->option_intellectual = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));

	return TRUE;
}

gboolean
biorhythmus_chart_get_option_intellectual (BiorhythmusChart *chart)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	return chart->priv->option_intellectual;
}

gboolean
biorhythmus_chart_set_option_total (BiorhythmusChart *chart, gboolean state)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	chart->priv->option_total = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));

	return TRUE;
}

gboolean
biorhythmus_chart_get_option_total (BiorhythmusChart *chart)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	return chart->priv->option_total;
}

/* ex:set ts=4 noet: */
