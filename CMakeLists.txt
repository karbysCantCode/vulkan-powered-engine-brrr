cmake_minimum_required(VERSION 3.12.0)
project(malodee VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(BUILD_MODE "DEBUG")
#set(BUILD_MODE "RELEASE")


file(GLOB_RECURSE SOURCE_CPP "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")
add_executable(malodee ${SOURCE_CPP})



target_include_directories(malodee PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glfw/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glm"
    "$ENV{VULKAN_SDK}/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)



add_subdirectory(Vendor/glfw) #compile glfw to lib
target_link_libraries(malodee PRIVATE #link glfw and vulkan lib
    glfw
    "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib"
)


target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC VK_SDK_PATH="$ENV{VULKAN_SDK}")



#attach the cmake script that compiles the shaders to execute at build time
add_custom_target(compile_shaders ALL
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeShaderCompiler.txt)
#pretty much calling the "function" above!
add_dependencies(malodee compile_shaders)

#copy shaders folder to build folder
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Shaders"
            "${CMAKE_BINARY_DIR}/Shaders"
)
#pretty much calling the "function" above!
add_dependencies(malodee copy_shaders)