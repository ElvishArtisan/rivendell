// rdsettings.cpp
//
// Audio Format Settings
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <QObject>

#include <rdsettings.h>
#include <rddb.h>
#include <rdescape_string.h>

RDSettings::RDSettings()
{
  clear();
}


QString RDSettings::name() const
{
  return set_name;
}


void RDSettings::setName(const QString &str)
{
  set_name=str;
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
  QString desc;
  QString sr=QString().sprintf("%d S/sec",set_sample_rate);
  switch(set_format) {
    case RDSettings::Pcm16:
      desc="PCM16, ";
      break;
      
    case RDSettings::Pcm24:
      desc="PCM24, ";
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
    case RDSettings::MpegL2Wav:
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


bool RDSettings::loadPreset(unsigned id)
{
  QString sql;
  RDSqlQuery *q=NULL;
  bool ret=false;

  sql=QString("select ")+
    "NAME,"+                 // 00
    "FORMAT,"+               // 01
    "CHANNELS,"+             // 02
    "SAMPLE_RATE,"+          // 03
    "BIT_RATE,"+             // 04
    "QUALITY,"+              // 05
    "NORMALIZATION_LEVEL,"+  // 06
    "AUTOTRIM_LEVEL "+       // 07
    "from ENCODER_PRESETS where "+
    QString().sprintf("ID=%u",id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=true;
    set_name=q->value(0).toString();
    set_format=(RDSettings::Format)q->value(1).toUInt();
    set_channels=q->value(2).toUInt();
    set_sample_rate=q->value(3).toUInt();
    set_bit_rate=q->value(4).toUInt();
    set_quality=q->value(5).toUInt();
    set_normalization_level=q->value(6).toInt();
    set_autotrim_level=q->value(7).toInt();
  }
  delete q;

  return ret;
}


unsigned RDSettings::addPreset()
{
  QString sql;

  set_name=MakeNewName();
  sql=QString("insert into ENCODER_PRESETS set ")+
    SqlFields();
  return RDSqlQuery::run(sql).toUInt();
}


bool RDSettings::savePreset(unsigned id) const
{
  QString sql;

  sql=QString("update ENCODER_PRESETS set ")+
    SqlFields()+" where "+
    QString().sprintf("ID=%u",id);

  return RDSqlQuery::apply(sql);
}


bool RDSettings::deletePreset(unsigned id) const
{
  QString sql;

  sql=QString("delete from ENCODER_PRESETS where ")+
    QString().sprintf("ID=%u",id);

  return RDSqlQuery::apply(sql);
}


QString RDSettings::pathName(QString pathname,RDSettings::Format fmt)
{
  QString ext;
  int index=pathname.findRev(".");
  if(index<0) {
    return pathname+"."+defaultExtension(fmt);
  }
  ext=pathname.right(pathname.length()-index);
  if(ext.lower()==defaultExtension(fmt)) {
    return pathname;
  }
  return pathname.replace(index,ext.length(),"."+defaultExtension(fmt));
}


QString RDSettings::defaultExtension(RDSettings::Format fmt)
{
  QString ret="dat";

  switch(fmt) {
  case RDSettings::Pcm16:
  case RDSettings::Pcm24:
  case RDSettings::MpegL2Wav:
    ret="wav";
    break;

  case RDSettings::MpegL1:
    ret="mp1";
    break;

  case RDSettings::MpegL2:
    ret="mp2";
    break;

  case RDSettings::MpegL3:
    ret="mp3";
    break;

  case RDSettings::Flac:
    ret="flac";
    break;

  case RDSettings::OggVorbis:
    ret="ogg";
    break;
  }
  return ret;
}


QString RDSettings::dump() const
{
  QString ret;

  ret+="RDSettings:\n";
  ret+="name(): "+name()+"\n";
  ret+=QString().sprintf("format(): %u\n",format());
  ret+="formatName(): "+formatName()+"\n";
  ret+=QString().sprintf("channels(): %u\n",channels());
  ret+=QString().sprintf("sampleRate(): %u\n",sampleRate());
  ret+=QString().sprintf("layer(): %u\n",layer());
  ret+=QString().sprintf("bitRate(): %u\n",bitRate());
  ret+=QString().sprintf("quality(): %u\n",quality());
  ret+=QString().sprintf("normalizationLevel(): %d\n",normalizationLevel());
  ret+=QString().sprintf("autotrimLevel(): %d\n",autotrimLevel());
  ret+="Name: "+name()+"\n";

  return ret;
}


void RDSettings::clear()
{
  set_name="["+QObject::tr("new profile")+"]";
  set_format=RDSettings::Pcm16;
  set_format_name="";
  set_channels=2;
  set_sample_rate=48000;
  set_bit_rate=0;
  set_quality=0;
  set_normalization_level=0;
  set_autotrim_level=0;
}


QString RDSettings::SqlFields() const
{
  return QString("NAME=")+"\""+RDEscapeString(set_name)+"\","+
    QString().sprintf("FORMAT=%u,",set_format)+
    QString().sprintf("CHANNELS=%u,",set_channels)+
    QString().sprintf("SAMPLE_RATE=%u,",set_sample_rate)+
    QString().sprintf("BIT_RATE=%u,",set_bit_rate)+
    QString().sprintf("QUALITY=%u,",set_quality)+
    QString().sprintf("NORMALIZATION_LEVEL=%d,",set_normalization_level)+
    QString().sprintf("AUTOTRIM_LEVEL=%d ",set_autotrim_level);
}


QString RDSettings::MakeNewName() const
{
  QString sql;
  RDSqlQuery *q=NULL;
  bool unique=false;
  int count=0;
  QString ret="["+QObject::tr("new profile")+"]";
  
  while(!unique) {
    sql=QString("select ")+
      "ID "+  // 00
      "from ENCODER_PRESETS where "+
      "NAME=\""+RDEscapeString(ret)+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      ret="["+QObject::tr("new profile")+QString().sprintf(" %d]",++count);
    }
    else {
      unique=true;
    }
  }

  return ret;
}
