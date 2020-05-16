QT        += \
          core \
          gui \
          charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS



SOURCES += \
    c_iexdata.cpp \
    c_login.cpp \
    c_main.cpp \
    c_mainwindow.cpp \
    c_qcustomplot.cpp \
    c_stockchart.cpp \
    c_webscrapper.cpp

HEADERS += \
    h_iexdata.h \
    h_login.h \
    h_mainwindow.h \
    h_qcustomplot.h \
    h_stockchart.h \
    h_webcurl.h \
    h_webscrapper.h

FORMS += \
   u_mainwindow.ui \
   u_login.ui

RC_ICONS = icon.ico.ico

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    r_res.qrc

win32: LIBS += -L$$PWD/../Libraries/vcpkg/installed/libcurl/lib/ -llibcurl
INCLUDEPATH += $$PWD/../Libraries/vcpkg/installed/libcurl/include
DEPENDPATH += $$PWD/../Libraries/vcpkg/installed/libcurl/include


win32: LIBS += -L$$PWD/../Libraries/vcpkg/installed/x64-windows/lib/ -ljsoncpp
INCLUDEPATH += $$PWD/../Libraries/vcpkg/installed/x64-windows/include
DEPENDPATH += $$PWD/../Libraries/vcpkg/installed/x64-windows/include
