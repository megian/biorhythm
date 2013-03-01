biorhythmus: biorhythmus.c
	gcc -o biorhythmus biorhythmus.c `pkg-config --cflags --libs gtk+-2.0 libgnomeui-2.0`
