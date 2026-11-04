cmake_minimum_required(VERSION 3.10)
project (GameDevelopmentProject)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCE_FILES
  source/main.cpp
)

include_directories(source)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Add GLFW library
add_subdirectory(thirdparty/glfw-3.4 "${CMAKE_CURRENT_BINARY_DIR}/glfw_build")
include_directories(thirdparty/glfw-3.4/include)

# Add Glew library
set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)
add_subdirectory(thirdparty/glew/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
include_directories(thirdparty/glew/include)
set_property(TARGET glew_s PROPERTY FOLDER GLEW)
set_property(TARGET glew   PROPERTY FOLDER GLEW)

# Link all thirdparty libraries
target_link_libraries(${PROJECT_NAME} 
    glfw 
    glew_s
)

