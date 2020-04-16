cmake_minimum_required(VERSION 3.10.2)

if(NOT TARGET glad)

project(glad LANGUAGES C)

add_library(glad STATIC
	"include/glad/glad.h"
	"include/KHR/khrplatform.h"
	"src/glad.c"
)

target_include_directories(glad PUBLIC "include")
target_link_libraries(glad PRIVATE ${CMAKE_DL_LIBS})

endif(NOT TARGET glad)
