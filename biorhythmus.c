/* -*-coding: utf-8;-*- */
/* 2003 by Gabriel Mainberger, Licenced under the GPL */

#include <gtk/gtk.h>
#include <math.h>
#include <libgnomeui/libgnomeui.h>

guint bio_birthday_d=1;
guint bio_birthday_m=1;
guint bio_birthday_y=2000;
guint bio_viewdata_d;
guint bio_viewdata_m;
guint bio_viewdata_y;

GtkWidget *option_bio23, *option_bio28, *option_bio33, *option_total, *dates, *about, *biodates, *map;
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

void consolebio(GtkMenuItem *eintrag, gpointer foo)
{
  gint daysoflife;

  daysoflife = (gint)(bio_daysto(bio_viewdata_d, bio_viewdata_m, bio_viewdata_y)-bio_daysto(bio_birthday_d, bio_birthday_m, bio_birthday_y));
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
  gtk_widget_show_all(GTK_WIDGET(about));
}

gboolean drawbio(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  gint i1, i2, daysoflife, daysinmonth, daypix, halfheight, startx, graphicheight, h100;
  gchar *s;
  GdkColor color;
  GdkGC *gc;

  bio_viewdata_m--;
  gtk_calendar_get_date((GtkCalendar*)dates, &bio_viewdata_y, &bio_viewdata_m, &bio_viewdata_d);
  bio_viewdata_m++;

  gc = gdk_gc_new(widget->window);

  // Set Parameter
  daysinmonth = bio_daysinmonth(bio_viewdata_m, bio_viewdata_y);
  daysoflife = (gint)(bio_daysto(1, bio_viewdata_m, bio_viewdata_y)-bio_daysto(bio_birthday_d, bio_birthday_m, bio_birthday_y));
  daypix = (widget->allocation.width-10) / daysinmonth;
  halfheight = widget->allocation.height/2;
  startx = 15;
  graphicheight = ((widget->allocation.height-20)/2);
  h100 = (gint)(floor(widget->allocation.height/100)+1);

  // Diagram
  gdk_draw_line(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], startx, halfheight, startx+((daysinmonth-1)*daypix), widget->allocation.height/2);

  // Day Linie
  gdk_draw_line(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], startx+(daypix*(bio_viewdata_d-1)), 0, startx+(daypix*(bio_viewdata_d-1)), widget->allocation.height);

  for(i1=0;i1<daysinmonth;i1++)
  {
    s = g_strdup_printf("%d", i1+1);
    gdk_draw_string(widget->window, gdk_font_load("-*-helvetica-*-r-normal--*-100-*-*-*-*-iso8859-1"), widget->style->fg_gc[GTK_WIDGET_STATE(widget)], startx+(i1*daypix)-4, halfheight+h100+10, s);
    gdk_draw_line(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], startx+(i1*daypix), halfheight-h100, startx+(i1*daypix), halfheight+h100);
    g_free(s);
  }

  // Draw curves
  for(i2=startx;i2<=startx+((daysinmonth-1)*daypix);i2++)
  {
    if(daysoflife<0)
    {
      if((bio_birthday_m==bio_viewdata_m) && (bio_birthday_y==bio_viewdata_y))
      {
        if(bio_birthday_d>((i2+startx)/daypix))
          continue;
      }
      else
        break;
    }

    // koerper
    if(gtk_check_menu_item_get_active((GtkCheckMenuItem*)option_bio23))
    {
      gdk_color_parse("red", &color);
      gdk_gc_set_rgb_fg_color(gc, &color);
      gdk_draw_point(widget->window, gc, i2, bio_biodaygraphic(i2, daysoflife, 23, halfheight, daypix, startx, graphicheight));
    }

    // seele
    if(gtk_check_menu_item_get_active((GtkCheckMenuItem*)option_bio28))
    {
      gdk_color_parse("green", &color);
      gdk_gc_set_rgb_fg_color(gc, &color);
      gdk_draw_point(widget->window, gc, i2, bio_biodaygraphic(i2, daysoflife, 28, halfheight, daypix, startx, graphicheight));
    }

    // seele
    if(gtk_check_menu_item_get_active((GtkCheckMenuItem*)option_bio33))
    {
      gdk_color_parse("blue", &color);
      gdk_gc_set_rgb_fg_color(gc, &color);
      gdk_draw_point(widget->window, gc, i2, bio_biodaygraphic(i2, daysoflife, 33, halfheight, daypix, startx, graphicheight));
    }

    // total
    if(gtk_check_menu_item_get_active((GtkCheckMenuItem*)option_total))
      gdk_draw_point(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], i2, bio_biodaygraphictotal(i2, daysoflife, halfheight, daypix, startx, graphicheight));
  }

  daysoflife = (gint)(bio_daysto(bio_viewdata_d, bio_viewdata_m, bio_viewdata_y)-bio_daysto(bio_birthday_d, bio_birthday_m, bio_birthday_y));
  s = g_strdup_printf("Datum: %d.%d.%d / Geburtstag: %d.%d.%d / Tage: %d / Koerper: %d / Seele: %d / Geist: %d / Total: %d", bio_viewdata_d, bio_viewdata_m, bio_viewdata_y, bio_birthday_d, bio_birthday_m, bio_birthday_y, daysoflife, bio_bioday(daysoflife, 23), bio_bioday(daysoflife, 28), bio_bioday(daysoflife, 33), bio_biodaytotal(daysoflife));
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
  bio_birthday_m--;
  gtk_calendar_get_date((GtkCalendar*)biodates, &bio_birthday_y, &bio_birthday_m, &bio_birthday_d);
  bio_birthday_m++;
  gtk_widget_queue_resize(map);
}

