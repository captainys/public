set(TARGET_NAME sample00-lineart sample01-squash sample02-echo sample03-mouse sample04-cube sample05-key sample06-15puzzle)

foreach(ONE_TARGET ${TARGET_NAME})
	add_executable(${ONE_TARGET} MACOSX_BUNDLE ${ONE_TARGET}/${ONE_TARGET}.cpp)
	target_link_libraries(${ONE_TARGET} fssimplewindow ysbitmapfont)
endforeach(ONE_TARGET)
