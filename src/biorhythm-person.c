/* -*-coding: utf-8;-*- */

/* biorhythm-person.c
 * This file is part of Biorhythm
 * Copyright (C) 2024, Gabriel Mainberger
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

#include "biorhythm-person.h"

G_DEFINE_TYPE (BiorhythmPerson, biorhythm_person, G_TYPE_OBJECT)

enum
{
	PROP_NAME = 1,
	PROP_BIRTHDAY = 2,
	N_PROPERTIES
};

static void
biorhythm_person_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	BiorhythmPerson *self = (BiorhythmPerson *) object;

	switch (prop_id)
	{
		case PROP_NAME:
			g_value_set_string (value, self->name);
			break;
		case PROP_BIRTHDAY:
			g_value_set_string (value, self->birthday);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
biorhythm_person_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	BiorhythmPerson *self = (BiorhythmPerson *) object;

	switch (prop_id)
	{
		case PROP_NAME:
			g_free (self->name);
			self->name = g_value_dup_string (value);
			break;
		case PROP_BIRTHDAY:
			g_free (self->birthday);
			self->birthday = g_value_dup_string (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
biorhythm_person_finalize (GObject *object)
{
	BiorhythmPerson *self = BIORHYTHM_PERSON (object);

	g_free (self->name);
	g_free (self->birthday);
	G_OBJECT_CLASS (biorhythm_person_parent_class)->finalize (object);
}

static void
biorhythm_person_init (BiorhythmPerson *item)
{
	item->name = NULL;
	item->birthday = NULL;
}

static GParamSpec *obj_properties[N_PROPERTIES] = {
	NULL,
};

static void
biorhythm_person_class_init (BiorhythmPersonClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);

	gobject_class->get_property = biorhythm_person_get_property;
	gobject_class->set_property = biorhythm_person_set_property;

	obj_properties[PROP_NAME] = g_param_spec_string (
		"name", "Name", "first column", NULL, G_PARAM_READWRITE);

	obj_properties[PROP_BIRTHDAY] = g_param_spec_string (
		"birthday", "Birthday", "second column", NULL, G_PARAM_READWRITE);

	g_object_class_install_properties (gobject_class, N_PROPERTIES,
									   obj_properties);
}

BiorhythmPerson *
biorhythm_person_new (const char *name, const char *birthday)
{
	BiorhythmPerson *person = g_object_new (BIORHYTHM_TYPE_PERSON, NULL);
	person->name = g_strdup (name);
	person->birthday = g_strdup (birthday);
	return person;
}

void
biorhythm_set_name (BiorhythmPerson *item, const char *name)
{
	g_free (item->name);
	item->name = g_strdup (name);
}

void
biorhythm_set_birthday (BiorhythmPerson *item, const char *birthday)
{
	g_free (item->birthday);
	item->birthday = g_strdup (birthday);
}

/* ex:set ts=4 noet: */
