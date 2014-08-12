// rdaudioinfo.h
//
// Get information about a cut in the audio store.
//
//   (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdaudioinfo.h,v 1.1.6.2 2013/11/13 23:36:31 cvs Exp $
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

#ifndef RDAUDIOINFO_H
#define RDAUDIOINFO_H

#include <qobject.h>

#include <rdconfig.h>
#include <rdwavefile.h>
#include <rdstation.h>

class RDAudioInfo : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorCode {ErrorOk=0,ErrorInternal=5,ErrorUrlInvalid=7,
		  ErrorService=8,ErrorInvalidUser=9,ErrorNoAudio=10};
  RDAudioInfo(RDStation *station,RDConfig *config,QObject *parent=0,
	      const char *name=0);
  RDWaveFile::Format format() const;
  unsigned channels() const;
  unsigned sampleRate() const;
  unsigned frames() const;
  unsigned length() const;
  void setCartNumber(unsigned cartnum);
  void setCutNumber(unsigned cutnum);
  RDAudioInfo::ErrorCode runInfo(const QString &username,
				 const QString &password);
  static QString errorText(RDAudioInfo::ErrorCode err);

 private:
  int ParseInt(const QString &tag,const QString &xml);
  RDStation *conv_station;
  RDConfig *conv_config;
  unsigned conv_cart_number;
  unsigned conv_cut_number;
  RDWaveFile::Format conv_format;
  unsigned conv_channels;
  unsigned conv_sample_rate;
  unsigned conv_frames;
  unsigned conv_length;
  QString conv_xml;
};


#endif  // RDAUDIOINFO_H
