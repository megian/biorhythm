/* -*-coding: utf-8;-*- */
/* 2003 by Gabriel Mainberger, Licenced under the GPL

#include <gtk/gtk.h>
#include <math.h>
#include <libgnomeui/libgnomeui.h>

//struct tm_
//{
//  int     tm_sec;         /* seconds */
//  int     tm_min;         /* minutes */
//  int     tm_hour;        /* hours */
//  int     tm_mday;        /* day of the month */
//  int     tm_mon;         /* month */
//  int     tm_year;        /* year */
//  int     tm_wday;        /* day of the week */
//  int     tm_yday;        /* day in the year */
//  int     tm_isdst;       /* daylight saving time */
//};

struct tm *tm1;

int bio_birthday_d = 1;
int bio_birthday_m = 1;
int bio_birthday_y = 1980;
int bio_viewdata_d = 1;
int bio_viewdata_m = 1;
int bio_viewdata_y = 2003;

GtkLabel *label1, *label2, *label3, *label4, *label5;
GtkWidget *option_bio23, *option_bio28, *option_bio33, *dates, *about;
GtkStatusbar *status;

gint delete_event(GtkWindow *widget, GdkEvent event, gpointer daten)
{
  return FALSE;
}

void ende(GtkWidget *widget, gpointer daten)
{
  gtk_main_quit();
}

int bio_daysinmonth(int m, int y)
{
  if(m==1||m==3||m==5||m==7||m==8||m==10||m==12) return(31);
  if(m==4||m==6||m==9||m==11) return(30);
  if(m==2)
    if(y%4==0 && (y%100!=0 || y%400==0))
      return(29);
    else
      return(28);
}

int bio_bioday(int daysoflife, int biodays)
{
  int rd;
  double pi2, result;

  rd = (int)(daysoflife - biodays * floor(daysoflife / biodays));
  pi2 = (float)(6.2831853); // 2 * 3.141592654 6.2831853
  result = (float)(sin(rd * pi2 / biodays)); // calculate
  return((int)floor(100*result+0.5));
}

int bio_biodaygraphic(int x, int daysoflife, int biodays, int halfheight, int daypix, int startx, int graphicheight)
{
  double pi2, calcsin, ri;

  pi2 = (float)(6.2831853); // 2 * 3.141592654 6.2831853
  ri = (daysoflife - biodays * floor(daysoflife / biodays));
  calcsin = sin((x - startx + daypix * ri) * pi2 / (daypix * biodays));
  return((int)(floor((halfheight - graphicheight * calcsin)+0.5)));
}

int bio_setpositiv(int i)
{
  if(i<0)
    i *= -1;

  return(i);
}

int bio_setpositivgraphic(int i, int halfheight)
{
  if(i>halfheight)
    return(halfheight-(i- halfheight));
  else
    return(i);
}

int bio_biodaytotal(int daysoflife)
{
  return((int)((bio_setpositiv(bio_bioday(daysoflife, 23))+bio_setpositiv(bio_bioday(daysoflife, 28))+bio_setpositiv(bio_bioday(daysoflife, 33)))/3));
}

int bio_biodaygraphictotal(int x, int daysoflife, int halfheight, int daypix, int startx, int graphicheight)
{
  return((int)((bio_setpositivgraphic(bio_biodaygraphic(x, daysoflife, 23, halfheight, daypix, startx, graphicheight),halfheight)+bio_setpositivgraphic(bio_biodaygraphic(x, daysoflife, 28, halfheight, daypix, startx, graphicheight),halfheight)+bio_setpositivgraphic(bio_biodaygraphic(x, daysoflife, 33, halfheight, daypix, startx, graphicheight),halfheight))/3));
}

