#ifndef CMEDIAINFOPARSER_H
#define CMEDIAINFOPARSER_H
#include <map>
#include <list>
#include <QString>

class QFile;
class QXmlStreamReader;

class CMediaInfoParser
{
	//constants:
	static const QString tag_track;
	static const QString tag_type;

	static const QString tag_general;
	static const QString tag_video;
	static const QString tag_audio;

	static const QString tag_format;
	static const QString tag_file_size;
	static const QString tag_duration;
	static const QString tag_overall_bit_rate;

	static const QString tag_width;
	static const QString tag_height;
	static const QString tag_frame_rate;
	//static const QString tag_format;	//already exists
	static const QString tag_codec_id;

	static const QString tag_channels;
	static const QString tag_bit_rate;
	static const QString tag_sampling_rate;
	static const QString tag_language;

	static const QString out_general;
	static const QString out_video;
	static const QString out_audio;

	static const QString out_format;
	static const QString out_size;
	static const QString out_duration;
	static const QString out_average_bitrate;
	static const QString out_overall_bitrate;

	static const QString out_width;
	static const QString out_height;
	static const QString out_frame_rate;
	//static const QString out_format;	//already exists
	static const QString out_codec_id;

	static const QString out_channels;
	static const QString out_bit_rate;
	static const QString out_sampling_rate;
	static const QString out_language;

	std::map<QString,QString> tmpMap;
	std::multimap<QString,std::map<QString,QString> > rawOutput;
	std::list<QString> outList;

	void parse(QFile*);
	bool parseGeneralTrack(QXmlStreamReader&, std::map<QString,QString> *);
	bool parseVideoTrack(QXmlStreamReader&, std::map<QString,QString> *);
	bool parseAudioTrack(QXmlStreamReader&, std::map<QString,QString> *);
	void makeOutput(std::multimap<QString,std::map<QString,QString> >&,std::list<QString> *);


public:
	CMediaInfoParser(const QString &fn);
	const std::list<QString> &outRef()	{return outList;}
};

#endif // CMEDIAINFOPARSER_H
