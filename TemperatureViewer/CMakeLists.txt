cmake_minimum_required(VERSION 3.5)

project(TemperatureViewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)
find_package(Qt6 REQUIRED COMPONENTS PrintSupport)

set(PROJECT_SOURCES
        main.cpp
        temperatureviewer.cpp
        temperatureviewer.h
        temperatureviewer.ui
)

add_subdirectory(QCustomPlot-library)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TemperatureViewer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET TemperatureViewerQt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(TemperatureViewer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(TemperatureViewer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(TemperatureViewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(TemperatureViewer PRIVATE Qt6::SerialPort)
target_link_libraries(TemperatureViewer PRIVATE Qt6::PrintSupport)
# Link Needed Libraries, QCustomPlot Library
target_link_libraries(TemperatureViewer PRIVATE qcustomplot)

# Compile needed definitions
target_compile_definitions(TemperatureViewer PRIVATE QCUSTOMPLOT_USE_LIBRARY)

set_target_properties(TemperatureViewer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS TemperatureViewer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TemperatureViewer)
endif()
