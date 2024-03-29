// rdconf.cpp
//
//  Small library for handling common configuration file tasks
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/timex.h>
#include <time.h>

#include <QDir>
#include <QProcess>
#include <QStringList>

#include <rdapplication.h>

#define RDCONF_FILE_SEPARATOR '/'

#include "rddb.h"
#include "rdconf.h"
#include "rddatetime.h"
#include "rdescape_string.h"

#define BUFFER_SIZE 1024

int GetPrivateProfileBool(const char *sFilename,const char *cHeader,
			 const char *cLabel,bool bDefault=false)
{
  char temp[255];

  if(GetPrivateProfileString(sFilename,cHeader,cLabel,temp,"",254)<0) {
    return bDefault;
  }
  if(temp[0]==0) {
    return bDefault;
  }
  if((!strcasecmp(temp,"yes"))||(!strcasecmp(temp,"on"))) {
    return true;
  }
  if((!strcasecmp(temp,"no"))||(!strcasecmp(temp,"off"))) {
    return false;
  }
  return bDefault;
}


int GetPrivateProfileString(const char *sFilename,const char *cHeader,
			    const char *cLabel,char *cValue,
			    const char *cDefault,int dValueLength)
{
  int i;
  
  i=GetIni(sFilename,cHeader,cLabel,cValue,dValueLength);
  if(i==0) {
    return 0;
  }
  else {
    strcpy(cValue,cDefault);
    return -1;
  }
}


int GetPrivateProfileInt(const char *sFilename,const char *cHeader,
			 const char *cLabel,int dDefault)
{
  int c;
  char sNum[12];

  if(GetIni(sFilename,cHeader,cLabel,sNum,11)==0) {
    if(sscanf(sNum,"%d",&c)==1) {
      return c;
    }
    else {
      return dDefault;
    }
  }
  else {
    return dDefault;
  }
}


int GetPrivateProfileHex(const char *sFilename,const char *cHeader,
			 const char *cLabel,int dDefault)
{
  char temp[256];
  int n=dDefault;

  GetPrivateProfileString(sFilename,cHeader,cLabel,temp,"",255);
  sscanf(temp,"0x%x",&n);
  return n;
}


double GetPrivateProfileDouble(const char *sFilename,const char *cHeader,
			 const char *cLabel,double dfDefault)
{
  char temp[256];
  double n=dfDefault;

  GetPrivateProfileString(sFilename,cHeader,cLabel,temp,"",255);
  sscanf(temp,"%lf",&n);
  return n;
}


int GetIni(const char *sFileName,const char *cHeader,const char *cLabel,
	   char *cValue,int dValueLength)	
     /* get a value from the ini file */
     
{
  FILE *cIniName;
  char sName[BUFFER_SIZE];
  char cIniBuffer[BUFFER_SIZE],cIniHeader[80],cIniLabel[80];
  int i,j;
  /*  int iFileStat=NULL;  */
  int iFileStat;

  strcpy(sName,sFileName);
  cIniName=fopen(sName,"r");
  if(cIniName==NULL) {
    return 2;	/* ini file doesn't exist! */
  }
  while(GetIniLine(cIniName,cIniBuffer)!=EOF) {
    if(cIniBuffer[0]=='[') {	/* start of section */
      i=1;
      while(cIniBuffer[i]!=']' && cIniBuffer!=0) {
	cIniHeader[i-1]=cIniBuffer[i];
	i++;
      }
      cIniHeader[i-1]=0;
      if(strcmp(cIniHeader,cHeader)==0) {		/* this is the right section! */
	iFileStat=EOF+1;   /* Make this anything other than EOF! */
	while(iFileStat!=EOF) {
	  iFileStat=GetIniLine(cIniName,cIniBuffer);
	  if(cIniBuffer[0]=='[') return 1;
	  i=0;
	  while(cIniBuffer[i]!='=' && cIniBuffer[i]!=0) {
	    cIniLabel[i]=cIniBuffer[i];
	    i++;
	  }
	  cIniLabel[i++]=0;
	  if(strcmp(cIniLabel,cLabel)==0) {	/* this is the right label! */
	    j=0;
	    while(j<dValueLength && cIniBuffer[i]!=0) {
	      cValue[j++]=cIniBuffer[i++];
	    }
	    cValue[j]=0;
	    fclose(cIniName);
	    return 0;		/* value found! */
	  }
	}
      }
    }
  }
  fclose(cIniName);
  return 1;		/* section or label not found! */
}




