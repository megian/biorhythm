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

void
biorhythm_chart_draw (GtkDrawingArea *area, cairo_t *cairo_context, int width, int height, gpointer user_data);
typedef struct
{
	GtkWidget *parent_widget;

	struct bio_date birthday;
	struct bio_date active_date;

	gchar *name_of_person;

	gboolean option_physical;
	gboolean option_emotional;
	gboolean option_intellectual;
	gboolean option_total;
} BiorhythmChartPrivate;

struct _BiorhythmChart
{
	GtkDrawingArea parent_instance;

	BiorhythmChartPrivate *priv;
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

G_DEFINE_TYPE_WITH_PRIVATE (BiorhythmChart, biorhythm_chart, GTK_TYPE_DRAWING_AREA)

/****************************************
 *                 Class                *
 ****************************************/

static void
biorhythm_chart_class_init (BiorhythmChartClass *klass)
{
}

void
biorhythm_chart_set_current_date (struct bio_date *date)
{
	time_t now = time (NULL);
	GDate *current_date;

	current_date = g_date_new ();
	g_date_set_time_t (current_date, now);

	date->day = g_date_get_day (current_date);
	date->month = g_date_get_month (current_date);
	date->year = g_date_get_year (current_date);

	g_date_free (current_date);
}

static void
biorhythm_chart_init (BiorhythmChart *self)
{
	BiorhythmChartPrivate *priv;

	self->priv = priv = biorhythm_chart_get_instance_private (self);

	priv->option_physical = TRUE;
	priv->option_emotional = TRUE;
	priv->option_intellectual = TRUE;
	priv->option_total = TRUE;

	priv->name_of_person = NULL;

	biorhythm_chart_set_current_date (&priv->active_date);
	biorhythm_chart_set_current_date (&priv->birthday);

	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (self), biorhythm_chart_draw, NULL, NULL);
}

BiorhythmChart *
biorhythm_chart_new ()
{
	return g_object_new (BIORHYTHM_TYPE_CHART, NULL);
}

/****************************************
 *           Private Functions          *
 ****************************************/

void
biorhythm_chart_draw_curves_select_dash (cairo_t *cairo_context, gint cycle_day)
{
	static const double dashed_physical[] = { 2.0, 2.0 };
	static int dashed_physical_length = sizeof (dashed_physical) / sizeof (dashed_physical[0]);

	static const double dashed_intellectual[] = { 10.0, 4.0 };
	static int dashed_intellectual_length = sizeof (dashed_intellectual) / sizeof (dashed_intellectual[0]);

	static const double dashed_emotional[] = { 2.0, 10.0 };
	static int dashed_emotional_length = sizeof (dashed_emotional) / sizeof (dashed_intellectual[0]);

	switch (cycle_day) {
		case BIORHYTHM_DAYS_PHYSICAL:
			cairo_set_dash (cairo_context, dashed_physical, dashed_physical_length, 0);
			break;
		case BIORHYTHM_DAYS_INTELLECTUAL:
			cairo_set_dash (cairo_context, dashed_intellectual, dashed_intellectual_length, 1);
			break;
		case BIORHYTHM_DAYS_EMOTIONAL:
			cairo_set_dash (cairo_context, dashed_emotional, dashed_emotional_length, 1);
			break;
		default:
			cairo_set_dash (cairo_context, NULL, 0, 0.0);
			break;
	}
}

void
biorhythm_chart_draw_curves_select_color (cairo_t *cairo_context, gint cycle_day)
{
	cairo_set_source_rgb (cairo_context,
		(cycle_day == BIORHYTHM_DAYS_PHYSICAL) * 1.0,
		(cycle_day == BIORHYTHM_DAYS_INTELLECTUAL) * 1.0,
		(cycle_day == BIORHYTHM_DAYS_EMOTIONAL) * 1.0
	);
}

