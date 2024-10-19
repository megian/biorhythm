/* -*-coding: utf-8;-*- */

/* biorhythm-cli.h
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

#ifndef __BIORHYTHM_CLI_H__
#define __BIORHYTHM_CLI_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define BIORHYTHM_TYPE_CLI (biorhythm_cli_get_type ())
G_DECLARE_FINAL_TYPE (BiorhythmCli, biorhythm_cli, BIORHYTHM, CLI, GObject)

struct _BiorhythmCliClass
{
	GObjectClass parent_class;
};

BiorhythmCli *biorhythm_cli_new ();

void biorhythm_cli_output (BiorhythmCli *cli);
void biorhythm_cli_set_birthday (BiorhythmCli *cli, guint day, guint month, guint year);
void biorhythm_cli_set_active_date (BiorhythmCli *cli, guint day, guint month, guint year);

G_END_DECLS

#endif /* __BIORHYTHM_CLI_H__ */

/* ex:set ts=4 noet: */