int GetIniLine(FILE *cIniName,char *cIniBuffer)		/* read a line from the ini file */
     
{
  int i;
  
  for(i=0;i<BUFFER_SIZE-1;i++) {
    cIniBuffer[i]=getc(cIniName);
    switch(cIniBuffer[i]) {
      
    case 10:
      cIniBuffer[i]=0;
      return 0;
    }
  }
  return 0;
}


void Prepend(char *sPathname,char *sFilename)
{
  char sTemp[256];

  if(sPathname[strlen(sPathname)-1]!='/' && sFilename[0]!='/') {
    strcat(sPathname,"/");
  }
  strcpy(sTemp,sPathname);
  strcat(sTemp,sFilename);
  strcpy(sFilename,sTemp);
}


/*
 * int StripLevel(char *sString)
 *
 * This strips the lower-most level from the pathname pointed to by 'sString'
 */

void StripLevel(char *sString)
{
  int i;                              /* General Purpose Pointer */
  int dString;                        /* Initial Length of 'sString' */

  dString=strlen(sString)-1;
  for(i=dString;i>=0;i--) {
    if(sString[i]=='/') {
      sString[i]=0;
      return;
    }
  }
  sString[0]=0;
}


QString RDGetPathPart(QString path)
{
  int c;

  c=path.lastIndexOf(RDCONF_FILE_SEPARATOR);
  if(c<0) {
    return QString("");
  }
  path.truncate(c+1);
  return path;
}


QString RDGetBasePart(QString path)
{
  int c;

  c=path.lastIndexOf(RDCONF_FILE_SEPARATOR);
  if(c<0) {
    return path;
  }
  path.remove(0,c+1);
  return path;
}


QString RDGetShortDate(QDate date)
{
  return QString::asprintf("%02d/%02d/%04d",
			   date.month(),date.day(),date.year());
}

QString RDGetShortDayNameEN(int weekday)
{
  QString day_name;
  if ( weekday < 1 || weekday > 7 )
    weekday = 1;

  if (weekday == 1)
    day_name = "Mon";
  else if (weekday == 2)
    day_name = "Tue";
  else if (weekday == 3)
    day_name = "Wed";
  else if (weekday == 4)
    day_name = "Thu";
  else if (weekday == 5)
    day_name = "Fri";
  else if (weekday == 6)
    day_name = "Sat";
  else if (weekday == 7)
    day_name = "Sun";
  return day_name;
}

QFont::Weight RDGetFontWeight(QString string)
{
  if(string.contains("Light",Qt::CaseInsensitive)) {
    return QFont::Light;
  }
  if(string.contains("Normal",Qt::CaseInsensitive)) {
    return QFont::Normal;
  }
  if(string.contains("DemiBold",Qt::CaseInsensitive)) {
    return QFont::DemiBold;
  }
  if(string.contains("Bold",Qt::CaseInsensitive)) {
    return QFont::Bold;
  }
  if(string.contains("Black",Qt::CaseInsensitive)) {
    return QFont::Black;
  }
  return QFont::Normal;
}


bool RDDetach(const QString &coredir)
{
  if(!coredir.isEmpty()) {
    RDCheckExitCode("RDDetach chdir",chdir(coredir.toUtf8()));
  }
  if(daemon(coredir.isEmpty(),0)) {
    return false;
  }
  return true;
}


bool RDBool(QString string)
{
  if(string.contains("Y",Qt::CaseInsensitive)) {
    return true;
  }
  return false;
}


QString RDYesNo(bool state)
{
  if(state) {
    return QString("Y");
  }
  return QString("N");
}


