// rdcopyaudio.h
//
// Copy an audio file in the audio store.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcopyaudio.h,v 1.1.6.1 2013/11/13 23:36:32 cvs Exp $
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

#ifndef RDCOPYAUDIO_H
#define RDCOPYAUDIO_H

#include <rdconfig.h>
#include <rdstation.h>

class RDCopyAudio
{
 public:
  enum ErrorCode {ErrorOk=0,ErrorNoCart=1,ErrorInternal=5,ErrorUrlInvalid=7,
		  ErrorService=8,ErrorInvalidUser=9};
  RDCopyAudio(RDStation *station,RDConfig *config);
  void setSourceCartNumber(unsigned cartnum);
  void setSourceCutNumber(unsigned cutnum);
  void setDestinationCartNumber(unsigned cartnum);
  void setDestinationCutNumber(unsigned cutnum);
  RDCopyAudio::ErrorCode runCopy(const QString &username,
				 const QString &password);
  static QString errorText(RDCopyAudio::ErrorCode err);

 private:
  RDStation *conv_station;
  RDConfig *conv_config;
  unsigned conv_source_cart_number;
  unsigned conv_source_cut_number;
  unsigned conv_destination_cart_number;
  unsigned conv_destination_cut_number;
};


#endif  // RDCOPYAUDIO_H
