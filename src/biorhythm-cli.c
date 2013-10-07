/* -*-coding: utf-8;-*- */

/* biorhythm-cli.c
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

#include "biorhythm-cli.h"

struct _BiorhythmCliPrivate
{
	struct bio_date birthday;
	struct bio_date active_date;
};

#define BIORHYTHM_CLI_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), BIORHYTHM_TYPE_CLI, BiorhythmCliPrivate));

G_DEFINE_TYPE (BiorhythmCli, biorhythm_cli, G_TYPE_OBJECT)

/****************************************
 *                 Class                *
 ****************************************/

static void
biorhythm_cli_class_init (BiorhythmCliClass *klass)
{
	g_type_class_add_private (klass, sizeof (BiorhythmCliPrivate));
}

static void
biorhythm_cli_set_current_date (struct bio_date *date)
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
biorhythm_cli_init (BiorhythmCli *cli)
{
	BiorhythmCliPrivate *priv;

	cli->priv = priv = BIORHYTHM_CLI_GET_PRIVATE (cli);

	biorhythm_cli_set_current_date (&priv->active_date);
	biorhythm_cli_set_current_date (&priv->birthday);
}

BiorhythmCli*
biorhythm_cli_new ()
{
	return g_object_new (BIORHYTHM_TYPE_CLI, NULL);
}

/****************************************
 *           Private Functions          *
 ****************************************/

void
biorhythm_cli_output (BiorhythmCli *cli)
{
	gint days_of_life = biorhythm_math_daysoflife (cli->priv->active_date, cli->priv->birthday);

	g_print (_("Age in days: %i\n"), days_of_life);
	g_print (_("Physical: %d\n"), biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_PHYSICAL));
	g_print (_("Emotional: %d\n"), biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_EMOTIONAL));
	g_print (_("Intellectual: %d\n"), biorhythm_math_bioday (days_of_life, BIORHYTHM_DAYS_INTELLECTUAL));
	g_print (_("Total: %d\n"), biorhythm_math_bioday_total (days_of_life));
}

/****************************************
 *             Private API              *
 ****************************************/

void
biorhythm_cli_set_birthday (BiorhythmCli *cli, guint day, guint month, guint year)
{
	g_return_if_fail (BIORHYTHM_IS_CLI (cli));

	cli->priv->birthday.day = day;
	cli->priv->birthday.month = month;
	cli->priv->birthday.year = year;
}

void
biorhythm_cli_set_active_date (BiorhythmCli *cli, guint day, guint month, guint year)
{
	g_return_if_fail (BIORHYTHM_IS_CLI (cli));

	cli->priv->active_date.day = day;
	cli->priv->active_date.month = month;
	cli->priv->active_date.year = year;
}

/* ex:set ts=4 noet: */
