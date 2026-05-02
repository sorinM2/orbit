
add_library (orbit
				core/system.cpp
				core/application.cpp
				core/input.cpp

				content/mesh.cpp
				content/model.cpp

		graphics/renderer.cpp

        graphics/common/camera.cpp
        graphics/common/device.cpp
        graphics/common/common.cpp


				graphics/d3d11/d3d11buffer.cpp
				graphics/d3d11/d3d11sampler.cpp
				graphics/d3d11/d3d11rasterizer.cpp
				graphics/d3d11/d3d11render_target.cpp
				graphics/d3d11/d3d11shader.cpp
				graphics/d3d11/d3d11device.cpp
				graphics/d3d11/d3d11texture.cpp
				graphics/d3d11/d3d11shader_resource.cpp
				graphics/d3d11/util.cpp

				graphics/d3d11/shaders/shaders_path.cpp

				logger/logger.cpp

				ecs/ecs.cpp

				ecs/components.h
 )


target_include_directories(orbit PRIVATE ../)
target_include_directories(orbit PUBLIC ../glm/)
target_include_directories(orbit PRIVATE ../glfw/include)
target_include_directories(orbit PRIVATE ../spdlog/include)
target_include_directories(orbit PRIVATE ../assimp/include)
target_include_directories(orbit PUBLIC ../entt/src)

target_link_libraries(orbit assimp)
target_link_libraries(orbit glfw)
target_link_libraries(orbit EnTT::EnTT)


