// rddbmgr.h
//
// Rivendell database management utility
//
//   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDDBMGR_H
#define RDDBMGR_H

#include <qobject.h>

#include <rdconfig.h>
#include <rdstation.h>

#define RDDBMGR_USAGE "[options]\n"

//
// RSS Templates
//
//
// RSS XML Templates (for the 'RSS_SCHEMAS' table)
//
#define RSS_2_0_2_NAME "RSS 2.0.2"

#define RSS_2_0_2_HEADER_XML "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\">"

#define RSS_2_0_2_CHANNEL_XML "<title>%TITLE%</title>\n<description>%DESCRIPTION%</description>\n<category>%CATEGORY%</category>\n<link>%LINK%</link>\n<language>%LANGUAGE%</language>\n<copyright>%COPYRIGHT%</copyright>\n<lastBuildDate>%BUILD_DATE%</lastBuildDate>\n<pubDate>%PUBLISH_DATE%</pubDate>\n<managingEditor>%EDITOR%</managingEditor>\n<webMaster>%WEBMASTER%</webMaster>\n<generator>%GENERATOR%</generator>\n<image>%IMAGE%</image>\n<atom:link href=\"%FEED_URL%\" rel=\"self\" type=\"application/rss+xml\" />"

#define RSS_2_0_2_ITEM_XML "<title>%ITEM_TITLE%</title>\n<link>%ITEM_LINK%</link>\n<guid isPermaLink=\"false\">%ITEM_GUID%</guid>\n<description>%ITEM_DESCRIPTION%</description>\n<author>%ITEM_AUTHOR%</author>\n<comments>%ITEM_COMMENTS%</comments>\n<source url=\"%ITEM_SOURCE_URL%\">%ITEM_SOURCE_TEXT%</source>\n<enclosure url=\"%ITEM_AUDIO_URL%\" length=\"%ITEM_AUDIO_LENGTH%\"  type=\"audio/mpeg\" />\n<category>%ITEM_CATEGORY%</category>\n<pubDate>%ITEM_PUBLISH_DATE%</pubDate>"


class MainObject : public QObject
{
 public:
  enum Command {NoCommand=0,ModifyCommand=1,CreateCommand=2,CheckCommand=3};
  MainObject(QObject *parent=0);

 private:
  //
  // check.cpp
  //
  bool Check(QString *err_msg);
  void CheckTableAttributes();
  void RewriteTable(const QString &tblname,
		    const QString &old_charset,
		    const QString &old_collation,
		    const QString &new_charset,
		    const QString &new_collation);
  bool RewriteFile(const QString &old_filename,const QString &old_str, 
		   const QString &new_filename,const QString &new_str,
		   QString *err_msg);
  void RelinkAudio(const QString &srcdir) const;
  void CheckOrphanedTracks() const;
  void CheckCutCounts() const;
  void CheckPendingCarts() const;
  void CheckOrphanedCarts() const;
  void CheckOrphanedCuts() const;
  void CheckOrphanedAudio() const;
  void ValidateAudioLengths() const;
  void Rehash(const QString &arg) const;
  void RehashCart(unsigned cartnum) const;
  void RehashCut(const QString &cutnum) const;
  void SetCutLength(const QString &cutname,int len) const;
  void RemoveCart(unsigned cartnum);
  bool CopyFile(const QString &destfile,const QString &srcfile) const;
  bool UserResponse() const;

  //
  // create.cpp
  //
  bool Create(const QString &station_name,bool gen_audio,
	      QString *err_msg) const;
  bool CreateNewDb(QString *err_msg) const;
  bool InititalizeNewDb(const QString &station_name,bool gen_audio,
			QString *err_msg) const;
  bool InsertImportFormats(QString *err_msg) const;
  bool InsertRDAirplayHotkeys(const QString &station_name,
			      QString *err_msg) const;
  bool CreateReconciliationTable(const QString &svc_name,
				 QString *err_msg) const;

  //
  // modify.cpp
  //
  bool Modify(QString *err_msg,int set_schema);
  int GetCurrentSchema() const;
  bool ModifyCharset(const QString &charset,const QString &collation);

  //
  // updateschema.cpp
  //
  bool UpdateSchema(int cur_schema,int set_schema,QString *err_msg);
  void AverageCuts89(unsigned cartnum) const;
  void TotalMacros89(unsigned cartnum) const;
  void CreateAuxFieldsTable143(const QString &key_name) const;
  void CreateFeedLog151(const QString &key_name) const;
  bool UpdateLogTable186(const QString &table,QString *err_msg) const;
  bool ConvertTimeField186(const QString &table,const QString &field,
			   QString *err_msg) const;
  bool ConvertArtistSep307(QString *err_msg) const;

  //
  // revertschema.cpp
  //
  bool RevertSchema(int cur_schema,int set_schema,QString *err_msg);

  //
  // schemamap.cpp
  //
  void InitializeSchemaMap();
  int GetVersionSchema(const QString &ver) const;
  QString GetSchemaVersion(int schema) const;

  //
  // printstatus.cpp
  //
  bool PrintStatus(QString *err_msg) const;

  void WriteSchemaVersion(int ver) const;
  bool TableExists(const QString &tbl_name) const;
  bool DropTable(const QString &tbl_name,QString *err_msg=NULL) const;
  bool ColumnExists(const QString &tbl_name,const QString &col_name) const;
  bool DropColumn(const QString &tbl_name,const QString &col_name,
		  QString *err_msg=NULL) const;
  bool DropIndex(const QString &tbl_name,const QString &idx_name,
		 QString *err_msg=NULL) const;
  Command db_command;
  QString db_mysql_hostname;
  QString db_mysql_loginname;
  QString db_mysql_password;
  QString db_mysql_database;
  QString db_mysql_driver;
  QString db_mysql_engine;
  bool db_verbose;
  bool db_yes;
  bool db_no;
  bool db_check_all;
  bool db_check_orphaned_audio;
  bool db_check_orphaned_tracks;
  bool db_check_orphaned_carts;
  bool db_check_orphaned_cuts;
  QString db_orphan_group_name;
  QString db_dump_cuts_dir;
  QString db_rehash;
  QString db_relink_audio;
  bool db_relink_audio_move;
  QString db_table_create_postfix;
  RDConfig *db_config;
};


#endif  // RDDBMGR_H
