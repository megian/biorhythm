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

	gchar *name_of_person;

	gboolean option_physical;
	gboolean option_emotional;
	gboolean option_intellectual;
	gboolean option_total;
};

struct _BiorhythmusChartDivision
{
	gint width;
	gint height;

	gint margin_top;
	gint margin_left;
	gint margin_bottom;
	gint margin_right;
};

typedef struct _BiorhythmusChartDivision BiorhythmusChartDivision;

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

	priv->name_of_person = NULL;

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
biorhythmus_chart_draw_curves_select_dash (cairo_t *cr, gint cycle_day)
{
	static const double dashed_physical[] = {40.0, 4.0};
	static int dashed_physical_length = sizeof (dashed_physical) / sizeof (dashed_physical[0]);

	static const double dashed_intellectual[] = {10.0, 6.0};
	static int dashed_intellectual_length  = sizeof (dashed_intellectual) / sizeof (dashed_intellectual[0]);

	static const double dashed_emotional[] = {20.0, 5.0, 2.0};
	static int dashed_emotional_length  = sizeof (dashed_emotional) / sizeof (dashed_intellectual[0]);

	if (cycle_day == BIORHYTHMUS_DAYS_PHYSICAL)
		cairo_set_dash (cr, dashed_physical, dashed_physical_length, 0);
	else if (cycle_day == BIORHYTHMUS_DAYS_INTELLECTUAL)
		cairo_set_dash (cr, dashed_intellectual, dashed_intellectual_length, 1);
	else if (cycle_day == BIORHYTHMUS_DAYS_EMOTIONAL)
		cairo_set_dash (cr, dashed_emotional, dashed_emotional_length, 1);
	else
		cairo_set_dash (cr, NULL, 0, 0.0);
}

void
biorhythmus_chart_draw_curves (BiorhythmusChartPrivate *priv, cairo_t *cr, double red, double green, double blue, BiorhythmusChartDivision *division, gint days_in_month, gint day_pix, gint days_of_life, gint cycle_day)
{
	gint i = 0;
	gint half_height = division->height / 2;

	if ((priv->birthday.month == priv->active_date.month) && (priv->birthday.year == priv->active_date.year))
	{
		i = (priv->birthday.day - 1) * day_pix;
		days_of_life = (priv->birthday.day - 1) * -1;
	}
	else if (days_of_life < 0)
		return;

	cairo_set_source_rgb (cr, red, green, blue);

	biorhythmus_chart_draw_curves_select_dash (cr, cycle_day);

	// Draw curves
	while (i <= ((days_in_month - 1) * day_pix))
	{
		if (cycle_day == BIORHYTHMUS_DAYS_TOTAL)
			cairo_line_to (cr, division->margin_left + i, division->margin_top + biorhythmus_math_bioday_graphic_total (i, days_of_life, half_height, day_pix));
		else
			cairo_line_to (cr, division->margin_left + i, division->margin_top + biorhythmus_math_bioday_graphic (i, days_of_life, cycle_day, half_height, day_pix));

		i++;
	}

	cairo_stroke (cr);
}

void
biorhythmus_chart_draw_lines (cairo_t *cr, BiorhythmusChartDivision *division, gint days_in_month, gint day_pix)
{
	gint line_y = division->margin_top + (division->height / 2);

	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_line_width (cr, 0.5);
	cairo_move_to (cr, division->margin_left, line_y);
	cairo_line_to (cr, division->margin_left + ((days_in_month - 1) * day_pix), line_y);
	cairo_stroke (cr);
}

void
biorhythmus_chart_draw_current_day_line(cairo_t *cr, BiorhythmusChartDivision *division, gint days_in_month, gint day_pix, gint current_day)
{
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_line_width (cr, 0.5);
	cairo_move_to (cr, division->margin_left + (day_pix * (current_day - 1)), division->margin_top);
	cairo_line_to (cr, division->margin_left + (day_pix * (current_day - 1)), division->margin_top + division->height);
	cairo_stroke (cr);
}

void
biorhythmus_chart_draw_day_lines(cairo_t *cr, BiorhythmusChartDivision *division, gint days_in_month, gint day_pix)
{
	gint i, h100, half_height;
	gchar *s;

	h100 = (gint)(floor (division->height / 100) + 1);
	half_height = division->height / 2;
	for (i = 0;i < days_in_month;i++)
	{
		s = g_strdup_printf ("%d", i + 1);
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size (cr, 10);
		cairo_move_to (cr, division->margin_left + (i * day_pix) - 4, division->margin_top + half_height + h100 + 10);
		cairo_show_text (cr, s);
		g_free (s);

		cairo_set_source_rgb (cr, 0, 0, 0);
		cairo_move_to (cr, division->margin_left + (i * day_pix), division->margin_top + half_height - h100);
		cairo_line_to (cr, division->margin_left + (i * day_pix), division->margin_top + half_height + h100);
	}
	cairo_stroke (cr);
}

