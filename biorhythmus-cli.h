/* -*-coding: utf-8;-*- */

/* biorhythmus-cli.h
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

#ifndef __BIORHYTHMUS_CLI_H__
#define __BIORHYTHMUS_CLI_H__

#include <math.h>
#include <glib-object.h>
#include "biorhythmus-math.h"

G_BEGIN_DECLS
	
#define BIORHYTHMUS_TYPE_CLI				(biorhythmus_cli_get_type ())
#define BIORHYTHMUS_CLI(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), BIORHYTHMUS_TYPE_CLI, BiorhythmusCli))
#define BIORHYTHMUS_IS_CLI(obj)				(G_TYPE_CHECK_INSTANCE_TYPE ((obj), BIORHYTHMUS_TYPE_CLI))
#define BIORHYTHMUS_CLI_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), BIORHYTHMUS_TYPE_CLI, BiorhythmusCliClass))
#define BIORHYTHMUS_IS_CLI_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), BIORHYTHMUS_TYPE_CLI))
#define BIORHYTHMUS_CLI_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), BIORHYTHMUS_TYPE_CLI, BiorhythmusCliClass))

typedef struct _BiorhythmusCli        BiorhythmusCli;
typedef struct _BiorhythmusCliClass   BiorhythmusCliClass;
typedef struct _BiorhythmusCliPrivate	BiorhythmusCliPrivate;

struct _BiorhythmusCli
{
	GObject parent_instance;

	BiorhythmusCliPrivate *priv;
};

struct _BiorhythmusCliClass
{
	GObjectClass parent_class;
};

/* used by BIORHYTHMUS_TYPE_CLI */
GType biorhythmus_cli_get_type (void) G_GNUC_CONST;

BiorhythmusCli* biorhythmus_cli_new ();

gboolean biorhythmus_cli_set_birthday (BiorhythmusCli *cli, gint day, gint month, gint year);
gboolean biorhythmus_cli_set_active_date (BiorhythmusCli *cli, gint day, gint month, gint year);

G_END_DECLS

#endif /* __BIORHYTHMUS_CLI_H__ */

/* ex:set ts=4 noet: */
