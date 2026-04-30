
add_executable( editor WIN32
        main.cpp
        editor.cpp

        panel.cpp
)

target_include_directories(editor PRIVATE ../)

target_link_libraries(editor orbit)
target_link_libraries(editor imgui)