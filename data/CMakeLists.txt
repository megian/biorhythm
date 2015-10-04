add_custom_command(
	OUTPUT biorhythm.desktop
	COMMAND intltool-merge -d ${CMAKE_SOURCE_DIR}/po ${CMAKE_CURRENT_SOURCE_DIR}/biorhythm.desktop.in biorhythm.desktop
	DEPENDS biorhythm.desktop.in
	)
add_custom_target(driver ALL DEPENDS biorhythm.desktop)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/biorhythm.desktop DESTINATION share/applications)
install(FILES biorhythm.mime biorhythm.keys DESTINATION share/mime-info)
install(FILES biorhythm-icon.png DESTINATION share/pixmaps)
install(FILES
	book-closed.png book-open.png biorhythm-icon.png biorhythm-splash.png
	helpdoc.png hide-pane.png show-pane.png biorhythm.glade 
	${CMAKE_CURRENT_BINARY_DIR}/default-prefs.js
	DESTINATION
	share/biorhythm)
