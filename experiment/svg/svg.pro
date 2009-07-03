TEMPLATE = app

CONFIG+= qt opengl stl exceptions release   # release debug
QT += opengl svg

SOURCES += svg.cpp

#######################################
# Disable assertions in release version

QMAKE_CXXFLAGS_RELEASE += -DNDEBUG
QMAKE_CFLAGS_RELEASE += -DNDEBUG

######################################
# Other stuff:
# Disable implicit cast from QString to char*

QMAKE_CXXFLAGS_RELEASE += -DQT_NO_ASCII_CAST
QMAKE_CXXFLAGS_DEBUG += -DQT_NO_ASCII_CAST

######################################
# Hide those crufty moc_ files away

MOC_DIR = moc
OBJECTS_DIR = obj
