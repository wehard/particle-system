cmake_minimum_required(VERSION 3.18.4)

project(particle-system)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES src/main.cpp
			src/ParticleSystem.cpp
			src/CLContext.cpp
			src/GLContext.cpp)

include_directories(include 
					gl-engine/include
					vendor/OpenCL-SDK/include/api
					vendor/OpenCL-SDK/include/cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(gl-engine)
add_subdirectory(vendor/OpenCL-SDK)

target_link_libraries(${PROJECT_NAME} gl-engine)
target_link_libraries(${PROJECT_NAME} OpenCL)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} GLEW)
target_link_libraries(${PROJECT_NAME} GL)

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
