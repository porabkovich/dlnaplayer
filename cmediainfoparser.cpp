#include "cmediainfoparser.h"
#include "tools.h"

#include <QXmlStreamReader>
#include <QFile>
#include <QTime>

#define NDEBUG
#include "debugmacros.h"

const QString CMediaInfoParser::tag_track = "track";
const QString CMediaInfoParser::tag_type = "type";

const QString CMediaInfoParser::tag_general = "General";
const QString CMediaInfoParser::tag_video = "Video";
const QString CMediaInfoParser::tag_audio = "Audio";

const QString CMediaInfoParser::tag_format = "Format";
const QString CMediaInfoParser::tag_file_size = "File_size";
const QString CMediaInfoParser::tag_duration = "Duration";
const QString CMediaInfoParser::tag_overall_bit_rate = "Overall_bit_rate";

const QString CMediaInfoParser::tag_width = "Width";
const QString CMediaInfoParser::tag_height = "Height";
const QString CMediaInfoParser::tag_frame_rate = "Frame_rate";
//const QString CMediaInfoParser::tag_format = "Format";	//already exists
const QString CMediaInfoParser::tag_codec_id = "Codec_ID";

const QString CMediaInfoParser::tag_channels = "Channel_s_";
const QString CMediaInfoParser::tag_bit_rate = "Bit_rate";
const QString CMediaInfoParser::tag_sampling_rate = "Sampling_rate";
const QString CMediaInfoParser::tag_language = "Language";

const QString CMediaInfoParser::out_general = "<b>General</b>";
const QString CMediaInfoParser::out_video = "<b>Video</b>";
const QString CMediaInfoParser::out_audio = "<b>Audio</b>";

const QString CMediaInfoParser::out_format = "Format";
const QString CMediaInfoParser::out_size = "Size";
const QString CMediaInfoParser::out_duration = "Duration";
const QString CMediaInfoParser::out_average_bitrate = "Average bitrate";
const QString CMediaInfoParser::out_overall_bitrate = "Overall bitrate";

const QString CMediaInfoParser::out_width = "Width";
const QString CMediaInfoParser::out_height = "Height";
const QString CMediaInfoParser::out_frame_rate = "Frame rate";
//const QString CMediaInfoParser::out_format = "Format";	//already exists
const QString CMediaInfoParser::out_codec_id = "Codec identifier";

const QString CMediaInfoParser::out_channels = "Number of channels";
const QString CMediaInfoParser::out_bit_rate = "Bit rate";
const QString CMediaInfoParser::out_sampling_rate = "Sampling rate";
const QString CMediaInfoParser::out_language = "Language";

using namespace std;

CMediaInfoParser::CMediaInfoParser(const QString &fn)
{
	QFile file(fn);
	if (!file.open(QFile::ReadOnly | QFile::Text))	{
		ERROR_INFO_ONE(file.errorString());
		file.close();
		outList.clear();
		return;
	}
	parse(&file);
	file.close();
}

void CMediaInfoParser::parse(QFile *file)
{
	DEBUG_INFO();
	tmpMap.clear();
	outList.clear();
	// parsing
	QXmlStreamReader reader(file);
	while(!reader.atEnd())	{
		if((reader.readNext() == QXmlStreamReader::StartElement) && (reader.name() == tag_track))
		{
			DEBUG_INFO_ONE("track found");
			QString typeVal = reader.attributes().value(tag_type).toString();
			if(typeVal == tag_general)	{
				if(parseGeneralTrack(reader,&tmpMap))
					rawOutput.insert(make_pair(out_general,tmpMap));
			}
			else if(typeVal == tag_audio){
				if(parseAudioTrack(reader,&tmpMap))
					rawOutput.insert(make_pair(out_audio,tmpMap));
			}
			else if(typeVal == tag_video){
				if(parseVideoTrack(reader,&tmpMap))
					rawOutput.insert(make_pair(out_video,tmpMap));
			}
		}
	}

	// checking errors
	if(reader.hasError())	{
		ERROR_INFO_ONE(reader.errorString());
		rawOutput.clear();
		return;
	}

	// making results
	makeOutput(rawOutput,&outList);
	rawOutput.clear();


//	map<QString,map<QString,QString> >::iterator itt=rawOutput.begin();
//	for(;itt!=rawOutput.end();++itt)	{
//		cout << itt->first << endl;
//		map<QString,QString>::iterator it = itt->second.begin();
//		for(;it!=itt->second.end();++it)	{
//			cout << "\t" << it->first << " = " << it->second << endl;
//		}
//	}


	list<QString>::iterator lit=outList.begin();
	for(; lit != outList.end(); ++lit)
	{
		cout << *lit <<endl;
	}
}

