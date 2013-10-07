/* -*-coding: utf-8;-*- */

/* biorhythm-chart.c
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

#include "biorhythm-chart.h"

#ifdef GTK2
static gboolean
biorhythm_chart_draw (GtkWidget *widget, GdkEventExpose *event, gpointer data);
#else
void
biorhythm_chart_draw (GtkWidget *widget, cairo_t *cr);
#endif

struct _BiorhythmChartPrivate
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

struct _BiorhythmChartDivision
{
	gint width;
	gint height;

	gint margin_top;
	gint margin_left;
	gint margin_bottom;
	gint margin_right;
};

typedef struct _BiorhythmChartDivision BiorhythmChartDivision;

#define BIORHYTHM_CHART_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), BIORHYTHM_TYPE_CHART, BiorhythmChartPrivate));

G_DEFINE_TYPE (BiorhythmChart, biorhythm_chart, GTK_TYPE_DRAWING_AREA)

/****************************************
 *                 Class                *
 ****************************************/

static void
biorhythm_chart_class_init (BiorhythmChartClass *klass)
{
	GtkWidgetClass *widget_class;

	g_type_class_add_private (klass, sizeof (BiorhythmChartPrivate));
	
	widget_class = GTK_WIDGET_CLASS (klass);

#ifdef GTK2
	widget_class->expose_event = (void*)biorhythm_chart_draw;
#else
	widget_class->draw = (void*)biorhythm_chart_draw;
#endif
}

void
biorhythm_chart_set_current_date (struct bio_date *date)
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
biorhythm_chart_init (BiorhythmChart *self)
{
	BiorhythmChartPrivate *priv;

	self->priv = priv = BIORHYTHM_CHART_GET_PRIVATE (self);

	priv->option_physical = TRUE;
	priv->option_emotional = TRUE;
	priv->option_intellectual = TRUE;
	priv->option_total = TRUE;

	priv->name_of_person = NULL;

	biorhythm_chart_set_current_date (&priv->active_date);
	biorhythm_chart_set_current_date (&priv->birthday);
}

GtkWidget*
biorhythm_chart_new ()
{
	return GTK_WIDGET (g_object_new (BIORHYTHM_TYPE_CHART, NULL));
}

/****************************************
 *           Private Functions          *
 ****************************************/

void
biorhythm_chart_draw_curves_select_dash (cairo_t *cr, gint cycle_day)
{
	static const double dashed_physical[] = {2.0, 2.0};
	static int dashed_physical_length = sizeof (dashed_physical) / sizeof (dashed_physical[0]);

	static const double dashed_intellectual[] = {10.0, 4.0};
	static int dashed_intellectual_length  = sizeof (dashed_intellectual) / sizeof (dashed_intellectual[0]);

	static const double dashed_emotional[] = {2.0, 10.0};
	static int dashed_emotional_length  = sizeof (dashed_emotional) / sizeof (dashed_intellectual[0]);

	if (cycle_day == BIORHYTHM_DAYS_PHYSICAL)
		cairo_set_dash (cr, dashed_physical, dashed_physical_length, 0);
	else if (cycle_day == BIORHYTHM_DAYS_INTELLECTUAL)
		cairo_set_dash (cr, dashed_intellectual, dashed_intellectual_length, 1);
	else if (cycle_day == BIORHYTHM_DAYS_EMOTIONAL)
		cairo_set_dash (cr, dashed_emotional, dashed_emotional_length, 1);
	else
		cairo_set_dash (cr, NULL, 0, 0.0);
}

void
biorhythm_chart_draw_curves_select_color (cairo_t *cr, gint cycle_day)
{
	if (cycle_day == BIORHYTHM_DAYS_PHYSICAL)
		cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	else if (cycle_day == BIORHYTHM_DAYS_INTELLECTUAL)
		cairo_set_source_rgb (cr, 0.0, 1.0, 0.0);
	else if (cycle_day == BIORHYTHM_DAYS_EMOTIONAL)
		cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
	else
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
}

