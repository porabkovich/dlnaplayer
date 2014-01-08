#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include <BRisa/BrisaUpnp/BrisaControlPoint>
#include <QList>
#include <QString>
#include <QtDebug>
#include <QTextStream>
#include <QCoreApplication>
#include <iostream>
 // To create the Control Point
//The binary light type
#define BINARY_LIGHT_TYPE    "urn:schemas-upnp-org:device:BinaryLight:1"

//Service that contains the actions we'll use
#define SERVICE_SWITCH_POWER "urn:schemas-upnp-org:service:SwitchPower:1"

using namespace BrisaUpnp;

class HandleCmds : public QThread
{
    Q_OBJECT
    public:
        void run() {
            QTextStream stream(stdin);
            do{
                QString line;
                QCoreApplication::processEvents();
                std::cout << ">>> ";
                line = stream.readLine();
                if(line == "exit") {
                    emit leave();
                    running = false;
                } else if(line == "list") {
                    emit list();
                } else if(line == "help"){
                    emit help();
                } else if(line == "get_target"){
                    emit getTarget();
                } else if(line == "get_status"){
                    emit getStatus();
                } else if(line == "turn on"){
                    emit turnOn();
                } else if(line == "turn off"){
                    emit turnOff();
                } else {
                    if (line.split(" ").size() == 2) {
                        if(line.split(" ")[0] == "set_light") {
                            emit setLight(line.split(" ")[1].toInt());
                        } else {
                            qDebug() << "Wrong usage, try 'help' to see the commands";
                        }
                    } else {
                        qDebug() << "Wrong usage, try 'help' to see the commands";
                    }
                }
            } while(running);
        }

    private:
        void setRunningCmds(bool running) { this->running = running; }

        bool running;

    signals:
        void leave();
        void list();
        void help();
        void setLight(int i);
        void turnOn();
        void turnOff();
        void getTarget();
        void getStatus();
};



class ControlPoint : BrisaControlPoint
{
    Q_OBJECT
    public:
        ControlPoint();   //ControlPoint Constructor
        ~ControlPoint();  //ControlPoint Destructor


    private:
        QString listServices(BrisaControlPointDevice *dev);
        QString listEmbeddedDevices(BrisaControlPointDevice *dev);

        QList <BrisaControlPointDevice*> devices;  //List to store the devices
        BrisaControlPointDevice *selected;         //Device to interact
        bool runningHandleCmds;
        HandleCmds *handle;                        //Thread that handle the commands

    private slots:
        void help();                 // Lists all commands
        void setLight(int index);    // Selects a device
        void getTarget();            // Calls the getTarget action
        void getStatus();            // Calls the getStatus action
        void turnOn();               // Turns on the light
        void turnOff();              // Turns off the light
        void exit();                 // Exits the application
        void list();                 // Lists all the devices;
        // Gets the SetTarget response
        void setTargetResponse(QString response, QString method);
        // Gets the GetTarget response
        void getTargetResponse(QString response, QString method);
        // Gets the GetStatus response
        void getStatusResponse(QString response, QString method);
        //Slot used when a device joins the network.
        void onNewDevice(BrisaControlPointDevice *dev);
        //Slot used when a device leaves the network.
        void onRemovedDevice(QString desc);
        //Slot used to handle error
        void requestError(QString errorMessage, QString methodName);
};



#endif // CONTROLPOINT_H
