
add_executable (orbit WIN32
				
				core/main.cpp
				core/system.cpp
				core/application.cpp
				core/input.cpp

				content/mesh.cpp
				content/model.cpp
				content/texture.cpp

				graphics/platform.cpp
				graphics/camera.cpp

				graphics/d3d11/d3d11core.cpp
				graphics/d3d11/content/mesh.cpp
				graphics/d3d11/content/texture.cpp
				graphics/d3d11/shaders/shader.cpp
				graphics/d3d11/shaders/shaders_path.cpp

				logger/logger.cpp
 )


target_include_directories(orbit PRIVATE ../)
target_include_directories(orbit PRIVATE ../glm/)
target_include_directories(orbit PRIVATE ../glfw/include)
target_include_directories(orbit PRIVATE ../spdlog/include)
target_include_directories(orbit PRIVATE ../assimp/include)

target_link_libraries(orbit glfw)
target_link_libraries(orbit assimp)