void
biorhythmus_chart_caption_text_rgb (cairo_t *cr, char *s, double red, double green, double blue)
{
	g_return_if_fail (cr != NULL);
	g_return_if_fail (s != NULL);

	cairo_set_source_rgb (cr, red, green, blue);
	cairo_show_text (cr, s);
	g_free (s);

	cairo_rel_move_to (cr, 10, 0);
}

void
biorhythmus_chart_caption_line_rgb (cairo_t *cr, double red, double green, double blue, gint cycle_day)
{
	g_return_if_fail (cr != NULL);

	cairo_set_source_rgb (cr, red, green, blue);

	biorhythmus_chart_draw_curves_select_dash (cr, cycle_day);
	cairo_set_line_width (cr, 0.5);
	cairo_rel_move_to (cr, 5.0, 0.0);
	cairo_rel_line_to (cr, 50.0, 0.0);
	
	cairo_rel_move_to (cr, 5, 0);
}

void
biorhythmus_chart_caption (BiorhythmusChartPrivate *priv, cairo_t *cr, BiorhythmusChartDivision *division)
{
	gint days_of_life;

	days_of_life = biorhythmus_math_daysoflife (priv->active_date, priv->birthday);

	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 11);
	cairo_move_to (cr, division->margin_left, division->margin_top + (division->height / 2));

	biorhythmus_chart_caption_text_rgb (cr, g_strdup_printf ("Date: %d.%d.%d", priv->active_date.day, priv->active_date.month, priv->active_date.year), 0.0, 0.0, 0.0);
	biorhythmus_chart_caption_text_rgb (cr, g_strdup_printf ("Birthday: %d.%d.%d", priv->birthday.day, priv->birthday.month, priv->birthday.year), 0.0, 0.0, 0.0);
	biorhythmus_chart_caption_text_rgb (cr, g_strdup_printf ("Days: %d", days_of_life), 0.0, 0.0, 0.0);
	
	// Physical
	if (priv->option_physical == TRUE)
	{
		//biorhythmus_chart_caption_line_rgb (cr,	1.0, 0.0, 0.0, BIORHYTHMUS_DAYS_PHYSICAL);
		biorhythmus_chart_caption_text_rgb (cr, g_strdup_printf ("Physical: %d", biorhythmus_math_bioday (days_of_life, BIORHYTHMUS_DAYS_PHYSICAL)), 1.0, 0.0, 0.0);
	}

	// Emotional
	if (priv->option_emotional == TRUE)
		biorhythmus_chart_caption_text_rgb (cr, g_strdup_printf ("Emotional: %d", biorhythmus_math_bioday (days_of_life, BIORHYTHMUS_DAYS_EMOTIONAL)), 0.0, 1.0, 0.0);

	// Intellectual
	if (priv->option_intellectual == TRUE)
		biorhythmus_chart_caption_text_rgb (cr, g_strdup_printf ("Intellectual: %d", biorhythmus_math_bioday (days_of_life, BIORHYTHMUS_DAYS_INTELLECTUAL)), 0.0, 0.0, 1.0);

	// Total
	if (priv->option_total == TRUE)
		biorhythmus_chart_caption_text_rgb (cr, g_strdup_printf ("Total: %d", biorhythmus_math_bioday_total (days_of_life)), 0.0, 0.0, 0.0);
	
	cairo_stroke (cr);
}

void
biorhythmus_chart_title (BiorhythmusChartPrivate *priv, cairo_t *cr, BiorhythmusChartDivision *division)
{
	gchar *s;
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 16);
	cairo_move_to (cr, division->margin_left, division->margin_top + (division->height / 2));

	if (priv->name_of_person == NULL)
		s = g_strdup_printf ("Biorhythmus for Unknown");
	else	
		s = g_strdup_printf ("Biorhythmus for %s", priv->name_of_person);

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
	gint days_of_life, days_in_month, day_pix, full_height, full_width;

#ifdef GTK2
	cairo_t *cr;
	GdkWindow *window;
#endif

	BiorhythmusChartPrivate *priv = BIORHYTHMUS_CHART_GET_PRIVATE (widget);
	BiorhythmusChartDivision *division_title = g_malloc (sizeof(BiorhythmusChartDivision));
	BiorhythmusChartDivision *division_chart = g_malloc (sizeof(BiorhythmusChartDivision));
	BiorhythmusChartDivision *division_caption = g_malloc (sizeof(BiorhythmusChartDivision));

	g_return_if_fail (priv != NULL);
	g_return_if_fail (division_chart != NULL);

#ifdef GTK2
	window = gtk_widget_get_window (widget);
	if ((cr = gdk_cairo_create (window))==NULL)
	{
		cairo_destroy (cr);
		return (FALSE);
	}
#endif

	// Inizialize
	division_chart->margin_top = 60;
	division_chart->margin_left = 25;
	division_chart->margin_bottom = 40;
	division_chart->margin_right = 25;

	division_caption->margin_left = 25;
	division_caption->height = 20;

	division_title->margin_top = 15;
	division_title->margin_left = 25;
	division_title->height = 50;

#ifdef GTK2
	full_height = widget->allocation.height;
	full_width = widget->allocation.width;
