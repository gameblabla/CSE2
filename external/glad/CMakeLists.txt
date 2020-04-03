cmake_minimum_required(VERSION 3.12)

project(glad LANGUAGES C)

add_library(glad
	"include/glad/glad.h"
	"include/KHR/khrplatform.h"
	"src/glad.c"
)

target_include_directories(glad PUBLIC "include")
