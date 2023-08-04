// rdconf.h
//
// The header file for the rconf package
//
//   (C) Copyright 1996-2021 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#ifndef RDCONF_H
#define RDCONF_H

#define MAX_RETRIES 10
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include <QColor>
#include <QDateTime>
#include <QFont>
#include <QHostAddress>
#include <QList>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QVariant>

#include <rdconfig.h>

/* Function Prototypes */
int GetPrivateProfileBool(const char *,const char *,const char *,bool);
int GetPrivateProfileString(const char *,const char *,const char *,char *,
			    const char *,int);
int GetPrivateProfileInt(const char *,const char *,const char *,int);
int GetPrivateProfileHex(const char *,const char *,const char *,int);
double GetPrivateProfileDouble(const char *,const char *,const char *,double);
int GetIni(const char *,const char *,const char *,char *,int);
int GetIniLine(FILE *,char *);
void Prepend(char *,char *);
void StripLevel(char *); 
QString RDGetPathPart(QString path);
QString RDGetBasePart(QString path);
QString RDGetShortDate(QDate);
/**
 * Returns the name of the weekday in english regardless of the locale
 * configured.
 * @param weekday Integer value for the weekday; 1 = "Mon", 2 = "Tue", 
 * ... 7 = "Sun".  If the value is out of range 1 is defaulted to.
 **/
QString RDGetShortDayNameEN(int weekday);
QFont::Weight RDGetFontWeight(QString);
bool RDDetach(const QString &coredir);
bool RDBool(QString);
QString RDYesNo(bool);
QHostAddress RDGetHostAddr();
QString RDGetDisplay(bool strip_point=false);
bool RDDoesRowExist(const QString &table,const QString &name,
		    const QString &test,QSqlDatabase *db=0);
bool RDDoesRowExist(const QString &table,const QString &name,unsigned test,
		    QSqlDatabase *db=0);
QVariant RDGetSqlValue(const QString &table,const QString &name,
		       const QString &test,const QString &param,
		       bool *valid=0);
QVariant RDGetSqlValue(const QString &table,const QString &name,unsigned test,
		       const QString &param,bool *valid=0);
bool RDIsSqlNull(const QString &table,const QString &name,
		 const QString &test,const QString &param,QSqlDatabase *db=0);
bool RDIsSqlNull(const QString &table,const QString &name,unsigned test,
		 const QString &param,QSqlDatabase *db=0);
QString RDGetTimeLength(int mseconds,bool leadzero=false,bool tenths=true);
int RDSetTimeLength(const QString &string);
bool RDCopy(const QString &srcfile,const QString &destfile);
bool RDCopy(const QString &srcfile,int dest_fd);
bool RDCopy(int src_fd,const QString &destfile);
bool RDCopy(int src_fd,int dest_fd);
bool RDWritePid(const QString &dirname,const QString &filename,int owner=-1,
		int group=-1);
void RDDeletePid(const QString &dirname,const QString &filename);
bool RDCheckPid(const QString &dirname,const QString &filename);
pid_t RDGetPid(const QString &pidfile);
QString RDGetHomeDir(bool *found=0);
bool RDTimeSynced();
QString RDTruncateAfterWord(QString str,int word,bool add_dots=false);
QString RDHomeDir();
QString RDTempDir();
QString RDTempFile();
QString RDTimeZoneName(const QDateTime &datetime);
QString RDDowCode(int dow);
QDateTime RDLocalToUtc(const QDateTime &localdatetime);
QTime RDLocalToUtc(const QTime &localtime);
QDateTime RDUtcToLocal(const QDateTime &gmtdatetime);
QTime RDUtcToLocal(const QTime &gmttime);
QColor RDGetTextColor(const QColor &background_color);
bool RDProcessActive(const QString &cmd);
bool RDProcessActive(const QStringList &cmds);
bool RDModulesActive();
QByteArray RDStringToData(const QString &str);
QString RDStringToHex(const QString &str);
QList<pid_t> RDGetPids(const QString &program);
int RDCheckExitCode(const QString &msg,int exit_code);
int RDCheckExitCode(RDConfig *config,const QString &msg,int exit_code);
int RDCheckReturnCode(const QString &msg,int code,int ok_value);
QString RDMimeType(const QString &filename,bool *ok);
QString RDMimeType(const QByteArray &data,bool *ok);
QString RDWrapText(const QString &str,int width);

#endif   // RDCONF_H
