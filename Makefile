DESTDIR =
BIN=$(DESTDIR)/usr/bin

all:
	gcc -o biorhythmus biorhythmus.c `pkg-config --cflags --libs gtk+-2.0 libgnomeui-2.0`

install:
	gcc -o biorhythmus biorhythmus.c `pkg-config --cflags --libs gtk+-2.0 libgnomeui-2.0`
	install -d $(BIN) $(DESTDIR)/usr/lib/menu/
	install ./biorhythmus $(BIN)
	install -m644 debian/menu $(DESTDIR)/usr/lib/menu/biorhythmus

deb:
	fakeroot debian/rules binary
