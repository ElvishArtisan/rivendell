// rdtrimaudio.h
//
// Get the trim points for an audio cut.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdtrimaudio.h,v 1.1.6.2 2014/01/16 02:44:59 cvs Exp $
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

#ifndef RDTRIMAUDIO_H
#define RDTRIMAUDIO_H

#include <qobject.h>

#include <rdconfig.h>
#include <rdstation.h>

class RDTrimAudio : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorCode {ErrorOk=0,ErrorInternal=5,ErrorUrlInvalid=7,
		  ErrorService=8,ErrorInvalidUser=9,ErrorNoAudio=10};
  RDTrimAudio(RDStation *station,RDConfig *config,QObject *parent=0,
	      const char *name=0);
  int startPoint() const;
  int endPoint() const;
  void setCartNumber(unsigned cartnum);
  void setCutNumber(unsigned cutnum);
  void setTrimLevel(int lvl);
  RDTrimAudio::ErrorCode runTrim(const QString &username,
				 const QString &password);
  static QString errorText(RDTrimAudio::ErrorCode err);

 private:
  bool ParseXml(const QString &xml);
  int ParsePoint(const QString &tag,const QString &xml);
  RDStation *conv_station;
  RDConfig *conv_config;
  unsigned conv_cart_number;
  unsigned conv_cut_number;
  int conv_trim_level;
  int conv_start_point;
  int conv_end_point;
  QString conv_xml;
};


#endif  // RDAUDIOIMPORT_H