long bio_daysto(int d, int m, int y)
{
  long l=d;

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
  int daysoflife;

  daysoflife = (int)(bio_daysto(bio_viewdata_d, bio_viewdata_m, bio_viewdata_y)-bio_daysto(bio_birthday_d, bio_birthday_m, bio_birthday_y));
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

void jaja(GtkMenuItem *eintrag, gpointer user_data)
{
  gtk_widget_show_all(GTK_WIDGET(about));
}

gboolean drawbio(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  int i1, i2, daysoflife, daysinmonth, daypix, halfheight, startx, graphicheight, h100;
  char s[32];
  gchar buf[512];
  GdkColor color;
  GdkGC *gc;
  time_t d;

  d = gnome_date_edit_get_time((GnomeDateEdit*)dates);
  tm1 = localtime(&d);

  bio_viewdata_d = tm1->tm_mday;
  bio_viewdata_m = tm1->tm_mon+1;
  bio_viewdata_y = tm1->tm_year+1900;

  gc = gdk_gc_new(widget->window);

  // Set Parameter
  daysinmonth = bio_daysinmonth(bio_viewdata_m, bio_viewdata_y);
  daysoflife = (int)(bio_daysto(1, bio_viewdata_m, bio_viewdata_y)-bio_daysto(bio_birthday_d, bio_birthday_m, bio_birthday_y));
  daypix = (widget->allocation.width - 15) / daysinmonth;
  halfheight = widget->allocation.height/2;
  startx = 15; // + (daypix/2)+(daypix/4);
  graphicheight = ((widget->allocation.height-100)/2);
  h100 = (int)(floor(widget->allocation.height/100)+1);

  // Diagram
//  gdk_draw_line(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], 10, 10, 10, widget->allocation.height-10);
  gdk_draw_line(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], startx, halfheight, startx+((daysinmonth-1)*daypix), widget->allocation.height/2);

  for(i1=0;i1<daysinmonth;i1++)
  {
    sprintf(s, "%d", i1+1);
    gdk_draw_string(widget->window, gdk_font_load("-*-helvetica-*-r-normal--*-100-*-*-*-*-iso8859-1"), widget->style->fg_gc[GTK_WIDGET_STATE(widget)], startx+(i1*daypix)-4/*+(daypix/2)+(daypix/4)*/, halfheight+h100+10, s);
    gdk_draw_line(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], startx+(i1*daypix)/*+(daypix/2)+(daypix/4)*/, halfheight-h100, startx+(i1*daypix)/*+(daypix/2)+(daypix/4)*/, halfheight+h100);
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
    gdk_draw_point(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], i2, bio_biodaygraphictotal(i2, daysoflife, halfheight, daypix, startx, graphicheight));
  }
  //0, 0, widget->allocation.width, widget->allocation.height, 0, 64 * 360);

  /*sprintf(s, "Koerper: %d", bio_bioday(daysoflife, 23));
  gtk_label_set_text(label2, s);
  sprintf(s, "Seele:   %d", bio_bioday(daysoflife, 28));
  gtk_label_set_text(label3, s);
  sprintf(s, "Geist:   %d", bio_bioday(daysoflife, 33));
  gtk_label_set_text(label4, s);
  sprintf(s, "Total:   %d", bio_biodaytotal(daysoflife));
  gtk_label_set_text(label5, s);*/

 
  daysoflife = (int)(bio_daysto(bio_viewdata_d, bio_viewdata_m, bio_viewdata_y)-bio_daysto(bio_birthday_d, bio_birthday_m, bio_birthday_y));
  g_sprintf(buf, "Datum: %d.%d.%d / Birthday: %d.%d.%d / Tage: %d / Koerper: %d / Seele: %d / Geist: %d / Total: %d", bio_viewdata_d, bio_viewdata_m, bio_viewdata_y, bio_birthday_d, bio_birthday_m, bio_birthday_y, daysoflife, bio_bioday(daysoflife, 23), bio_bioday(daysoflife, 28), bio_bioday(daysoflife, 33), bio_biodaytotal(daysoflife));
  gtk_statusbar_push(status, 0, buf);

  return TRUE;
}

void datechange(GnomeDateEdit *dateedit, gpointer user_data)
{
  gtk_widget_queue_resize(user_data);
}

