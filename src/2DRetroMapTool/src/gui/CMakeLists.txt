set(TARGET_NAME "2DRetroMapTool")
add_executable(${TARGET_NAME} 
	fsgui3dapp.cpp
	fsgui3dmain.cpp
	singleton.cpp
	config/retromap_config.cpp
	filemenu/filemenu.cpp
	menusetup.cpp
	render.cpp
	textresource.cpp
	editmenu/editmenu.cpp
	selectmenu/selectmenu.cpp
	filename.cpp
	configmenu/configmenu.cpp
	fieldmenu/fieldmenu.cpp
	worldmenu/worldmenu.cpp
	markupmenu/markupmenu.cpp
	mappiecemenu/mappiecemenu.cpp
	viewmenu/viewmenu.cpp
	fsgui3dapp.h
	config/retromap_config.h
	textresource.h
	filename.h
)
target_link_libraries(${TARGET_NAME} fslazywindow ysgl fsgui3d fsguilib_gl2 fsgui3d_gl2 ystexturemanager ystexturemanager_gl retromap_kernel retromap_kernel_gl2 retromap_kernelutil fsguifiledialog ysport fsguilib fsguilib_gl2)
