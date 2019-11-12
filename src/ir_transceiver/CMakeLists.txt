#if(WIN32)
	set(TARGET_NAME irsensor_gui irsend)

	add_subdirectory(gui)
	add_subdirectory(cui_transmitter)
#endif()
