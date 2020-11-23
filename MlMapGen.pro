QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
Release:LIBS += -LD:\opencv\build\x64\vc14\lib -lopencv_world450
Debug:LIBS += -LD:\opencv\build\x64\vc14\lib -lopencv_world450d
INCLUDEPATH += D:\opencv\build\include
DEPENDPATH += D:\opencv\build\include

SOURCES += \
    main.cpp \
    mainwindow.cpp \
#    richdem/depressions/main.cpp \
#    richdem/flats/generate_square_grid.cpp \
 \#    richdem/flats/main.cpp
    richdem/common/random.cpp \
    richdem/richdem.cpp

HEADERS += \
    FastNoiseLite.h \
    asmOpenCV.h \
    mainwindow.h \
    richdem/common/Array2D.hpp \
    richdem/common/Array3D.hpp \
    richdem/common/Layoutfile.hpp \
    richdem/common/ManagedVector.hpp \
    richdem/common/ProgressBar.hpp \
    richdem/common/communication-threads.hpp \
    richdem/common/communication.hpp \
    richdem/common/constants.hpp \
    richdem/common/gdal.hpp \
    richdem/common/grid_cell.hpp \
    richdem/common/loaders.hpp \
    richdem/common/logger.hpp \
    richdem/common/math.hpp \
    richdem/common/memory.hpp \
    richdem/common/random.hpp \
    richdem/common/timer.hpp \
    richdem/common/version.hpp \
    richdem/depressions/Barnes2014.hpp \
    richdem/depressions/Lindsay2016.hpp \
    richdem/depressions/Wei2018.hpp \
    richdem/depressions/Zhou2016.hpp \
    richdem/depressions/depressions.hpp \
    richdem/flats/Barnes2014.hpp \
    richdem/flats/find_flats.hpp \
    richdem/flats/flat_resolution.hpp \
    richdem/flats/flat_resolution_dinf.hpp \
    richdem/flats/flats.hpp \
    richdem/flats/garbrecht.hpp \
    richdem/flowmet/Fairfield1991.hpp \
    richdem/flowmet/Freeman1991.hpp \
    richdem/flowmet/Holmgren1994.hpp \
    richdem/flowmet/OCallaghan1984.hpp \
    richdem/flowmet/Orlandini2003.hpp \
    richdem/flowmet/Quinn1991.hpp \
    richdem/flowmet/Seibert2007.hpp \
    richdem/flowmet/Tarboton1997.hpp \
    richdem/flowmet/d8_flowdirs.hpp \
    richdem/flowmet/dinf_flowdirs.hpp \
    richdem/methods/d8_methods.hpp \
    richdem/methods/dinf_methods.hpp \
    richdem/methods/flow_accumulation.hpp \
    richdem/methods/flow_accumulation_generic.hpp \
    richdem/methods/strahler.hpp \
    richdem/methods/terrain_attributes.hpp \
    richdem/misc/misc_methods.hpp \
    richdem/richdem.hpp \
    richdem/terrain_generation.hpp \
    richdem/tiled/A2Array2D.hpp \
    richdem/tiled/lru.hpp \
    richdem/ui/cli_options.hpp

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
