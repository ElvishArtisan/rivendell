// rconf.c
// A small library for handling common configuration file tasks
// 
// Adopted from conflib
//
//   (C) Copyright 1996-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdconf.cpp,v 1.15.4.7.2.1 2014/06/24 18:27:04 cvs Exp $
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

#include <qhostaddress.h>
#include <qvariant.h>
#include <qmessagebox.h>
#include <qdir.h>
#ifdef WIN32
#define RDCONF_FILE_SEPARATOR '\\'
#else
#define RDCONF_FILE_SEPARATOR '/'
#include <unistd.h>
#include <netdb.h>
#include <sys/timex.h>
#include <time.h>
#endif
#include <rddb.h>
#include <rdconf.h>
#include <rdescape_string.h>

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
#ifdef WIN32
  if((!stricmp(temp,"yes"))||(!stricmp(temp,"on"))) {
    return true;
  }
  if((!stricmp(temp,"no"))||(!stricmp(temp,"off"))) {
    return false;
  }
#else
  if((!strcasecmp(temp,"yes"))||(!strcasecmp(temp,"on"))) {
    return true;
  }
  if((!strcasecmp(temp,"no"))||(!strcasecmp(temp,"off"))) {
    return false;
  }
#endif
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
#ifdef WIN32
  for(int i=0;i<strlen(sName);i++) {
    if(sName[i]=='/') {
      sName[i]='\\';
    }
  }
#endif  // WIN32
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

  
#ifndef WIN32
int IncrementIndex(char *sPathname,int dMaxIndex)
{
  int dLockname=-1;
  FILE *hPathname;
  int i;
  char sLockname[256];
  char sAccum[256];
  int dIndex,dNewIndex;

  /* Lock the index */
  strcpy(sLockname,sPathname);
  strcat(sLockname,".LCK");
  i=0;
  while(dLockname<0 && i<MAX_RETRIES) {
      dLockname=open(sLockname,O_WRONLY|O_EXCL|O_CREAT,S_IRUSR|S_IWUSR);
    i++;
  }
  if(dLockname<0) {
    return -1;
  }
  sprintf(sAccum,"%d",getpid());
  write(dLockname,sAccum,strlen(sAccum));
  close(dLockname);

  /* We've got the lock, so read the index */
  hPathname=fopen(sPathname,"r");
  if(hPathname==NULL) {
    unlink(sLockname);
    return -1;
  }
  if(fscanf(hPathname,"%d",&dIndex)!=1) {
    fclose(hPathname);
    unlink(sLockname);
    return -1;
  }
  fclose(hPathname);

  /* Update the index */
  if((dIndex<dMaxIndex) || (dMaxIndex==0)) {
    dNewIndex=dIndex+1;
  }
  else {
    dNewIndex=1;
  }

  /* Write it back */
  hPathname=fopen(sPathname,"w");
  if(hPathname==NULL) {
    unlink(sLockname);
    return -1;
  }
  fprintf(hPathname,"%d",dNewIndex);
  fclose(hPathname);

  /* Release the lock */
  unlink(sLockname);

  /* Ensure a sane value to return and then exit */
  if((dIndex>dMaxIndex)&&(dMaxIndex!=0)) {
    dIndex=1;
  }

  return dIndex;
}
#endif


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




#ifndef WIN32
bool GetLock(const char *sLockname)
{
  int fd;
  char sAccum[256];

  if((fd=open(sLockname,O_WRONLY|O_EXCL|O_CREAT,S_IRUSR|S_IWUSR))<0) {
    printf("failed!\n");
    if(RDCheckPid(RDGetPathPart(sLockname),RDGetBasePart(sLockname))) {
      return false;
    }
    ClearLock(sLockname);
    if((fd=open(sLockname,O_WRONLY|O_EXCL|O_CREAT,S_IRUSR|S_IWUSR))<0) {
      return false;
    }
  }
  sprintf(sAccum,"%d",getpid());
  write(fd,sAccum,strlen(sAccum));
  close(fd);
  return true;
}
#endif




#ifndef WIN32
void ClearLock(const char *sLockname)
{
  unlink(sLockname);
}
#endif  // WIN32


QString RDGetPathPart(QString path)
{
  int c;

  //  c=path.findRev('/');
  c=path.findRev(RDCONF_FILE_SEPARATOR);
  if(c<0) {
    return QString("");
  }
  path.truncate(c+1);
  return path;
}


QString RDGetBasePart(QString path)
{
  int c;

  //  c=path.findRev('/');
  c=path.findRev(RDCONF_FILE_SEPARATOR);
  if(c<0) {
    return path;
  }
  path.remove(0,c+1);
  return path;
}


