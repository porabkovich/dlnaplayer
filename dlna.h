#ifndef DLNA_H
#define DLNA_H

#include <QtGui/QWidget>
#include <QString>
#include <QIcon>
#include <QtCore>
#include <QCoreApplication>
#include <QDebug>
#include <QSplashScreen>

#include <BRisa/BrisaUpnp/BrisaDevice>
#include <BRisa/BrisaUtils/BrisaLog>

#include "dlnaservice.h"

using namespace BrisaUpnp;

#define DEVICE_TYPE              "urn:schemas-upnp-org:device:MediaRenderer:1"
#define DEVICE_FRIENDLY_NAME     "Eltex MediaRenderer"
#define DEVICE_MANUFACTURER      "Brisa Team. Embedded Laboratory and INdT Brazil"
#define DEVICE_MANUFACTURER_URL  "https://garage.maemo.org/projects/brisa"
#define DEVICE_MODEL_DESCRIPTION "An UPnP Binary Light Device"
#define DEVICE_MODEL_NAME        "Eltex_MediaRenderer"
#define DEVICE_MODEL_NUMBER      "0.1"
#define DEVICE_MODEL_URL         "https://garage.maemo.org/projects/brisa"
#define DEVICE_SERIAL_NUMBER     "1.0"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
signals:
    void ReadyToPlay(QString);
    void WindowStop();

public slots:
    void statechanged(BrisaStateVariable *);
    void gotAVTUri(QString s);
    void WidgetStop();

private:
    BrisaDevice binaryLight;
    AVTransport *avtransport;
    ConnectionManager *connection;
    RenderingControl *rendering;

    //METHODS FOR UDN CREATION
    QString createUdn();
    int get1RandomNumber();
    int get2RandomNumber();
    int get3RandomNumber();

};

#endif // DLNA_H
