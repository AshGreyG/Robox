cmake_minimum_required(VERSION 3.5)

project(Robox VERSION 0.1 LANGUAGES CXX)

include_directories(src)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        src/main.cpp
        src/gui/robox_main_window.h
        src/gui/robox_main_window.cc
        src/core/core.h
        src/core/core.cc
)

set(TEST_CORE_1_SOURCES
        src/core/core.h
        src/core/core.cc
        test/test_core_1.cpp
)

set(TEST_CONSOLE_SORCES
        src/core/core.h
        src/core/core.cc
        src/cli/console.h
        src/cli/console.cc
        test/test_console.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Robox
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_add_executable(Test-Core-1
        ${TEST_CORE_1_SOURCES}
    )
    qt_add_executable(Test-Console
        ${TEST_CONSOLE_SORCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Robox APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

else()
    if(ANDROID)
        add_library(Robox SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Robox
            ${PROJECT_SOURCES}
        )
        add_executable(Test-Core-1
            ${TEST_CORE_1_SOURCES}
        )
        add_executable(Test-Console
            ${TEST_CONSOLE_SORCES}
        )
    endif()

endif()

target_link_libraries(Robox PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Robox)
endif()
set_target_properties(Robox PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Robox
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Robox)
endif()
