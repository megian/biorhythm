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
#include <glib/gi18n.h>
#include <json-glib/json-glib.h>
#include <math.h>

struct bio_date
{
  guint day;
  guint month;
  guint year;
};

struct bio_date bio_birthday;
struct bio_date bio_viewdata;

enum
{
  PERSON_VIEW_COLUMN_NAME = 0,
  PERSON_VIEW_COLUMN_BIRTHDAY,
  PERSON_VIEW_COLUMN_NUM_COLS
};

GtkWidget *option_bio23, *option_bio28, *option_bio33, *option_total, *dates, *map;
GtkStatusbar *status;

gint delete_event(GtkWindow *widget, GdkEvent event, gpointer daten)
{
  return FALSE;
}

void bio_gui_close(GtkWidget *widget, gpointer daten)
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

void bio_gui_help_info_dialog(GtkMenuItem *eintrag, gpointer user_data)
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

void bio_read_each_person(JsonArray *array, guint index_, JsonNode *element_node, gpointer user_data)
{
  JsonObject *element;
  JsonNode *name, *birthday;
  GList *list = NULL;
  GList **list_ptr = (GList**)user_data;

  element = json_node_get_object(element_node);
  name = json_object_get_member(element, "Name");
  birthday = json_object_get_member(element, "Birthday");

  list = g_list_append(list, json_node_dup_string(name));
  list = g_list_append(list, json_node_dup_string(birthday));
  *list_ptr = g_list_append(*list_ptr, list);
}

int bio_read_file(GList **list)
{
  JsonParser *parser;
  JsonNode *root, *result;
  JsonObject *element;
  GError *error;
  JsonArray *array;

  parser = json_parser_new();
  error = NULL;
  json_parser_load_from_file(parser, "default.bio", &error);
  if(error)
  {
    g_print("Unable to parse `%s': %s\n", "default.bio", error->message);
    g_error_free(error);
    g_object_unref(parser);
    return 1;
  }

  root = json_parser_get_root(parser);
  element = json_node_get_object(root);
  result = json_object_get_member(element, "Persons");
  array = json_node_get_array(result);
  json_array_foreach_element(array, bio_read_each_person, list);

  g_object_unref(parser);

  return 0;
}

void bio_gui_foreach_person(gpointer data, gpointer user_data)
{
  GtkTreeIter iter;
  gchar *name, *birthday;

  name = g_list_nth_data((GList*)data, PERSON_VIEW_COLUMN_NAME);
  birthday = g_list_nth_data((GList*)data, PERSON_VIEW_COLUMN_BIRTHDAY);
  g_print("%s %s\n", name, birthday);
  gtk_list_store_append((GtkListStore*)user_data, &iter);
  gtk_list_store_set((GtkListStore*)user_data, &iter, PERSON_VIEW_COLUMN_NAME, name, PERSON_VIEW_COLUMN_BIRTHDAY, birthday, -1);
}

void bio_gui_persons_add_empty_row(GtkListStore *list_store)
{
  GtkTreeIter iter;

  gtk_list_store_append(list_store, &iter);
  gtk_list_store_set(list_store, &iter, PERSON_VIEW_COLUMN_NAME, "", PERSON_VIEW_COLUMN_BIRTHDAY, "", -1);
}

void bio_gui_persons_date_changed(GtkTreeView *tree_view, gpointer user_data)
{
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *str;
  GDate *date;

  selection = gtk_tree_view_get_selection(tree_view);
  if(gtk_tree_selection_get_selected(selection, &model, &iter))
  {
     gtk_tree_model_get(model, &iter, PERSON_VIEW_COLUMN_BIRTHDAY, &str, -1);

     date = g_date_new();
     g_date_set_parse(date, str);
     if(g_date_valid(date))
     {
       bio_birthday.day = g_date_get_day(date);
       bio_birthday.month = g_date_get_month(date);
       bio_birthday.year = g_date_get_year(date);
     }

     g_free(str);
     g_date_free(date);
  }

  gtk_widget_queue_resize(map);
}