bool CMediaInfoParser::parseGeneralTrack(QXmlStreamReader &reader, map<QString,QString> *out)
{
	DEBUG_INFO();
	out->clear();
	float duration = 0.0;
	long long fileSize = 0;
	int sec, s, m, h;
	while(!reader.atEnd())	{
		reader.readNext();
		if(reader.isEndElement() && reader.name() == tag_track)
			break;
		if(reader.isStartElement())	{
			if(reader.name() == tag_format)	{
				reader.readNext();
				(*out)[out_format] = reader.text().toString();
			}
			else if(reader.name() == tag_overall_bit_rate)	{
				reader.readNext();
				(*out)[out_average_bitrate] = reader.text().toString();
			}
			else if(reader.name() == tag_duration)	{
				reader.readNext();
				duration = reader.text().toString().toFloat();
				if(duration != 0){
					sec = duration/1000;
					s = sec%60;
					m = sec/60;
					h = m/60;
					if(m >= 60)	m -= h*60;
					QTime time(h,m,s);
					(*out)[out_duration] = ", " + time.toString("hh:mm:ss");
				}
			}
			else if(reader.name() == tag_file_size)	{
				reader.readNext();
				(*out)[out_size] = reader.text().toString();
				fileSize = reader.text().toString().toLongLong();
			}
			if(duration != 0 && fileSize != 0){
				(*out)[out_overall_bitrate] = ", " +  QString::number((fileSize/(long long)duration)*8) + " Kbit/s";
			}
		}
	}
	return true;
}

bool CMediaInfoParser::parseVideoTrack(QXmlStreamReader &reader, map<QString,QString> *out)
{
	DEBUG_INFO();
	out->clear();
	while(!reader.atEnd())	{
		reader.readNext();
		if(reader.isEndElement() && reader.name() == tag_track)
			break;
		if(reader.isStartElement())	{
			if(reader.name() == tag_width)	{
				reader.readNext();
				QString width = reader.text().toString();
				if(width.length() != 0)
					(*out)[out_width] = ", " + width;
			}
			else if(reader.name() == tag_height)	{
				reader.readNext();
				QString height = reader.text().toString();
				if(height.length() != 0)
					(*out)[out_height] = " x " + height;
			}
//			else if(reader.name() == tag_duration)	{
//				reader.readNext();
//				(*out)[out_duration] = reader.text().toString();
//			}
			else if(reader.name() == tag_frame_rate)	{
				reader.readNext();
				QString frameRate = reader.text().toString();
				if(frameRate.length() != 0)
					(*out)[out_frame_rate] = ", " + frameRate;
			}
			else if(reader.name() == tag_format)	{
				reader.readNext();
				(*out)[out_format] = reader.text().toString();
			}
			else if(reader.name() == tag_codec_id)	{
				reader.readNext();
				(*out)[out_codec_id] = reader.text().toString();
			}
		}
	}
	return true;
}

bool CMediaInfoParser::parseAudioTrack(QXmlStreamReader &reader, map<QString,QString> *out)
{
	DEBUG_INFO();
	out->clear();
	while(!reader.atEnd())	{
		reader.readNext();
		if(reader.isEndElement() && reader.name() == tag_track)
			break;
		if(reader.isStartElement())	{
			if(reader.name() == tag_channels)	{
				reader.readNext();
				(*out)[out_channels] = reader.text().toString();
			}
//			else if(reader.name() == tag_duration)	{
//				reader.readNext();
//				(*out)[out_duration] = reader.text().toString();
//			}
			else if(reader.name() == tag_format)	{
				reader.readNext();
				(*out)[out_format] = reader.text().toString();
			}
			else if(reader.name() == tag_bit_rate)	{
				reader.readNext();
				(*out)[out_bit_rate] = reader.text().toString();
			}
			else if(reader.name() == tag_sampling_rate)	{
				reader.readNext();
				QString samplingRate = reader.text().toString();
				if(samplingRate.length() != 0)
					(*out)[out_sampling_rate] = ", " + samplingRate + " Hz";
			}
			else if(reader.name() == tag_language)	{
				reader.readNext();
				QString lang = reader.text().toString();
				if(lang.length() != 0)
					(*out)[out_language] = ", " + lang;
			}
		}
	}
	return true;
}

