// rdcdripper.h
//
// Rip an audio from from CD
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcdripper.h,v 1.2.8.3 2014/01/10 02:25:35 cvs Exp $
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

#ifndef RDCDRIPPER_H
#define RDCDRIPPER_H

#include <sndfile.h>
#include <stdio.h>
#include <qobject.h>
#include <unistd.h>

class RDCdRipper : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorCode {ErrorOk=0,ErrorNoDevice=1,ErrorNoDestination=2,
		  ErrorInternal=3,ErrorNoDisc=4,ErrorNoTrack=5,
		  ErrorAborted=6};
  RDCdRipper(FILE *profile_msgs,QObject *parent=0);
  ~RDCdRipper();
  void setDevice(const QString &device);
  void setDestinationFile(const QString &filename);
  int totalSteps() const;
  RDCdRipper::ErrorCode rip(int track);
  RDCdRipper::ErrorCode rip(int first_track,int last_track);
  static QString errorText(RDCdRipper::ErrorCode err);

 public slots:
  void abort();

 signals:
  void progressChanged(int step);

 private:
  void Profile(const QString &msg);
  QString conv_device;
  QString conv_dst_filename;
  bool conv_aborting;
  FILE *conv_profile_msgs;
};


#endif  // RDCDRIPPER_H
