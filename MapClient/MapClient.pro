!include(deployment.pri ) {
    error( "Couldn't find the deployment.pri file!" )
}

QT += 3dcore 3drender 3dinput 3dquick qml quick 3dquickextras

HEADERS += \

SOURCES += \
    main.cpp

RESOURCES += \
    metalbarrel.qrc \
    textures.qrc \
    materials.qrc \