void
biorhythm_chart_draw_curves (BiorhythmChartPrivate *priv, cairo_t *cairo_context, BiorhythmChartDivision *division, gint days_in_month, gint day_pix, gint month_day_offset, gint cycle_day)
{
	gint counter = 0;
	gint half_height = division->height / 2;

	if ((priv->birthday.month == priv->active_date.month) && (priv->birthday.year == priv->active_date.year))
	{
		counter = (priv->birthday.day - 1) * day_pix;
		month_day_offset = (priv->birthday.day - 1) * -1;
	}
	else if (month_day_offset < 0)
	{
		return;
	}

	biorhythm_chart_draw_curves_select_color (cairo_context, cycle_day);
	biorhythm_chart_draw_curves_select_dash (cairo_context, cycle_day);

	// Draw curves
	while (counter <= ((days_in_month - 1) * day_pix))
	{
		if (cycle_day == BIORHYTHM_DAYS_TOTAL)
		{
			cairo_line_to (cairo_context, division->margin_left + counter, division->margin_top + biorhythm_math_bioday_graphic_total (counter, month_day_offset, half_height, day_pix));
		}
		else
		{
			cairo_line_to (cairo_context, division->margin_left + counter, division->margin_top + biorhythm_math_bioday_graphic (counter, month_day_offset, cycle_day, half_height, day_pix));
		}

		counter++;
	}

	cairo_stroke (cairo_context);
}

void
biorhythm_chart_draw_lines (cairo_t *cairo_context, BiorhythmChartDivision *division, gint days_in_month, gint day_pix)
{
	gint line_y = division->margin_top + (division->height / 2);

	cairo_set_source_rgb (cairo_context, 0, 0, 0);
	cairo_set_line_width (cairo_context, BIORHYTHM_CHART_LINE_WIDTH_DEFAULT);
	cairo_move_to (cairo_context, division->margin_left, line_y);
	cairo_line_to (cairo_context, division->margin_left + ((days_in_month - 1) * day_pix), line_y);
	cairo_stroke (cairo_context);
}

void
biorhythm_chart_draw_current_day_line (cairo_t *cairo_context, BiorhythmChartDivision *division, gint days_in_month, gint day_pix, gint current_day)
{
	cairo_set_source_rgb (cairo_context, 0, 0, 0);
	cairo_set_line_width (cairo_context, BIORHYTHM_CHART_LINE_WIDTH_DAY);
	cairo_move_to (cairo_context, division->margin_left + (day_pix * (current_day - 1)), division->margin_top);
	cairo_line_to (cairo_context, division->margin_left + (day_pix * (current_day - 1)), division->margin_top + division->height);
	cairo_stroke (cairo_context);
}

