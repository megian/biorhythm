/* -*-coding: utf-8;-*- */

/* biorhythmus-fileview.h
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

#ifndef __BIORHYTHMUS_FILE_VIEW_H__
#define __BIORHYTHMUS_FILE_VIEW_H__

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS
	
#define BIORHYTHMUS_TYPE_FILE_VIEW				(biorhythmus_file_view_get_type ())
#define BIORHYTHMUS_FILE_VIEW(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), BIORHYTHMUS_TYPE_FILE_VIEW, BiorhythmusFileView))
#define BIORHYTHMUS_IS_FILE_VIEW(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), BIORHYTHMUS_TYPE_FILE_VIEW))
#define BIORHYTHMUS_FILE_VIEW_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), BIORHYTHMUS_TYPE_FILE_VIEW, BiorhythmusFileViewClass))
#define BIORHYTHMUS_IS_FILE_VIEW_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), BIORHYTHMUS_TYPE_FILE_VIEW))
#define BIORHYTHMUS_FILE_VIEW_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), BIORHYTHMUS_TYPE_FILE_VIEW, BiorhythmusFileViewClass))

typedef struct _BiorhythmusFileView			BiorhythmusFileView;
typedef struct _BiorhythmusFileViewClass	BiorhythmusFileViewClass;
typedef struct _BiorhythmusFileViewPrivate	BiorhythmusFileViewPrivate;

struct _BiorhythmusFileView
{
	GtkTreeView parent_instance;

	BiorhythmusFileViewPrivate *priv;
};

struct _BiorhythmusFileViewClass
{
	GtkTreeViewClass parent_class;

	void (* date_changed) (BiorhythmusFileView *self);
	void (* name_changed) (BiorhythmusFileView *self);
};

/* used by BIORHYTHMUS_TYPE_FILE_VIEW */
GType biorhythmus_file_view_get_type (void) G_GNUC_CONST;

GtkWidget* biorhythmus_file_view_new ();

gchar* biorhythmus_file_view_get_name (BiorhythmusFileView *file_view);
gboolean biorhythmus_file_view_new_file (BiorhythmusFileView *file_view);
gboolean biorhythmus_file_view_load_from_file (BiorhythmusFileView *file_view, gchar *filename);
gboolean biorhythmus_file_view_save_to_file (BiorhythmusFileView *file_view);
gboolean biorhythmus_file_view_save_to_new_file (BiorhythmusFileView *file_view, gchar *filename);

G_END_DECLS

#endif /* __BIORHYTHMUS_FILE_VIEW_H__ */

/* ex:set ts=4 noet: */