gboolean bio_gui_persons_check_other_cells_empty(GtkListStore *list_store, GtkTreeIter *iter, gint except_column)
{
  gint i;
  gchar *cell_text;
  gboolean status=FALSE;

  for(i=0;i<PERSON_VIEW_COLUMN_NUM_COLS;i++)
  {
    if(i!=except_column)
    {
      gtk_tree_model_get(GTK_TREE_MODEL(list_store), iter, i, &cell_text, -1);
      if(cell_text!=NULL)
      {
        if(g_strcmp0(cell_text, "")==0)
        {
           status = status || TRUE;
        }
      }
    }
  }
  return(status);
}

void bio_gui_persons_add_row_if_needed(GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
{
  GtkTreeIter iter;
  gchar *cell_text;

  if(gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(list_store), &iter, path_string))
  {
    gtk_tree_model_get(GTK_TREE_MODEL(list_store), &iter, list_store_column, &cell_text, -1);

    if(cell_text!=NULL)
    {
      // Check if the new text is not "" and the old was empty
      if((g_strcmp0(new_text, "")!=0) && (g_strcmp0(cell_text, "")==0))
      {
        if(bio_gui_persons_check_other_cells_empty(list_store, &iter, list_store_column))
        {
          bio_gui_persons_add_empty_row(list_store);
        }
      }
    }
  }
}

gboolean bio_gui_persons_delete_row_if_needed(GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
{
  GtkTreeIter iter;
  gchar *cell_text;

  if(gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(list_store), &iter, path_string))
  {
    gtk_tree_model_get(GTK_TREE_MODEL(list_store), &iter, list_store_column, &cell_text, -1);

    if(cell_text!=NULL)
    {
      // Check if the new text is "" and the old was not empty
      if((g_strcmp0(new_text, "")==0) && (g_strcmp0(cell_text, "")!=0))
      {
         if(bio_gui_persons_check_other_cells_empty(list_store, &iter, list_store_column))
         {
           gtk_list_store_remove(GTK_LIST_STORE(list_store), &iter);
           return TRUE;
         }
      }
    }
  }
  return FALSE;
}

void bio_gui_persons_change_cell_text(GtkListStore *list_store, gchar *path_string, gint list_store_column, gchar *new_text)
{
  GtkTreeIter iter;

  if(gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(list_store), &iter, path_string))
  {
        gtk_list_store_set(GTK_LIST_STORE(list_store), &iter, list_store_column, new_text, -1);
  }
}

void bio_gui_persons_textrenderer_callback_name_edited(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
  bio_gui_persons_add_row_if_needed(user_data, path_string, PERSON_VIEW_COLUMN_NAME, new_text);
  if(bio_gui_persons_delete_row_if_needed(user_data, path_string, PERSON_VIEW_COLUMN_NAME, new_text)==FALSE)
    bio_gui_persons_change_cell_text(user_data, path_string, PERSON_VIEW_COLUMN_NAME, new_text);
}

void bio_gui_persons_textrenderer_callback_birthday_edited_list(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
  bio_gui_persons_add_row_if_needed(user_data, path_string, PERSON_VIEW_COLUMN_NAME, new_text);
  if(bio_gui_persons_delete_row_if_needed(user_data, path_string, PERSON_VIEW_COLUMN_BIRTHDAY, new_text)==FALSE)
    bio_gui_persons_change_cell_text(user_data, path_string, PERSON_VIEW_COLUMN_BIRTHDAY, new_text);
}

void bio_gui_persons_textrenderer_callback_birthday_edited_view(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
  g_signal_emit_by_name(G_OBJECT(user_data), "cursor-changed");
}

void bio_gui_persons_textrenderer_mark_empty_cell_green(GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter)
{
  gchar *str;

  gtk_tree_model_get(tree_model, iter, PERSON_VIEW_COLUMN_NAME, &str, -1);

  // Is cell empty? Mark it light green!
  if(g_strcmp0(str, "")==0)
    g_object_set(cell, "background", "lightgreen", "background-set", TRUE, NULL);
  else
    g_object_set(cell, "background-set", FALSE, NULL); /* print this normal */

  g_free(str);
}

