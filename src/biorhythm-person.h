/* biorhythm-person.h
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

#ifndef __BIORHYTHM_PERSON_H__
#define __BIORHYTHM_PERSON_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define BIORHYTHM_TYPE_PERSON (biorhythm_person_get_type ())
G_DECLARE_FINAL_TYPE (BiorhythmPerson, biorhythm_person, BIORHYTHM, PERSON, GObject)

struct _BiorhythmPerson
{
	GObject parent_instance;
	char *name;
	char *birthday;
};

struct _BiorhythmPersonClass
{
	GObjectClass parent_class;
};

BiorhythmPerson *biorhythm_person_new (const char *name, const char *birthday);
void biorhythm_set_name (BiorhythmPerson *item, const char *name);
void biorhythm_set_birthday (BiorhythmPerson *item, const char *birthday);

G_END_DECLS

#endif /* __BIORHYTHM_PERSON_H__ */

/* ex:set ts=4 noet: */
