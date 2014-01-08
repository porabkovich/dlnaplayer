#ifndef AVTRANSPORT_H
#define AVTRANSPORT_H

#include <BRisa/BrisaUpnp/BrisaAction>
#include <BRisa/BrisaUpnp/BrisaService>
#include <QDebug>


#define SERVICE_AVT_TYPE "urn:schemas-upnp-org:service:AVTransport:1"
#define SERVICE_AVT_ID "AVTransport"
#define SERVICE_AVT_XML_PATH "/AVTransport/AVTransport.xml"
#define SERVICE_AVT_CONTROL "/AVTransport/control"
#define SERVICE_AVT_EVENT_SUB "/AVTransport/eventSub"

using namespace BrisaUpnp;

class SetAVTransportURI : public BrisaAction
{
    Q_OBJECT
    public:
        SetAVTransportURI(BrisaService *service) : BrisaAction("SetAVTransportURI", service) {}
    //signals:
        //void getUri(QString );
    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            qDebug()<<"MY CurrentURI "<< inArguments["CurrentURI"];
            qDebug()<<"MY InstanceID "<< inArguments["InstanceID"];
            getStateVariable("AVTransportURI")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["CurrentURI"]);
            getStateVariable("LastChange")->setAttribute(BrisaStateVariable::Value,"AVTransportURI");
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            getStateVariable("AVTransportURIMetaData")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["CurrentURIMetaData"]);
            QString str=inArguments["CurrentURI"];
            QMap<QString, QString> outArgs;
            qDebug()<<"MY SetAVTransportURI";
            //emit getUri(str);
            return outArgs;
        }

};

class SetNextAVTransportURI : public BrisaAction
{
    public:
        SetNextAVTransportURI(BrisaService *service) : BrisaAction("SetNextAVTransportURI", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("NextAVTransportURI")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["NextURI"]);
            getStateVariable("LastChange")->setAttribute(BrisaStateVariable::Value,"NextAVTransportURI");
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            getStateVariable("NextAVTransportURIMetaData")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["NextURIMetaData"]);
            QMap<QString, QString> outArgs;
            qDebug()<<"SetNextAVTransportURI";
            return outArgs;
        }
};

