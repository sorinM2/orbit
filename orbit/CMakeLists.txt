
add_executable (orbit WIN32
				
				core/main.cpp
				core/system.cpp
				core/application.cpp
				
				graphics/d3d11/d3d11core.cpp
				graphics/d3d11/content/mesh.cpp
				graphics/d3d11/shaders/shader.cpp
				graphics/d3d11/shaders/shaders_path.cpp
 "core/platform/platform_common.h")


target_include_directories(orbit PRIVATE .)
target_include_directories(orbit PRIVATE ../)
target_include_directories(orbit PRIVATE ../glm/)




