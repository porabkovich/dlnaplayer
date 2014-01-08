#ifndef DLNASERVICE_H
#define DLNASERVICE_H


#define SERVICE_REND_TYPE "urn:schemas-upnp-org:service:RenderingControl:1"
#define SERVICE_REND_ID "RenderingControl"
#define SERVICE_REND_XML_PATH "/RenderingControl/RenderingControl.xml"
#define SERVICE_REND_CONTROL "/RenderingControl/control"
#define SERVICE_REND_EVENT_SUB "/RenderingControl/eventSub"

#include <BRisa/BrisaUpnp/BrisaAction>
#include <BRisa/BrisaUpnp/BrisaService>
#include "avtransport.h"
#include "connectionmanager.h"
#include <QDebug>

using namespace BrisaUpnp;

class ListPresets : public BrisaAction
{
    public:
        ListPresets(BrisaService *service) : BrisaAction("ListPresets", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"ListPresets";
            outArgs.insert("CurrentPresetNameList", this->getStateVariable("PresetNameList")
                                               ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};


class SelectPreset : public BrisaAction
{
    public:
        SelectPreset(BrisaService *service) : BrisaAction("SelectPreset", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("A_ARG_TYPE_PresetName")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["PresetName"]);
            QMap<QString, QString> outArgs;
            qDebug()<<"SelectPreset";
            return outArgs;
        }
};

class GetBrightness : public BrisaAction
{
    public:
        GetBrightness(BrisaService *service) : BrisaAction("GetBrightness", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            qDebug()<<"GetBrightness";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentBrightness", this->getStateVariable("Brightness")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetBrightness : public BrisaAction
{
    public:
        SetBrightness(BrisaService *service) : BrisaAction("SetBrightness", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("Brightness")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredBrightness"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetBrightness";
            return outArgs;
        }
};

class GetContrast : public BrisaAction
{
    public:
        GetContrast(BrisaService *service) : BrisaAction("GetContrast", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            qDebug()<<"GetContrast";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentContrast", this->getStateVariable("Contrast")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetContrast : public BrisaAction
{
    public:
        SetContrast(BrisaService *service) : BrisaAction("SetContrast", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("Contrast")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredContrast"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetContrast";
            return outArgs;
        }
};

class GetSharpness : public BrisaAction
{
    public:
        GetSharpness(BrisaService *service) : BrisaAction("GetSharpness", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);
            qDebug()<<"GetSharpness";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentSharpness", this->getStateVariable("Sharpness")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetSharpness : public BrisaAction
{
    public:
        SetSharpness(BrisaService *service) : BrisaAction("SetSharpness", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("Sharpness")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredSharpness"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetSharpness";
            return outArgs;
        }
};

class GetRedVideoGain : public BrisaAction
{
    public:
        GetRedVideoGain(BrisaService *service) : BrisaAction("GetRedVideoGain", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);


            qDebug()<<"GetRedVideoGain";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentRedVideoGain", this->getStateVariable("RedVideoGain")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetRedVideoGain : public BrisaAction
{
    public:
        SetRedVideoGain(BrisaService *service) : BrisaAction("SetRedVideoGain", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("RedVideoGain")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredRedVideoGain"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetRedVideoGain";
            return outArgs;
        }
};

class GetGreenVideoGain : public BrisaAction
{
    public:
        GetGreenVideoGain(BrisaService *service) : BrisaAction("GetGreenVideoGain", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);


            qDebug()<<"GetGreenVideoGain";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentGreenVideoGain", this->getStateVariable("CurrentGreenVideoGain")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetGreenVideoGain : public BrisaAction
{
    public:
        SetGreenVideoGain(BrisaService *service) : BrisaAction("SetGreenVideoGain", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("GreenVideoGain")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredGreenVideoGain"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetGreenVideoGain";
            return outArgs;
        }
};

class GetBlueVideoGain : public BrisaAction
{
    public:
        GetBlueVideoGain(BrisaService *service) : BrisaAction("GetBlueVideoGain", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            qDebug()<<"GetBlueVideoGain";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentBlueVideoGain", this->getStateVariable("BlueVideoGain")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetBlueVideoGain : public BrisaAction
{
    public:
        SetBlueVideoGain(BrisaService *service) : BrisaAction("SetBlueVideoGain", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("BlueVideoGain")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredBlueVideoGain"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetGreenVideoGain";
            return outArgs;
        }
};

class GetRedVideoBlackLevel : public BrisaAction
{
    public:
        GetRedVideoBlackLevel(BrisaService *service) : BrisaAction("GetRedVideoBlackLevel", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            qDebug()<<"GetRedVideoBlackLevel";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentRedVideoBlackLevel", this->getStateVariable("RedVideoBlackLevel")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetRedVideoBlackLevel : public BrisaAction
{
    public:
        SetRedVideoBlackLevel(BrisaService *service) : BrisaAction("SetRedVideoBlackLevel", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("RedVideoBlackLevel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredRedVideoBlackLevel"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetRedVideoBlackLevel";
            return outArgs;
        }
};

class GetGreenVideoBlackLevel : public BrisaAction
{
    public:
        GetGreenVideoBlackLevel(BrisaService *service) : BrisaAction("GetGreenVideoBlackLevel", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);


            qDebug()<<"GetGreenVideoBlackLevel";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentGreenVideoBlackLevel", this->getStateVariable("GreenVideoBlackLevel")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetGreenVideoBlackLevel : public BrisaAction
{
    public:
        SetGreenVideoBlackLevel(BrisaService *service) : BrisaAction("SetGreenVideoBlackLevel", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("GreenVideoBlackLevel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredGreenVideoBlackLevel"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetGreenVideoBlackLevel";
            return outArgs;
        }
};

class GetBlueVideoBlackLevel : public BrisaAction
{
    public:
        GetBlueVideoBlackLevel(BrisaService *service) : BrisaAction("GetBlueVideoBlackLevel", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);


            qDebug()<<"GetBlueVideoBlackLevel";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentBlueVideoBlackLevel", this->getStateVariable("BlueVideoBlackLevel")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetBlueVideoBlackLevel : public BrisaAction
{
    public:
        SetBlueVideoBlackLevel(BrisaService *service) : BrisaAction("SetBlueVideoBlackLevel", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("BlueVideoBlackLevel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredBlueVideoBlackLevel"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetBlueVideoBlackLevel";
            return outArgs;
        }
};

class GetColorTemperature : public BrisaAction
{
    public:
        GetColorTemperature(BrisaService *service) : BrisaAction("GetColorTemperature", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);


            qDebug()<<"GetColorTemperature";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentColorTemperature", this->getStateVariable("ColorTemperature")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetColorTemperature : public BrisaAction
{
    public:
        SetColorTemperature(BrisaService *service) : BrisaAction("SetColorTemperature", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("ColorTemperature")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredColorTemperature"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetColorTemperature";
            return outArgs;
        }
};

class GetHorizontalKeystone : public BrisaAction
{
    public:
        GetHorizontalKeystone(BrisaService *service) : BrisaAction("GetHorizontalKeystone", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);


            qDebug()<<"GetHorizontalKeystone";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentHorizontalKeystone", this->getStateVariable("HorizontalKeystone")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetHorizontalKeystone : public BrisaAction
{
    public:
        SetHorizontalKeystone(BrisaService *service) : BrisaAction("SetHorizontalKeystone", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("HorizontalKeystone")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredHorizontalKeystone"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetHorizontalKeystone";
            return outArgs;
        }
};

class GetVerticalKeystone : public BrisaAction
{
    public:
        GetVerticalKeystone(BrisaService *service) : BrisaAction("GetVerticalKeystone", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            qDebug()<<"GetVerticalKeystone";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentVerticalKeystone", this->getStateVariable("VerticalKeystone")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetVerticalKeystone : public BrisaAction
{
    public:
        SetVerticalKeystone(BrisaService *service) : BrisaAction("SetVerticalKeystone", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("VerticalKeystone")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredVerticalKeystone"]);

            QMap<QString, QString> outArgs;
            qDebug()<<"SetVerticalKeystone";
            return outArgs;
        }
};

class GetMute : public BrisaAction
{
    public:
        GetMute(BrisaService *service) : BrisaAction("GetMute", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("A_ARG_TYPE_Channel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Channel"]);
            qDebug()<<"GetMute";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentMute", this->getStateVariable("Mute")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetMute : public BrisaAction
{
    public:
        SetMute(BrisaService *service) : BrisaAction("SetMute", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("A_ARG_TYPE_Channel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Channel"]);
            getStateVariable("Mute")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredMute"]);


            QMap<QString, QString> outArgs;
            qDebug()<<"SetMute";
            return outArgs;
        }
};

class GetVolume : public BrisaAction
{
    public:
        GetVolume(BrisaService *service) : BrisaAction("GetVolume", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("A_ARG_TYPE_Channel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Channel"]);

            qDebug()<<"GetVolume";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentVolume", this->getStateVariable("Volume")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetVolume : public BrisaAction
{
    public:
        SetVolume(BrisaService *service) : BrisaAction("SetVolume", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("A_ARG_TYPE_Channel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Channel"]);
            getStateVariable("Volume")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredVolume"]);


            QMap<QString, QString> outArgs;
            qDebug()<<"SetVolume";
            return outArgs;
        }
};

class GetVolumeDB : public BrisaAction
{
    public:
        GetVolumeDB(BrisaService *service) : BrisaAction("GetVolumeDB", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("A_ARG_TYPE_Channel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Channel"]);

            qDebug()<<"GetVolumeDB";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentVolumeDB", this->getStateVariable("VolumeDB")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class SetVolumeDB : public BrisaAction
{
    public:
        SetVolumeDB(BrisaService *service) : BrisaAction("SetVolumeDB", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("A_ARG_TYPE_Channel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Channel"]);
            getStateVariable("VolumeDB")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredVolume"]);


            QMap<QString, QString> outArgs;
            qDebug()<<"SetVolumeDB";
            return outArgs;
        }
};

class GetVolumeDBRange : public BrisaAction
{
    public:
        GetVolumeDBRange(BrisaService *service) : BrisaAction("GetVolumeDBRange", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("A_ARG_TYPE_Channel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Channel"]);

            qDebug()<<"GetVolumeDBRange";
            QMap<QString, QString> outArgs;
            outArgs.insert("MinValue", this->getStateVariable("VolumeDB")
                                               ->getAttribute(BrisaStateVariable::Value));
            outArgs.insert("MaxValue", this->getStateVariable("VolumeDB")
                                               ->getAttribute(BrisaStateVariable::Value));

           return outArgs;
        }
};


class GetLoudness : public BrisaAction
{
    public:
        GetLoudness(BrisaService *service) : BrisaAction("GetLoudness", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("A_ARG_TYPE_Channel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Channel"]);


            qDebug()<<"GetLoudness";
            QMap<QString, QString> outArgs;
            outArgs.insert("CurrentLoudness", this->getStateVariable("Loudness")
                                               ->getAttribute(BrisaStateVariable::Value));
            return outArgs;
        }
};

class SetLoudness : public BrisaAction
{
    public:
        SetLoudness(BrisaService *service) : BrisaAction("SetLoudness", service) {}

    private:
        QMap<QString, QString> run(const QMap<QString, QString> &inArguments)
        {
            getStateVariable("A_ARG_TYPE_InstanceID")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["InstanceID"]);

            getStateVariable("A_ARG_TYPE_Channel")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["Channel"]);
            getStateVariable("Loudness")->setAttribute(BrisaStateVariable::Value,
                                                               inArguments["DesiredLoudness"]);


            QMap<QString, QString> outArgs;
            qDebug()<<"SetLoudness";
            return outArgs;
        }
};


// The  RenderingControl Service
class RenderingControl : public BrisaService
{
private:
    BrisaStateVariable *PresetNameList;
    BrisaStateVariable *LastChange;
    BrisaStateVariable *Brightness;
    BrisaStateVariable *Contrast;
    BrisaStateVariable *Sharpness;
    BrisaStateVariable *RedVideoGain;
    BrisaStateVariable *GreenVideoGain;
    BrisaStateVariable *BlueVideoGain;
    BrisaStateVariable *RedVideoBlackLevel;
    BrisaStateVariable *GreenVideoBlackLevel;
    BrisaStateVariable *BlueVideoBlackLevel;
    BrisaStateVariable *ColorTemperature;
    BrisaStateVariable *HorizontalKeystone;
    BrisaStateVariable *VerticalKeystone;
    BrisaStateVariable *Mute;
    BrisaStateVariable *Volume;
    BrisaStateVariable *VolumeDB;
    BrisaStateVariable *Loudness;
    BrisaStateVariable *A_ARG_TYPE_Channel;
    BrisaStateVariable *A_ARG_TYPE_InstanceID;
    BrisaStateVariable *A_ARG_TYPE_PresetName;

    //Create variables
    void createRenderingControlVariables(){

        qDebug() <<"start createRenderingControlVariables ";
        PresetNameList=new BrisaStateVariable("no","PresetNameList","string");
         //PresetNameList->setAttribute(BrisaStateVariable::Value,"no");
         //PresetNameList->addAllowedValue("");
         this->addStateVariable(PresetNameList);
        LastChange=new BrisaStateVariable("yes","LastChange","string");
         //LastChange->addAllowedValue("");
         this->addStateVariable(LastChange);
        Brightness=new BrisaStateVariable("no","Brightness","ui2","","","0","1");
        Contrast=new BrisaStateVariable("no","Contrast","ui2","","","0","1");
        Sharpness=new BrisaStateVariable("no","Sharpness","ui2","","","0","1");
        RedVideoGain=new BrisaStateVariable("no","RedVideoGain","ui2");
        GreenVideoGain=new BrisaStateVariable("no","GreenVideoGain","ui2","","","0","1");
        BlueVideoGain=new BrisaStateVariable("no","BlueVideoGain","ui2","","","0","1");
        RedVideoBlackLevel=new BrisaStateVariable("no","RedVideoBlackLevel","ui2","","","0","1");
        GreenVideoBlackLevel=new BrisaStateVariable("no","GreenVideoBlackLevel","ui2","","","0","1");
        BlueVideoBlackLevel=new BrisaStateVariable("no","BlueVideoBlackLevel","ui2","","","0","1");
        ColorTemperature=new BrisaStateVariable("no","ColorTemperature","ui2","","","0","1");
        HorizontalKeystone=new BrisaStateVariable("no","ColorTemperature","i2","","","","1");
        VerticalKeystone=new BrisaStateVariable("no","VerticalKeystone","i2","","","","1");
        Mute=new BrisaStateVariable("no","Mute","boolean");
        Volume=new BrisaStateVariable("no","Volume","ui2","","","0","1");
        VolumeDB=new BrisaStateVariable("no","VolumeDB","i2");
        Loudness=new BrisaStateVariable("no","Loudness","boolean");
        A_ARG_TYPE_Channel=new BrisaStateVariable("no","A_ARG_TYPE_Channel","string");
         A_ARG_TYPE_Channel->addAllowedValue("Master");
         //A_ARG_TYPE_Channel->addAllowedValue("");
         this->addStateVariable(A_ARG_TYPE_Channel);
        A_ARG_TYPE_InstanceID=new BrisaStateVariable("no","A_ARG_TYPE_InstanceID","ui4");
        A_ARG_TYPE_PresetName=new BrisaStateVariable("no","A_ARG_TYPE_PresetName","string");
         //A_ARG_TYPE_PresetName->addAllowedValue("");
         A_ARG_TYPE_PresetName->addAllowedValue("FactoryDefaults");
         this->addStateVariable(A_ARG_TYPE_PresetName);
        qDebug() <<"end createRenderingControlVariables ";

    }
public:
        RenderingControl() : BrisaService(SERVICE_REND_TYPE,
                                     SERVICE_REND_ID,
                                     SERVICE_REND_XML_PATH,
                                     SERVICE_REND_CONTROL,
                                     SERVICE_REND_EVENT_SUB)
        {
            addAction(new ListPresets(this));
            addAction(new SelectPreset(this));
            addAction(new GetBrightness(this));
            addAction(new SetBrightness(this));
            addAction(new GetContrast(this));
            addAction(new SetContrast(this));
            addAction(new GetSharpness(this));
            addAction(new SetSharpness(this));
            addAction(new GetRedVideoGain(this));
            addAction(new SetRedVideoGain(this));
            addAction(new GetGreenVideoGain(this));
            addAction(new SetGreenVideoGain(this));
            addAction(new GetBlueVideoGain(this));
            addAction(new SetBlueVideoGain(this));
            addAction(new GetRedVideoBlackLevel(this));
            addAction(new SetRedVideoBlackLevel(this));
            addAction(new GetGreenVideoBlackLevel(this));
            addAction(new SetGreenVideoBlackLevel(this));
            addAction(new GetBlueVideoBlackLevel(this));
            addAction(new SetBlueVideoBlackLevel(this));
            addAction(new GetColorTemperature(this));
            addAction(new SetColorTemperature(this));
            addAction(new GetHorizontalKeystone(this));
            addAction(new SetHorizontalKeystone(this));
            addAction(new GetVerticalKeystone(this));
            addAction(new SetVerticalKeystone(this));
            addAction(new GetMute(this));
            addAction(new SetMute(this));
            addAction(new GetVolume(this));
            addAction(new SetVolume(this));
            addAction(new GetVolumeDB(this));
            addAction(new SetVolumeDB(this));
            addAction(new GetVolumeDBRange(this));
            addAction(new GetLoudness(this));
            addAction(new SetLoudness(this));
            createRenderingControlVariables();
        }
};

#endif // DLNASERVICE_H