void
biorhythm_chart_draw_day_lines (cairo_t *cairo_context, BiorhythmChartDivision *division, gint days_in_month, gint day_pix)
{
	const double font_size = BIORHYTHM_CHART_FONT_SIZE;
	gint day_in_month;
	gint h100;
	gint half_height;
	gchar *text;

	h100 = (gint) (floor (division->height / 100) + 1);
	half_height = division->height / 2;
	for (day_in_month = 0; day_in_month < days_in_month; day_in_month++)
	{
		text = g_strdup_printf ("%d", day_in_month + 1);
		cairo_set_source_rgb (cairo_context, 0.0, 0.0, 0.0);
		cairo_select_font_face (cairo_context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size (cairo_context, font_size);
		cairo_move_to (cairo_context, division->margin_left + (day_in_month * day_pix) - 4, division->margin_top + half_height + h100 + 10);
		cairo_show_text (cairo_context, text);
		g_free (text);

		cairo_set_source_rgb (cairo_context, 0, 0, 0);
		cairo_move_to (cairo_context, division->margin_left + (day_in_month * day_pix), division->margin_top + half_height - h100);
		cairo_line_to (cairo_context, division->margin_left + (day_in_month * day_pix), division->margin_top + half_height + h100);
	}
	cairo_stroke (cairo_context);
}

void
biorhythm_chart_caption_text (cairo_t *cairo_context, char *text)
{
	const double caption_text_margin_right = BIORHYTHM_CHART_CAPTION_TEXT_MARGIN_RIGHT;
	g_return_if_fail (cairo_context != NULL);
	g_return_if_fail (text != NULL);

	cairo_show_text (cairo_context, text);
	g_free (text);

	cairo_rel_move_to (cairo_context, caption_text_margin_right, 0);
}

void
biorhythm_chart_caption_line (cairo_t *cairo_context, gint cycle_day)
{
	const double caption_line_length = BIORHYTHM_CHART_CAPTION_LINE_LENGTH;
	const double caption_line_margin_left = BIORHYTHM_CHART_CAPTION_LINE_MARGIN_LEFT;
	const double caption_line_margin_right = BIORHYTHM_CHART_CAPTION_LINE_MARGIN_RIGHT;
	const double caption_line_line_width = BIORHYTHM_CHART_CAPTION_LINE_LINE_WIDTH;
	const double font_size = BIORHYTHM_CHART_FONT_SIZE;
	const double half_font_size = font_size / 2;
	double current_x;
	double current_y;
	g_return_if_fail (cairo_context != NULL);

	cairo_get_current_point (cairo_context, &current_x, &current_y);
	biorhythm_chart_draw_curves_select_dash (cairo_context, cycle_day);
	biorhythm_chart_draw_curves_select_color (cairo_context, cycle_day);
	cairo_set_line_width (cairo_context, caption_line_line_width);
	cairo_rel_move_to (cairo_context, caption_line_margin_left, half_font_size * -1);
	cairo_rel_line_to (cairo_context, caption_line_length, 0.0);
	cairo_stroke (cairo_context);

	cairo_move_to (cairo_context, current_x + caption_line_length + caption_line_margin_right, current_y);
}

void
biorhythm_chart_caption_option (cairo_t *cairo_context, char *text, gint cycle_day)
{
	biorhythm_chart_caption_line (cairo_context, cycle_day);
	biorhythm_chart_draw_curves_select_color (cairo_context, cycle_day);
	biorhythm_chart_caption_text (cairo_context, text);
}

char *
biorhythm_chart_caption_weekday (struct bio_date date)
{
	GDate *gdate;
	char *text;
	static char *weekdays[] = { "", N_ ("Mon"), N_ ("Tue"), N_ ("Wed"), N_ ("Thu"), N_ ("Fri"), N_ ("Sat"), N_ ("Sun") };

	gdate = g_date_new_dmy (date.day, date.month, date.year);
	text = weekdays[g_date_get_weekday (gdate)];
	g_date_free (gdate);

	return text;
}

void
biorhythm_chart_caption (BiorhythmChartPrivate *priv, cairo_t *cairo_context, BiorhythmChartDivision *division)
{
	const double font_size = BIORHYTHM_CHART_CAPTION_LINE_FONT_SIZE;
	gint days_of_life;

	days_of_life = biorhythm_math_daysoflife (priv->active_date, priv->birthday);

	cairo_select_font_face (cairo_context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cairo_context, font_size);
	cairo_move_to (cairo_context, division->margin_left, division->margin_top + (division->height / 2));

	biorhythm_chart_caption_text (cairo_context, g_strdup_printf (_ ("Date: %s %d.%d.%d"), biorhythm_chart_caption_weekday (priv->active_date), priv->active_date.day, priv->active_date.month, priv->active_date.year));
	biorhythm_chart_caption_text (cairo_context, g_strdup_printf (_ ("Birthday: %s %d.%d.%d"), biorhythm_chart_caption_weekday (priv->birthday), priv->birthday.day, priv->birthday.month, priv->birthday.year));
	biorhythm_chart_caption_text (cairo_context, g_strdup_printf (_ ("Days: %d"), days_of_life));

	// Physical
	if (priv->option_physical == TRUE)
	{
		biorhythm_chart_caption_option (cairo_context, g_strdup_printf (_ ("Physical: %d"), biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_PHYSICAL)), BIORHYTHM_DAYS_PHYSICAL);
	}

	// Emotional
	if (priv->option_emotional == TRUE)
	{
		biorhythm_chart_caption_option (cairo_context, g_strdup_printf (_ ("Emotional: %d"), biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_EMOTIONAL)), BIORHYTHM_DAYS_EMOTIONAL);
	}

	// Intellectual
	if (priv->option_intellectual == TRUE)
	{
		biorhythm_chart_caption_option (cairo_context, g_strdup_printf (_ ("Intellectual: %d"), biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_INTELLECTUAL)),
										BIORHYTHM_DAYS_INTELLECTUAL);
	}

	// Total
	if (priv->option_total == TRUE)
	{
		biorhythm_chart_caption_option (cairo_context, g_strdup_printf (_ ("Total: %d"), biorhythm_math_bioday_total (days_of_life)), BIORHYTHM_DAYS_TOTAL);
	}

	cairo_stroke (cairo_context);
}

void
biorhythm_chart_title (BiorhythmChartPrivate *priv, cairo_t *cairo_context, BiorhythmChartDivision *division)
{
	const double font_size = 16;
	gchar *text;
	cairo_set_source_rgb (cairo_context, 0.0, 0.0, 0.0);
	cairo_select_font_face (cairo_context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cairo_context, font_size);
	cairo_move_to (cairo_context, division->margin_left, division->margin_top + (division->height / 2));

	if (priv->name_of_person == NULL)
	{
		text = g_strdup_printf (_ ("Biorhythm for Unknown"));
	}
	else
	{
		text = g_strdup_printf (_ ("Biorhythm for %s"), priv->name_of_person);
	}

	cairo_show_text (cairo_context, text);
	g_free (text);
	cairo_stroke (cairo_context);
}

