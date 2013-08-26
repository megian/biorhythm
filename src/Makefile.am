bin_PROGRAMS = biorhythmus

biorhythmus_SOURCES = \
	biorhythmus.h biorhythmus.c \
	biorhythmus-chart.h biorhythmus-chart.c \
	biorhythmus-cli.h biorhythmus-cli.c \
	biorhythmus-fileview.h biorhythmus-fileview.c \
	biorhythmus-math.h biorhythmus-math.c \
	biorhythmus-print.h biorhythmus-print.c

biorhythmus_CFLAGS  = $(GTK_CFLAGS) $(JSONGLIB_CFLAGS)
# $(GNOMEPRINT_CFLAGS)
biorhythmus_LDADD   = $(GTK_LIBS) $(INTLLIBS) $(JSONGLIB_LIBS)
# $(GNOMEPRINT_LIBS)