QHostAddress RDGetHostAddr()
{
  FILE *file;
  char host_name[256];
  struct hostent *host_ent;
  int host_address;

  if((file=fopen("/etc/HOSTNAME","r"))==NULL) {
    return QHostAddress();
  }
  if(fscanf(file,"%s",host_name)!=1) {
    return QHostAddress();
  }
  if((host_ent=gethostbyname(host_name))==NULL) {
    return QHostAddress();
  }
  host_address=16777216*(host_ent->h_addr_list[0][0]&0xff)+
    65536*(host_ent->h_addr_list[0][1]&0xff)+
    256*(host_ent->h_addr_list[0][2]&0xff)+
    (host_ent->h_addr_list[0][3]&0xff);
  return QHostAddress((uint32_t)host_address);
}


QString RDGetDisplay(bool strip_point)
{
  QString display;
  int l;

  if(getenv("DISPLAY")[0]==':') {
    display=RDGetHostAddr().toString()+QString(getenv("DISPLAY"));
  }
  else {
    display=QString(getenv("DISPLAY"));
  }
  if(strip_point) {
    l=display.length();
    while(display.at(l)!=':') {
      if(display.at(l--)=='.') {
	return display.left(l+1);
      }    
    }
  }
  return display;
}


bool RDDoesRowExist(const QString &table,const QString &name,
		    const QString &test,QSqlDatabase *db)
{
  RDSqlQuery *q;
  QString sql;

  sql="select `"+name+"` from `"+table+"` where `"+name+"`="+
    "\""+RDEscapeString(test)+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    delete q;
    return true;
  }
  delete q;
  return false;
}


bool RDDoesRowExist(const QString &table,const QString &name,unsigned test,
		    QSqlDatabase *db)
{
  RDSqlQuery *q;
  QString sql;

  sql="select `"+name+"` from `"+table+"` where `"+name+"`="+
    QString::asprintf("%d",test);
  q=new RDSqlQuery(sql);
  if(q->size()>0) {
    delete q;
    return true;
  }
  delete q;
  return false;
}


QVariant RDGetSqlValue(const QString &table,const QString &name,
		       const QString &test,const QString &param,
		       bool *valid)
{
  RDSqlQuery *q;
  QString sql;
  QVariant v;

  sql="select `"+param+"` from `"+table+"` where `"+name+"`="+
    "\""+RDEscapeString(test)+"\"";
  q=new RDSqlQuery(sql);
  if(q->isActive()) {
    q->first();
    v=q->value(0);
    if(valid!=NULL) {
      *valid=!q->isNull(0);
    }
    delete q;
    return v;
  }
  delete q;
  return QVariant();
}


bool RDIsSqlNull(const QString &table,const QString &name,const QString &test,
		 const QString &param,QSqlDatabase *db)
{
  RDSqlQuery *q;
  QString sql;

  sql="select `"+param+"` from `"+table+"` where `"+name+"`="+
    "\""+RDEscapeString(test)+"\"";
  q=new RDSqlQuery(sql);
  if(q->isActive()) {
    q->first();
    if(q->isNull(0)) {
      delete q;
      return true;
    }
    else {
      delete q;
      return false;
    }
  }
  delete q;
  return true;
}


bool RDIsSqlNull(const QString &table,const QString &name,unsigned test,
		 const QString &param,QSqlDatabase *db)
{
  RDSqlQuery *q;
  QString sql;

  sql="select `"+param+"` from `"+table+"` where `"+name+"`="+
    QString::asprintf("%d",test);
  q=new RDSqlQuery(sql);
  if(q->isActive()) {
    q->first();
    if(q->isNull(0)) {
      delete q;
      return true;
    }
    else {
      delete q;
      return false;
    }
  }
  delete q;
  return true;
}


QVariant RDGetSqlValue(const QString &table,const QString &name,unsigned test,
		       const QString &param,bool *valid)
{
  RDSqlQuery *q;
  QString sql;
  QVariant v;

  sql="select `"+param+"` from `"+table+"` where `"+name+"`="+
    QString::asprintf("%u",test);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    v=q->value(0);
    if(valid!=NULL) {
      *valid=!q->isNull(0);
    }
    delete q;
    return v;
  }
  delete q;
  return QVariant();
}


