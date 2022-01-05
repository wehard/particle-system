cmake_minimum_required(VERSION 3.18.4)

project(particle-system)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES src/main.cpp
			src/ParticleSystem.cpp
			src/CLContext.cpp
			src/GLContext.cpp
			src/GUIContext.cpp
			imgui/imgui.cpp
			imgui/imgui_demo.cpp
			imgui/imgui_draw.cpp
			imgui/imgui_tables.cpp
			imgui/imgui_widgets.cpp
			imgui/backends/imgui_impl_glfw.cpp
			imgui/backends/imgui_impl_opengl3.cpp)

include_directories(include
					imgui
					imgui/backends
					gl-engine/include
					/usr/local/include)



add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(gl-engine)

if (NOT APPLE)
add_subdirectory(vendor/OpenCL-SDK)
endif()

find_package(OpenGL REQUIRED)

if (NOT APPLE)
include_directories(
	vendor/OpenCL-SDK/include/api
	vendor/OpenCL-SDK/include/cpp)
endif()


if (APPLE)
find_library(OPENCL OpenCL)
if (NOT OPENCL)
message(FATAL_ERROR "OpenCL not found!")
endif()
endif()


target_link_libraries(${PROJECT_NAME} gl-engine)

if (NOT APPLE)
target_link_libraries(${PROJECT_NAME} OpenCL)
target_link_libraries(${PROJECT_NAME} OpenGL)
target_link_libraries(${PROJECT_NAME} GLEW)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} GL)
endif()

if (APPLE)
target_link_libraries(${PROJECT_NAME} "-framework Cocoa -framework OpenGL -framework OpenCL -framework IOKit -framework CoreVideo")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/gl-engine/lib/include)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${CMAKE_CURRENT_SOURCE_DIR}/gl-engine/lib/libglfw3.a)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${CMAKE_CURRENT_SOURCE_DIR}/gl-engine/lib/libGLEW.a)
endif()


# add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E copy_directory
#                        ${CMAKE_SOURCE_DIR}/res/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)

set (res_source ${CMAKE_SOURCE_DIR}/res)
set (res_dest ${CMAKE_CURRENT_BINARY_DIR}/res)
add_custom_command(
 TARGET ${PROJECT_NAME} POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E create_symlink ${res_source} ${res_dest} 
 DEPENDS ${destination}
 COMMENT "symbolic link resources folder from ${source} => ${destination}"
)