void CMediaInfoParser::makeOutput(std::multimap<QString, std::map<QString, QString> > &ro, std::list<QString> *outL)
{
	multimap<QString,map<QString,QString> >::iterator itt,itlow,itup;

	//general information:
	itt = ro.find(out_general);
	if(itt != ro.end())	{
		outL->push_back(itt->first);
		map<QString,QString> &ref = itt->second;
		outL->back() = ref[out_duration]
						+ ref[out_overall_bitrate];
	}
	//video tracks
	size_t count = ro.count(out_video);
	itlow = ro.lower_bound(out_video);
	itup = ro.upper_bound(out_video);
	itt = itlow;
	for(size_t i = 0; itt !=itup ; ++i,++itt)	{
		map<QString,QString> &ref = itt->second;
		map<QString,QString>::iterator it = ref.begin();
		bool emptyTrack = true;
		while (it != ref.end()) {
			if(!it->second.isEmpty()) {
				emptyTrack = false;
				break;
			}
			++it;
		}
		if(!emptyTrack) {
			outL->push_back(itt->first);
			if(count > 1)	{
				outL->back() += "<b>"+QString::number(i)+"</b>";
			}
			outL->back() += "<b>: </b>" + ref[out_format]
							+ ref[out_frame_rate]
							+ ref[out_width].remove(" pixels")
							+ ref[out_height].remove(" pixels");
		}
	}

	//audio tracks
	count = ro.count(out_audio);
	itlow = ro.lower_bound(out_audio);
	itup = ro.upper_bound(out_audio);
	itt = itlow;
	for(size_t i = 0; itt !=itup ; ++i,++itt)	{
		map<QString,QString> &ref = itt->second;
		map<QString,QString>::iterator it = ref.begin();
		bool emptyTrack = true;
		while (it != ref.end()) {
			if(!it->second.isEmpty()) {
				emptyTrack = false;
				break;
			}
			++it;
		}
		if(!emptyTrack) {
			outL->push_back(itt->first);
			if(count > 1)	{
				outL->back() += "<b>"+QString::number(i)+"</b>";
			}

			outL->back() += "<b>: </b>" + ref[out_format]
							+ ref[out_sampling_rate]
							+ ref[out_language];
		}
	}
}

//void CMediaInfoParser::makeOutput(const std::multimap<QString, std::map<QString, QString> > &ro, std::list<QString> *outL)
//{
//	map<QString,map<QString,QString> >::iterator itt,itlow,itup;
//	map<QString,QString>::iterator it;

//	//general information:
//	itt = ro.find(out_general);
//	if(itt != ro.end())	{
//		outL->push_back(itt->first);
//		it = itt->second.begin();
//		for(;it!=itt->second.end();++it)	{
//			outL->push_back(it->first + ": " + it->second);
//		}
//	}
//	//video tracks
//	size_t count = ro.count(out_video);
//	if(count)	{
//		if(count > 1)	{
//			outL->push_back("Number of video tracks: " + QString::number(count));
//		}
//		itlow = ro.lower_bound(out_video);
//		itup = ro.upper_bound(out_video);
//		for(itt = itlow;itt != itup;++itt)	{
//			outL->push_back(itt->first);
//			it = itt->second.begin();
//			for(;it!=itt->second.end();++it)	{
//				outL->push_back(it->first + ": " + it->second);
//			}
//		}
//	}

//	//audio tracks
//	count = ro.count(out_audio);
//	if(count)	{
//		if(count > 1)	{
//			outL->push_back("Number of audio tracks: " + QString::number(count));
//		}
//		itlow = ro.lower_bound(out_audio);
//		itup = ro.upper_bound(out_audio);
//		for(itt = itlow;itt != itup;++itt)	{
//			outL->push_back(itt->first);
//			it = itt->second.begin();
//			for(;it!=itt->second.end();++it)	{
//				outL->push_back(it->first + ": " + it->second);
//			}
//		}
//	}
////	<b>General information:</b>
////	Average bitrate: 51.4 Mbps
////	Duration: 191ms
////	Format: BDAV
////	Size: 34.1 GiB
////	<b>Video:</b>
////	Format: AVC
////	Frame rate: 23.976 fps
////	Height: 1 080 pixels
////	Width: 1 920 pixels
////	Number of audio tracks: 4
////	<b>Audio:</b>
////	Bit rate: 448 Kbps
////	Format: AC-3
////	Language: Russian
////	Number of channels: 6 channels
////	<b>Audio:</b>
////	Bit rate: 754 Kbps
////	Format: DTS
////	Language: Russian
////	Number of channels: 6 channels
////	<b>Audio:</b>
////	Bit rate: 1 510 Kbps
////	Format: DTS
////	Language: English
////	Number of channels: 6 channels
////	<b>Audio:</b>
////	Bit rate: Variable / 640 Kbps
////	Format: AC-3
////	Language: English
////	Number of channels: 6 channels
//}



//				QXmlStreamAttributes atr = reader.attributes();
//				for(int i=0;i < atr.size(); ++i)	{
//					DEBUG_INFO_TWO("name =",atr[i].name().toString());
//					DEBUG_INFO_TWO("namespaceUri =",atr[i].namespaceUri().toString());
//					DEBUG_INFO_TWO("prefix =",atr[i].prefix().toString());
//					DEBUG_INFO_TWO("qualifiedName =",atr[i].qualifiedName().toString());
//					DEBUG_INFO_TWO("value =",atr[i].value().toString());
//				}

