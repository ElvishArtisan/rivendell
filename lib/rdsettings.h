// rdsettings.h
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

#ifndef RDSETTINGS_H
#define RDSETTINGS_H

#include <qstring.h>

#include <rdcae.h>

class RDSettings
{
 public:
  enum Format {Pcm16=0,MpegL1=1,MpegL2=2,MpegL3=3,Flac=4,OggVorbis=5,
	       MpegL2Wav=6,Pcm24=7};
  RDSettings();
  QString name() const;
  void setName(const QString &str);
  RDSettings::Format format() const;
  void setFormat(Format format);
  void setFormat(RDCae::AudioCoding coding);
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
  bool loadPreset(unsigned id);
  unsigned addPreset();
  bool savePreset(unsigned id) const;
  bool deletePreset(unsigned id) const;
  static QString pathName(QString pathname,RDSettings::Format fmt);
  static QString defaultExtension(RDSettings::Format fmt);
  static QString description(Format fmt,unsigned chans,unsigned samprate,
			     unsigned bitrate,unsigned qual);
  QString dump() const;
  void clear();

 private:
  QString SqlFields() const;
  QString MakeNewName() const;
  QString set_name;
  Format set_format;
  QString set_format_name;
  unsigned set_channels;
  unsigned set_sample_rate;
  unsigned set_layer;
  unsigned set_bit_rate;
  unsigned set_quality;
  int set_normalization_level;
  int set_autotrim_level;
};


#endif  // RDSETTINGS_H
