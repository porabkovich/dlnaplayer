#include "dlna.h"
#include <QDebug>

using namespace BrisaUpnp;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    binaryLight(DEVICE_TYPE,
                DEVICE_FRIENDLY_NAME,
                DEVICE_MANUFACTURER,
                DEVICE_MANUFACTURER_URL,
                DEVICE_MODEL_DESCRIPTION,
                DEVICE_MODEL_NAME,
                DEVICE_MODEL_NUMBER,
                DEVICE_MODEL_URL,
                DEVICE_SERIAL_NUMBER,
                createUdn())
{
    qDebug() << "Brisa Device 1";
    avtransport = new AVTransport();
    connection=new ConnectionManager();
    rendering=new RenderingControl();

    avtransport->setDescriptionFile("/sdk/AVTransport1.xml");
    connection->setDescriptionFile("/sdk/ConnectionManager1.xml");
    rendering->setDescriptionFile("/sdk/RenderingControl1.xml");
    binaryLight.addService(connection);
    binaryLight.addService(rendering);
    binaryLight.addService(avtransport);
    binaryLight.start();
    //this->TransportState = binaryLight.getServiceByType("urn:schemas-upnp-org:service:AVTransport:1")->getVariable("AVTransportURI");
    //qDebug()<<"this->status " << this->TransportState;
    //this->target = binaryLight.getServiceByType("urn:schemas-upnp-org:service:AVTransport:1")->getVariable("Target");*/
    connect(this->avtransport, SIGNAL(getAVTUri(QString )), this, SLOT(gotAVTUri(QString )));
    connect(this->avtransport, SIGNAL(AvtStop()), this, SLOT(WidgetStop()));
}

Widget::~Widget()
{
    qDebug()<<"start ~Widget()";
    /*delete status;
    delete target;*/
    qDebug()<<"start delete avtransport()";
    /*delete avtransport;
    qDebug()<<"start delete connection()";
    delete connection;
    qDebug()<<"start delete rendering()";
    delete rendering;*/
    qDebug()<<"end ~Widget()";

}

void Widget::statechanged(BrisaStateVariable *var)
{
    qDebug() <<" statechanged "<< BrisaStateVariable::Value;
    if(var->getAttribute(BrisaStateVariable::Value) == "1") {
        qDebug() << "Light switched on";
    } else {
        qDebug() << "Light switched off";
    }
}

void Widget::gotAVTUri(QString s)
{
    qDebug()<<"Widget gotAVTUri "<<s;
    emit ReadyToPlay(s);
}

void Widget::WidgetStop()
{
    qDebug()<<"Widget Stop";
    emit WindowStop();
}

QString Widget::createUdn()
{
    QString randNumber1;
    QString randNumber2;
    QString randNumber3;
    QString randNumber4;
    QString randNumber5;
    QString randNumber6;
    QString randNumber7;
    QString udn;

    randNumber1.setNum(get3RandomNumber());
    randNumber2.setNum(get1RandomNumber());
    randNumber3.setNum(get1RandomNumber());
    randNumber4.setNum(get2RandomNumber());
    randNumber5.setNum(get1RandomNumber());
    randNumber6.setNum(get1RandomNumber());
    randNumber7.setNum(get3RandomNumber());

    //udn = "uuid:" + QString(randNumber1) + QString(randNumber2) + QString(randNumber3) +"a4fa7-cf68-4cc8-844d-0af4c615cecb";
    //udn="136a6fa6-cf46-6cc6-136d-0af4c615cecb";
    udn.append("uuid:");
    udn.append(randNumber1);
    udn.append("a");
    udn.append(randNumber2);
    udn.append("fa");
    udn.append(randNumber3);
    udn.append("-cf");
    udn.append(randNumber4);
    udn.append("-");
    udn.append(randNumber5);
    udn.append("cc");
    udn.append(randNumber6);
    udn.append("-");
    udn.append(randNumber7);
    udn.append("d-");
    udn.append("0af4c615cecb");

    qDebug()<<"udn"<< udn;
    return udn;
}

int Widget::get1RandomNumber()
{
    srand(time(NULL));
    return rand() % 10;
}

int Widget::get2RandomNumber()
{
    srand(time(NULL));
    return rand() % 90 + 10;
}

int Widget::get3RandomNumber()
{
    srand(time(NULL));
    return rand() % 900 + 100;
}
