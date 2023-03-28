set(src
    vecy/main.cc
)
source_group("" ${src})
add_executable(vecy WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(vecy
    PUBLIC
        external::wx
        external::glm
        external::open_color
)
