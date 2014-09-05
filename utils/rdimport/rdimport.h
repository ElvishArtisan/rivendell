// rdimport.h
//
// A Batch Importer for Rivendell.
//
//   (C) Copyright 2002-2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdimport.h,v 1.17.6.3.2.3 2014/07/15 00:45:17 cvs Exp $
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

#include <rdwavedata.h>
#include <rdwavefile.h>
#include <rdconfig.h>
#include <rdcmd_switch.h>
#include <rdgroup.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdripc.h>
#include <rduser.h>
#include <rdsystem.h>
#include <rdstation.h>

#define RDIMPORT_TEMP_BASENAME "rdimp"
#define RDIMPORT_STDIN_BUFFER_LENGTH 1024
#define RDIMPORT_DROPBOX_SCAN_INTERVAL 5
#define RDIMPORT_DROPBOX_PASSES 3
#define RDIMPORT_USAGE "[options] <group> <filespec> [<filespec>]*\n\nImport one or more files into the specified group in the library.  By\ndefault, a new cart will be created for each file imported.  A <filespec> of \n'-' will cause RDImport to read the list of filespecs from standard input.\n\nThe following options are available:\n\n --verbose\n     Print progress messages during processing.\n\n --log-mode\n     Prepend date/time information to each line of printed status (implies\n     the '--verbose' option).\n\n --normalization-level=<level>\n     Specify the level to use for normalizing the audio, in dBFS.\n     Specifying '0' will turn off normalization.\n\n --autotrim-level=<level>\n     Specify the threshold level to use for autotrimming the audio, in\n     dBFS.  Specifying '0' will turn off autotrimming.\n\n --single-cart\n     If more than one file is imported, place them within multiple cuts\n     within a single cart, rather than creating separate carts for each\n     file.\n\n --segue-level=<level>\n     Specify the threshold level to use for setting the segue markers,\n     in dBFS.\n\n --segue-length=<length>\n     Length of the added segue in msecs.\n\n --to-cart=<cartnum>\n     Specify the cart to import the audio into, rather than using the next\n     available cart number for the group.  If the cart does not exist, it will\n     be created.  Each file will be imported into a separate new cut within the\n     cart.  Use of this option implies the '--single-cart' option as well,\n     and is mutually exclusive with the '--use-cartchunk-cutid' option.\n\n --use-cartchunk-cutid\n     Import the audio into the cart specified by the CartChunk CutID parameter\n     associated with the file.  If the cart does not exist, it will be\n     created.  Use of this option is mutually exclusive with the '--to-cart'\n     option.\n\n --title-from-cartchunk-cutid\n     Set the cart title from CartChunk CutID.\n\n --cart-number-offset=<num>\n     Add <num> to the cart number as determined from metadata pattern\n     or --use-cartchunk-cutid\n\n --delete-source\n     Delete each source file after successful import.  Use with caution!\n\n --delete-cuts\n     Delete all cuts within the destination cart before importing.  Use\n     with caution!\n\n --drop-box\n     Operate in DropBox mode.  RDImport will run continuously, periodically\n     scanning for the specified files, importing and then deleting them when\n     found.  WARNING:  use of this option in command-line mode also implies\n     the '--delete-source' option!\n\n --metadata-pattern=<pattern>\n     Attempt to read metadata parameters from the source filename, using\n     the pattern <pattern>.  Patterns consist of a sequence of macros and\n     regular characters to indicate boundaries between metadata fields.\n     The available macros are:\n\n          %a - Artist\n          %b - Record Label\n          %c - Client\n          %e - Agency\n          %g - Rivendell Group\n          %i - Cut Description\n          %l - Album\n          %m - Composer\n          %n - Rivendell Cart Number\n          %o - Outcue\n          %p - Publisher\n          %r - Conductor\n          %s - Song ID\n          %t - Title\n          %u - User Defined\n          %y - Release Year (four digit)\n          %% - A literal '%'\n\n     Detection of either the Rivendell Group [%g] or Rivendell Cart [%n]\n     will cause RDImport to attempt to import the file to the specified Group\n     and/or Cart, overriding whatever values were specified elsewhere on the\n     command line.  If the '--set-user-defined=<str>' option has been used,\n     then the value specified there will be used instead of %u.\n\n     Boundaries between metadata fields are indicated by placing regular\n     characters between macros.  For example, the pattern '%t_%a_%g_%n.',\n     when processing a filename of 'My Song_My Artist_TEMP_123456.mp3',\n     would extract 'My Song' as the title and 'My Artist' as the artist,\n     while importing it into cart 123456 in the TEMP group.\n\n --startdate-offset=<days>\n     If the imported file references a start date, offset the value by\n     <days> days.\n\n --enddate-offset=<days>\n     If the imported file references an end date, offset the value by\n     <days> days.\n\n --create-startdate-offset=<days>\n     If the imported file does not reference a start date, create with \n     startdate offset by <days> days relative to the current date.\n     Cannot be greater than the value for --create-enddate-offset\n     (default = 0).\n\n --create-enddate-offset=<days>\n     If the imported file does not reference an end date, create with\n     end date offset by <days> days relative to the current date.\n     Cannot be less than the value the value for --create-startdate-offset\n     (default = 0).\n\n --set-daypart-times=<start-time>,<end-time>\n     Set the start and end daypart times, in the format HHMMSS.\n\n --fix-broken-formats\n     Attempt to work around malformed audio input data.\n\n --add-scheduler-code=<str>\n     Add Scheduler Code <str> to the target cart.  The specified\n     code must exist in RDAdmin->SchedulerCodes.  This option may be\n     specified multiple times.\n\n --set-user-defined=<str>\n     Set the User Defined field for the target cart to <str>.  This will\n     override any value that might otherwise be set (see the\n    '--metadata-pattern=<pattern>' option above).\n\nNOTES\nIt may be necessary to enclose individual <filespec> clauses in quotes in\norder to protect wildcard characters from expansion by the shell.  A typical\nindicator that this is necessary is the failure of RDImport to process newly\nadded files when running in DropBox mode.\n"
#define RDIMPORT_GLOB_SIZE 10

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private slots:
  void userData();

 private:
  enum Result {Success=0,FileBad=1,NoCart=2,NoCut=3};
  void RunDropBox();
  void ProcessFileList(const QString &flist);
  void ProcessFileEntry(const QString &entry);
  MainObject::Result ImportFile(const QString &filename,unsigned *cartnum);
  void VerifyFile(const QString &filename,unsigned *cartnum);
  RDWaveFile *FixFile(const QString &filename,RDWaveData *wavedata);
  bool IsWav(int fd);
  bool FindChunk(int fd,const char *name,bool *fix_needed);
  bool FixChunkSizes(const QString &filename);
  bool RunPattern(const QString &pattern,const QString &filename,
		  RDWaveData *wavedata,QString *groupname);
  bool VerifyPattern(const QString &pattern);
  void PrintLogDateTime(FILE *f=stdout);
  void DeleteCuts(unsigned cartnum);
  QDateTime GetCachedTimestamp(const QString &filename);
  void WriteTimestampCache(const QString &filename,const QDateTime &dt);
  bool SchedulerCodeExists(const QString &code) const;
  RDConfig *import_config;
  RDCmdSwitch *import_cmd;
  unsigned import_file_key;
  RDGroup *import_group;
  RDRipc *import_ripc;
  RDUser *import_user;
  bool import_verbose;
  bool import_log_mode;
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
  QTime import_dayparts[2];
  bool import_fix_broken_formats;
  int import_persistent_dropbox_id;
  unsigned import_format;
  unsigned import_samprate;
  unsigned import_bitrate;
  unsigned import_channels;
  int import_src_converter;
  int import_normalization_level;
  int import_autotrim_level;
  int import_segue_level;
  int import_segue_length;
  unsigned import_cart_number;
  QString import_metadata_pattern;
  struct DropboxList {
    QString filename;
    unsigned size;
    unsigned pass;
    bool checked;
    bool failed;
  };
  std::list<DropboxList *> import_dropbox_list;
  QString import_temp_fix_filename;
  RDSystem *import_system;
  RDStation *import_station;
};


#endif  // RDIMPORT_H
