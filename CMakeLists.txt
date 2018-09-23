cmake_minimum_required (VERSION 2.8)
project (pong)
set (CMAKE_C_FLAGS "-Wall -Werror")
add_executable(pong pong.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ALLEGRO5 REQUIRED allegro-5)

include_directories("${PROJECT_BINARY_DIR}")

find_library(ALLEGRO_LIBRARY allegro)
find_library(ALLEGRO_FONT_LIBRARY allegro_font)
find_library(ALLEGRO_TTF_LIBRARY allegro_ttf)
find_library(ALLEGRO_PRIMITIVES_LIBRARY allegro_primitives)

target_link_libraries(pong PUBLIC ${ALLEGRO_LIBRARY} ${ALLEGRO_FONT_LIBRARY} ${ALLEGRO_TTF_LIBRARY} ${ALLEGRO_PRIMITIVES_LIBRARY})