class GetMediaInfo : public BrisaAction
{
    public:
        GetMediaInfo(BrisaService *service) : BrisaAction("GetMediaInfo", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            QMap<QString, QString> outArgs;
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            qDebug()<<"GetMediaInfo";
            outArgs.insert("NrTracks", this->getStateVariable("NumberOfTracks")
                                                 ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("MediaDuration", this->getStateVariable("CurrentMediaDuration")
                                                 ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("CurrentURI", this->getStateVariable("AVTransportURI")
                                                 ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("CurrentURIMetaData", this->getStateVariable("AVTransportURIMetaData")
                                                 ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("NextURI", this->getStateVariable("NextAVTransportURI")
                                                 ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("NextURIMetaData", this->getStateVariable("NextAVTransportURIMetaData")
                                                 ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("PlayMedium", this->getStateVariable("PlaybackStorageMedium")
                                                 ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("RecordMedium", this->getStateVariable("RecordStorageMedium")
                                                 ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("WriteStatus", this->getStateVariable("RecordMediumWriteStatus")
                                                 ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};

class GetTransportInfo : public BrisaAction
{
    public:
        GetTransportInfo(BrisaService *service) : BrisaAction("GetTransportInfo", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"GetTransportInfo TransportState"<<this->getStateVariable("TransportState")
                      ->getAttribute(BrisaStateVariable::Value);
            qDebug()<<"GetTransportInfo TransportStatus"<<this->getStateVariable("TransportStatus")
                      ->getAttribute(BrisaStateVariable::Value);

            qDebug()<<"GetTransportInfo TransportPlaySpeed"<<this->getStateVariable("TransportPlaySpeed")
                      ->getAttribute(BrisaStateVariable::Value);
            outArgs.insert("CurrentTransportState", this->getStateVariable("TransportState")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("CurrentTransportStatus", this->getStateVariable("TransportStatus")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("CurrentSpeed", this->getStateVariable("TransportPlaySpeed")
                                               ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};

class GetPositionInfo : public BrisaAction
{
    public:
        GetPositionInfo(BrisaService *service) : BrisaAction("GetPositionInfo", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            QMap<QString, QString> outArgs;
            qDebug()<<"My GetPositionInfo";
            outArgs.insert("Track", this->getStateVariable("CurrentTrack")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("TrackDuration", this->getStateVariable("CurrentTrackDuration")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("TrackMetaData", this->getStateVariable("CurrentTrackMetaData")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("TrackURI", this->getStateVariable("CurrentTrackURI")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("RelTime", this->getStateVariable("RelativeTimePosition")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("AbsTime", this->getStateVariable("AbsoluteTimePosition")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("RelCount", this->getStateVariable("RelativeCounterPosition")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("AbsCount", this->getStateVariable("AbsoluteCounterPosition")
                                               ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};


class GetDeviceCapabilities : public BrisaAction
{
    public:
        GetDeviceCapabilities(BrisaService *service) : BrisaAction("GetDeviceCapabilities", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"My GetDeviceCapabilities";
            outArgs.insert("PlayMedia", this->getStateVariable("PossiblePlaybackStorageMedia")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("RecMedia", this->getStateVariable("PossibleRecordStorageMedia")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("RecQualityModes", this->getStateVariable("PossibleRecordQualityModes")
                                               ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};

class GetTransportSettings : public BrisaAction
{
    public:
        GetTransportSettings(BrisaService *service) : BrisaAction("GetTransportSettings", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"My GetTransportSettings";
            outArgs.insert("PlayMode", this->getStateVariable("CurrentPlayMode")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("RecQualityMode", this->getStateVariable("CurrentRecordQualityMode")
                                               ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};

class Stop : public BrisaAction
{
    public:
        Stop(BrisaService *service) : BrisaAction("Stop", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            getStateVariable("LastChange")->setAttribute(BrisaStateVariable::Value,"Stop");

            QMap<QString, QString> outArgs;
            qDebug()<<"My Stop";
            return outArgs;
        }
};

class Play : public BrisaAction
{
    public:
        Play(BrisaService *service) : BrisaAction("Play", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            qDebug()<<"Play with speed"<< inArguments["TransportPlaySpeed"];
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            getStateVariable("TransportPlaySpeed")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Speed"]);
            getStateVariable("LastChange")->setAttribute(BrisaStateVariable::Value,"Play");

            QMap<QString, QString> outArgs;
            qDebug()<<"My Play";
            return outArgs;
        }
};

class Pause : public BrisaAction
{
    public:
        Pause(BrisaService *service) : BrisaAction("Pause", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            getStateVariable("LastChange")->setAttribute(BrisaStateVariable::Value,"Pause");

            qDebug()<<"Pause "<< inArguments["InstanceID"];
            QMap<QString, QString> outArgs;
            qDebug()<<"My Pause";
            return outArgs;
        }
};

class Record : public BrisaAction
{
    public:
        Record(BrisaService *service) : BrisaAction("Record", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            qDebug()<<"Record "<< inArguments["InstanceID"];
            QMap<QString, QString> outArgs;
            qDebug()<<"My Record";
            return outArgs;
        }
};

class Seek : public BrisaAction
{
    public:
        Seek(BrisaService *service) : BrisaAction("Seek", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            qDebug()<<"Seek "<< inArguments["InstanceID"];
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            getStateVariable("A_ARG_TYPE_SeekMode")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Unit"]);
            getStateVariable("A_ARG_TYPE_SeekTarget")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Target"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"My Seek";
            return outArgs;
        }
};

class Next : public BrisaAction
{
    public:
        Next(BrisaService *service) : BrisaAction("Next", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            qDebug()<<"Next "<< inArguments["InstanceID"];
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            getStateVariable("LastChange")->setAttribute(BrisaStateVariable::Value,"Next");

            QMap<QString, QString> outArgs;
            qDebug()<<"My Next";
            return outArgs;
        }
};

class Previous : public BrisaAction
{
    public:
        Previous(BrisaService *service) : BrisaAction("Previous", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            qDebug()<<"Previous "<< inArguments["InstanceID"];
            getStateVariable("LastChange")->setAttribute(BrisaStateVariable::Value,"Previous");
            QMap<QString, QString> outArgs;
            qDebug()<<"My Previous";
            return outArgs;
        }
};

class SetPlayMode : public BrisaAction
{
    public:
        SetPlayMode(BrisaService *service) : BrisaAction("SetPlayMode", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            getStateVariable("CurrentPlayMode")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["NewPlayMode"]);

            qDebug()<<"SetPlayMode "<< inArguments["InstanceID"];
            QMap<QString, QString> outArgs;
            qDebug()<<"My SetPlayMode";
            return outArgs;
        }
};

class SetRecordQualityMode : public BrisaAction
{
    public:
        SetRecordQualityMode(BrisaService *service) : BrisaAction("SetRecordQualityMode", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            getStateVariable("CurrentRecordQualityMode")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["NewRecordQualityMode"]);

            qDebug()<<"SetRecordQualityMode "<< inArguments["InstanceID"];
            QMap<QString, QString> outArgs;
            qDebug()<<"My SetRecordQualityMode";
            return outArgs;
        }
};

class GetCurrentTransportActions : public BrisaAction
{
    public:
        GetCurrentTransportActions(BrisaService *service) : BrisaAction("GetCurrentTransportActions", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            qDebug()<<"GetCurrentTransportActions "<< inArguments["InstanceID"];
            QMap<QString, QString> outArgs;
            qDebug()<<"My GetCurrentTransportActions";
            outArgs.insert("Actions", this->getStateVariable("CurrentTransportActions")
                                               ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};


//The AVTransport
class AVTransport : public BrisaService
{
    Q_OBJECT
    private:
        SetAVTransportURI *Settransport;
        //StateVariables
        BrisaStateVariable *TransportState;
        BrisaStateVariable *TransportStatus;
        BrisaStateVariable *PlaybackStorageMedium;
        BrisaStateVariable *RecordStorageMedium;
        BrisaStateVariable *PossiblePlaybackStorageMedia;
        BrisaStateVariable *PossibleRecordStorageMedia;
        BrisaStateVariable *CurrentPlayMode;
        BrisaStateVariable *TransportPlaySpeed;
        BrisaStateVariable *RecordMediumWriteStatus;
        BrisaStateVariable *CurrentRecordQualityMode;
        BrisaStateVariable *PossibleRecordQualityModes;
        BrisaStateVariable *NumberOfTracks;
        BrisaStateVariable *CurrentTrack;
        BrisaStateVariable *CurrentTrackDuration;
        BrisaStateVariable *CurrentMediaDuration;
        BrisaStateVariable *CurrentTrackMetaData;
        BrisaStateVariable *CurrentTrackURI;
        BrisaStateVariable *AVTransportURI;
        BrisaStateVariable *AVTransportURIMetaData;
        BrisaStateVariable *NextAVTransportURI;
        BrisaStateVariable *NextAVTransportURIMetaData;
        BrisaStateVariable *RelativeTimePosition;
        BrisaStateVariable *AbsoluteTimePosition;
        BrisaStateVariable *RelativeCounterPosition;
        BrisaStateVariable *AbsoluteCounterPosition;
        BrisaStateVariable *CurrentTransportActions;
        BrisaStateVariable *LastChange;
        BrisaStateVariable *A_ARG_TYPE_SeekMode;
        BrisaStateVariable *A_ARG_TYPE_SeekTarget;
        BrisaStateVariable *A_ARG_TYPE_InstanceID;

        //Create variables
        void createAVTransportVariables(){

            qDebug() <<"start createAVTransportVariables ";
            //BrisaUpnp::BrisaStateVariable::BrisaStateVariableAttribute t;

           TransportState=new BrisaStateVariable("no","TransportState","string");
                //TransportState->addAllowedValue("");
                TransportState->addAllowedValue("PLAYING");
                TransportState->addAllowedValue("STOPPED");
                //TransportState->setAttribute(BrisaStateVariable::SendEvents,"no");
                this->addStateVariable(TransportState);
            TransportStatus=new BrisaStateVariable("no","TransportStatus","string");
                //TransportStatus->addAllowedValue("");
                TransportStatus->addAllowedValue("OK");
                TransportStatus->addAllowedValue("ERROR_OCCURRED");
                this->addStateVariable(TransportStatus);

            PlaybackStorageMedium=new BrisaStateVariable("no","PlaybackStorageMedium","string");
                //PlaybackStorageMedium->addAllowedValue("");
                this->addStateVariable(PlaybackStorageMedium);
            RecordStorageMedium=new BrisaStateVariable("no","RecordStorageMedium","string");
                //RecordStorageMedium->addAllowedValue("");
                this->addStateVariable(RecordStorageMedium);
            PossiblePlaybackStorageMedia=new BrisaStateVariable("no","PossiblePlaybackStorageMedia","string");
                //PossiblePlaybackStorageMedia->addAllowedValue("");
                this->addStateVariable(PossiblePlaybackStorageMedia);
            PossibleRecordStorageMedia=new BrisaStateVariable("no","PossibleRecordStorageMedia","string");
                //PossibleRecordStorageMedia->addAllowedValue("");
                this->addStateVariable(PossibleRecordStorageMedia);
            CurrentPlayMode=new BrisaStateVariable("no","CurrentPlayMode","string","NORMAL");
                //CurrentPlayMode->addAllowedValue("");
                this->addStateVariable(CurrentPlayMode);
            TransportPlaySpeed=new BrisaStateVariable("no","TransportPlaySpeed","string");
                //TransportPlaySpeed->addAllowedValue("");
                this->addStateVariable(TransportPlaySpeed);
            RecordMediumWriteStatus=new BrisaStateVariable("no","RecordMediumWriteStatus","string");
                //RecordMediumWriteStatus->addAllowedValue("");
                this->addStateVariable(RecordMediumWriteStatus);
            CurrentRecordQualityMode=new BrisaStateVariable("no","CurrentRecordQualityMode","string");
                //CurrentRecordQualityMode->addAllowedValue("");
                this->addStateVariable(CurrentRecordQualityMode);
            PossibleRecordQualityModes=new BrisaStateVariable("no","PossibleRecordQualityModes","string");
                //PossibleRecordQualityModes->addAllowedValue("");
                this->addStateVariable(PossibleRecordQualityModes);
            NumberOfTracks=new BrisaStateVariable("no","NumberOfTracks","ui4","","","0");
            CurrentTrack=new BrisaStateVariable("no","CurrentTrack","ui4","","","0","1");
            CurrentTrackDuration=new BrisaStateVariable("no","CurrentTrackDuration","string");
                //CurrentTrackDuration->addAllowedValue("");
                this->addStateVariable(CurrentTrackDuration);
            CurrentMediaDuration=new BrisaStateVariable("no","CurrentMediaDuration","string");
                //CurrentMediaDuration->addAllowedValue("");
                this->addStateVariable(CurrentMediaDuration);
            CurrentTrackMetaData=new BrisaStateVariable("no","CurrentTrackMetaData","string");
                //CurrentTrackMetaData->addAllowedValue("");
                this->addStateVariable(CurrentTrackMetaData);
            CurrentTrackURI=new BrisaStateVariable("no","CurrentTrackURI","string");
                //CurrentTrackURI->addAllowedValue("");
                this->addStateVariable(CurrentTrackURI);
            AVTransportURI=new BrisaStateVariable("no","AVTransportURI","string");
                //AVTransportURI->addAllowedValue("");
                //AVTransportURI->setAttribute(BrisaStateVariable::SendEvents,"yes");
                this->addStateVariable(AVTransportURI);
            AVTransportURIMetaData=new BrisaStateVariable("no","AVTransportURIMetaData","string");
                //AVTransportURIMetaData->addAllowedValue("");
                this->addStateVariable(AVTransportURIMetaData);
            NextAVTransportURI=new BrisaStateVariable("no","NextAVTransportURI","string");
                //NextAVTransportURI->addAllowedValue("");
                this->addStateVariable(NextAVTransportURI);
            NextAVTransportURIMetaData=new BrisaStateVariable("no","NextAVTransportURIMetaData","string");
                //NextAVTransportURIMetaData->addAllowedValue("");
                this->addStateVariable(NextAVTransportURIMetaData);
            RelativeTimePosition=new BrisaStateVariable("no","RelativeTimePosition","string");
                //RelativeTimePosition->addAllowedValue("");
                this->addStateVariable(RelativeTimePosition);
            AbsoluteTimePosition=new BrisaStateVariable("no","AbsoluteTimePosition","string");
                //AbsoluteTimePosition->addAllowedValue("");
                this->addStateVariable(AbsoluteTimePosition);
            RelativeCounterPosition=new BrisaStateVariable("no","AVTransportURIMetaData","i4");
            AbsoluteCounterPosition=new BrisaStateVariable("no","NextAVTransportURI","i4");
            CurrentTransportActions=new BrisaStateVariable("no","NextAVTransportURIMetaData","string");
                //CurrentTransportActions->addAllowedValue("");
                this->addStateVariable(CurrentTransportActions);
            LastChange=new BrisaStateVariable("yes","LastChange","string");
                //LastChange->addAllowedValue("");
                this->addStateVariable(LastChange);
            A_ARG_TYPE_SeekMode=new BrisaStateVariable("no","A_ARG_TYPE_SeekMode","string");
                //A_ARG_TYPE_SeekMode->addAllowedValue("");
                A_ARG_TYPE_SeekMode->addAllowedValue("TRACK_NR");
                A_ARG_TYPE_InstanceID=new BrisaStateVariable("no","A_ARG_TYPE_InstanceID","ui4");
                this->addStateVariable(A_ARG_TYPE_SeekMode);
            qDebug() <<"end createAVTransportVariables ";

        }


    signals:
        void getAVTUri(QString);
        void AvtStop();

    private slots:
        void gotUri(QString s){
            emit getAVTUri(s);
        }
        void StateChanged(BrisaStateVariable *var)
        {
            qDebug()<<"StateChanged " << var->getAttribute(BrisaStateVariable::Name);
            qDebug()<<"StateChanged " << var->getAttribute(BrisaStateVariable::Value);

            QString StateVariable=var->getAttribute(BrisaStateVariable::Value);

            if(StateVariable=="AVTransportURI"){
                qDebug()<<"Changed AVTransportURI"<<this->getVariable("AVTransportURI")->getAttribute(BrisaStateVariable::Value);
                emit getAVTUri(this->getVariable("AVTransportURI")->getAttribute(BrisaStateVariable::Value));
            }
            else
                if(StateVariable=="Play"){
                    qDebug()<<"Changed Play "<<this->getVariable("AVTransportURI")->getAttribute(BrisaStateVariable::Value);
                    emit getAVTUri(this->getVariable("AVTransportURI")->getAttribute(BrisaStateVariable::Value));
            }
                else
                    if(StateVariable=="Next"){
                        qDebug()<<"Changed Next "<<this->getVariable("NextAVTransportURI")->getAttribute(BrisaStateVariable::Value);
                        emit getAVTUri(this->getVariable("NextAVTransportURI")->getAttribute(BrisaStateVariable::Value));
                    }
                    else
                        if(StateVariable=="Previous"){
                            qDebug()<<"Changed Previous "<<this->getVariable("NextAVTransportURI")->getAttribute(BrisaStateVariable::Value);
                            emit getAVTUri(this->getVariable("NextAVTransportURI")->getAttribute(BrisaStateVariable::Value));
                        }
                        else{
                            if(StateVariable=="Stop"){
                                qDebug()<<"Changed Stop "<<this->getVariable("NextAVTransportURI")->getAttribute(BrisaStateVariable::Value);
                                emit  AvtStop();
                            }
                            else{
                                qDebug()<<"Last Changed "<<this->getVariable("LastChange")->getValue();
                            }
                        }
        }
    public:
        AVTransport() : BrisaService(SERVICE_AVT_TYPE,
                                     SERVICE_AVT_ID,
                                     SERVICE_AVT_XML_PATH,
                                     SERVICE_AVT_CONTROL,
                                     SERVICE_AVT_EVENT_SUB)
        {
            Settransport=new SetAVTransportURI(this);
            //connect(Settransport,SIGNAL(getUri(QString )),this, SLOT(gotUri(QString)));
            addAction(Settransport);
            addAction(new GetTransportInfo(this));
            addAction(new GetMediaInfo(this));
            addAction(new SetNextAVTransportURI(this));
            addAction(new Play(this));
            addAction(new GetPositionInfo(this));
            addAction(new GetCurrentTransportActions(this));
            addAction(new SetRecordQualityMode(this));
            addAction(new SetPlayMode(this));
            addAction(new Previous(this));
            addAction(new Next(this));
            addAction(new Seek(this));
            addAction(new Record(this));
            addAction(new Pause(this));
            addAction(new Stop(this));
            addAction(new GetTransportSettings(this));
            addAction(new GetDeviceCapabilities(this));
            createAVTransportVariables();
            connect(this->LastChange, SIGNAL(changed(BrisaStateVariable *)), this, SLOT(StateChanged(BrisaStateVariable *)));
        }
};
#endif // AVTRANSPORT_H
