cmake_minimum_required(VERSION 3.16)
project(MSFSDashboard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# User-provided Qt path
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/msvc2022_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# Add loguru logging library
include(FetchContent)
FetchContent_Declare(LoguruGitRepo
    GIT_REPOSITORY "https://github.com/emilk/loguru"
    GIT_TAG        "master"
)
# Set loguru compile-time flags
set(LOGURU_WITH_STREAMS TRUE)
FetchContent_MakeAvailable(LoguruGitRepo)

# User-provided SimConnect SDK path
set(SIMCONNECT_SDK_PATH "C:/MSFS SDK/SimConnect SDK")
include_directories(
    ${SIMCONNECT_SDK_PATH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Automatically find source and header files
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")
file(GLOB UI_FILES "src/*.ui")

add_executable(MSFSDashboard
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

target_link_libraries(MSFSDashboard PRIVATE 
    Qt6::Widgets 
    Qt6::Core
    Qt6::Gui
    loguru::loguru
    "${SIMCONNECT_SDK_PATH}/lib/SimConnect.lib"
)

# Copy SimConnect.dll to the build directory
add_custom_command(TARGET MSFSDashboard POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SIMCONNECT_SDK_PATH}/lib/SimConnect.dll"
    $<TARGET_FILE_DIR:MSFSDashboard>
    COMMENT "Copying SimConnect.dll..."
)

# Deploy Qt dependencies using windeployqt
if(WIN32)
    add_custom_command(
        TARGET MSFSDashboard POST_BUILD
        COMMAND $<TARGET_PROPERTY:Qt6::windeployqt,LOCATION>
            --dir "$<TARGET_FILE_DIR:MSFSDashboard>"
            "$<TARGET_FILE:MSFSDashboard>"
        COMMENT "Deploying Qt dependencies with windeployqt..."
        VERBATIM
    )
endif()

# Set the correct subsystem for a GUI application on Windows
set_target_properties(MSFSDashboard PROPERTIES WIN32_EXECUTABLE ON) 