/* -*-coding: utf-8;-*- */
/* biorhythmus-math.c
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

#include <glib.h>
#include <math.h>
#include "biorhythmus-math.h"

gint
bio_bioday (gint days_of_life, gint bio_cycle_days)
{
	gint rd;
	gdouble pi2, result;

	pi2 = (gfloat)(6.2831853); // 2 * (PI) 3.141592654 = 6.2831853
	rd = (gint)(days_of_life - (bio_cycle_days * floor(days_of_life / bio_cycle_days)));
	result = (gfloat)(sin(rd * pi2 / bio_cycle_days)); // calculate
	return((gint)floor(100 * result + 0.5));
}

gint
bio_bioday_graphic (gint x, gint days_of_life, gint bio_cycle_days, gint half_height, gint day_pix, gint margin)
{
	gdouble pi2, calcsin, ri;

	pi2 = (gfloat)(6.2831853); // 2 * (PI) 3.141592654 = 6.2831853
	ri = (days_of_life - (bio_cycle_days * floor(days_of_life / bio_cycle_days)));
	calcsin = sin((x - margin + (day_pix * ri)) * pi2 / (day_pix * bio_cycle_days));
	return((gint)(floor(half_height - ((half_height-margin) * calcsin)) + 0.5));
}

gint
bio_setpositiv (gint i)
{
	if (i < 0)
		return(i * -1);
	else
		return(i);
}

gint
bio_setpositivgraphic (gint i, gint half_height)
{
	if (i > half_height)
		return (half_height - (i - half_height));
	else
		return (i);
}

gint
bio_biodaytotal (gint days_of_life)
{
	gint result_physical, result_emotional, result_intellectual;

	result_physical = bio_setpositiv (bio_bioday (days_of_life, 23));
	result_emotional = bio_setpositiv (bio_bioday (days_of_life, 28));
	result_intellectual = bio_setpositiv (bio_bioday (days_of_life, 33));

	return((gint)((result_physical + result_emotional + result_intellectual) / 3));
}

gint
bio_bioday_graphic_total (gint x, gint days_of_life, gint half_height, gint day_pix, gint margin)
{
	gint result_physical, result_emotional, result_intellectual;
	
	result_physical = bio_setpositivgraphic(bio_bioday_graphic (x, days_of_life, 23, half_height, day_pix, margin), half_height);
	result_emotional = bio_setpositivgraphic(bio_bioday_graphic (x, days_of_life, 28, half_height, day_pix, margin), half_height);
	result_intellectual = bio_setpositivgraphic(bio_bioday_graphic (x, days_of_life, 33, half_height, day_pix, margin), half_height);

	return((gint)((result_physical + result_emotional + result_intellectual) / 3));
}

glong
bio_daysto (gint day, gint month, gint year)
{
	gint result=day;

	month--;
	// Month
	while (month)
	{
 		result += g_date_get_days_in_month(month, year);
		month--;
	}

	year--;
	// Years
	while (year)
	{
		if(year%4 == 0 && ( year%100 != 0 || year%400 == 0))
			result += 366;
		else
			result += 365;

		year--;
	}

	return(result);
}

gint
bio_daysoflife (struct bio_date date_selection, struct bio_date date_birthday)
{
	return (gint)(bio_daysto(date_selection.day, date_selection.month, date_selection.year)-bio_daysto(date_birthday.day, date_birthday.month, date_birthday.year));
}


/* ex:set ts=4 noet: */