void
biorhythm_chart_draw_curves (BiorhythmChartPrivate *priv, cairo_t *cr, BiorhythmChartDivision *division, gint days_in_month, gint day_pix, gint month_day_offset, gint cycle_day)
{
	gint i = 0;
	gint half_height = division->height / 2;

	if ((priv->birthday.month == priv->active_date.month) && (priv->birthday.year == priv->active_date.year))
	{
		i = (priv->birthday.day - 1) * day_pix;
		month_day_offset = (priv->birthday.day - 1) * -1;
	}
	else if (month_day_offset < 0)
		return;

	biorhythm_chart_draw_curves_select_color (cr, cycle_day);
	biorhythm_chart_draw_curves_select_dash (cr, cycle_day);

	// Draw curves
	while (i <= ((days_in_month - 1) * day_pix))
	{
		if (cycle_day == BIORHYTHM_DAYS_TOTAL)
			cairo_line_to (cr, division->margin_left + i, division->margin_top + biorhythm_math_bioday_graphic_total (i, month_day_offset, half_height, day_pix));
		else
			cairo_line_to (cr, division->margin_left + i, division->margin_top + biorhythm_math_bioday_graphic (i, month_day_offset, cycle_day, half_height, day_pix));

		i++;
	}

	cairo_stroke (cr);
}

void
biorhythm_chart_draw_lines (cairo_t *cr, BiorhythmChartDivision *division, gint days_in_month, gint day_pix)
{
	gint line_y = division->margin_top + (division->height / 2);

	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_line_width (cr, 0.5);
	cairo_move_to (cr, division->margin_left, line_y);
	cairo_line_to (cr, division->margin_left + ((days_in_month - 1) * day_pix), line_y);
	cairo_stroke (cr);
}

void
biorhythm_chart_draw_current_day_line(cairo_t *cr, BiorhythmChartDivision *division, gint days_in_month, gint day_pix, gint current_day)
{
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_line_width (cr, 0.5);
	cairo_move_to (cr, division->margin_left + (day_pix * (current_day - 1)), division->margin_top);
	cairo_line_to (cr, division->margin_left + (day_pix * (current_day - 1)), division->margin_top + division->height);
	cairo_stroke (cr);
}

void
biorhythm_chart_draw_day_lines(cairo_t *cr, BiorhythmChartDivision *division, gint days_in_month, gint day_pix)
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
biorhythm_chart_caption_text (cairo_t *cr, char *s)
{
	g_return_if_fail (cr != NULL);
	g_return_if_fail (s != NULL);

	cairo_show_text (cr, s);
	g_free (s);

	cairo_rel_move_to (cr, 10, 0);
}

void
biorhythm_chart_caption_line (cairo_t *cr, gint cycle_day)
{
	double x, y;
	g_return_if_fail (cr != NULL);

	cairo_get_current_point (cr, &x, &y);
	biorhythm_chart_draw_curves_select_dash (cr, cycle_day);
	biorhythm_chart_draw_curves_select_color (cr, cycle_day);
	cairo_set_line_width (cr, 0.2);
	cairo_rel_move_to (cr, 5.0, -4.0);
	cairo_rel_line_to (cr, 25.0, 0.0);
	cairo_stroke (cr);

	cairo_move_to (cr, x + 25.0 + 10.0, y);
}

void
biorhythm_chart_caption_option (cairo_t *cr, char *s, gint cycle_day)
{
	biorhythm_chart_caption_line (cr, cycle_day);
	biorhythm_chart_draw_curves_select_color (cr, cycle_day);
	biorhythm_chart_caption_text (cr, s);
}

