/* -*-coding: utf-8;-*- */

/* biorhythmus-cli.c
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

#include "biorhythmus-cli.h"

struct _BiorhythmusCliPrivate
{
	struct bio_date birthday;
	struct bio_date active_date;

	gboolean option_physical;
	gboolean option_emotional;
	gboolean option_intellectual;
	gboolean option_total;
};

#define BIORHYTHMUS_CLI_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), BIORHYTHMUS_TYPE_CLI, BiorhythmusCliPrivate));

G_DEFINE_TYPE (BiorhythmusCli, biorhythmus_cli, G_TYPE_OBJECT)

void
biorhythmus_cli_output (BiorhythmusCli *self)
{
	gint daysoflife=5;

	daysoflife = biorhythmus_math_daysoflife (self->priv->active_date, self->priv->birthday);
	g_print ("Age in days: %i\n\n", daysoflife);

	g_print ("Physical: %d\n", biorhythmus_math_bioday (daysoflife, BIORHYTHMUS_DAYS_PHYSICAL));
	g_print ("Emotional: %d\n", biorhythmus_math_bioday (daysoflife, BIORHYTHMUS_DAYS_EMOTIONAL));
	g_print ("Intellectual: %d\n", biorhythmus_math_bioday (daysoflife, BIORHYTHMUS_DAYS_INTELLECTUAL));

	g_print ("\nTotal: %d\n", biorhythmus_math_bioday_total (daysoflife));
}

void
biorhythmus_cli_set_birthday (BiorhythmusCli *self, gint day, gint month, gint year)
{
	self->priv->birthday.day = day;
	self->priv->birthday.month = month;
	self->priv->birthday.year = year;
}

void
biorhythmus_cli_set_active_date (BiorhythmusCli *self, gint day, gint month, gint year)
{
    self->priv->active_date.day = day;
	self->priv->active_date.month = month;
	self->priv->active_date.year = year;
}

void
biorhythmus_cli_set_option_physical (BiorhythmusCli *self, gboolean state)
{
	self->priv->option_physical = state;
}

gboolean
biorhythmus_cli_get_option_physical (BiorhythmusCli *self)
{
	return self->priv->option_physical;
}

void
biorhythmus_cli_set_option_emotional (BiorhythmusCli *self, gboolean state)
{
	self->priv->option_emotional = state;
}

gboolean
biorhythmus_cli_get_option_emotional (BiorhythmusCli *self)
{
	return self->priv->option_emotional;
}

void
biorhythmus_cli_set_option_intellectual (BiorhythmusCli *self, gboolean state)
{
	self->priv->option_intellectual = state;
}

gboolean
biorhythmus_cli_get_option_intellectual (BiorhythmusCli *self)
{
	return self->priv->option_intellectual;
}

void
biorhythmus_cli_set_option_total (BiorhythmusCli *self, gboolean state)
{
	self->priv->option_total = state;
}

gboolean
biorhythmus_cli_get_option_total (BiorhythmusCli *self)
{
	return self->priv->option_total;
}

static void
biorhythmus_cli_class_init (BiorhythmusCliClass *klass)
{
	g_type_class_add_private (klass, sizeof (BiorhythmusCliPrivate));
}

biorhythmus_cli_set_current_date(struct bio_date *date)
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
biorhythmus_cli_init (BiorhythmusCli *self)
{
	BiorhythmusCliPrivate *priv;

	self->priv = priv = BIORHYTHMUS_CLI_GET_PRIVATE (self);

	priv->option_physical = TRUE;
	priv->option_emotional = TRUE;
	priv->option_intellectual = TRUE;
	priv->option_total = TRUE;

	biorhythmus_cli_set_current_date (&priv->active_date);
	biorhythmus_cli_set_current_date (&priv->birthday);
}

BiorhythmusCli*
biorhythmus_cli_new ()
{
	return g_object_new (BIORHYTHMUS_TYPE_CLI, NULL);
}

/* ex:set ts=4 noet: */
