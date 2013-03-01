/* -*-coding: utf-8;-*- */
/* Biorhythmus
   Copyright (C) 2003 by Gabriel Mainberger

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

#include <gtk/gtk.h>
#include <math.h>

struct bio_date
{
  guint day;
  guint month;
  guint year;
};

struct bio_date bio_birthday;
struct bio_date bio_viewdata;

GtkWidget *option_bio23, *option_bio28, *option_bio33, *option_total, *dates, *biodates, *map;
GtkStatusbar *status;
GtkWindow *dialog;

gint delete_event(GtkWindow *widget, GdkEvent event, gpointer daten)
{
  return FALSE;
}

void ende(GtkWidget *widget, gpointer daten)
{
  gtk_main_quit();
}

gint bio_daysinmonth(gint m, gint y)
{
  if(m==1||m==3||m==5||m==7||m==8||m==10||m==12) return(31);
  if(m==4||m==6||m==9||m==11) return(30);
  if(m==2)
    if(y%4==0 && (y%100!=0 || y%400==0))
      return(29);
    else
      return(28);
}

gint bio_bioday(gint daysoflife, gint biodays)
{
  gint rd;
  gdouble pi2, result;

  rd = (gint)(daysoflife - biodays * floor(daysoflife / biodays));
  pi2 = (gfloat)(6.2831853); // 2 * 3.141592654 6.2831853
  result = (gfloat)(sin(rd * pi2 / biodays)); // calculate
  return((gint)floor(100*result+0.5));
}

gint bio_biodaygraphic(gint x, gint daysoflife, gint biodays, gint halfheight, gint daypix, gint startx, gint graphicheight)
{
  gdouble pi2, calcsin, ri;

  pi2 = (gfloat)(6.2831853); // 2 * 3.141592654 6.2831853
  ri = (daysoflife - biodays * floor(daysoflife / biodays));
  calcsin = sin((x - startx + daypix * ri) * pi2 / (daypix * biodays));
  return((gint)(floor((halfheight - graphicheight * calcsin)+0.5)));
}

gint bio_setpositiv(gint i)
{
  if(i<0)
    i *= -1;

  return(i);
}

gint bio_setpositivgraphic(gint i, gint halfheight)
{
  if(i>halfheight)
    return(halfheight-(i-halfheight));
  else
    return(i);
}

gint bio_biodaytotal(gint daysoflife)
{
  return((gint)((bio_setpositiv(bio_bioday(daysoflife, 23))+bio_setpositiv(bio_bioday(daysoflife, 28))+bio_setpositiv(bio_bioday(daysoflife, 33)))/3));
}

gint bio_biodaygraphictotal(gint x, gint daysoflife, gint halfheight, gint daypix, gint startx, gint graphicheight)
{
  return((gint)((bio_setpositivgraphic(bio_biodaygraphic(x, daysoflife, 23, halfheight, daypix, startx, graphicheight),halfheight)+bio_setpositivgraphic(bio_biodaygraphic(x, daysoflife, 28, halfheight, daypix, startx, graphicheight),halfheight)+bio_setpositivgraphic(bio_biodaygraphic(x, daysoflife, 33, halfheight, daypix, startx, graphicheight),halfheight))/3));
}

glong bio_daysto(gint d, gint m, gint y)
{
  glong l=d;

  m--;

  // Monate
  while(m)
  {
    l += bio_daysinmonth(m, y);
    m--;
  }

  y--;
  // Jahre
  while(y)
  {
    if(y%4==0 && (y%100!=0 || y%400==0))
      l+=366;
    else
      l+=365;

    y--;
  }

  return(l);
}

gint bio_daysoflife(struct bio_date date_selection, struct bio_date date_birthday)
{
  return (gint)(bio_daysto(date_selection.day, date_selection.month, date_selection.year)-bio_daysto(date_birthday.day, date_birthday.month, date_birthday.year));
}

void consolebio(GtkMenuItem *eintrag, gpointer foo)
{
  gint daysoflife;

  daysoflife = bio_daysoflife(bio_viewdata, bio_birthday);
  printf("Ihr Alter:   %d\n\n", daysoflife);

  printf("Koerperlich: %d\n", bio_bioday(daysoflife, 23));
  printf("Seelich:     %d\n", bio_bioday(daysoflife, 28));
  printf("Geistig:     %d\n", bio_bioday(daysoflife, 33));

  printf("\nTotal:       %d\n", bio_biodaytotal(daysoflife));
}

void refreshbiodraw(GtkMenuItem *eintrag, gpointer user_data)
{
  gtk_widget_queue_resize(user_data);
}

void showabout(GtkMenuItem *eintrag, gpointer user_data)
{
  static const gchar *authors[] = {"Gabriel Mainberger <gabisoft@freesurf.ch>", NULL};
  gtk_show_about_dialog (NULL, "authors", authors, "program-name", "Biorhythmus", "title", "Funny but useless :)", "version", "0.0.1", "copyright", "Copyright (c) 2003-2012 Gabriel Mainberger", NULL);
}

void drawbio_curves(cairo_t *cr, double red, double green, double blue, gint offsetx, gint daysinmonth, gint daypix, gint daysoflife, gint cycleday, gint halfheight, gint graphicheight)
{
  gint i=offsetx;

  if((bio_birthday.month==bio_viewdata.month) && (bio_birthday.year==bio_viewdata.year))
    i = offsetx + ((bio_birthday.day-1)*daypix);
  else if(daysoflife<0)
    return;

  cairo_set_source_rgb(cr, red, green, blue);

  // Draw curves
  while(i<=offsetx+((daysinmonth-1)*daypix))
  {
    if(cycleday==1)
      cairo_line_to(cr, i, bio_biodaygraphictotal(i, daysoflife, halfheight, daypix, offsetx, graphicheight));
    else
      cairo_line_to(cr, i, bio_biodaygraphic(i, daysoflife, cycleday, halfheight, daypix, offsetx, graphicheight));

    i++;
  }

  cairo_stroke(cr);
}

gboolean drawbio(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  gint i1, i2, daysoflife, daysinmonth, daypix, fullheight, halfheight, fullwidth, offsetx, graphicheight, h100;
  gchar *s;
  cairo_t *cr;
  GdkWindow *mywindow;

  bio_viewdata.month--;
  gtk_calendar_get_date((GtkCalendar*)dates, &bio_viewdata.year, &bio_viewdata.month, &bio_viewdata.day);
  bio_viewdata.month++;

  mywindow = gtk_widget_get_window(widget);
  cr = gdk_cairo_create(mywindow);

#ifdef GTK2
  fullheight = widget->allocation.height;
  fullwidth = widget->allocation.width;
#else
  fullheight = gtk_widget_get_allocated_height(widget);
  fullwidth = gtk_widget_get_allocated_width(widget);
#endif

  // Set Parameter
  daysinmonth = bio_daysinmonth(bio_viewdata.month, bio_viewdata.year);
  daysoflife = bio_daysoflife(bio_viewdata, bio_birthday);
  daypix = (fullwidth-15) / daysinmonth;
  halfheight = fullheight/2;
  offsetx = 15;
  graphicheight = (fullheight-20)/2;
  h100 = (gint)(floor(fullheight/100)+1);

  // Diagram
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width(cr, 0.5);
  cairo_move_to(cr, offsetx, halfheight);
  cairo_line_to(cr, offsetx+((daysinmonth-1)*daypix), fullheight/2);

  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width (cr, 0.5);
  cairo_move_to(cr, offsetx+(daypix*(bio_viewdata.day-1)), 0);
  cairo_line_to(cr, offsetx+(daypix*(bio_viewdata.day-1)), fullheight);
  cairo_stroke(cr);

  // Day Linie
  for(i1=0;i1<daysinmonth;i1++)
  {
    s = g_strdup_printf("%d", i1+1);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 10);
    cairo_move_to(cr, offsetx+(i1*daypix)-4, halfheight+h100+10);
    cairo_show_text(cr, s);
    g_free(s);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, offsetx+(i1*daypix), halfheight-h100);
    cairo_line_to(cr, offsetx+(i1*daypix), halfheight+h100);
  }
  cairo_stroke(cr);

  // Körper
  if(gtk_check_menu_item_get_active((GtkCheckMenuItem*)option_bio23))
    drawbio_curves(cr, 1, 0, 0, offsetx, daysinmonth, daypix, daysoflife, 23, halfheight, graphicheight);

  // Seele
  if(gtk_check_menu_item_get_active((GtkCheckMenuItem*)option_bio28))
    drawbio_curves(cr, 0, 1, 0, offsetx, daysinmonth, daypix, daysoflife, 28, halfheight, graphicheight);

  // Geist
  if(gtk_check_menu_item_get_active((GtkCheckMenuItem*)option_bio33))
    drawbio_curves(cr, 0, 0, 1, offsetx, daysinmonth, daypix, daysoflife, 33, halfheight, graphicheight);

  if(gtk_check_menu_item_get_active((GtkCheckMenuItem*)option_total))
    drawbio_curves(cr, 0, 0, 0, offsetx, daysinmonth, daypix, daysoflife, 1, halfheight, graphicheight);

  cairo_stroke(cr);
  cairo_destroy(cr);

  s = g_strdup_printf("Datum: %d.%d.%d / Geburtstag: %d.%d.%d / Tage: %d / Koerper: %d / Seele: %d / Geist: %d / Total: %d", bio_viewdata.day, bio_viewdata.month,
  bio_viewdata.year,
  bio_birthday.day, bio_birthday.month, bio_birthday.year, daysoflife, bio_bioday(daysoflife, 23), bio_bioday(daysoflife, 28), bio_bioday(daysoflife, 33), bio_biodaytotal(daysoflife));
  gtk_statusbar_push(status, 0, s);
  g_free(s);

  return TRUE;
}

void datechange(GtkCalendar *calendar, gpointer user_data)
{
  gtk_widget_queue_resize(map);
}

void birthday(GtkCalendar *calendar, gpointer user_data)
{
  bio_birthday.month--;
  gtk_calendar_get_date((GtkCalendar*)biodates, &bio_birthday.year, &bio_birthday.month, &bio_birthday.day);
  bio_birthday.month++;
  gtk_widget_queue_resize(map);
}

void showbiodialog(GtkMenuItem *eintrag, gpointer user_data)
{
  dialog = g_object_new(GTK_TYPE_WINDOW, "title", "Geburtstag", "default-width", 100, "default-height", 100, "destroy-with-parent", TRUE, NULL);
  biodates = gtk_calendar_new();
  gtk_calendar_set_display_options((GtkCalendar*)biodates, GTK_CALENDAR_SHOW_HEADING|GTK_CALENDAR_SHOW_DAY_NAMES);
  gtk_calendar_select_day((GtkCalendar*)biodates, bio_birthday.day);
  gtk_calendar_select_month((GtkCalendar*)biodates, bio_birthday.month-1, bio_birthday.year);
  g_signal_connect(biodates, "day-selected", G_CALLBACK(birthday), NULL);
  g_signal_connect(biodates, "month-changed", G_CALLBACK(birthday), NULL);
  gtk_container_add(GTK_CONTAINER(dialog), GTK_WIDGET(biodates));
  gtk_widget_show_all(GTK_WIDGET(dialog));
}

int main(int argc, char **argv)
{
  GtkWindow *fenster;
  GtkAccelGroup *kuerzel;
  GtkMenuBar *menues;
  GtkWidget *datei, *options, *hilfe, *datei_schliessen, *datei_bio, *console, *hilfe_info;
  GtkMenu *dateimenue, *optionmenu, *hilfemenue;
  GtkVBox *vbox;

  bio_birthday.day=1;
  bio_birthday.month=11;
  bio_birthday.year=2012;

  gtk_init(&argc, &argv);

  fenster = g_object_new(GTK_TYPE_WINDOW, "title", "Biorhythmus", "default-width", 400, "default-height", 400, NULL);
  g_signal_connect(fenster, "delete-event", G_CALLBACK(delete_event), NULL);
  g_signal_connect(fenster, "destroy", G_CALLBACK(ende), NULL);

  /* VBOX */
  vbox = g_object_new(GTK_TYPE_VBOX, NULL);

  /* MAP */
  map = gtk_drawing_area_new();

  /* AccelGroup */
  kuerzel = g_object_new(GTK_TYPE_ACCEL_GROUP, NULL);
  gtk_window_add_accel_group(GTK_WINDOW(fenster), kuerzel);

  menues = g_object_new(GTK_TYPE_MENU_BAR, NULL);
  status = g_object_new(GTK_TYPE_STATUSBAR, NULL);
  gtk_statusbar_push(status, 0, "Herzlich Willkommen!");

  /* menu -> datei -> datei schliessen */
  datei_bio = gtk_menu_item_new_with_mnemonic("_Geburtstag");
  g_signal_connect(datei_bio, "activate", G_CALLBACK(showbiodialog), NULL);
  datei_schliessen = gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE, kuerzel);
  g_signal_connect(datei_schliessen, "activate", G_CALLBACK(ende), NULL);

  /* menu -> datei */
  dateimenue = g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(dateimenue), datei_bio);
  gtk_menu_shell_append(GTK_MENU_SHELL(dateimenue), gtk_separator_menu_item_new());
  gtk_menu_shell_append(GTK_MENU_SHELL(dateimenue), datei_schliessen);

  datei = gtk_menu_item_new_with_mnemonic("_Datei");
  gtk_menu_shell_append(GTK_MENU_SHELL(menues), datei);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(datei), GTK_WIDGET(dateimenue));

  /* menu -> optionen -> ... */
  option_bio23 = gtk_check_menu_item_new_with_mnemonic("_Koerper");
  gtk_check_menu_item_set_active((GtkCheckMenuItem*)option_bio23, TRUE);
  g_signal_connect(option_bio23, "activate", G_CALLBACK(refreshbiodraw), map);

  option_bio28 = gtk_check_menu_item_new_with_mnemonic("_Seele");
  gtk_check_menu_item_set_active((GtkCheckMenuItem*)option_bio28, TRUE);
  g_signal_connect(option_bio28, "activate", G_CALLBACK(refreshbiodraw), map);

  option_bio33 = gtk_check_menu_item_new_with_mnemonic("_Geist");
  gtk_check_menu_item_set_active((GtkCheckMenuItem*)option_bio33, TRUE);
  g_signal_connect(option_bio33, "activate", G_CALLBACK(refreshbiodraw), map);

  option_total = gtk_check_menu_item_new_with_mnemonic("_Total");
  gtk_check_menu_item_set_active((GtkCheckMenuItem*)option_total, TRUE);
  g_signal_connect(option_total, "activate", G_CALLBACK(refreshbiodraw), map);

  console = gtk_menu_item_new_with_mnemonic("_Konsole");
  g_signal_connect(console, "activate", G_CALLBACK(consolebio), NULL);

  /* menu -> optionen */
  optionmenu = g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionmenu), option_bio23);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionmenu), option_bio28);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionmenu), option_bio33);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionmenu), option_total);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionmenu), gtk_separator_menu_item_new());
  gtk_menu_shell_append(GTK_MENU_SHELL(optionmenu), console);

  options = gtk_menu_item_new_with_mnemonic("_Optionen");
  gtk_menu_shell_append(GTK_MENU_SHELL(menues), options);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(options), GTK_WIDGET(optionmenu));

  /* menu -> hilfe -> .. */
  hilfe_info = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, kuerzel);
  hilfemenue = g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(hilfemenue), hilfe_info);

  /* hilfe -> about */
  g_signal_connect(hilfe_info, "activate", G_CALLBACK(showabout), NULL);

  /* hilfe */
  hilfe = gtk_menu_item_new_with_mnemonic("_Hilfe");
  gtk_menu_shell_append(GTK_MENU_SHELL(menues), hilfe);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(hilfe), GTK_WIDGET(hilfemenue));

  /* Add Menubar to vbox */
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(menues), FALSE, TRUE, 0);

  /* Add MAP */
#ifdef GTK2
  g_signal_connect(G_OBJECT(map), "expose_event", G_CALLBACK(drawbio), NULL);
#else
  g_signal_connect(G_OBJECT(map), "draw", G_CALLBACK(drawbio), NULL);
#endif
  gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(map));

  /* Calendar */
  dates = gtk_calendar_new();
  gtk_calendar_set_display_options((GtkCalendar*)dates, GTK_CALENDAR_SHOW_HEADING|GTK_CALENDAR_SHOW_DAY_NAMES);
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(dates), FALSE, TRUE, 0);
  g_signal_connect(G_OBJECT(dates), "day-selected", G_CALLBACK(datechange), NULL);
  g_signal_connect(G_OBJECT(dates), "month-changed", G_CALLBACK(datechange), NULL);

  /* Statusbar */
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(status), FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(fenster), GTK_WIDGET(vbox));
  gtk_widget_show_all(GTK_WIDGET(fenster));
  gtk_main();
  return(0);
}