QString RDGetShortDate(QDate date)
{
  return QString().sprintf("%02d/%02d/%04d",
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
  if(string.contains("Light",false)) {
    return QFont::Light;
  }
  if(string.contains("Normal",false)) {
    return QFont::Normal;
  }
  if(string.contains("DemiBold",false)) {
    return QFont::DemiBold;
  }
  if(string.contains("Bold",false)) {
    return QFont::Bold;
  }
  if(string.contains("Black",false)) {
    return QFont::Black;
  }
  return QFont::Normal;
}


#ifndef WIN32
bool RDDetach(const QString &coredir)
{
  if(!coredir.isEmpty()) {
    chdir(coredir);
  }
  if(daemon(coredir.isEmpty(),0)) {
    return false;
  }
  return true;
}
#endif


bool RDBool(QString string)
{
  if(string.contains("Y",false)) {
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


#ifndef WIN32
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
  return QHostAddress((Q_UINT32)host_address);
}
#endif  // WIN32


QString RDGetDisplay(bool strip_point)
{
#ifdef WIN32
  return QString("win32");
#else
  QString display;
  int l;

  if(getenv("DISPLAY")[0]==':') {
    display=QString().sprintf("%s%s",(const char *)RDGetHostAddr().toString(),
			     getenv("DISPLAY"));
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
#endif  // WIN32
}


bool RDDoesRowExist(const QString &table,const QString &name,
		    const QString &test,QSqlDatabase *db)
{
  RDSqlQuery *q;
  QString sql;

  sql="select `"+name+"` from `"+table+"` where `"+name+"`="+
    "\""+RDEscapeString(test)+"\"";
  q=new RDSqlQuery(sql,db);
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
    QString().sprintf("%d",test);
  q=new RDSqlQuery(sql,db);
  if(q->size()>0) {
    delete q;
    return true;
  }
  delete q;
  return false;
}


QVariant RDGetSqlValue(const QString &table,const QString &name,
		       const QString &test,const QString &param,
		       QSqlDatabase *db,bool *valid)
{
  RDSqlQuery *q;
  QString sql;
  QVariant v;

  sql="select `"+param+"` from `"+table+"` where `"+name+"`="+
    "\""+RDEscapeString(test)+"\"";
  q=new RDSqlQuery(sql,db);
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


QVariant RDGetSqlValue(const QString &table,
		       const QString &name1,const QString &test1,
		       const QString &name2,const QString &test2,
                       const QString &name3,const QString &test3,
		       const QString &param,QSqlDatabase *db,bool *valid)
{
  RDSqlQuery *q;
  QString sql;
  QVariant v;

  sql="select `"+param+"` from `"+table+"` where "+
    "(`"+name1+"`=\""+RDEscapeString(test1)+"\")&&"+
    "(`"+name2+"`=\""+RDEscapeString(test1)+"\")&&"+
    "(`"+name3+"`=\""+RDEscapeString(test1)+"\")";
  q=new RDSqlQuery(sql,db);
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
  q=new RDSqlQuery(sql,db);
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
    QString().sprintf("%d",test);
  q=new RDSqlQuery(sql,db);
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
		       const QString &param,QSqlDatabase *db,bool *valid)
{
  RDSqlQuery *q;
  QString sql;
  QVariant v;

  sql="select `"+param+"` from `"+table+"` where `"+name+"`="+
    QString().sprintf("%u",test);
  q=new RDSqlQuery(sql,db);
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
     return QString().sprintf("%s%d:%02d:%02d.%d",negative,hour,min,seconds,
			      tenthsecs);
    }
    return QString().sprintf("%s%d:%02d:%02d",negative,hour,min,seconds);
  }
  if((hour==0)&&(min==0)) {
    if(tenths) {
      return QString().sprintf("%s:%02d.%d",negative,seconds,tenthsecs);
    }
    return QString().sprintf("%s:%02d",negative,seconds);
  }
  if(hour==0) {
    if(tenths) {
      return QString().sprintf("%s%2d:%02d.%d",negative,min,seconds,
			       tenthsecs);
    }
    return QString().sprintf("%s%2d:%02d",negative,min,seconds);
  }
  if(tenths) {
    return QString().sprintf("%s%2d:%02d:%02d.%d",negative,hour,min,seconds,
			     tenthsecs);
  }
  return QString().sprintf("%s%2d:%02d:%02d",negative,hour,min,seconds);
}


int RDSetTimeLength(const QString &str)
{
  int istate=2;
  QString field;
  int res=0;

  if(str.isEmpty()) {
    return -1;
  }
  for(unsigned i=0;i<str.length();i++) {
    if(str.at(i)==':') {
      istate--;
    }
  }
  if(istate<0) {
    return -1;
  }
  for(unsigned i=0;i<str.length();i++) {
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


#ifndef WIN32
bool RDCopy(const QString &srcfile,const QString &destfile)
{
  int src_fd;
  int dest_fd;
  struct stat src_stat;
  struct stat dest_stat;
  char *buf=NULL;
  int n;

  if((src_fd=open((const char *)srcfile,O_RDONLY))<0) {
    return false;
  }
  if(fstat(src_fd,&src_stat)<0) {
    close(src_fd);
    return false;
  }
  if((dest_fd=open((const char *)destfile,O_RDWR|O_CREAT,src_stat.st_mode))
     <0) {
    close(src_fd);
    return false;
  }
  if(fstat(dest_fd,&dest_stat)<0) {
    close(src_fd);
    close(dest_fd);
    return false;
  }
  buf=(char *)malloc(dest_stat.st_blksize);
  while((n=read(src_fd,buf,dest_stat.st_blksize))==dest_stat.st_blksize) {
    write(dest_fd,buf,dest_stat.st_blksize);
  }
  write(dest_fd,buf,n);
  free(buf);
  close(src_fd);
  close(dest_fd);
  return true;
}
#endif  // WIN32


#ifndef WIN32
bool RDWritePid(const QString &dirname,const QString &filename,int owner,
		int group)
{
  FILE *file;
  mode_t prev_mask;
  QString pathname=QString().sprintf("%s/%s",
				     (const char *)dirname,
				     (const char *)filename);

  prev_mask = umask(0113);      // Set umask so pid files are user and group writable.
  file=fopen((const char *)pathname,"w");
  umask(prev_mask);
  if(file==NULL) {
    return false;
  }
  fprintf(file,"%d",getpid());
  fclose(file);
  chown((const char *)pathname,owner,group);

  return true;
}


void RDDeletePid(const QString &dirname,const QString &filename)
{
  QString pid=QString().sprintf("%s/%s",
				(const char *)dirname,
				(const char *)filename);
  unlink((const char *)pid);
}


bool RDCheckPid(const QString &dirname,const QString &filename)
{
  QDir dir;
  QString path;
  path=QString("/proc/")+
    QString().sprintf("%d",RDGetPid(dirname+QString("/")+filename));
  dir.setPath(path);
  return dir.exists();
}


pid_t RDGetPid(const QString &pidfile)
{
  FILE *handle;
  pid_t ret;

  if((handle=fopen((const char *)pidfile,"r"))==NULL) {
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
#endif  // WIN32


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
  QString simple=str.simplifyWhiteSpace();
  int quan=0;
  int point;

  for(unsigned i=0;i<simple.length();i++) {
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
#ifdef WIN32
  if(getenv("TEMP")!=NULL) {
    return QString(getenv("TEMP"));
  }
  if(getenv("TMP")!=NULL) {
    return QString(getenv("TMP"));
  }
  return QString("C:\\");
#else
  if(getenv("TMPDIR")!=NULL) {
    return QString(getenv("TMPDIR"));
  }
  return QString("/tmp");
#endif  // WIN32
}


QString RDTempFile()
{
#ifndef WIN32
  return QString(tmpnam(NULL));
#endif  // WIN32
  return QString();
}


#ifndef WIN32
QString RDTimeZoneName(const QDateTime &datetime)
{
  char name[20];
  time_t time=datetime.toTime_t();
  strftime(name,20,"%Z",localtime(&time));
  return QString(name);
}
#endif  // WIN32


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


int RDTimeZoneOffset()
{
#ifdef WIN32
  return 0;
#else
  time_t t=time(&t);
  struct tm *tm=localtime(&t);
  time_t local_time=3600*tm->tm_hour+60*tm->tm_min+tm->tm_sec;
  tm=gmtime(&t);
  time_t gmt_time=3600*tm->tm_hour+60*tm->tm_min+tm->tm_sec;

  return gmt_time-local_time;
#endif  // WIN32
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
#ifndef WIN32
  QStringList dirs;
  QDir *proc_dir=new QDir("/proc");
  bool ok=false;
  FILE *f=NULL;
  char line[1024];
  QString cmdline;

  proc_dir->setFilter(QDir::Dirs);
  dirs=proc_dir->entryList();
  for(unsigned i=0;i<dirs.size();i++) {
    dirs[i].toInt(&ok);
    if(ok) {
      if((f=fopen(QString("/proc/")+dirs[i]+"/cmdline","r"))!=NULL) {
	if(fgets(line,1024,f)!=NULL) {
	  QStringList f1=f1.split(" ",QString(line));
	  QStringList f2=f2.split("/",f1[0]);
	  cmdline=f2[f2.size()-1];
	  for(unsigned j=0;j<cmds.size();j++) {
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
#endif  // WIN32
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
  QByteArray ret;
#ifndef WIN32
  int istate=0;
  QString hexcode="";

  for(unsigned i=0;i<str.length();i++) {
    switch(istate) {
    case 0:
      if((str.at(i)=='%')&&(i<(str.length()-2))) {
	hexcode="";
	istate=1;
      }
      else {
	ret.resize(ret.size()+1);
	ret[ret.size()-1]=str.at(i);
      }
      break;

    case 1:
      hexcode=str.at(i);
      istate=2;
      break;

    case 2:
      hexcode+=str.at(i);
      ret.resize(ret.size()+1);
      ret[ret.size()-1]=0xFF&hexcode.toUInt(NULL,16);
      istate=0;
      break;

    default:
      istate=0;
      break;
    }
  }
#endif  // WIN32
  return ret;
}
