
#DESTDIR =
BIN=$(DESTDIR)/usr/bin

all:
#	gcc `pkg-config --cflags gtk+-2.0 json-glib-1.0` -o biorhythmus biorhythmus*.c `pkg-config --libs gtk+-2.0 json-glib-1.0` -lm -DGTK2
	gcc `pkg-config --cflags gtk+-3.0 json-glib-1.0` -o biorhythmus biorhythmus*.c `pkg-config --libs gtk+-3.0 json-glib-1.0` -lm

install:
	gcc `pkg-config --cflags gtk+-3.0` -o biorhythmus biorhythmus.c `pkg-config --libs gtk+-3.0`
	install -d $(BIN) $(DESTDIR)/usr/lib/menu/
	install ./biorhythmus $(BIN)
	install -m644 debian/menu $(DESTDIR)/usr/lib/menu/biorhythmus

deb:
	fakeroot debian/rules binary
