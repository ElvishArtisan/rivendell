// rdrender.h
//
// Render a Rivendell log.
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDRENDER_H
#define RDRENDER_H

#include <stdint.h>

#include <sndfile.h>

#include <qdatetime.h>
#include <qobject.h>

#include <rdconfig.h>
#include <rddb.h>
#include <rdripc.h>
#include <rdsettings.h>
#include <rdstation.h>
#include <rdsystem.h>
#include <rduser.h>

#include "logline.h"

#define RDRENDER_DEFAULT_CHANNELS 2
#define RDRENDER_DEFAULT_FORMAT RDSettings::Pcm16
#define RDRENDER_DEFAULT_BITRATE 256000
#define RDRENDER_DEFAULT_QUALITY 3
#define RDRENDER_DEFAULT_NORMALIZATION_LEVEL 0
#define RDRENDER_USAGE "[options] <logname> <output-file>\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void userData();

 private:
  int MainLoop();
  void Sum(float *pcm_out,LogLine *ll,sf_count_t frames);
  uint64_t FramesFromMsec(uint64_t msec);
  void Verbose(const QString &msg);
  void Verbose(const QTime &time,int line,const QString &trans,
	       const QString &msg);
  bool GetCutFile(const QString &cutname,int start_pt,int end_pt,
		  QString *dest_filename) const;
  void DeleteCutFile(const QString &dest_filename) const;
  bool ConvertAudio(const QString &srcfile,const QString &dstfile,
		    RDSettings *s,QString *err_msg);
  bool render_verbose;
  QString render_logname;
  QString render_output_filename;
  QString render_temp_output_filename;
  unsigned render_channels;
  QTime render_start_time;
  int render_first_line;
  int render_last_line;
  QTime render_first_time;
  QTime render_last_time;
  bool render_ignore_stops;
  RDSettings render_settings;
  bool render_settings_modified;
  RDRipc *render_ripc;
  RDStation *render_station;
  RDSystem *render_system;
  RDUser *render_user;
  RDConfig *render_config;
};


#endif  // RDRENDER_H