void
biorhythm_chart_caption (BiorhythmChartPrivate *priv, cairo_t *cr, BiorhythmChartDivision *division)
{
	gint days_of_life;

	days_of_life = biorhythm_math_daysoflife (priv->active_date, priv->birthday);

	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 11);
	cairo_move_to (cr, division->margin_left, division->margin_top + (division->height / 2));

	biorhythm_chart_caption_text (cr, g_strdup_printf (_("Date: %d.%d.%d"), priv->active_date.day, priv->active_date.month, priv->active_date.year));
	biorhythm_chart_caption_text (cr, g_strdup_printf (_("Birthday: %d.%d.%d"), priv->birthday.day, priv->birthday.month, priv->birthday.year));
	biorhythm_chart_caption_text (cr, g_strdup_printf (_("Days: %d"), days_of_life));
	
	// Physical
	if (priv->option_physical == TRUE)
		biorhythm_chart_caption_option (cr, g_strdup_printf (_("Physical: %d"), biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_PHYSICAL)), BIORHYTHM_DAYS_PHYSICAL);

	// Emotional
	if (priv->option_emotional == TRUE)
		biorhythm_chart_caption_option (cr, g_strdup_printf (_("Emotional: %d"), biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_EMOTIONAL)), BIORHYTHM_DAYS_EMOTIONAL);

	// Intellectual
	if (priv->option_intellectual == TRUE)
		biorhythm_chart_caption_option (cr, g_strdup_printf (_("Intellectual: %d"), biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_INTELLECTUAL)),
		BIORHYTHM_DAYS_INTELLECTUAL);

	// Total
	if (priv->option_total == TRUE)
		biorhythm_chart_caption_option (cr, g_strdup_printf (_("Total: %d"), biorhythm_math_bioday_total (days_of_life)), BIORHYTHM_DAYS_TOTAL);
	
	cairo_stroke (cr);
}

void
biorhythm_chart_title (BiorhythmChartPrivate *priv, cairo_t *cr, BiorhythmChartDivision *division)
{
	gchar *s;
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 16);
	cairo_move_to (cr, division->margin_left, division->margin_top + (division->height / 2));

	if (priv->name_of_person == NULL)
		s = g_strdup_printf (_("Biorhythm for Unknown"));
	else	
		s = g_strdup_printf (_("Biorhythm for %s"), priv->name_of_person);

	cairo_show_text (cr, s);
	g_free (s);
	cairo_stroke (cr);
}

void
biorhythm_chart_draw_cairo (BiorhythmChart *chart, cairo_t *cr, gint full_height, gint full_width)
{
	gint month_day_offset, days_in_month, day_pix;

	BiorhythmChartPrivate *priv = BIORHYTHM_CHART_GET_PRIVATE (chart);
	BiorhythmChartDivision *division_title = g_malloc (sizeof(BiorhythmChartDivision));
	BiorhythmChartDivision *division_chart = g_malloc (sizeof(BiorhythmChartDivision));
	BiorhythmChartDivision *division_caption = g_malloc (sizeof(BiorhythmChartDivision));

	g_return_if_fail (priv != NULL);
	g_return_if_fail (division_chart != NULL);

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

	division_chart->height = full_height - division_chart->margin_top - division_chart->margin_bottom;
	division_chart->width = full_width - division_chart->margin_left - division_chart->margin_right;

	division_caption->margin_top = full_height - division_caption->height;

	// Set Parameter
	days_in_month = g_date_get_days_in_month (priv->active_date.month, priv->active_date.year);
	month_day_offset = biorhythm_math_monthdayoffset (priv->active_date, priv->birthday);
	day_pix = division_chart->width / (days_in_month - 1);

	biorhythm_chart_draw_lines (cr, division_chart, days_in_month, day_pix);
	biorhythm_chart_draw_current_day_line (cr, division_chart, days_in_month, day_pix, priv->active_date.day);
	biorhythm_chart_draw_day_lines (cr, division_chart, days_in_month, day_pix);

	// Physical
	if (priv->option_physical == TRUE)
		biorhythm_chart_draw_curves (priv, cr, division_chart, days_in_month, day_pix, month_day_offset, BIORHYTHM_DAYS_PHYSICAL);

	// Emotional
	if (priv->option_emotional == TRUE)
		biorhythm_chart_draw_curves (priv, cr, division_chart, days_in_month, day_pix, month_day_offset, BIORHYTHM_DAYS_EMOTIONAL);

	// Intellectual
	if (priv->option_intellectual == TRUE)
		biorhythm_chart_draw_curves (priv, cr, division_chart, days_in_month, day_pix, month_day_offset, BIORHYTHM_DAYS_INTELLECTUAL);

	// Total
	if (priv->option_total == TRUE)
		biorhythm_chart_draw_curves (priv, cr, division_chart, days_in_month, day_pix, month_day_offset, BIORHYTHM_DAYS_TOTAL);

	cairo_stroke (cr);

	// Title
    biorhythm_chart_title (priv, cr, division_title);

    // Caption
	biorhythm_chart_caption (priv, cr, division_caption);

	g_free (division_title);
	g_free (division_chart);
	g_free (division_caption);
}

