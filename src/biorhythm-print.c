/* -*-coding: utf-8;-*- */

/* biorhythm-print.c
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

#include "biorhythm-print.h"

static void
biorhythm_print_draw_page (GtkPrintOperation *operation, GtkPrintContext *context, gint page_nr, gpointer user_data)
{
	cairo_t *cr;
	gdouble p_width, p_height;
	GtkPageSetup *page_setup;
	BiorhythmChart *chart;

	chart = (BiorhythmChart *) user_data;

	page_setup = gtk_print_context_get_page_setup (context);
	p_width =  gtk_page_setup_get_page_width (page_setup, GTK_UNIT_POINTS);
	p_height = gtk_page_setup_get_page_height (page_setup, GTK_UNIT_POINTS);

	cr = gtk_print_context_get_cairo_context (context);

  	biorhythm_chart_draw_cairo (chart, cr, p_height, p_width);
}

GtkPrintOperation *
biorhythm_print_operation_new (BiorhythmChart *chart)
{
	GtkPrintOperation *print;
	GtkPageSetup *page_setup;

	print = gtk_print_operation_new ();
	gtk_print_operation_set_n_pages (print, 1);
	gtk_print_operation_set_job_name (print, "Print Biorhythm");
	gtk_print_operation_set_embed_page_setup (print, FALSE);

	page_setup = gtk_page_setup_new ();
	gtk_page_setup_set_orientation (page_setup, GTK_PAGE_ORIENTATION_LANDSCAPE);
	gtk_print_operation_set_default_page_setup (print, page_setup);

	g_signal_connect (print, "draw_page", G_CALLBACK (biorhythm_print_draw_page), chart);

	return print;
}

/* ex:set ts=4 noet: */