void bio_gui_persons_textrenderer_mark_date_cell_if_invalid_date(GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter)
{
  gchar *str;
  GDate *date;

  gtk_tree_model_get(tree_model, iter, PERSON_VIEW_COLUMN_BIRTHDAY, &str, -1);

  // Is the date valid?
  date = g_date_new();
  g_date_set_parse(date, str);
  if(g_date_valid(date))
    g_object_set(cell, "foreground-set", FALSE, NULL); /* print this normal */
  else
    g_object_set(cell, "foreground", "red", "foreground-set", TRUE, NULL);

  g_free(str);
  g_date_free(date);
}

void bio_gui_persons_textrenderer_callback_name(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
  bio_gui_persons_textrenderer_mark_empty_cell_green (cell, tree_model, iter);
}

void bio_gui_persons_textrenderer_callback_date(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
  bio_gui_persons_textrenderer_mark_empty_cell_green (cell, tree_model, iter);
  bio_gui_persons_textrenderer_mark_date_cell_if_invalid_date (cell, tree_model, iter);
}

GtkTreeView* bio_gui_persons(GtkListStore *list)
{
  GtkCellRenderer *TextRendererName, *textrenderer_birthday;
  GtkTreeViewColumn *column0, *column1;
  GtkTreeView *view;

  view = g_object_new(GTK_TYPE_TREE_VIEW, "model", list, "rules-hint", TRUE, "headers-clickable", TRUE, "reorderable", TRUE, "enable-search", TRUE, "search-column", 0,
  "enable-grid-lines", GTK_TREE_VIEW_GRID_LINES_BOTH, "rubber-banding", FALSE, NULL);
  TextRendererName = gtk_cell_renderer_text_new();
  g_object_set(TextRendererName, "editable", TRUE, NULL);
  g_signal_connect(TextRendererName, "edited", (GCallback) bio_gui_persons_textrenderer_callback_name_edited, list);
  column0 = gtk_tree_view_column_new_with_attributes("Name", TextRendererName, "text", PERSON_VIEW_COLUMN_NAME, NULL);
  g_object_set(column0, "resizable", TRUE, "clickable", TRUE, "reorderable", TRUE, NULL);
  gtk_tree_view_column_set_cell_data_func(column0, TextRendererName, bio_gui_persons_textrenderer_callback_name, NULL, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(view), column0);

  textrenderer_birthday = gtk_cell_renderer_text_new();
  g_object_set(textrenderer_birthday, "editable", TRUE, NULL);
  g_signal_connect(textrenderer_birthday, "edited", (GCallback) bio_gui_persons_textrenderer_callback_birthday_edited_list, list);
  g_signal_connect(textrenderer_birthday, "edited", (GCallback) bio_gui_persons_textrenderer_callback_birthday_edited_view, view);
  column1 = gtk_tree_view_column_new_with_attributes("Birthday", textrenderer_birthday, "text", PERSON_VIEW_COLUMN_BIRTHDAY, NULL);
  g_object_set(column1, "resizable", TRUE, "clickable", TRUE, "reorderable", TRUE, NULL);
  gtk_tree_view_column_set_cell_data_func(column1, textrenderer_birthday, bio_gui_persons_textrenderer_callback_date, NULL, NULL);
  gtk_tree_view_append_column(view, column1);

  g_signal_connect(view, "cursor-changed", G_CALLBACK(bio_gui_persons_date_changed), NULL);

  return(view);
}