QString RDGetTimeLength(int mseconds,bool leadzero,bool tenths)
{
  int hour,min,seconds,tenthsecs;
  char negative[2];

  if(mseconds<0) {
    mseconds=-mseconds;
    strcpy(negative,"-");
  }
  else {
    negative[0]=0;
  }
  QTime time_length(QTime(0,0,0).addMSecs(mseconds));
  hour = time_length.hour();
  min = time_length.minute();
  seconds = time_length.second();
  mseconds = time_length.msec();
  tenthsecs=mseconds/100;
  if(leadzero) {
    if(tenths) {
      return QString(negative)+
	QString::asprintf("%d:%02d:%02d.%d",hour,min,seconds,tenthsecs);
    }
    return QString(negative)+
      QString::asprintf("%d:%02d:%02d",hour,min,seconds);
  }
  if((hour==0)&&(min==0)) {
    if(tenths) {
      return QString(negative)+QString::asprintf(":%02d.%d",seconds,tenthsecs);
    }
    return QString(negative)+QString::asprintf(":%02d",seconds);
  }
  if(hour==0) {
    if(tenths) {
      return QString(negative)+
	QString::asprintf("%2d:%02d.%d",min,seconds,tenthsecs);
    }
    return QString(negative)+QString::asprintf("%2d:%02d",min,seconds);
  }
  if(tenths) {
    return QString(negative)+
      QString::asprintf("%2d:%02d:%02d.%d",hour,min,seconds,tenthsecs);
  }
  return QString(negative)+QString::asprintf("%2d:%02d:%02d",hour,min,seconds);
}


int RDSetTimeLength(const QString &str)
{
  int istate=2;
  QString field;
  int res=0;

  if(str.isEmpty()) {
    return -1;
  }
  for(int i=0;i<str.length();i++) {
    if(str.at(i)==':') {
      istate--;
    }
  }
  if(istate<0) {
    return -1;
  }
  for(int i=0;i<str.length();i++) {
    if(str.at(i).isNumber()) {
      field+=str.at(i);
    }
    else {
      if((str.at(i)==':')||(str.at(i)=='.')) {
	if(field.length()>2) {
	  return -1;
	}
	switch(istate) {
	    case 0:
	      res+=3600000*field.toInt();
	      break;

	    case 1:
	      res+=60000*field.toInt();
	      break;

	    case 2:
	      res+=1000*field.toInt();
	      break;
	}
	istate++;
	field="";
      }
      else {
	if(!str.at(i).isSpace()) {
	  return -2;
	}
      }
    }
  }
  switch(istate) {
      case 2:
	res+=1000*field.toInt();
	break;

      case 3:
	switch(field.length()) {
	    case 1:
	      res+=100*field.toInt();
	      break;

	    case 2:
	      res+=10*field.toInt();
	      break;

	    case 3:
	      res+=field.toInt();
	      break;
	}
  }

  return res;
}


bool RDCopy(const QString &srcfile,const QString &destfile)
{
  int src_fd;
  int dest_fd;

  if((src_fd=open(srcfile.toUtf8(),O_RDONLY))<0) {
    return false;
  }
  if((dest_fd=open(destfile.toUtf8(),O_WRONLY|O_CREAT,S_IWUSR))<0) {
    close(src_fd);
    return false;
  }
  bool ret=RDCopy(src_fd,dest_fd);
  close(src_fd);
  close(dest_fd);

  return ret;
}


bool RDCopy(const QString &srcfile,int dest_fd)
{
  int src_fd;

  if((src_fd=open(srcfile.toUtf8(),O_RDONLY))<0) {
    return false;
  }
  bool ret=RDCopy(src_fd,dest_fd);
  close(src_fd);

  return ret;
}


