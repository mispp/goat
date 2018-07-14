find_package(Qt5 5.9.5 REQUIRED COMPONENTS Core Gui Sql)
find_package(Qt5Sql REQUIRED)

set(goat_src
    src/main.cpp
    src/ConnectionManager.cpp
    src/ConnectionStandardItem.cpp
)

set(goat_ui
    ui/MainWindow.cpp
    ui/MainWindow.ui
    ui/ConnectionTab.cpp
    ui/ConnectionTab.ui
    ui/ConnectionManagerDialog.ui
    ui/Highlighter.cpp
)

add_executable(Goat ${SOURCES}
    ${goat_src}
    ${goat_ui}
)

qt5_use_modules(Goat Core Sql) # This macro depends from Qt version
target_link_libraries(Goat Qt5::Core Qt5::Gui Qt5::Sql)

#FORMS    += \
#    ui/MainWindow.ui \
#    ui/ConnectionTab.ui \
#    ui/ConnectionManagerDialog.ui
#
#DISTFILES += \
#    LICENSE \
#    README.md
