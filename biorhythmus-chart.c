/* -*-coding: utf-8;-*- */
/* biorhythmus-chart.c
   This file is part of biorhythmus

   Copyright (C) 2003-2013 by Gabriel Mainberger

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2 of the License,
   or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA

*/

#define BIORHYTHMUS_CHART_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BIORHYTHMUS_TYPE_CHART, BiorhythmusChartPrivate))

struct _BiorhythmusChartPrivate
{
	int hsize;
};

static void
biorhythmus_chart_class_init (BiorhythmusChartClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (Bio▣ythmusChartPrivate));
}

static void
biorhythmus_chart_init (MamanBar *self)
    {
      MamanBarPrivate *priv;

        self->priv = priv = MAMAN_BAR_GET_PRIVATE (self);

          priv->hsize = 42;
          }

/* ex:set ts=4 noet: */
