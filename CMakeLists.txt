cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)

project(mesh-spy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enalbe folders of IDE organization.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#---------------------------
# Qt configuration
#---------------------------
set(QT_VERSION_MAJOR 6)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    OpenGL
    OpenGLWidgets
)

#---------------------------
# OpenGL configuration
#---------------------------
find_package(OpenGL REQUIRED)


#---------------------------
# Dependencies
#---------------------------
include(FetchContent)

# STB
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(stb)

# Create interface target for stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${stb_SOURCE_DIR})
target_compile_definitions(stb_image INTERFACE STB_IMAGE_IMPLEMENTATION)

# tinygltf
FetchContent_Declare(tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
    GIT_TAG v2.9.7
    GIT_SHALLOW TRUE
)

FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_SHALLOW TRUE
)

# Make both available (tinygltf depends on nlohmann_json)
FetchContent_MakeAvailable(tinygltf nlohmann_json)

# GLM
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)


#---------------------------
# Source files
#---------------------------
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/glviewwidget.cpp
    src/gbuffer.cpp
    src/deferredrenderer.cpp
    src/gltfloader.cpp
    src/model.cpp
    src/camera.cpp
    src/skybox.cpp)

set(HEADERS
    src/mainwindow.h
    src/glviewwidget.h
    src/gbuffer.h
    src/deferredrenderer.h
    src/meshdata.h
    src/gltfloader.h
    src/model.h
    src/camera.h
    src/renderconfig.h
    src/skybox.h)

set(RESOURCES
    resources.qrc
    qdarkstyle/dark/darkstyle.qrc)

set(FORMS)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
    ${FORMS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR})


# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    OpenGL::GL
    stb_image
    tinygltf
    nlohmann_json::nlohmann_json
    glm
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl.lib)
endif()

# Add definitions for stb_image implementation
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<IF:$<CONFIG:Debug>,STBI_FAILURE_USERMSG,>
)