#else
	full_height = gtk_widget_get_allocated_height (widget);
	full_width = gtk_widget_get_allocated_width (widget);
#endif

	division_chart->height = full_height - division_chart->margin_top - division_chart->margin_bottom;
	division_chart->width = full_width - division_chart->margin_left - division_chart->margin_right;

	division_caption->margin_top = full_height - division_caption->height;

	// Title
	biorhythmus_chart_title (priv, cr, division_title);

	// Caption
	biorhythmus_chart_caption (priv, cr, division_caption);

	// Set Parameter
	days_in_month = g_date_get_days_in_month (priv->active_date.month, priv->active_date.year);
	days_of_life = biorhythmus_math_daysoflife (priv->active_date, priv->birthday);
	day_pix = division_chart->width / (days_in_month - 1);

	biorhythmus_chart_draw_lines (cr, division_chart, days_in_month, day_pix);
	biorhythmus_chart_draw_current_day_line (cr, division_chart, days_in_month, day_pix, priv->active_date.day);
	biorhythmus_chart_draw_day_lines (cr, division_chart, days_in_month, day_pix);

	// Physical
	if (priv->option_physical == TRUE)
		biorhythmus_chart_draw_curves (priv, cr, 1.0, 0.0, 0.0, division_chart, days_in_month, day_pix, days_of_life, BIORHYTHMUS_DAYS_PHYSICAL);

	// Emotional
	if (priv->option_emotional == TRUE)
		biorhythmus_chart_draw_curves (priv, cr, 0.0, 1.0, 0.0, division_chart, days_in_month, day_pix, days_of_life, BIORHYTHMUS_DAYS_EMOTIONAL);

	// Intellectual
	if (priv->option_intellectual == TRUE)
		biorhythmus_chart_draw_curves (priv, cr, 0.0, 0.0, 1.0, division_chart, days_in_month, day_pix, days_of_life, BIORHYTHMUS_DAYS_INTELLECTUAL);

	if (priv->option_total == TRUE)
		biorhythmus_chart_draw_curves (priv, cr, 0.0, 0.0, 0.0, division_chart, days_in_month, day_pix, days_of_life, BIORHYTHMUS_DAYS_TOTAL);

	cairo_stroke (cr);
	g_free (division_title);
	g_free (division_chart);
	g_free (division_caption);

#ifdef GTK2
	cairo_destroy (cr);
	return (FALSE);
#endif
}

/****************************************
 *              Public API              *
 ****************************************/

void
biorhythmus_chart_set_birthday (BiorhythmusChart *chart, gint day, gint month, gint year)
{
	g_return_if_fail (BIORHYTHMUS_IS_CHART (chart));

	chart->priv->birthday.day = day;
	chart->priv->birthday.month = month;
	chart->priv->birthday.year = year;

	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

void
biorhythmus_chart_set_active_date (BiorhythmusChart *chart, gint day, gint month, gint year)
{
	g_return_if_fail (BIORHYTHMUS_IS_CHART (chart));

	chart->priv->active_date.day = day;
	chart->priv->active_date.month = month;
	chart->priv->active_date.year = year;

	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

void
biorhythmus_chart_set_name (BiorhythmusChart *chart, gchar *s)
{
	g_return_if_fail (BIORHYTHMUS_IS_CHART (chart));
	g_return_if_fail (s != NULL);

	chart->priv->name_of_person = g_strdup (s);

	gtk_widget_queue_resize (GTK_WIDGET (chart));
}


void
biorhythmus_chart_set_option_physical (BiorhythmusChart *chart, gboolean state)
{
	g_return_if_fail (BIORHYTHMUS_IS_CHART (chart));

	chart->priv->option_physical = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

gboolean
biorhythmus_chart_get_option_physical (BiorhythmusChart *chart)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	return chart->priv->option_physical;
}

void
biorhythmus_chart_set_option_emotional (BiorhythmusChart *chart, gboolean state)
{
	g_return_if_fail (BIORHYTHMUS_IS_CHART (chart));

	chart->priv->option_emotional = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

gboolean
biorhythmus_chart_get_option_emotional (BiorhythmusChart *chart)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	return chart->priv->option_emotional;
}

void
biorhythmus_chart_set_option_intellectual (BiorhythmusChart *chart, gboolean state)
{
	g_return_if_fail (BIORHYTHMUS_IS_CHART (chart));

	chart->priv->option_intellectual = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

gboolean
biorhythmus_chart_get_option_intellectual (BiorhythmusChart *chart)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	return chart->priv->option_intellectual;
}

void
biorhythmus_chart_set_option_total (BiorhythmusChart *chart, gboolean state)
{
	g_return_if_fail (BIORHYTHMUS_IS_CHART (chart));

	chart->priv->option_total = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

gboolean
biorhythmus_chart_get_option_total (BiorhythmusChart *chart)
{
	g_return_val_if_fail (BIORHYTHMUS_IS_CHART (chart), FALSE);

	return chart->priv->option_total;
}

/* ex:set ts=4 noet: */
