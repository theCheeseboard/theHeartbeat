cmake_minimum_required(VERSION 3.21.0)

project(theheartbeat VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus Sql Concurrent)
find_package(libcontemporary REQUIRED)

set(SOURCES
    main.cpp
    mainwindow.cpp
    processes/process.cpp
    processes/processmanager.cpp
    processes/processmodel.cpp
    system/systemmanager.cpp
    panes/percentagepane.cpp
    panes/minmaxpane.cpp
    processaction.cpp
    panes/minipercentagepane.cpp
    panes/sidepane.cpp
    panes/numberpane.cpp
    panes/mininumberpane.cpp

    mainwindow.ui
    panes/percentagepane.ui
    panes/minmaxpane.ui
    processaction.ui
    panes/minipercentagepane.ui
    panes/numberpane.ui
    panes/mininumberpane.ui

    resources.qrc
)

set(HEADERS
    mainwindow.h
    processes/process.h
    processes/processmanager.h
    processes/processmodel.h
    system/systemmanager.h
    panes/percentagepane.h
    panes/minmaxpane.h
    processaction.h
    panes/minipercentagepane.h
    panes/sidepane.h
    panes/numberpane.h
    panes/mininumberpane.h

)

add_executable(theheartbeat ${SOURCES} ${HEADERS})

set_target_properties(theheartbeat PROPERTIES
        MACOSX_BUNDLE TRUE
#        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        MACOSX_BUNDLE_GUI_IDENTIFIER com.vicr123.theheartbeat
        WIN32_EXECUTABLE TRUE
        CNTP_DATA_SUBDIR theheartbeat)

cntp_init(theheartbeat 20)
cntp_translate(theheartbeat)
cntp_target_name(theheartbeat "theHeartbeat")
cntp_app_icon(theheartbeat
    BASEICON icons/baseicon.svg
#    BASEICON_MAC icons/baseicon-mac.svg
)
cntp_target_desktop_file(theheartbeat
    DESKTOP_FILE com.vicr123.theheartbeat.desktop
    BLUEPRINT_DESKTOP_FILE com.vicr123.theheartbeat_blueprint.desktop)
cntp_defaults_file(theheartbeat
    DEFAULTS_FILE theheartbeat.conf)

target_link_libraries(theheartbeat Qt6::Widgets Qt6::DBus Qt6::Concurrent libcontemporary)

install(TARGETS theheartbeat
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION /Applications)