bool RDCopy(int src_fd,const QString &destfile)
{
  int dest_fd;

  if((dest_fd=open(destfile.toUtf8(),O_WRONLY|O_CREAT,S_IWUSR))<0) {
    return false;
  }
  bool ret=RDCopy(src_fd,dest_fd);
  close(dest_fd);

  return ret;
}


bool RDCopy(int src_fd,int dest_fd)
{
  struct stat src_stat;
  struct stat dest_stat;
  char *buf=NULL;
  int n;

  if(fstat(src_fd,&src_stat)<0) {
    return false;
  }
  if(fstat(dest_fd,&dest_stat)<0) {
    return false;
  }
  if(fchmod(dest_fd,src_stat.st_mode)<0) {
    return false;
  }
  buf=(char *)malloc(dest_stat.st_blksize);
  while((n=read(src_fd,buf,dest_stat.st_blksize))==dest_stat.st_blksize) {
    RDCheckExitCode("RDCopy write",write(dest_fd,buf,dest_stat.st_blksize));
  }
  RDCheckExitCode("RDCopy write",write(dest_fd,buf,n));
  free(buf);

  return true;
}


bool RDWritePid(const QString &dirname,const QString &filename,int owner,
		int group)
{
  FILE *file;
  mode_t prev_mask;
  QString pathname=dirname+"/"+filename;

  prev_mask = umask(0113);      // Set umask so pid files are user and group writable.
  file=fopen(pathname.toUtf8(),"w");
  umask(prev_mask);
  if(file==NULL) {
    return false;
  }
  fprintf(file,"%d",getpid());
  fclose(file);
  RDCheckExitCode("RDWritePid chown",chown(pathname.toUtf8(),owner,group));

  return true;
}


void RDDeletePid(const QString &dirname,const QString &filename)
{
  QString pid=dirname+"/"+filename;
  unlink(pid.toUtf8());
}


bool RDCheckPid(const QString &dirname,const QString &filename)
{
  QDir dir;
  QString path;
  path=QString("/proc/")+
    QString::asprintf("%d",RDGetPid(dirname+QString("/")+filename));
  dir.setPath(path);
  return dir.exists();
}


pid_t RDGetPid(const QString &pidfile)
{
  FILE *handle;
  pid_t ret;

  if((handle=fopen(pidfile.toUtf8(),"r"))==NULL) {
    return -1;
  }
  if(fscanf(handle,"%d",&ret)!=1) {
    ret=-1;
  }
  fclose(handle);
  return ret;
}


bool RDTimeSynced()
{
  struct timex timex;

  memset(&timex,0,sizeof(struct timex));
  if(adjtimex(&timex)==TIME_OK) {
    return true;
  }
  return false;
}


QString RDGetHomeDir(bool *found)
{
  if(getenv("HOME")==NULL) {
    if(found!=NULL) {
      *found=false;
    }
    return QString("/");
  }
  if(found!=NULL) {
    *found=true;
  }
  return QString(getenv("HOME"));
}


QString RDTruncateAfterWord(QString str,int word,bool add_dots)
{
  QString simple=str.simplified();
  int quan=0;
  int point;

  for(int i=0;i<simple.length();i++) {
    if(simple.at(i).isSpace()) {
      quan++;
      point=i;
      if(quan==word) {
	if(add_dots) {
	  return simple.left(point)+QString("...");
	}
	else {
	  return simple.left(point);
	}
      }
    }
  }
  return simple;
}


QString RDHomeDir()
{
  if(getenv("HOME")==NULL) {
    return QString("/");
  }
  return QString(getenv("HOME"));
}


QString RDTempDir()
{
  char path[PATH_MAX]="/tmp/rddbmgrXXXXXX";

  return QString(mkdtemp(path));
}


QString RDTempFile()
{
  int fd=-1;
  char dirname[PATH_MAX];
  strncpy(dirname,"/tmp/rivendellXXXXXX",PATH_MAX);
  if((fd=mkstemp(dirname))>0) {
    close(fd);
    return QString(dirname);
  }
  return QString();
}


