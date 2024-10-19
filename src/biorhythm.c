/* -*-coding: utf-8;-*- */

/* Biorhythm
 * Copyright (C) 2003-2023, Gabriel Mainberger
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

#include <biorhythm-app.h>

int
main (int argc, char **argv)
{
	BiorhythmApp *app;
	int status;

	app = g_object_new (BIORHYTHM_TYPE_APP,
						"application-id", "org.gtk.biorhythm",
						"flags", G_APPLICATION_DEFAULT_FLAGS,
						NULL);

	status = g_application_run (G_APPLICATION (app), argc, argv);

	g_object_unref (app);

	return status;
}

/* ex:set ts=4 noet: */
