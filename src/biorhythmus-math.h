/* -*-coding: utf-8;-*- */
/* biorhythmus-math.h
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

#ifndef __BIORHYTHMUS_MATH_H__
#define __BIORHYTHMUS_MATH_H__

#define BIORHYTHMUS_DAYS_PHYSICAL		23
#define BIORHYTHMUS_DAYS_EMOTIONAL		28
#define BIORHYTHMUS_DAYS_INTELLECTUAL	33
#define BIORHYTHMUS_DAYS_TOTAL			0

struct bio_date
{
	guint day;
	guint month;
	guint year;
};

gint biorhythmus_math_bioday (gint days_of_life, gint bio_cycle_days);
gint biorhythmus_math_bioday_graphic (gint x, gint days_of_life, gint bio_cycle_days, gint half_height, gint day_pix);
gint biorhythmus_math_setpositiv (gint i);
gint biorhythmus_math_setpositivgraphic (gint i, gint half_height);
gint biorhythmus_math_bioday_total (gint days_of_life);
gint biorhythmus_math_bioday_graphic_total (gint x, gint days_of_life, gint half_height, gint day_pix);
glong biorhythmus_math_daysto (gint day, gint month, gint year);
gint biorhythmus_math_daysoflife (struct bio_date date_active, struct bio_date date_birthday);

#endif /* __BIORHYTHMUS_MATH_H__ */

/* ex:set ts=4 noet: */