QString RDTimeZoneName(const QDateTime &datetime)
{
  char name[20];
  time_t time=datetime.toTime_t();
  strftime(name,20,"%Z",localtime(&time));
  return QString(name);
}


QString RDDowCode(int dow)
{
  QString ret;
  switch(dow) {
    case 1:
      ret=QString("MON");
      break;

    case 2:
      ret=QString("TUE");
      break;

    case 3:
      ret=QString("WED");
      break;

    case 4:
      ret=QString("THU");
      break;

    case 5:
      ret=QString("FRI");
      break;

    case 6:
      ret=QString("SAT");
      break;

    case 7:
      ret=QString("SUN");
      break;
  }
  return ret;
}


QDateTime RDLocalToUtc(const QDateTime &localdatetime)
{
  return localdatetime.addSecs(RDTimeZoneOffset());
}


QTime RDLocalToUtc(const QTime &localtime)
{
  return localtime.addSecs(RDTimeZoneOffset());
}


QDateTime RDUtcToLocal(const QDateTime &gmtdatetime)
{
  return gmtdatetime.addSecs(-RDTimeZoneOffset());
}


QTime RDUtcToLocal(const QTime &gmttime)
{
  return gmttime.addSecs(-RDTimeZoneOffset());
}


QColor RDGetTextColor(const QColor &background_color)
{
  int h,s,v;
  QColor color=background_color;

  background_color.getHsv(&h,&s,&v);
  if(v<128) {
    color=Qt::white;
  }
  else {
    if((h>210)&&(h<270)&&(s>128)) {  // Special case for blue
      color=Qt::white;
    }
    else {
      color=Qt::black;
    }
  }

  return color;
}


bool RDProcessActive(const QString &cmd)
{
  QStringList cmds;

  cmds.push_back(cmd);
  return RDProcessActive(cmds);
}


bool RDProcessActive(const QStringList &cmds)
{
  QStringList dirs;
  QDir *proc_dir=new QDir("/proc");
  bool ok=false;
  FILE *f=NULL;
  char line[1024];
  QString cmdline;

  proc_dir->setFilter(QDir::Dirs);
  dirs=proc_dir->entryList();
  for(int i=0;i<dirs.size();i++) {
    dirs[i].toInt(&ok);
    if(ok) {
      if((f=fopen((QString("/proc/")+dirs[i]+"/cmdline").toUtf8(),"r"))!=NULL) {
	if(fgets(line,1024,f)!=NULL) {
	  QStringList f1=QString(line).split(" ",QString::SkipEmptyParts);
	  QStringList f2=f1[0].split("/",QString::SkipEmptyParts);
	  cmdline=f2[f2.size()-1];
	  for(int j=0;j<cmds.size();j++) {
	    if(cmdline==cmds[j]) {
	      fclose(f);
	      return true;
	    }
	  }
	}
	fclose(f);
      }
    }
  }

  delete proc_dir;
  return false;
}


bool RDModulesActive()
{
  QStringList cmds;

  cmds.push_back("rdadmin");
  cmds.push_back("rdairplay");
  cmds.push_back("rdcastmanager");
  cmds.push_back("rdcatch");
  cmds.push_back("rdlibrary");
  cmds.push_back("rdlogedit");
  cmds.push_back("rdlogin");
  cmds.push_back("rdlogmanager");
  cmds.push_back("rdpanel");
  cmds.push_back("rddbcheck");
  cmds.push_back("rdgpimon");
  return RDProcessActive(cmds);
}


QByteArray RDStringToData(const QString &str)
{
  int istate=0;
  unsigned n;
  QString code;
  QByteArray ret;
  bool ok=false;

  for(int i=0;i<str.length();i++) {
    switch(istate) {
    case 0:
      if(str.at(i)==QChar('%')) {
	istate=1;
      }
      else {
	ret+=str.at(i);
      }
      break;

    case 1:
      n=str.mid(i,1).toUInt(&ok);
      if((!ok)||(n>9)) {
	istate=0;
      }
      code=str.mid(i,1);
      istate=2;
      break;

    case 2:
      n=str.mid(i,1).toUInt(&ok);
      if((!ok)||(n>9)) {
	istate=0;
      }
      code+=str.mid(i,1);
      ret+=code.toUInt(NULL,16);
      istate=0;
      break;
    }
  }

  return ret;
}


