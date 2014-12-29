bin_PROGRAMS = biorhythm

biorhythm_SOURCES = \
	biorhythm.h biorhythm.c \
	biorhythm-chart.h biorhythm-chart.c \
	biorhythm-cli.h biorhythm-cli.c \
	biorhythm-fileview.h biorhythm-fileview.c \
	biorhythm-math.h biorhythm-math.c \
	biorhythm-print.h biorhythm-print.c

biorhythm_CFLAGS  = $(GTK_CFLAGS) $(JSONGLIB_CFLAGS) -DLOCALEDIR=\"$(localedir)\"
# $(GNOMEPRINT_CFLAGS)
biorhythm_LDADD   = $(GTK_LIBS) $(INTLLIBS) $(JSONGLIB_LIBS)
# $(GNOMEPRINT_LIBS)
