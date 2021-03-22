cmake_minimum_required(VERSION 3.17.5)

project(detex-view)

find_package(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)
 
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
 
include_directories(${PROJECT_SOURCE_DIR})
add_executable(${PROJECT_NAME} detex-view.c bits.h bptc-tables.h detex.h detex-png.h file-info.h half-float.h hdr.h misc.h /
 convert.c bptc-tables.c bits.c clamp.c dds.c decompress-bc.c decompress-bptc.c decompress-bptc-float.c /
 decompress-etc.c decompress-eac.c decompress-rgtc.c division-tables.c file-info.c half-float.c hdr.c ktx.c misc.c raw.c texture.c)
 
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)

target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES})