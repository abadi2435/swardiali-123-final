QT += core \
    gui \
    opengl
TARGET = cs123_final
TEMPLATE = app
INCLUDEPATH += src \
    lib \
    math \
    support
DEPENDPATH += src \
    lib \
    math \
    support
HEADERS += src/glwidget.h \
    lib/targa.h \
    lib/glm.h \
    math/vector.h \
    support/resourceloader.h \
    support/mainwindow.h \
    support/camera.h \
    lib/targa.h \
    src/scene.h \
    src/spacescene.h \
    src/tablescene.h
SOURCES += src/glwidget.cpp \
    lib/targa.cpp \
    lib/glm.cpp \
    support/resourceloader.cpp \
    support/mainwindow.cpp \
    support/main.cpp \
    support/camera.cpp \
    src/scene.cpp \
    src/spacescene.cpp \
    src/tablescene.cpp
FORMS += mainwindow.ui \
    support/mainwindow.ui
OTHER_FILES += shaders/refract.vert \
    shaders/refract.frag \
    shaders/reflect.vert \
    shaders/reflect.frag \
    shaders/brightpass.frag \
    shaders/blur.frag
RESOURCES += 
