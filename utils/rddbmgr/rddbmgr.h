// rddbmgr.h
//
// Rivendell database management utility
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#define RDDBMGR_USAGE "[options]\n"

class MainObject : public QObject
{
 public:
  enum Command {NoCommand=0,ModifyCommand=1,CreateCommand=2,CheckCommand=3};
  MainObject(QObject *parent=0);

 private:
  //
  // check.cpp
  //
  bool Check(QString *err_msg) const;

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
  bool Modify(QString *err_msg,int set_schema,const QString &set_version) const;
  int GetCurrentSchema() const;
  int GetVersionSchema(const QString &ver) const;

  //
  // updateschema.cpp
  //
  bool UpdateSchema(int cur_schema,int set_schema,QString *err_msg) const;
  void AverageCuts89(unsigned cartnum) const;
  void TotalMacros89(unsigned cartnum) const;
  void CreateAuxFieldsTable143(const QString &key_name) const;
  void CreateFeedLog151(const QString &key_name) const;
  bool UpdateLogTable186(const QString &table,QString *err_msg) const;
  bool ConvertTimeField186(const QString &table,const QString &field,
			   QString *err_msg) const;

  //
  // revertschema.cpp
  //
  bool RevertSchema(int cur_schema,int set_schema,QString *err_msg) const;


  Command db_command;
  QString db_mysql_hostname;
  QString db_mysql_loginname;
  QString db_mysql_password;
  QString db_mysql_database;
  QString db_mysql_driver;
  QString db_mysql_engine;
  QString db_mysql_charset;
  QString db_mysql_collation;
  bool db_verbose;
  QString db_table_create_postfix;
  RDConfig *db_config;
};


#endif  // RDDBMGR_H