QString RDStringToHex(const QString &str)
{
  QByteArray bytes=str.toUtf8();
  QString ret="";

  for(int i=0;i<bytes.length();i++) {
    ret+=QString::asprintf("%02X ",0xFF&bytes[i]);
  }

  return ret;
}


QList<pid_t> RDGetPids(const QString &program)
{
  QList<pid_t> pids;
  bool ok=false;

  QDir procdir("/proc");
  QStringList files=
    procdir.entryList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);

  for(int i=0;i<files.size();i++) {
    pid_t pid=files.at(i).toInt(&ok);
    if(ok) {
      QFile file(QString("/proc/")+files.at(i)+"/cmdline");
      if(file.open(QIODevice::ReadOnly)) {
	QTextStream strm(&file);
	strm.setCodec("UTF-8");
	QStringList f0=strm.readLine().split(" ");
	QStringList f1=f0.at(0).split("/");
	if(f1.back().left(f1.back().length()-1)==program.trimmed()) {
	  pids.push_back(pid);
	}
      }
    }
  }

  return pids;
}


int RDCheckExitCode(const QString &msg,int exit_code)
{
  if(exit_code<0) {
    rda->syslog(LOG_WARNING,"%s returned non-zero exit code %d [%s]",
		msg.toUtf8().constData(),exit_code,strerror(errno));
  }
  return exit_code;
}


int RDCheckExitCode(RDConfig *config,const QString &msg,int exit_code)
{
  if(exit_code<0) {
    RDApplication::syslog(config,LOG_WARNING,
			  "%s returned non-zero exit code %d [%s]",
			  msg.toUtf8().constData(),exit_code,strerror(errno));
  }
  return exit_code;
}


int RDCheckReturnCode(const QString &msg,int code,int ok_value)
{
  if(code!=ok_value) {
    rda->syslog(LOG_WARNING,"%s returned %d, was expecting %d",
		msg.toUtf8().constData(),code,ok_value);
  }
  return code;
}


QString RDMimeType(const QString &filename,bool *ok)
{
  QStringList args;
  QString ret;

  args.push_back("--mime-type");
  args.push_back(filename);
  QProcess *proc=new QProcess();
  proc->start("/usr/bin/file",args);
  proc->waitForFinished();
  if((proc->exitStatus()!=QProcess::NormalExit)||(proc->exitCode()!=0)) {
    *ok=false;
    delete proc;
    return ret;
  }
  *ok=true;
  ret=QString(proc->readAllStandardOutput()).
    split(":",QString::SkipEmptyParts).last().trimmed();

  delete proc;

  return ret;
}


QString RDMimeType(const QByteArray &data,bool *ok)
{
  QStringList args;
  QString ret;
  QByteArray ret_data;

  args.push_back("--mime-type");
  args.push_back("-");
  QProcess *proc=new QProcess();
  proc->start("/usr/bin/file",args);
  proc->waitForStarted();
  proc->write(data);
  proc->closeWriteChannel();
  proc->waitForFinished();
  if((proc->exitStatus()!=QProcess::NormalExit)||(proc->exitCode()!=0)) {
    *ok=false;
    delete proc;
    return ret;
  }
  *ok=true;

  ret=QString(proc->readAllStandardOutput()).
    split(":",QString::SkipEmptyParts).last().trimmed();

  delete proc;

  return ret;
}


QString RDWrapText(const QString &str,int width)
{
  QString line;
  QString ret;
  QStringList f0=str.split(" ",QString::KeepEmptyParts);
  int fn=0;

  while(fn<f0.size()) {
    if((line.length()+1+f0.at(fn).length())<width) {
      line+=" "+f0.at(fn++);
    }
    else {
      ret+=line.trimmed()+"\n";
      line="";
    }
  }
  ret+=line.trimmed()+"\n";
  
  return ret.trimmed();
}
