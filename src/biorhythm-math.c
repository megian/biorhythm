/* -*-coding: utf-8;-*- */

/* biorhythm-math.c
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

#include "biorhythm-math.h"

#define DAYS_IN_A_REGULAR_YEAR 365
#define LEAP_YEAR 1

gint
biorhythm_math_bioday (gint days_of_life, gint bio_cycle_days)
{
	gint rd;
	gdouble result;

	rd = (gint) (days_of_life - (bio_cycle_days * floor (days_of_life / bio_cycle_days)));
	result = (gfloat) (sin (rd * (M_PI * 2) / bio_cycle_days));
	return ((gint) floor (100 * result + 0.5));
}

gint
biorhythm_math_bioday_graphic (gint x, gint month_day_offset, gint bio_cycle_days, gint half_height, gint day_pix)
{
	gdouble calcsin;
	gdouble ri;

	ri = month_day_offset - (bio_cycle_days * floor (month_day_offset / bio_cycle_days));
	calcsin = sin ((x + (day_pix * ri)) * (M_PI * 2) / (day_pix * bio_cycle_days));
	return (gint) (floor (half_height - (half_height * calcsin) + 0.5));
}

gint
biorhythm_math_setpositiv (gint i)
{
	if (i < 0)
	{
		return (i * -1);
	}

	return (i);
}

gint
biorhythm_math_setpositivgraphic (gint i, gint half_height)
{
	if (i > half_height)
	{
		return (half_height - (i - half_height));
	}

	return (i);
}

gint
biorhythm_math_bioday_total (gint days_of_life)
{
	gint result_physical;
	gint result_emotional;
	gint result_intellectual;

	result_physical = biorhythm_math_setpositiv (biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_PHYSICAL));
	result_emotional = biorhythm_math_setpositiv (biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_EMOTIONAL));
	result_intellectual = biorhythm_math_setpositiv (biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_INTELLECTUAL));

	return ((gint) ((result_physical + result_emotional + result_intellectual) / 3));
}

gint
biorhythm_math_bioday_graphic_total (gint x, gint days_of_life, gint half_height, gint day_pix)
{
	gint result_physical;
	gint result_emotional;
	gint result_intellectual;

	result_physical = biorhythm_math_setpositivgraphic (biorhythm_math_bioday_graphic (x, days_of_life, BIORHYTHM_DAYS_PHYSICAL, half_height, day_pix), half_height);
	result_emotional = biorhythm_math_setpositivgraphic (biorhythm_math_bioday_graphic (x, days_of_life, BIORHYTHM_DAYS_EMOTIONAL, half_height, day_pix), half_height);
	result_intellectual = biorhythm_math_setpositivgraphic (biorhythm_math_bioday_graphic (x, days_of_life, BIORHYTHM_DAYS_INTELLECTUAL, half_height, day_pix), half_height);

	return ((gint) ((result_physical + result_emotional + result_intellectual) / 3));
}

glong
biorhythm_math_daysto (gint day, gint month, gint year)
{
	gint result = day;

	month--;
	// Month
	while (month)
	{
		result += g_date_get_days_in_month (month, year);
		month--;
	}

	year--;
	// Years
	while (year)
	{
		result += DAYS_IN_A_REGULAR_YEAR;

		if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
		{
			result += LEAP_YEAR;
		}

		year--;
	}

	return (result);
}

gint
biorhythm_math_daysoflife (struct bio_date date_active, struct bio_date date_birthday)
{
	return (gint) (biorhythm_math_daysto (date_active.day, date_active.month, date_active.year) - biorhythm_math_daysto (date_birthday.day, date_birthday.month, date_birthday.year));
}

gint
biorhythm_math_monthdayoffset (struct bio_date date_active, struct bio_date date_birthday)
{
	return (gint) (biorhythm_math_daysto (1, date_active.month, date_active.year) - biorhythm_math_daysto (date_birthday.day, date_birthday.month, date_birthday.year));
}

/* ex:set ts=4 noet: */