void
biorhythm_chart_draw_cairo (BiorhythmChart *chart, cairo_t *cairo_context, gint full_height, gint full_width)
{
	gint month_day_offset;
	gint days_in_month;
	gint day_pix;

	BiorhythmChartPrivate *priv = biorhythm_chart_get_instance_private (chart);
	BiorhythmChartDivision *division_title = g_malloc (sizeof (BiorhythmChartDivision));
	BiorhythmChartDivision *division_chart = g_malloc (sizeof (BiorhythmChartDivision));
	BiorhythmChartDivision *division_caption = g_malloc (sizeof (BiorhythmChartDivision));

	g_return_if_fail (priv != NULL);
	g_return_if_fail (division_chart != NULL);

	// Inizialize
	division_chart->margin_top = BIORHYTHM_CHART_DIVISION_CHART_MARGIN_TOP;
	division_chart->margin_left = BIORHYTHM_CHART_DIVISION_CHART_MARGIN_LEFT;
	division_chart->margin_bottom = BIORHYTHM_CHART_DIVISION_CHART_MARGIN_BOTTOM;
	division_chart->margin_right = BIORHYTHM_CHART_DIVISION_CHART_MARGIN_RIGHT;

	division_caption->margin_left = BIORHYTHM_CHART_DIVISION_CAPTION_MARGIN_LEFT;
	division_caption->height = BIORHYTHM_CHART_DIVISION_CAPTION_MARGIN_HIGHT;

	division_title->margin_top = BIORHYTHM_CHART_DIVISION_TITLE_MARGIN_TOP;
	division_title->margin_left = BIORHYTHM_CHART_DIVISION_TITLE_MARGIN_LEFT;
	division_title->height = BIORHYTHM_CHART_DIVISION_TITLE_HEIGHT;

	division_chart->height = full_height - division_chart->margin_top - division_chart->margin_bottom;
	division_chart->width = full_width - division_chart->margin_left - division_chart->margin_right;

	division_caption->margin_top = full_height - division_caption->height;

	// Set Parameter
	days_in_month = g_date_get_days_in_month (priv->active_date.month, priv->active_date.year);
	month_day_offset = biorhythm_math_monthdayoffset (priv->active_date, priv->birthday);
	day_pix = division_chart->width / (days_in_month - 1);

	biorhythm_chart_draw_lines (cairo_context, division_chart, days_in_month, day_pix);
	biorhythm_chart_draw_current_day_line (cairo_context, division_chart, days_in_month, day_pix, priv->active_date.day);
	biorhythm_chart_draw_day_lines (cairo_context, division_chart, days_in_month, day_pix);

	// Physical
	if (priv->option_physical == TRUE)
	{
		biorhythm_chart_draw_curves (priv, cairo_context, division_chart, days_in_month, day_pix, month_day_offset, BIORHYTHM_DAYS_PHYSICAL);
	}

	// Emotional
	if (priv->option_emotional == TRUE)
	{
		biorhythm_chart_draw_curves (priv, cairo_context, division_chart, days_in_month, day_pix, month_day_offset, BIORHYTHM_DAYS_EMOTIONAL);
	}

	// Intellectual
	if (priv->option_intellectual == TRUE)
	{
		biorhythm_chart_draw_curves (priv, cairo_context, division_chart, days_in_month, day_pix, month_day_offset, BIORHYTHM_DAYS_INTELLECTUAL);
	}

	// Total
	if (priv->option_total == TRUE)
	{
		biorhythm_chart_draw_curves (priv, cairo_context, division_chart, days_in_month, day_pix, month_day_offset, BIORHYTHM_DAYS_TOTAL);
	}

	cairo_stroke (cairo_context);

	// Title
	biorhythm_chart_title (priv, cairo_context, division_title);

	// Caption
	biorhythm_chart_caption (priv, cairo_context, division_caption);

	g_free (division_title);
	g_free (division_chart);
	g_free (division_caption);
}

void
biorhythm_chart_draw (GtkDrawingArea *area, cairo_t *cairo_context, int width, int height, gpointer user_data)
{
	biorhythm_chart_draw_cairo (BIORHYTHM_CHART (area), cairo_context, height, width);
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
biorhythm_chart_set_name (BiorhythmChart *chart, gchar *name)
{
	g_return_if_fail (BIORHYTHM_IS_CHART (chart));
	g_return_if_fail (name != NULL);

	chart->priv->name_of_person = g_strdup (name);

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