/*gboolean editchange(GtkEditable *editable, gpointer user_data)
{
  gchar *s;
  int d = bio_viewdata_d;
  int m = bio_viewdata_m;
  int y = bio_viewdata_y;
  //gchar buf[512];

  // year
  s = gtk_editable_get_chars(editable, 4, 8);
  if(strlen(s))
    y = atoi(s);
  g_free(s);

  if((y<=9999) && (y>0))
    bio_viewdata_y = y;

  // month
  s = gtk_editable_get_chars(editable, 2, 4);
  if(strlen(s))
    m = atoi(s);
  g_free(s);

  if((m<=31) && (m>0))
    bio_viewdata_m = m;

  // day
  s = gtk_editable_get_chars(editable, 0, 2);
  if(strlen(s))
    d = atoi(s);
  g_free(s);

  if((d<=bio_daysinmonth(bio_viewdata_m, bio_viewdata_y)) && (d>0))
    bio_viewdata_d = d;

  //g_sprintf(buf, "%d.%d.%d", bio_viewdata_d, bio_viewdata_m, bio_viewdata_y);
  //gtk_statusbar_push(status, 0, buf);
*/
  /*if(strlen(s))
    i = atoi(s);
  g_free(s);

  if((i>12) || (i<1))
    i = bio_viewdata_m;

  bio_viewdata_m = i;i*/

  //gtk_widget_queue_resize(user_data);

  //gtk_signal_emit_by_name(user_data, "expose_event");
  /*s = gtk_editable_get_chars(editable, 2, 4);
  bio_viewdata_m = atoi(s);
  g_free(s);*/

  /*s = gtk_editable_get_chars(editable, 4, 7);
  bio_viewdata_y = atoi(s);
  g_free(s);*/
//}

