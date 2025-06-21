cmake_minimum_required(VERSION 3.31)
project(a7loader C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_FLAGS "-fsanitize=address")

add_executable(a7loader
        src/main.c
        src/type.c
        src/type.h
        src/app.c
        src/app.h
        src/paths.c
        src/paths.h
        src/vector.c
        src/vector.h
        src/buffers.c
        src/buffers.h
        src/loader/ipa.c
        src/loader/ipa.h
        src/loader/bundle.c
        src/loader/bundle.h
        src/virt_io/dir.c
        src/virt_io/dir.h
        src/virt_io/file.c
        src/virt_io/file.h
        src/virt_io/zipped_dir.c
        src/virt_io/zipped_dir.h
        src/virt_io/zipped_file.c
        src/virt_io/zipped_file.h
        src/virt_io/type.c
        src/virt_io/type.h
        src/hle/collection.c
        src/hle/collection.h
        src/hle/content.c
        src/hle/content.h
        src/formats/property_list.c
        src/formats/property_list.h
)

target_include_directories(a7loader PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:>
)

set(BUILD_REGRESS OFF CACHE BOOL "Build regression tests" FORCE)
add_subdirectory(vendor/libzip)
target_link_libraries(a7loader PRIVATE libzip::zip m)