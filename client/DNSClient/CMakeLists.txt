cmake_minimum_required(VERSION 3.5)

project(DNSClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport Svg SvgWidgets)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/qcustomplot
)

set(PROJECT_SOURCES
    #________________________________________
    #---------------- SOURCES ---------------
    sources/main.cpp
    sources/mainwindow.cpp
    #-------------- THEMES -------------
    sources/themes/darkTheme.cpp
    #---------------- UI ---------------
    sources/ui/toolBar.cpp
    sources/ui/statusBar.cpp
    sources/ui/centralWidget.cpp
    #------------ Dashboar W -----------
    sources/ui/centralDashboardWidgets/requestsChartWidget.cpp
    sources/ui/centralDashboardWidgets/cacheHitRate.cpp
    sources/ui/centralDashboardWidgets/latencyP95.cpp
    sources/ui/centralDashboardWidgets/topDomains.cpp
    sources/ui/centralDashboardWidgets/querryTypes.cpp
    sources/ui/centralDashboardWidgets/recentErrors.cpp
    sources/ui/centralDashboardWidgets/dnsTypeCardWidget.cpp
    #-------------- Model --------------
    sources/models/errorsModel.cpp

    #________________________________________
    #---------------- HEADERS ---------------
    headers/mainwindow.h
    #-------------- THEMES -------------
    headers/themes/darkTheme.h
    #---------------- UI ---------------
    headers/ui/toolBar.h
    headers/ui/statusBar.h
    headers/ui/centralWidget.h
    #------------ Dashboar W -----------
    headers/ui/centralDashboardWidgets/requestsChartWidget.h
    headers/ui/centralDashboardWidgets/cacheHitRate.h
    headers/ui/centralDashboardWidgets/latencyP95.h
    headers/ui/centralDashboardWidgets/topDomains.h
    headers/ui/centralDashboardWidgets/querryTypes.h
    headers/ui/centralDashboardWidgets/recentErrors.h
    headers/ui/centralDashboardWidgets/dnsTypeCardWidget.h
    #-------------- Model --------------
    headers/models/errorsModel.h

    #________________________________________
    #-------------- Dependencies ------------
    dependencies/qcustomplot/qcustomplot.h
    dependencies/qcustomplot/qcustomplot.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DNSClient
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DNSClient APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(DNSClient SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(DNSClient
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(DNSClient PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::SvgWidgets
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.DNSClient)
endif()
set_target_properties(DNSClient PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS DNSClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DNSClient)
endif()
