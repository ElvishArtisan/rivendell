// nexgen_filter.h
//
// A Library import filter for the Prophet NexGen system
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: nexgen_filter.h,v 1.1.2.5 2013/06/20 20:24:45 cvs Exp $
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

#ifndef NEXGEN_FILTER_H
#define NEXGEN_FILTER_H

#include <stdio.h>

#include <qobject.h>
#include <qapplication.h>
#include <qsqldatabase.h>
#include <qdatetime.h>
#include <qdir.h>

#include <rdstation.h>
#include <rdripc.h>
#include <rdgroup.h>
#include <rdwavedata.h>

#define NEXGEN_FILTER_USAGE "[options] <xml-file>|<pkt-file> [...]\n\nImport audio from a Prophet NexGen system, using metadata contained in\none or more XML files.  Options are:\n\n--group=<group-name>\n     The Rivendell group to use.  This option is mandatory and has no default.\n\n--audio-dir=<path>\n     The full path to the directory containing the audio files.  This option\n     is ignored when importing PKT files, but mandatory for importing\n     using XML data.  It has no default.\n\n--reject-dir=<path>\n     The full path to the directory in which to place copies of XML files\n     which were unable to be processed.  Default is '/dev/null'.\n\n--cart-offset=<offset>\n     Apply integer <offset> to the NexGen cart number before importing.\n     Default is '0'.\n\n--delete-cuts\n     If the destination cart already exists, delete any existing cuts\n     within it before importing.\n\n--normalization-level=<level>\n     The level to use for normalizing the audio, in dBFS.  Specifying '0'\n     will turn off normalization.  Default is '0'.\n\n--verbose\n     Print status messages as files are processed.\n\n<xml-file> [..]\n     Filespec for XML file(s) containing import metadata.\n\n"

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  void ProcessArchive(const QString &filename);
  void ProcessXmlFile(const QString &xml,const QString &wavname="",
		      const QString &arcname="");
  bool OpenXmlFile(const QString &xml,RDWaveData *data,int *cartnum,
		   QString *filename);
  void ProcessXmlLine(const QString &line,RDWaveData *data,int *cartnum,
		      QString *filename,int *crossfade,int *fadeup_start,
		      int *fadeup_len);
  bool PreprocessAudio(QString filename);
  void WriteReject(const QString &filename);
  QDateTime GetDateTime(const QString &str) const;
  QString SwapCase(const QString &str) const;
  bool IsXmlFile(const QString &filename);
  void Print(const QString &msg) const;
  RDGroup *filter_group;
  QDir *filter_audio_dir;
  QDir *filter_reject_dir;
  QDir *filter_temp_dir;
  QString filter_temp_audiofile;
  int filter_cart_offset;
  bool filter_delete_cuts;
  int filter_normalization_level;
  RDStation *filter_rdstation;
  RDRipc *filter_ripc;
  QSqlDatabase *filter_db;
  bool filter_verbose;
};


#endif 
