
add_executable (orbit WIN32
				
				core/main.cpp
				core/system.cpp
				core/application.cpp
				
				graphics/d3d11/d3d11core.cpp
	
 "core/primitive_types.h")

target_include_directories(orbit PRIVATE .)




