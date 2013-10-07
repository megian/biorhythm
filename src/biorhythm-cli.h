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

#include <math.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include "biorhythm-math.h"

G_BEGIN_DECLS
	
#define BIORHYTHM_TYPE_CLI		(biorhythm_cli_get_type ())
#define BIORHYTHM_CLI(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), BIORHYTHM_TYPE_CLI, BiorhythmCli))
#define BIORHYTHM_IS_CLI(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), BIORHYTHM_TYPE_CLI))
#define BIORHYTHM_CLI_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), BIORHYTHM_TYPE_CLI, BiorhythmCliClass))
#define BIORHYTHM_IS_CLI_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), BIORHYTHM_TYPE_CLI))
#define BIORHYTHM_CLI_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), BIORHYTHM_TYPE_CLI, BiorhythmCliClass))

typedef struct _BiorhythmCli        	BiorhythmCli;
typedef struct _BiorhythmCliClass   	BiorhythmCliClass;
typedef struct _BiorhythmCliPrivate	BiorhythmCliPrivate;

struct _BiorhythmCli
{
	GObject parent_instance;

	BiorhythmCliPrivate *priv;
};

struct _BiorhythmCliClass
{
	GObjectClass parent_class;
};

/* used by BIORHYTHM_TYPE_CLI */
GType biorhythm_cli_get_type (void) G_GNUC_CONST;

BiorhythmCli* biorhythm_cli_new ();

void biorhythm_cli_output (BiorhythmCli *cli);
void biorhythm_cli_set_birthday (BiorhythmCli *cli, guint day, guint month, guint year);
void biorhythm_cli_set_active_date (BiorhythmCli *cli, guint day, guint month, guint year);

G_END_DECLS

#endif /* __BIORHYTHM_CLI_H__ */

/* ex:set ts=4 noet: */