#ifdef GTK2
static gboolean
biorhythm_chart_draw (GtkWidget *widget, GdkEventExpose *event, gpointer data)
#else
void
biorhythm_chart_draw (GtkWidget *widget, cairo_t *cr)
#endif
{
	gint full_height, full_width;

#ifdef GTK2
	cairo_t *cr;
	GdkWindow *window;

	window = gtk_widget_get_window (widget);
	if ((cr = gdk_cairo_create (window))==NULL)
	{
		cairo_destroy (cr);
		return (FALSE);
	}
#endif

#ifdef GTK2
	full_height = widget->allocation.height;
	full_width = widget->allocation.width;
#else
	full_height = gtk_widget_get_allocated_height (widget);
	full_width = gtk_widget_get_allocated_width (widget);
#endif

	biorhythm_chart_draw_cairo (BIORHYTHM_CHART (widget), cr, full_height, full_width);

#ifdef GTK2
	cairo_destroy (cr);
	return (FALSE);
#endif
}

/****************************************
 *              Public API              *
 ****************************************/

void
biorhythm_chart_set_birthday (BiorhythmChart *chart, gint day, gint month, gint year)
{
	g_return_if_fail (BIORHYTHM_IS_CHART (chart));

	chart->priv->birthday.day = day;
	chart->priv->birthday.month = month;
	chart->priv->birthday.year = year;

	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

void
biorhythm_chart_set_active_date (BiorhythmChart *chart, gint day, gint month, gint year)
{
	g_return_if_fail (BIORHYTHM_IS_CHART (chart));

	chart->priv->active_date.day = day;
	chart->priv->active_date.month = month;
	chart->priv->active_date.year = year;

	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

void
biorhythm_chart_set_name (BiorhythmChart *chart, gchar *s)
{
	g_return_if_fail (BIORHYTHM_IS_CHART (chart));
	g_return_if_fail (s != NULL);

	chart->priv->name_of_person = g_strdup (s);

	gtk_widget_queue_resize (GTK_WIDGET (chart));
}


void
biorhythm_chart_set_option_physical (BiorhythmChart *chart, gboolean state)
{
	g_return_if_fail (BIORHYTHM_IS_CHART (chart));

	chart->priv->option_physical = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

gboolean
biorhythm_chart_get_option_physical (BiorhythmChart *chart)
{
	g_return_val_if_fail (BIORHYTHM_IS_CHART (chart), FALSE);

	return chart->priv->option_physical;
}

void
biorhythm_chart_set_option_emotional (BiorhythmChart *chart, gboolean state)
{
	g_return_if_fail (BIORHYTHM_IS_CHART (chart));

	chart->priv->option_emotional = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

gboolean
biorhythm_chart_get_option_emotional (BiorhythmChart *chart)
{
	g_return_val_if_fail (BIORHYTHM_IS_CHART (chart), FALSE);

	return chart->priv->option_emotional;
}

void
biorhythm_chart_set_option_intellectual (BiorhythmChart *chart, gboolean state)
{
	g_return_if_fail (BIORHYTHM_IS_CHART (chart));

	chart->priv->option_intellectual = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

gboolean
biorhythm_chart_get_option_intellectual (BiorhythmChart *chart)
{
	g_return_val_if_fail (BIORHYTHM_IS_CHART (chart), FALSE);

	return chart->priv->option_intellectual;
}

void
biorhythm_chart_set_option_total (BiorhythmChart *chart, gboolean state)
{
	g_return_if_fail (BIORHYTHM_IS_CHART (chart));

	chart->priv->option_total = state;
	gtk_widget_queue_resize (GTK_WIDGET (chart));
}

gboolean
biorhythm_chart_get_option_total (BiorhythmChart *chart)
{
	g_return_val_if_fail (BIORHYTHM_IS_CHART (chart), FALSE);

	return chart->priv->option_total;
}

/* ex:set ts=4 noet: */
