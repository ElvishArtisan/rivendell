// rdsettings.cpp
//
// RDLibrary Settings
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdsettings.cpp,v 1.11.8.1 2012/12/13 22:33:44 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <rdsettings.h>
#include <rddb.h>
#include <rdescape_string.h>

RDSettings::RDSettings()
{
  clear();
}


RDSettings::Format RDSettings::format() const
{
  return set_format;
}


void RDSettings::setFormat(Format format)
{
  set_format=format;
}


QString RDSettings::formatName() const
{
  return set_format_name;
}


void RDSettings::setFormatName(const QString &str)
{
  set_format_name=str;
}


unsigned RDSettings::channels() const
{
  return set_channels;
}


void RDSettings::setChannels(unsigned channels)
{
  set_channels=channels;
}


unsigned RDSettings::sampleRate() const
{
  return set_sample_rate;
}


void RDSettings::setSampleRate(unsigned rate)
{
  set_sample_rate=rate;
}


unsigned RDSettings::layer() const
{
  return set_layer;
}


void RDSettings::setLayer(unsigned layer)
{
  set_layer=layer;
}


unsigned RDSettings::bitRate() const
{
  return set_bit_rate;
}


void RDSettings::setBitRate(unsigned rate)
{
  set_bit_rate=rate;
}


unsigned RDSettings::quality() const
{
  return set_quality;
}


void RDSettings::setQuality(unsigned qual)
{
  set_quality=qual;
}


int RDSettings::normalizationLevel() const
{
  return set_normalization_level;
}


void RDSettings::setNormalizationLevel(int level)
{
  set_normalization_level=level;
}


int RDSettings::autotrimLevel() const
{
  return set_autotrim_level;
}


void RDSettings::setAutotrimLevel(int level)
{
  set_autotrim_level=level;
}


QString RDSettings::description()
{
  QString sql;
  RDSqlQuery *q;
  QString desc;
  QString sr=QString().sprintf("%d S/sec",set_sample_rate);
  switch(set_format) {
    case RDSettings::Pcm16:
      desc="PCM16, ";
      break;
      
    case RDSettings::MpegL1:
      desc="MPEG L1, ";
      if(set_bit_rate==0) {
	desc+=QString().sprintf("Qual %d, ",set_quality);
      }
      else {
	desc+=QString().sprintf("%d kbit/sec, ",set_bit_rate/1000);
      }
      break;
      
    case RDSettings::MpegL2:
      desc="MPEG L2, ";
      if(set_bit_rate==0) {
	desc+=QString().sprintf("Qual %d, ",set_quality);
      }
      else {
	desc+=QString().sprintf("%d kbit/sec, ",set_bit_rate/1000);
      }
      break;
      
    case RDSettings::MpegL3:
      desc="MPEG L3, ";
      if(set_bit_rate==0) {
	desc+=QString().sprintf("Qual %d, ",set_quality);
      }
      else {
	desc+=QString().sprintf("%d kbit/sec, ",set_bit_rate/1000);
      }
      break;
      
    case RDSettings::Flac:
      desc="FLAC, ";
      break;
      
    case RDSettings::OggVorbis:
      desc=QString().sprintf("OggVorbis, Qual %d, ",set_quality);
      break;
      
    default:  // Custom format
      if(set_format_name.isEmpty()) {
	sql=QString().sprintf("select NAME from ENCODERS where ID=%d",
			      set_format);
	q=new RDSqlQuery(sql);
	if(q->first()) {
	  set_format_name=q->value(0).toString();
	}
	else {
	  set_format_name="Unknown";
	}
	delete q;
      }
      desc=set_format_name+" ";
      if(set_bit_rate>0) {
	desc+=" "+QString().sprintf("%d kbit/sec, ",set_bit_rate/1000);
      }
  }
  if(set_sample_rate>0) {
    desc+=QString().sprintf("%d samp/sec, ",set_sample_rate);
  }
  switch(set_channels) {
      case 1:
	desc+="Mono";
	break;

      case 2:
	desc+="Stereo";
	break;

      default:
	desc+=QString().sprintf("%d chans",set_channels);
	break;
  }
  return desc;
}


QString RDSettings::customCommandLine() const
{
  return set_custom_command_line;
}


void RDSettings::setCustomCommandLine(const QString &str)
{
  set_custom_command_line=str;
}


QString RDSettings::resolvedCustomCommandLine(const QString &destfile)
{
  if(set_custom_command_line.isEmpty()) {
    QString sql;
    RDSqlQuery *q;
    sql=QString().sprintf("select COMMAND_LINE from ENCODERS where ID=%d",
			  set_format);
    q=new RDSqlQuery(sql);
    if(q->first()) {
      set_custom_command_line=q->value(0).toString();
    }
    delete q;
  }
  QString ret=set_custom_command_line;

  ret.replace("%f",destfile);
  ret.replace("%c",QString().sprintf("%u",set_channels));
  ret.replace("%r",QString().sprintf("%u",set_sample_rate));
  ret.replace("%b",QString().sprintf("%u",set_bit_rate));

  return ret;
}


QString RDSettings::pathName(const QString &stationname,QString pathname,
			     RDSettings::Format fmt)
{
  QString ext;
  int index=pathname.findRev(".");
  if(index<0) {
    return pathname+"."+defaultExtension(stationname,fmt);
  }
  ext=pathname.right(pathname.length()-index);
  if(ext.lower()==defaultExtension(stationname,fmt)) {
    return pathname;
  }
  return pathname.replace(index,ext.length(),"."+
			  defaultExtension(stationname,fmt));
}


QString RDSettings::defaultExtension(const QString &stationname,
				     RDSettings::Format fmt)
{
  switch(fmt) {
      case RDSettings::Pcm16:
      case RDSettings::MpegL2Wav:
	return QString("wav");

      case RDSettings::MpegL1:
	return QString("mp1");

      case RDSettings::MpegL2:
	return QString("mp2");

      case RDSettings::MpegL3:
	return QString("mp3");

      case RDSettings::Flac:
	return QString("flac");

      case RDSettings::OggVorbis:
	return QString("ogg");
  }

  //
  // Custom Format
  //
  QString sql;
  RDSqlQuery *q;
  QString ret;

  sql=QString().sprintf("select DEFAULT_EXTENSION from ENCODERS \
                         where (ID=%d)&&(STATION_NAME=\"%s\")",
			fmt,(const char *)RDEscapeString(stationname));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=q->value(0).toString();
  }
  delete q;

  return ret;
}


unsigned RDSettings::bytesPerSec(const QString &stationname,
				 RDSettings::Format fmt,unsigned quality)
{
  //
  // Guesstimate the bit rate for a VBR encoding.  This is *not* exact,
  // but is intended merely to provide a half-way sane value for use with
  // progress bars and such.
  //
  switch(fmt) {
      case RDSettings::MpegL3:
	return 16000;
	break;

      case RDSettings::OggVorbis:
	return 4173*quality+7977;
	break;

      case RDSettings::Flac:
	return 72500;
	break;

      default:
	break;
  }
  return 0;
}


void RDSettings::clear()
{
  set_format=RDSettings::Pcm16;
  set_format_name="";
  set_channels=2;
  set_sample_rate=48000;
  set_bit_rate=0;
  set_quality=0;
  set_normalization_level=0;
  set_autotrim_level=0;
  set_custom_command_line="";
}
