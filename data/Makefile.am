desktopdir 	 = $(datadir)/applications
desktop_in_files = biorhythm.desktop.in
desktop_DATA 	 = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

man_MANS = biorhythm.1

install-data-local:

EXTRA_DIST = 				\
	$(desktop_in_files)		\
	$(man_MANS)			

CLEANFILES =	 			\
	$(desktop_DATA)			
