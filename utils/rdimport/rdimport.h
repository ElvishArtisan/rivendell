// rdimport.h
//
// A Batch Importer for Rivendell.
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

#ifndef RDIMPORT_H
#define RDIMPORT_H

#include <list>
#include <vector>

#include <qobject.h>
#include <qsqldatabase.h>
#include <qfileinfo.h>
#include <qdatetime.h>
#include <QList>
#include <QStringList>

#include <rdapplication.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdgroup.h>
#include <rdnotification.h>
#include <rdwavedata.h>
#include <rdwavefile.h>

#include "journal.h"
#include "markerset.h"

#define RDIMPORT_TEMP_BASENAME "rdimp"
#define RDIMPORT_STDIN_BUFFER_LENGTH 1024
#define RDIMPORT_DROPBOX_SCAN_INTERVAL 5
#define RDIMPORT_DROPBOX_PASSES 3
#define RDIMPORT_USAGE "[options] <group> <filespec> [<filespec>]*\n\nAudio importation tool for the Rivendell Radio Automation System.\nDo 'man 1 rdimport' for the full manual.\n"
#define RDIMPORT_GLOB_SIZE 10

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void userData();

 private:
  enum Result {Success=0,FileBad=1,NoCart=2,NoCut=3,DuplicateTitle=4};
  void RunDropBox();
  void ProcessFileEntry(const QString &entry);
  MainObject::Result ImportFile(const QString &filename,unsigned *cartnum);
  bool OpenAudioFile(RDWaveFile **wavefile,RDWaveData *wavedata);
  void VerifyFile(const QString &filename,unsigned *cartnum);
  RDWaveFile *FixFile(const QString &filename,RDWaveData *wavedata);
  bool IsWav(int fd);
  bool FindChunk(int fd,const char *name,bool *fix_needed);
  bool FixChunkSizes(const QString &filename);
  bool RunPattern(const QString &pattern,const QString &filename,
		  RDWaveData *wavedata,QString *groupname);
  bool VerifyPattern(const QString &pattern);
  void DeleteCuts(unsigned cartnum);
  QDateTime GetCachedTimestamp(const QString &filename);
  void WriteTimestampCache(const QString &filename,const QDateTime &dt);
  bool SchedulerCodeExists(const QString &code) const;
  void ReadXmlFile(const QString &basename,RDWaveData *wavedata) const;
  void Log(int prio,const QString &msg) const;
  void SendNotification(RDNotification::Action action,unsigned cartnum);
  void NormalExit() const;
  void ErrorExit(RDApplication::ExitCode code) const;
  unsigned import_file_key;
  RDGroup *import_group;
  bool import_verbose;
  bool import_log_syslog;
  bool import_log_file;
  QString import_log_filename;
  bool import_to_mono;
  bool import_use_cartchunk_cutid;
  int import_cart_number_offset;
  bool import_single_cart;
  bool import_title_from_cartchunk_cutid;
  bool import_delete_source;
  bool import_delete_cuts;
  bool import_drop_box;
  std::vector<QString> import_add_scheduler_codes;
  QString import_set_user_defined;
  bool import_stdin_specified;
  int import_startdate_offset;
  int import_enddate_offset;
  bool import_create_dates;
  int import_create_startdate_offset;
  int import_create_enddate_offset;
  QDateTime import_datetimes[2];
  bool import_clear_datetimes;
  QTime import_dayparts[2];
  bool import_clear_dayparts;
  bool import_fix_broken_formats;
  int import_persistent_dropbox_id;
  bool import_xml;
  unsigned import_format;
  unsigned import_samprate;
  unsigned import_bitrate;
  unsigned import_channels;
  int import_src_converter;
  int import_normalization_level;
  int import_autotrim_level;
  int import_segue_level;
  int import_segue_length;
  bool import_send_mail;
  bool import_mail_per_file;
  unsigned import_cart_number;
  QString import_metadata_pattern;
  QString import_output_pattern;
  QString import_string_agency;
  QString import_string_album;
  QString import_string_artist;
  int import_string_bpm;
  QString import_string_client;
  QString import_string_composer;
  QString import_string_conductor;
  QString import_string_description;
  QString import_string_outcue;
  QString import_string_isrc;
  QString import_string_isci;
  QString import_string_recording_mbid;
  QString import_string_release_mbid;
  QString import_string_publisher;
  QString import_string_label;
  QString import_string_song_id;
  QString import_string_title;
  QString import_string_user_defined;
  int import_string_year;
  int import_failed_imports;
  struct DropboxList {
    QString filename;
    unsigned size;
    unsigned pass;
    bool checked;
    bool failed;
  };
  std::list<DropboxList *> import_dropbox_list;
  QString import_temp_fix_filename;
  MarkerSet *import_cut_markers;
  MarkerSet *import_talk_markers;
  MarkerSet *import_hook_markers;
  MarkerSet *import_segue_markers;
  MarkerSet *import_fadedown_marker;
  MarkerSet *import_fadeup_marker;
  Journal *import_journal;
};


#endif  // RDIMPORT_H
