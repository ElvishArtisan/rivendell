// rdconf.h
//
// The header file for the rconf package
//
//   (C) Copyright 1996-2004 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdconf.h,v 1.10.6.3 2013/12/03 23:34:34 cvs Exp $
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

#include <qstring.h>
#include <qdatetime.h>
#include <qfont.h>
#include <qhostaddress.h>
#include <qsqldatabase.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qcolor.h>
#include <qstringlist.h>
#include <qcstring.h>

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
int IncrementIndex(char *,int);
void StripLevel(char *); 
bool GetLock(const char *);
void ClearLock(const char *);
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
		       QSqlDatabase *db=0,bool *valid=0);
QVariant RDGetSqlValue(const QString &table,const QString &name,unsigned test,
		       const QString &param,QSqlDatabase *db=0,bool *valid=0);
QVariant RDGetSqlValue(const QString &table,
		       const QString &name1,const QString &test1,
		       const QString &name2,const QString &test2,
		       const QString &name3,const QString &test3,
		       const QString &param,QSqlDatabase *db=0,bool *valid=0);
bool RDIsSqlNull(const QString &table,const QString &name,
		 const QString &test,const QString &param,QSqlDatabase *db=0);
bool RDIsSqlNull(const QString &table,const QString &name,unsigned test,
		 const QString &param,QSqlDatabase *db=0);
QString RDGetTimeLength(int mseconds,bool leadzero=false,bool tenths=true);
int RDSetTimeLength(const QString &string);
bool RDCopy(const QString &srcfile,const QString &destfile);
#ifndef WIN32
bool RDWritePid(const QString &dirname,const QString &filename,int owner=-1,
		int group=-1);
void RDDeletePid(const QString &dirname,const QString &filename);
bool RDCheckPid(const QString &dirname,const QString &filename);
pid_t RDGetPid(const QString &pidfile);
#endif  // WIN32
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
int RDTimeZoneOffset();
QColor RDGetTextColor(const QColor &background_color);
bool RDProcessActive(const QString &cmd);
bool RDProcessActive(const QStringList &cmds);
bool RDModulesActive();
QByteArray RDStringToData(const QString &str);

#endif   // RDCONF_H
