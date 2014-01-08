#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#define SERVICE_CON_TYPE "urn:schemas-upnp-org:service:ConnectionManager:1"
#define SERVICE_CON_ID "ConnectionManager"
#define SERVICE_CON_XML_PATH "/ConnectionManager/ConnectionManager.xml"
#define SERVICE_CON_CONTROL "/ConnectionManager/control"
#define SERVICE_CON_EVENT_SUB "/ConnectionManager/eventSub"


#include <BRisa/BrisaUpnp/BrisaAction>
#include <BRisa/BrisaUpnp/BrisaService>
#include <QDebug>

using namespace BrisaUpnp;

class GetProtocolInfo : public BrisaAction
{
    public:
        GetProtocolInfo(BrisaService *service) : BrisaAction("GetProtocolInfo", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            Q_UNUSED(inArguments);
            QMap<QString, QString> outArgs;
            qDebug()<<"GetProtocolInfo";
            outArgs.insert("Source", this->getStateVariable("SourceProtocolInfo")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("Sink", this->getStateVariable("SinkProtocolInfo")
                                               ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};

class PrepareForConnection : public BrisaAction
{
    public:
        PrepareForConnection(BrisaService *service) : BrisaAction("PrepareForConnection", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_ProtocolInfo")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["RemoteProtocolInfo"]);
            getStateVariable("A_ARG_TYPE_ConnectionManager")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["PeerConnectionManager"]);
            getStateVariable("A_ARG_TYPE_ConnectionID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["PeerConnectionID"]);
            getStateVariable("A_ARG_TYPE_Direction")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Direction"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"PrepareForConnection";
            outArgs.insert("ConnectionID", this->getStateVariable("A_ARG_TYPE_ConnectionID")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("AVTransportID", this->getStateVariable("A_ARG_TYPE_AVTransportID")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("RcsID", this->getStateVariable("A_ARG_TYPE_RcsID")
                                               ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};

class ConnectionComplete : public BrisaAction
{
    public:
        ConnectionComplete(BrisaService *service) : BrisaAction("ConnectionComplete", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_ConnectionID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["ConnectionID"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"ConnectionComplete";
            return outArgs;
        }
};

class GetCurrentConnectionIDs : public BrisaAction
{
    public:
        GetCurrentConnectionIDs(BrisaService *service) : BrisaAction("GetCurrentConnectionIDs", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            Q_UNUSED(inArguments);
            QMap<QString, QString> outArgs;
            qDebug()<<"GetCurrentConnectionIDs";
            outArgs.insert("ConnectionIDs", this->getStateVariable("CurrentConnectionIDs")
                                               ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};

class GetCurrentConnectionInfo : public BrisaAction
{
    public:
        GetCurrentConnectionInfo(BrisaService *service) : BrisaAction("GetCurrentConnectionInfo", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_ConnectionID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["ConnectionID"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"GetCurrentConnectionInfo";
            outArgs.insert("RcsID", this->getStateVariable("A_ARG_TYPE_RcsID")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("AVTransportID", this->getStateVariable("A_ARG_TYPE_AVTransportID")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("ProtocolInfo", this->getStateVariable("A_ARG_TYPE_ProtocolInfo")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("PeerConnectionManager", this->getStateVariable("A_ARG_TYPE_ConnectionManager")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("PeerConnectionID", this->getStateVariable("A_ARG_TYPE_ConnectionID")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("Direction", this->getStateVariable("A_ARG_TYPE_Direction")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("Status", this->getStateVariable("A_ARG_TYPE_ConnectionStatus")
                                               ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};

//The ConnectionManager Service
class ConnectionManager : public BrisaService
{
private:
    BrisaStateVariable *SourceProtocolInfo;
    BrisaStateVariable *SinkProtocolInfo;
    BrisaStateVariable *CurrentConnectionIDs;
    BrisaStateVariable *A_ARG_TYPE_ConnectionStatus;
    BrisaStateVariable *A_ARG_TYPE_ConnectionManager;
    BrisaStateVariable *A_ARG_TYPE_Direction;
    BrisaStateVariable *A_ARG_TYPE_ProtocolInfo;
    BrisaStateVariable *A_ARG_TYPE_ConnectionID;
    BrisaStateVariable *A_ARG_TYPE_AVTransportID;
    BrisaStateVariable *A_ARG_TYPE_RcsID;

    //Create variables
    void createConnectionVariables(){

        qDebug() <<"start createConnectionVariables ";
        SourceProtocolInfo=new BrisaStateVariable("yes","SourceProtocolInfo","string");
         //SourceProtocolInfo->addAllowedValue("");
         this->addStateVariable(SourceProtocolInfo);
        SinkProtocolInfo=new BrisaStateVariable("yes","SinkProtocolInfo","string");
         //SinkProtocolInfo->addAllowedValue("");
         this->addStateVariable(SinkProtocolInfo);
        CurrentConnectionIDs=new BrisaStateVariable("yes","SinkProtocolInfo","string");
         //CurrentConnectionIDs->addAllowedValue("");
         this->addStateVariable(CurrentConnectionIDs);
        A_ARG_TYPE_ConnectionStatus=new BrisaStateVariable("no","A_ARG_TYPE_ConnectionStatus","string");
         A_ARG_TYPE_ConnectionStatus->addAllowedValue("OK");
         A_ARG_TYPE_ConnectionStatus->addAllowedValue("InsufficientBandwidth");
         A_ARG_TYPE_ConnectionStatus->addAllowedValue("ContentFormatMismatch");
         A_ARG_TYPE_ConnectionStatus->addAllowedValue("UnreliableChannel");
         A_ARG_TYPE_ConnectionStatus->addAllowedValue("Unknown");
         //A_ARG_TYPE_ConnectionStatus->addAllowedValue("");
         this->addStateVariable(A_ARG_TYPE_ConnectionStatus);
        A_ARG_TYPE_ConnectionManager=new BrisaStateVariable("no","A_ARG_TYPE_ConnectionManager","string");
         //A_ARG_TYPE_ConnectionManager->addAllowedValue("");
         this->addStateVariable(A_ARG_TYPE_ConnectionManager);
        A_ARG_TYPE_Direction=new BrisaStateVariable("no","A_ARG_TYPE_Direction","string");
         A_ARG_TYPE_Direction->addAllowedValue("Input");
         A_ARG_TYPE_Direction->addAllowedValue("Output");
         //A_ARG_TYPE_Direction->addAllowedValue("");
         this->addStateVariable(A_ARG_TYPE_Direction);
        A_ARG_TYPE_ProtocolInfo=new BrisaStateVariable("no","A_ARG_TYPE_ProtocolInfo","string");
         //A_ARG_TYPE_ProtocolInfo->addAllowedValue("");
         this->addStateVariable(A_ARG_TYPE_ProtocolInfo);
        A_ARG_TYPE_ConnectionID=new BrisaStateVariable("no","A_ARG_TYPE_ConnectionID","i4");
        A_ARG_TYPE_AVTransportID=new BrisaStateVariable("no","A_ARG_TYPE_AVTransportID","i4");
        A_ARG_TYPE_RcsID=new BrisaStateVariable("no","A_ARG_TYPE_RcsID","i4");
        qDebug() <<"end createConnectionVariables ";

    }

    public:
        ConnectionManager() : BrisaService(SERVICE_CON_TYPE,
                                     SERVICE_CON_ID,
                                     SERVICE_CON_XML_PATH,
                                     SERVICE_CON_CONTROL,
                                     SERVICE_CON_EVENT_SUB)
        {
            addAction(new GetProtocolInfo(this));
            addAction(new PrepareForConnection(this));
            addAction(new ConnectionComplete(this));
            addAction(new GetCurrentConnectionIDs(this));
            addAction(new GetCurrentConnectionInfo(this));
            createConnectionVariables();
        }
};

#endif // CONNECTIONMANAGER_H
