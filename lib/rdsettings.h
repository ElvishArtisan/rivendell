// rdsettings.h
//
// Rivendell Audio Settings
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdsettings.h,v 1.11 2010/07/29 19:32:34 cvs Exp $
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

#ifndef RDSETTINGS_H
#define RDSETTINGS_H

#include <qstring.h>

class RDSettings
{
 public:
  enum Format {Pcm16=0,MpegL1=1,MpegL2=2,MpegL3=3,Flac=4,OggVorbis=5,
	       MpegL2Wav=6};
  RDSettings();
  RDSettings::Format format() const;
  void setFormat(Format format);
  QString formatName() const;
  void setFormatName(const QString &str);
  unsigned channels() const;
  void setChannels(unsigned channels);
  unsigned sampleRate() const;
  void setSampleRate(unsigned rate);
  unsigned layer() const;
  void setLayer(unsigned layer);
  unsigned bitRate() const;
  void setBitRate(unsigned rate);
  unsigned quality() const;
  void setQuality(unsigned qual);
  int normalizationLevel() const;
  void setNormalizationLevel(int level);
  int autotrimLevel() const;
  void setAutotrimLevel(int level);
  QString description();
  QString customCommandLine() const;
  void setCustomCommandLine(const QString &str);
  QString resolvedCustomCommandLine(const QString &destfile);
  static QString pathName(const QString &stationname,QString pathname,
			  RDSettings::Format fmt);
  static QString defaultExtension(const QString &stationname,
				  RDSettings::Format fmt);
  static unsigned bytesPerSec(const QString &stationname,
			      RDSettings::Format fmt,unsigned quality);
  void clear();

 private:
  Format set_format;
  QString set_format_name;
  unsigned set_channels;
  unsigned set_sample_rate;
  unsigned set_layer;
  unsigned set_bit_rate;
  unsigned set_quality;
  int set_normalization_level;
  int set_autotrim_level;
  QString set_custom_command_line;
};


#endif