void showbiodialog(GtkMenuItem *eintrag, gpointer user_data)
{
  dialog = g_object_new(GTK_TYPE_WINDOW, "title", "Geburtstag", "default-width", 100, "default-height", 100, NULL);
  biodates = gtk_calendar_new();
  gtk_calendar_display_options((GtkCalendar*)biodates, GTK_CALENDAR_WEEK_START_MONDAY|GTK_CALENDAR_SHOW_HEADING|GTK_CALENDAR_SHOW_DAY_NAMES);
  gtk_calendar_select_day((GtkCalendar*)biodates, bio_birthday_d);
  gtk_calendar_select_month((GtkCalendar*)biodates, bio_birthday_m-1, bio_birthday_y);
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
  const gchar *authors[] = {"Gabriel Mainberger <gabisoft@freesurf.ch>", NULL};

  gtk_init(&argc, &argv);

  fenster = g_object_new(GTK_TYPE_WINDOW, "title", "Biorhythmus", "default-width", 300, "default-height", 300, NULL);
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
  about = gnome_about_new("Biorhythmus", "0.0.1", "Copyright (c) 2003 Gabriel Mainberger", "Funny but useless ;-)", authors, NULL, NULL, NULL);
  g_signal_connect(hilfe_info, "activate", G_CALLBACK(showabout), NULL);

  /* hilfe */
  hilfe = gtk_menu_item_new_with_mnemonic("_Hilfe");
  gtk_menu_shell_append(GTK_MENU_SHELL(menues), hilfe);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(hilfe), GTK_WIDGET(hilfemenue));

  /* Add Menubar to vbox */
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(menues), FALSE, TRUE, 0);

  /* Add MAP */
  g_signal_connect(G_OBJECT(map), "expose_event", G_CALLBACK(drawbio), NULL);
  gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(map));

  /* Calendar */
  dates = gtk_calendar_new();
  gtk_calendar_display_options((GtkCalendar*)dates, GTK_CALENDAR_WEEK_START_MONDAY|GTK_CALENDAR_SHOW_HEADING|GTK_CALENDAR_SHOW_DAY_NAMES);
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
