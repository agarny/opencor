project(DragAndDropViewPlugin)

# Add the plugin

add_plugin(DragAndDropView
    SOURCES
        ../../i18ninterface.cpp
        ../../plugininfo.cpp
        ../../plugininterface.cpp
        ../../viewinterface.cpp

        src/draganddropviewplugin.cpp
        src/draganddropviewwidget.cpp
    PLUGINS
        Core
)
