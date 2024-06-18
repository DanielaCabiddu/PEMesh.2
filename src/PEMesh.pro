#-------------------------------------------------
#
# Project created by QtCreator 2019-09-02T11:30:54
#
#-------------------------------------------------

QT += core gui opengl charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = PEMesh
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -Wno-deprecated-declarations # gluQuadric gluSphere and gluCylinde are deprecated in macOS 10.9
CONFIG += sdk_no_version_check

linux { QMAKE_CXXFLAGS += -fopenmp
        LIBS += -fopenmp }

VEM_BENCHMARK_DIR = meshes

DEFINES     += CINOLIB_USES_OPENGL
DEFINES     += CINOLIB_USES_QT
INCLUDEPATH += ../external/cinolib/include
INCLUDEPATH += ../external/cinolib/external/eigen

DEFINES     += CINOLIB_USES_BOOST
macos { INCLUDEPATH += /Users/tommy/QT-projects/libs/boost }

DEFINES     += CINOLIB_USES_TRIANGLE
INCLUDEPATH += ../external/triangle
LIBS         += -L$$PWD/../external/triangle/build -ltriangle

#INCLUDEPATH += meshes

#DEFINES += DEVELOP_MODE

CONFIG += c++14

SOURCES += \
        $${VEM_BENCHMARK_DIR}/abstract_vem_element.cpp \
        $${VEM_BENCHMARK_DIR}/mesh_metrics.cpp \
        $${VEM_BENCHMARK_DIR}/mirroring.cpp \
        $${VEM_BENCHMARK_DIR}/vem_elements.cpp \
        addpolygondialog.cpp \
        aggregatedialog.cpp \
        customizedchartview.cpp \
        dataset.cpp \
        datasetwidget.cpp \
        geometrygeometryscatterplotswidget.cpp \
        geometryperformancescatterplotswidget.cpp \
        main.cpp \
        mainwindow.cpp \
        meshmetricsgraphicwidget.cpp \
        meshmetricswidget.cpp \
        parametricdatasetsettingsdialog.cpp \
        scatterplotmarkersettingwidget.cpp \
        solverresultswidget.cpp \
        solversettingsdialog.cpp \
        solverwidget.cpp \
        sortgeometricqualitiesdialog.cpp

HEADERS += \
        $${VEM_BENCHMARK_DIR}/abstract_vem_element.h \
        $${VEM_BENCHMARK_DIR}/mesh_metrics.h \
        $${VEM_BENCHMARK_DIR}/mirroring.h \
        $${VEM_BENCHMARK_DIR}/non_uniform_scaling_01.h \
        $${VEM_BENCHMARK_DIR}/vem_elements.h \
        addpolygondialog.h \
        aggregatedialog.h \
        customizedchartview.h \
        dataset.h \
        dataset_classes.h \
        datasetwidget.h \
        geometrygeometryscatterplotswidget.h \
        geometryperformancescatterplotswidget.h \
        mainwindow.h \
        meshmetricsgraphicwidget.h \
        meshmetricswidget.h \
        parametricdatasetsettingsdialog.h \
        quality_metrics.h \
        scatterplotmarkersettingwidget.h \
        solverresultswidget.h \
        solversettingsdialog.h \
        solverwidget.h \
        sortgeometricqualitiesdialog.h

FORMS += \
        addpolygondialog.ui \
        aggregatedialog.ui \
        datasetwidget.ui \
        geometrygeometryscatterplotswidget.ui \
        geometryperformancescatterplotswidget.ui \
        mainwindow.ui \
        meshmetricsgraphicwidget.ui \
        meshmetricswidget.ui \
        parametricdatasetsettingsdialog.ui \
        scatterplotmarkersettingwidget.ui \
        solverresultswidget.ui \
        solversettingsdialog.ui \
        solverwidget.ui \
        sortgeometricqualitiesdialog.ui

qnx: target.path = /tmp/$${TARGET}/bin  # Default rules for deployment.
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx  {DEFINES += GL_GLEXT_PROTOTYPES
            LIBS     += -lGLU}

RESOURCES += images.qrc