int main(int argc, char **argv)
{
  GtkWindow *fenster;
  GtkAccelGroup *kuerzel;
  GtkMenuBar *menues;
  GtkWidget *datei, *options, *hilfe, *datei_schliessen, *console, *hilfe_inhalt, *hilfe_info;
  GtkMenu *dateimenue, *optionmenu, *hilfemenue;
  GtkToolbar *werkzeug;
  GtkWidget *map;
  GtkVBox *vbox, *vbox2;
  GtkHBox *hbox;
  GtkWidget *edit;
  const gchar *authors[] = {"Gabriel Mainberger <gabisoft@freesurf.ch>", NULL};

  gtk_init(&argc, &argv);

  fenster = g_object_new(GTK_TYPE_WINDOW, "title", "Biorhythmus", "default-width", 300, "default-height", 300, NULL);
  g_signal_connect(fenster, "delete-event", G_CALLBACK(delete_event), NULL);
  g_signal_connect(fenster, "destroy", G_CALLBACK(ende), NULL);

  vbox = g_object_new(GTK_TYPE_VBOX, NULL);

  map = gtk_drawing_area_new();

  kuerzel = g_object_new(GTK_TYPE_ACCEL_GROUP, NULL);
  gtk_window_add_accel_group(GTK_WINDOW(fenster), kuerzel);

  menues = g_object_new(GTK_TYPE_MENU_BAR, NULL);
//  werkzeug = g_object_new(GTK_TYPE_TOOLBAR, NULL);
  status = g_object_new(GTK_TYPE_STATUSBAR, NULL);
  gtk_statusbar_push(status, 0, "Herzlich Willkommen!");

  /* Menu -> Datei -> Datei schliessen */
  datei_schliessen = gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE, kuerzel);
  g_signal_connect(datei_schliessen, "activate", G_CALLBACK(ende), NULL);

  /* Menu -> Datei */
  dateimenue = g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(dateimenue), datei_schliessen);

  datei = gtk_menu_item_new_with_mnemonic("_Datei");
  gtk_menu_shell_append(GTK_MENU_SHELL(menues), datei);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(datei), GTK_WIDGET(dateimenue));

  /* Menu -> Optionen -> ... */
  option_bio23 = gtk_check_menu_item_new_with_mnemonic("_Koerper");
  gtk_check_menu_item_set_active((GtkCheckMenuItem*)option_bio23, TRUE);
  g_signal_connect(option_bio23, "activate", G_CALLBACK(refreshbiodraw), map);

  option_bio28 = gtk_check_menu_item_new_with_mnemonic("_Seele");
  gtk_check_menu_item_set_active((GtkCheckMenuItem*)option_bio28, TRUE);
  g_signal_connect(option_bio28, "activate", G_CALLBACK(refreshbiodraw), map);

  option_bio33 = gtk_check_menu_item_new_with_mnemonic("_Geist");
  gtk_check_menu_item_set_active((GtkCheckMenuItem*)option_bio33, TRUE);
  g_signal_connect(option_bio33, "activate", G_CALLBACK(refreshbiodraw), map);

  console = gtk_menu_item_new_with_mnemonic("_Konsole");
  g_signal_connect(console, "activate", G_CALLBACK(consolebio), NULL);

  /* Menu -> Optionen */
  optionmenu = g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionmenu), option_bio23);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionmenu), option_bio28);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionmenu), option_bio33);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionmenu), gtk_separator_menu_item_new());
  gtk_menu_shell_append(GTK_MENU_SHELL(optionmenu), console);

  options = gtk_menu_item_new_with_mnemonic("_Optionen");
  gtk_menu_shell_append(GTK_MENU_SHELL(menues), options);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(options), GTK_WIDGET(optionmenu));

  /* Menu -> Hilfe -> .. */
  hilfe_inhalt = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, kuerzel);
  hilfe_info = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, kuerzel);

  hilfemenue = g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(hilfemenue), hilfe_inhalt);
  gtk_menu_shell_append(GTK_MENU_SHELL(hilfemenue), hilfe_info);

  about = gnome_about_new("Biorhythmus", "0.0.1", "Copyright (c) 2003 Gabriel Mainberger", "Funny but useless ;-)", authors, NULL, NULL, NULL);
  g_signal_connect(hilfe_info, "activate", G_CALLBACK(jaja), NULL);

  hilfe = gtk_menu_item_new_with_mnemonic("_Hilfe");
  gtk_menu_shell_append(GTK_MENU_SHELL(menues), hilfe);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(hilfe), GTK_WIDGET(hilfemenue));

  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(menues), FALSE, TRUE, 0);

  /*gtk_toolbar_insert_stock(werkzeug, GTK_STOCK_CUT, "Ausschneiden", "...", G_CALLBACK(ausschneiden), status, -1);
  gtk_toolbar_insert_stock(werkzeug, GTK_STOCK_COPY, "Kopieren", "...-", G_CALLBACK(kopieren), NULL, -1);
  gtk_toolbar_insert_stock(werkzeug, GTK_STOCK_PASTE, "Einfuegen", "", G_CALLBACK(einfuegen), NULL, -1);
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(werkzeug), FALSE, TRUE, 0);*/

  g_signal_connect(G_OBJECT(map), "expose_event", G_CALLBACK(drawbio), NULL);

  dates = gnome_date_edit_new_flags((time_t)0, 1);
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(dates), FALSE, TRUE, 0);
  g_signal_connect(G_OBJECT(dates), "date-changed", G_CALLBACK(datechange), map);

  //edit = g_object_new(GTK_TYPE_ENTRY, NULL);
  //g_signal_connect(G_OBJECT(edit), "changed", G_CALLBACK(editchange), map);
  //gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(edit), FALSE, TRUE, 0);

  hbox = g_object_new(GTK_TYPE_HBOX, NULL);
  vbox2 = g_object_new(GTK_TYPE_VBOX, NULL);

  label1 = g_object_new(GTK_TYPE_LABEL, "label", "Biorhythmus fuer Gabriel", NULL);
  label2 = g_object_new(GTK_TYPE_LABEL, "label", "Koerper: 100", NULL);
  label3 = g_object_new(GTK_TYPE_LABEL, "label", "Seele:   100", NULL);
  label4 = g_object_new(GTK_TYPE_LABEL, "label", "Geist:   100", NULL);
  label5 = g_object_new(GTK_TYPE_LABEL, "label", "Total:   100", NULL);

  gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(label1), FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(label2), FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(label3), FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(label4), FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(label5), FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(hbox), GTK_WIDGET(map));
  //gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(vbox2), FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(hbox));
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(status), FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(fenster), GTK_WIDGET(vbox));
  gtk_widget_show_all(GTK_WIDGET(fenster));
  gtk_main();
  return(0);
}
