set(TARGET_NAME ys_bitmap_command_base ysmakebmptile ysmakeblackwhite ysresizebmp ysmakewhitetransparent ysnegatebmp ysmakeblankbmp ysinvertbmp ysmakedensitymap ysmakealphamask yspastebmp yscropbmp)

foreach(ONE_TARGET ${TARGET_NAME})
	add_executable(${ONE_TARGET} ${ONE_TARGET}.cpp)
	target_link_libraries(${ONE_TARGET} ysclass ysbitmap)
endforeach(ONE_TARGET)