int main(int argc, char **argv)
{
  GtkWindow *window;
  GtkAccelGroup *accel;
  GtkMenuBar *menu;
  GtkWidget *file_menu_item, *file_close_menu_item, *option_menu_item, *console, *help_menu_item, *help_info_menu_item;
  GtkMenu *file_menu, *option_menu, *help_menu;
  GtkVBox *vbox;
  GList *person = NULL;
  GtkListStore *list;
  GtkTreeIter iter;
  GtkTreeView *view;
  GtkTreeViewColumn *column0, *column1;

  /* Init type system as soon as possible */
  g_type_init();

  bio_read_file(&person);

  list = gtk_list_store_new(PERSON_VIEW_COLUMN_NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);
  g_list_foreach(person, bio_gui_foreach_person, list);
  bio_gui_persons_add_empty_row(list);

  view = bio_gui_persons(list);

  bio_birthday.day=1;
  bio_birthday.month=11;
  bio_birthday.year=2012;

  gtk_init(&argc, &argv);

  window = g_object_new(GTK_TYPE_WINDOW, "title", "Biorhythmus", "default-width", 400, "default-height", 400, NULL);
  g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
  g_signal_connect(window, "destroy", G_CALLBACK(bio_gui_close), NULL);

  /* VBOX */
  vbox = g_object_new(GTK_TYPE_VBOX, NULL);

  /* MAP */
  map = gtk_drawing_area_new();

  /* AccelGroup */
  accel = g_object_new(GTK_TYPE_ACCEL_GROUP, NULL);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);

  menu = g_object_new(GTK_TYPE_MENU_BAR, NULL);
  status = g_object_new(GTK_TYPE_STATUSBAR, NULL);

  /* menu -> file -> file close */
  file_close_menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE, accel);
  g_signal_connect(file_close_menu_item, "activate", G_CALLBACK(bio_gui_close), NULL);

  /* menu -> file */
  file_menu = g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_close_menu_item);

  file_menu_item = gtk_menu_item_new_with_mnemonic("_File");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), file_menu_item);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), GTK_WIDGET(file_menu));

  /* menu -> option -> ... */
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

  console = gtk_menu_item_new_with_mnemonic("_Console");
  g_signal_connect(console, "activate", G_CALLBACK(consolebio), NULL);

  /* menu -> option */
  option_menu = g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_bio23);
  gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_bio28);
  gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_bio33);
  gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), option_total);
  gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), gtk_separator_menu_item_new());
  gtk_menu_shell_append(GTK_MENU_SHELL(option_menu), console);

  option_menu_item = gtk_menu_item_new_with_mnemonic("_Options");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), option_menu_item);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(option_menu_item), GTK_WIDGET(option_menu));

  /* menu -> help -> .. */
  help_info_menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, accel);
  g_signal_connect(help_info_menu_item, "activate", G_CALLBACK(bio_gui_help_info_dialog), NULL);

  /* menu -> help */
  help_menu = g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), help_info_menu_item);
  help_menu_item = gtk_menu_item_new_with_mnemonic(_("_Help"));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), help_menu_item);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_menu_item), GTK_WIDGET(help_menu));

  /* Add Menubar to vbox */
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(menu), FALSE, TRUE, 0);

  /* Add MAP */
#ifdef GTK2
  g_signal_connect(G_OBJECT(map), "expose_event", G_CALLBACK(drawbio), NULL);
#else
  g_signal_connect(G_OBJECT(map), "draw", G_CALLBACK(drawbio), NULL);
#endif
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(map), TRUE, TRUE, 0);

  /* Calendar */
  dates = gtk_calendar_new();
  gtk_calendar_set_display_options((GtkCalendar*)dates, GTK_CALENDAR_SHOW_HEADING|GTK_CALENDAR_SHOW_DAY_NAMES);
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(dates), FALSE, TRUE, 0);
  g_signal_connect(G_OBJECT(dates), "day-selected", G_CALLBACK(datechange), NULL);
  g_signal_connect(G_OBJECT(dates), "month-changed", G_CALLBACK(datechange), NULL);

  /* View */
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(view), FALSE, FALSE, 0);

  /* Statusbar */
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(status), FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(vbox));
  gtk_widget_show_all(GTK_WIDGET(window));
  gtk_main();
  return(0);
}
