/* -*-coding: utf-8;-*- */

/* biorhythm-fileview.h
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

#ifndef __BIORHYTHM_FILE_VIEW_H__
#define __BIORHYTHM_FILE_VIEW_H__

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS
	
#define BIORHYTHM_TYPE_FILE_VIEW		(biorhythm_file_view_get_type ())
#define BIORHYTHM_FILE_VIEW(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), BIORHYTHM_TYPE_FILE_VIEW, BiorhythmFileView))
#define BIORHYTHM_IS_FILE_VIEW(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), BIORHYTHM_TYPE_FILE_VIEW))
#define BIORHYTHM_FILE_VIEW_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), BIORHYTHM_TYPE_FILE_VIEW, BiorhythmFileViewClass))
#define BIORHYTHM_IS_FILE_VIEW_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), BIORHYTHM_TYPE_FILE_VIEW))
#define BIORHYTHM_FILE_VIEW_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), BIORHYTHM_TYPE_FILE_VIEW, BiorhythmFileViewClass))

typedef struct _BiorhythmFileView		BiorhythmFileView;
typedef struct _BiorhythmFileViewClass		BiorhythmFileViewClass;
typedef struct _BiorhythmFileViewPrivate	BiorhythmFileViewPrivate;

struct _BiorhythmFileView
{
	GtkTreeView parent_instance;

	BiorhythmFileViewPrivate *priv;
};

struct _BiorhythmFileViewClass
{
	GtkTreeViewClass parent_class;

	void (* date_changed) (BiorhythmFileView *self);
	void (* name_changed) (BiorhythmFileView *self);
};

/* used by BIORHYTHM_TYPE_FILE_VIEW */
GType biorhythm_file_view_get_type (void) G_GNUC_CONST;

GtkWidget* biorhythm_file_view_new ();

void biorhythm_file_view_get_date (BiorhythmFileView *file_view, guint *day, guint *month, guint *year);
gchar* biorhythm_file_view_get_name (BiorhythmFileView *file_view);
gboolean biorhythm_file_view_new_file (BiorhythmFileView *file_view);
gboolean biorhythm_file_view_load_from_file (BiorhythmFileView *file_view, gchar *filename);
gboolean biorhythm_file_view_save_to_file (BiorhythmFileView *file_view);
gboolean biorhythm_file_view_save_to_new_file (BiorhythmFileView *file_view, gchar *filename);

G_END_DECLS

#endif /* __BIORHYTHM_FILE_VIEW_H__ */

/* ex:set ts=4 noet: */
