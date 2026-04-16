cmake_minimum_required (VERSION 3.10)

IF(WIN32)
add_compile_definitions(NOMINMAX)
add_compile_definitions(WIN32_LEAN_AND_MEAN)
ENDIF()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 23)

add_subdirectory (orbit)
add_subdirectory (glfw)
add_subdirectory(glm)
add_subdirectory(assimp)
