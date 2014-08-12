// rdweb.cpp
//
// Functions for interfacing with web components using the
// Common Gateway Interface (CGI) Standard 
//
//   (C) Copyright 1996-2007 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <qdatetime.h>
#include <qstringlist.h>

#include <rduser.h>
#include <rddb.h>
#include <rdconf.h>
#include <rdwebresult.h>

#include <rdweb.h>


/* RDReadPost(char *cBuffer,int dSize) */

/* This function reads POST data (such as that submitted by an HTML form) into
the buffer pointed to by cBuffer.  The size of the buffer is indicated by 
dSize.

RETURNS: Number of bytes read if the function is successful
         -1 if an error is encountered. */

int RDReadPost(char *cBuffer,int dSize)

{
  int dPostSize=0;

  if(strcasecmp(getenv("REQUEST_METHOD"),"POST")!=0) {   /* No post data to receive! */
    return -1;
  }
  sscanf(getenv("CONTENT_LENGTH"),"%d",&dPostSize);
  if(dPostSize>=dSize) {  /* Data block too large! */
    return -1;
  }
  dPostSize++;
  fgets(cBuffer,dPostSize,stdin);
  return dPostSize;  
}



/*
 * int RDPutPostString(char *sPost,char *sArg,char *sValue,int dMaxSize)
 *
 * This function changes the contents of the POST buffer pointed to by
 * 'sPost'.  If the entry pointed to by 'sArg' exists, it's value is
 * replaced by the string pointed to by 'sValue'.  If the entry doesn't
 * exist, it is created.  'dMaxSize' is the maximum allowable size of 'sPost'.
 *
 * RETURNS:  If successful, a pointer to the start of the updated value
 *           If unsuccessful, -1
 */
int RDPutPostString(char *sPost,char *sArg,char *sValue,int dMaxSize)
{
  int dOrigin;         /* Start of insert point */
  int dValue;          /* Length of sValue */
  int i;               /* General purpose counter */
  char sAccum[CGI_ACCUM_SIZE];

  /*
   * Does the argument already exist?
   */
  dOrigin=RDFindPostString(sPost,sArg,sAccum,CGI_ACCUM_SIZE);
  if(dOrigin<0) {
    /* 
     * Create a new entry
     * Will it fit?
     */
    dOrigin=strlen(sPost);
    if((dOrigin+strlen(sArg)+strlen(sValue)+2)>=(unsigned)dMaxSize) {
      return -1;
    }
    /*
     * Append to the end
     */
    strcat(sPost,"&");
    strcat(sPost,sArg);
    strcat(sPost,"=");
    dOrigin=strlen(sPost);
    strcat(sPost,sValue);
  }
  else {
    /*
     * The argument exists, so update it
     */
    dValue=strlen(sValue);
    if(RDBufferDiff(sPost,dOrigin,dValue-strlen(sAccum),dMaxSize)<0) {
      return -1;
    }
    for(i=0;i<dValue;i++) {
      sPost[dOrigin+i]=sValue[i];
    }
    sPost[dOrigin+dValue]='&';
  }
  return dOrigin;
} 


/*
 * int RDFindPostString(char *cBuffer,char *sSearch,char *sReturn,
 * int dReturnSize)
 *
 * This function returns the argument value associated with field name 
 * pointed to by sSearch in the POST buffer cBuffer.  The argument value 
 * is returned in the buffer pointed to by sReturn, of maximum size 
 * dReturnSize.
 *
 * RETURNS:  Pointer to the start of the value, if successful
 *          -1 if the search is unsuccessful
 */

int RDFindPostString(const char *cBuffer,const char *sSearch,char *sReturn,int dReturnSize)

{
  int i=0,j=0;
  int dMatch,dOrigin;

  while(cBuffer[i]!=0) {
    j=0;
    dMatch=0;
    while(cBuffer[i]!='=' && cBuffer[i]!=0) {
      if(cBuffer[i++]!=sSearch[j++]) dMatch=1;
    }
    if(dMatch==0 && cBuffer[i]=='=' && sSearch[j]==0) {   /* Found it! */
      j=0;
      i++;
      dOrigin=i;
      while(cBuffer[i]!='&' && cBuffer[i]!=0 && j<dReturnSize-1) {
	sReturn[j++]=cBuffer[i++];
      }
      sReturn[j]=0;
      return dOrigin;
    }
    else {
      while(cBuffer[i]!='&' && cBuffer[i]!=0) i++;
    }
    if(cBuffer[i]==0) {
      sReturn[0]=0;
      return -1;   /* No match found! */
    }
    else {  /* advance to next field */
      i++;
    }
  }
  sReturn[0]=0;
  return -1;
}




/*
 * int GetPostString(char *cBuffer,char *sSearch,char *sReturn,
 * int dReturnSize)
 *
 * This function returns the argument value associated with field name 
 * pointed to by sSearch in the POST buffer cBuffer.  The argument value 
 * is returned in the buffer pointed to by sReturn, of maximum size 
 * dReturnSize.  The argument value is also processed to convert any
 * CGI escape sequences back into normal characters.
 *
 * RETURNS:  0 if successful
 *          -1 if the search is unsuccessful
 */

int RDGetPostString(const char *cBuffer,const char *sSearch,
		    char *sReturn,int dReturnSize)
{
  if(RDFindPostString(cBuffer,sSearch,sReturn,dReturnSize)<0) {
    return -1;
  }
  RDDecodeString(sReturn);
  return 0;
}



/*
 * int GetPostInt(char *cBuffer,char *sSearch,int *dReturn)
 *
 * This function returns the integer argument value associated with field name 
 * pointed to by sSearch in the POST buffer cBuffer.  The argument value 
 * is returned in the integer variable pointed to by dReturn.
 *
 * RETURNS:  0 if successful
 *          -1 if the search is unsuccessful
 */

int RDGetPostInt(const char *cBuffer,const char *sSearch,int *dReturn)
{
  char sAccum[256];

  if(RDGetPostString(cBuffer,sSearch,sAccum,255)<0) {
    return -1;
  }
  if(sscanf(sAccum,"%d",dReturn)!=1) {
    return -1;
  }
  return 0;
}




int RDGetPostLongInt(const char *cBuffer,const char *sSearch,long int *dReturn)
{
  char sAccum[256];

  if(RDGetPostString(cBuffer,sSearch,sAccum,255)<0) {
    return -1;
  }
  if(sscanf(sAccum,"%ld",dReturn)!=1) {
    return -1;
  }
  return 0;
}




/*
 * int PurgePostString(char *sPost,char *sArg)
 *
 * This function removes the argument/value pair pointed to by 'sArg'.
 *
 * RETURNS:  If successful, the new size of 'sPost'
 *           If unsuccessful, -1
 */ 
int RDPurgePostString(char *sPost,char *sArg,int dMaxSize)
{
  char sAccum[CGI_ACCUM_SIZE];
  int dPointer;

  dPointer=RDFindPostString(sPost,sArg,sAccum,CGI_ACCUM_SIZE);
  if(dPointer<0) {
    return -1;
  }
  dPointer-=(strlen(sArg)+1);
  RDBufferDiff(sPost,dPointer,-(strlen(sArg)+strlen(sAccum)+2),dMaxSize);
  return strlen(sPost);
}



/*
 * int RDEncodeString(char *sString,int dMaxSize)
 *
 * This function processes the string pointed to by 'sString', replacing
 * any spaces with + and escaping most punctuation characters in accordance
 * with the Common Gateway Interface (CGI) standard
 *
 * RETURNS: If successful, the new size of 'sString'
 *          If unsuccessful, -1
 */
int RDEncodeString(char *sString,int dMaxSize)
{
  int i;                  /* General Purpose Counter */
  char sAccum[4];          /* General String Buffer */

  i=0;
  while(sString[i]!=0) {
    if(((sString[i]!=' ') && (sString[i]!='*') && (sString[i]!='-') &&
	(sString[i]!='_') && (sString[i]!='.')) && 
       ((sString[i]<'0') ||
       ((sString[i]>'9') && (sString[i]<'A')) ||
       ((sString[i]>'Z') && (sString[i]<'a')) ||
       (sString[i]>'z'))) {
      if(RDBufferDiff(sString,i,2,dMaxSize)<0) {
	return -1;
      }
      sprintf(sAccum,"%%%2x",sString[i]);
      sString[i++]=sAccum[0];
      sString[i++]=sAccum[1];
      sString[i]=sAccum[2];
    }
    if(sString[i]==' ') {
      sString[i]='+';
    }
    i++;
  }
  return strlen(sString);
}


/*
 * int RDEncodeSQLString(char *sString,int dMaxSize)
 *
 * This function processes the string pointed to by 'sString', 
 * escaping the ' \ and " characters.
 *
 * RETURNS: If successful, the new size of 'sString'
 *          If unsuccessful, -1
 */
int RDEncodeSQLString(char *sString,int dMaxSize)
{
  int i;                  /* General Purpose Counter */
  char sAccum[4];          /* General String Buffer */

  i=0;
  while(sString[i]!=0) {
    if((sString[i]=='%')||(sString[i]==34)||(sString[i]==39)) {
      if(RDBufferDiff(sString,i,2,dMaxSize)<0) {
	return -1;
      }
      sprintf(sAccum,"%%%2x",sString[i]);
      sString[i++]=sAccum[0];
      sString[i++]=sAccum[1];
      sString[i]=sAccum[2];
    }
    i++;
  }
  return strlen(sString);
}




int RDDecodeString(char *sString)

{
  int i=0,j=0,k;
  char sAccum[4];

  while(sString[i]!=0) {
    switch(sString[i]) {
      
      case '+':
      sString[j]=' ';
      break;

      case '%':   /* escape sequence */
      sAccum[0]=sString[++i];
      sAccum[1]=sString[++i];
      sAccum[2]=0;
      sscanf(sAccum,"%x",&k);
      sString[j]=(char)k;
      break;

      default:
      sString[j]=sString[i];
      break;
    }
    i++;
    j++;
  }
  sString[j]=0;
  return --j;
}



/*
 * RDPutPlaintext(char *sPost,int dMaxSize)
 *
 * This function appends a block of text consisting of the *decoded* values
 * of all the POST values found in the buffer pointed to by 'sPost' into
 * the buffer pointed to by 'sPost'.  The block is enclosed by the HTML
 * start and end comment sequence (<! ... -->).  'sPost' is of maximum size
 * 'dMaxSize'.
 *
 * RETURNS: If successful, the new size of 'sPost'.
 *          If unsuccessful, -1.  
 */
int RDPutPlaintext(char *sPost,int dMaxSize)
{
  int dOriginalsize=0,dPostsize=0;   /* Current post buffer length */
  int i,j=0;                           /* General purpose counter */
  int iState=0;                      /* State Counter */
  char sAccum[CGI_ACCUM_SIZE];       /* General String Buffer */
  int dAccum;                        /* Length of sAccum */

  /*
   * Initialize some data structures
   */
  dOriginalsize=strlen(sPost);
  dPostsize=dOriginalsize;

  /*
   * Append the start of comment sequence
   */
  if((dPostsize+3)>=dMaxSize) {
    return -1;
  }
  strcat(sPost,"&< ");
  dPostsize+=3;

  /* 
   * Scan for value strings
   */
  for(i=0;i<dOriginalsize+1;i++) {
    switch(iState) {

    case 0:    /* Looking for a start of value or comment */
      switch(sPost[i]) {

      case '=':   /* Start of value */
	j=0;
	sAccum[j]=0;
	iState=1;
	break;

      case '<':   /* Start of comment sequence */
	iState=10;
	break;
      }
      break;

    case 1:
      switch(sPost[i]) {

      case '&':   /* End of value string */
	sAccum[j++]=' ';
	sAccum[j]=0;
	RDDecodeString(sAccum);
	dAccum=strlen(sAccum);
	if(dAccum>=dMaxSize) {
	  return -1;
	}
	strcat(sPost,sAccum);
	dPostsize+=dAccum;
	iState=0;
	break;

      default:    /* Another character in value string */
	if((sPost[i]!='<') && (sPost[i]!='>')) {
	  sAccum[j++]=sPost[i];
	}
	break;
      }
    case 10:    /* Middle of a comment */
      switch(sPost[i]) {
      case '>':   /* End of comment */
	iState=0;
	break;
      }
      break;
    default:    /* Parser error! */
      return -1;
      break;
    }
  }

  /*
   * Append the end of comment sequence
   */
  if((dPostsize+1)>=dMaxSize) {
    return -1;
  }
  strcat(sPost,">");
  dPostsize+=1;

  return dPostsize;
}




/*
 * int RDPurgePlaintext(char *sPost,int dMaxSize)
 *
 * This function removes one or more plaintext blocks enclosed by HTML comment
 * sequences (<! ... -->) from the buffer pointed to by 'sPost', of
 * maximum size 'dMaxSize'.  
 *
 * RETURNS: If successful, the new size of 'sPost'. 
 *          If unsuccessful, -1
 */
int RDPurgePlaintext(char *sPost,int dMaxSize)
{
  int i=0;                    /* General Purpose Counters */
  int dComments=0;            /* Comment State Switch */
  int dStart=0;               /* Comment Startpoint Pointer */

  /*
   * Scan for comment sequences
   */
  while(sPost[i]!=0) {
    if((sPost[i]=='<') && (dComments==0)) {   /* Start of comment */
      dStart=i;
      dComments=1;
    }
    if((sPost[i]=='>') && (dComments==1)) {   /* End of comment */
      if(RDBufferDiff(sPost,dStart,dStart-i-1,dMaxSize)<0) {
	return -1;
      }
      if(sPost[i]==0) {   /* Ensure a proper exit if at end of string */
	i--;
      }
    }
    i++;
  }

  /*
   * Clean up and exit nicely
   */
  RDPruneAmp(sPost);
  return strlen(sPost);
}




void RDCgiError(const char *str,int resp_code)
{
  /* The cgi header */
  printf("Content-type: text/html\n");
  printf("Status: %d\n",resp_code);
  printf("\n");

  /* The html header */
  printf("<html>\n");
  printf("<head>\n");
  printf("<title>");
  printf("CGI Internal Error %d",resp_code);
  printf("</title>\n");
  printf("</head>\n");

  /* The body of the message */
  printf("<h1>Oops!</h1><br>\n");
  printf("We seem to have encountered a problem!  The system says: <br>\n");
  printf("<pre>%d<br>%s</pre><br>\n",resp_code,str);

  /* The html footer */
  printf("</body>\n");
  exit(0);
}


#ifndef WIN32
extern void RDXMLResult(const char *str,int resp_code,
			RDAudioConvert::ErrorCode err)
{
  RDWebResult *we=new RDWebResult(str,resp_code,err);

  printf("Content-type: application/xml\n");
  printf("Status: %d\n",resp_code);
  printf("\n");
  printf("%s",(const char *)we->xml());
  delete we;

  exit(0);
}
#endif  // WIN32


/*
 * int BufferDiff(char sString,int dOrigin,int dDiff,int dMaxSize)
 *
 * This function adds (+ value) or deletes (- value) 'dDiff' characters 
 * from the string pointed to by 'sString' at the offset location pointed 
 * to by 'dOrigin'.  'dMaxSize' is the maximum allowable size of 'sString'.
 *
 * RETURNS:  If successful, the new size of 'sString'
 *           If unsuccessful, -1
 */
int RDBufferDiff(char *sString,int dOrigin,int dDiff,int dMaxSize)
{
  int dOldSize,dNewSize;
  int i;

  /*
   * Will it fit?
   */
  dOldSize=strlen(sString);
  if((dOldSize+dDiff)>=dMaxSize) {
    return -1;
  }
  dNewSize=dOldSize+dDiff;

  /* 
   * Adding characters 
   */
  if(dDiff>0) {
    for(i=dOldSize;i>dOrigin;i--) {
      sString[i+dDiff]=sString[i];
    }
    return dNewSize;
  }

  /* 
   * No Change
   */
  if(dDiff==0) {
    return dNewSize;
  }

  /*
   * Deleting Characters
   */
  if(dDiff<0) {
    for(i=dOrigin;i<dOldSize;i++) {
      sString[i]=sString[i-dDiff];
    }
    return dNewSize;
  }
  return -1; 
}




void RDPruneAmp(char *sPost)
{
  if(sPost[strlen(sPost)-1]=='&') {
    sPost[strlen(sPost)-1]=0;
  }
}


int RDEscapeQuotes(const char *src,char *dest,int maxlen)
{
  int i=0;
  int j=0;
  while(src[i]!=0) {
    if(src[i]==34) {  // Double Quotes
      if((j+7)>maxlen) {
	dest[j]=0;
	return j;
      }
      dest[j]=0;
      strcat(dest,"&quot;");
      i++;
      j+=6;
    }      
    else {
      if((j+2)>maxlen) {
	dest[j]=0;
	return j;
      }
      dest[j++]=src[i++];
    }
  }
  dest[j]=0;
  return j;
}


long int RDAuthenticateLogin(const QString &username,const QString &passwd,
			     const QHostAddress &addr)
{
  //
  // Authenticate User
  //
  RDUser *user=new RDUser(username);
  if(!user->exists()) {
    delete user;
    return -1;
  }
  if(!user->checkPassword(passwd,true)) {
    delete user;
    return -1;
  }
  delete user;

  //
  // Create Session
  //
  time_t timeval;
  timeval=time(&timeval);
  srandom(timeval);
  long int session=random();
  QString sql=QString().sprintf("insert into WEB_CONNECTIONS set \
                                 SESSION_ID=%ld,\
                                 LOGIN_NAME=\"%s\",\
                                 IP_ADDRESS=\"%s\",\
                                 TIME_STAMP=now()",
				session,
				(const char *)username,
				(const char *)addr.toString());
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;

  return session;
}


QString RDAuthenticateSession(long int session_id,const QHostAddress &addr)
{
  //
  // Expire Stale Sessions
  //
  QDateTime current_datetime=
    QDateTime(QDate::currentDate(),QTime::currentTime());
  QString sql=QString().sprintf("delete from WEB_CONNECTIONS \
                                 where TIME_STAMP<\"%s\"",
				(const char *)current_datetime.
				addSecs(-RD_WEB_SESSION_TIMEOUT).
				toString("yyyy-MM-dd hh:mm:ss"));
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;

  //
  // Check for Session
  //
  sql=QString().sprintf("select LOGIN_NAME,IP_ADDRESS from WEB_CONNECTIONS \
                         where SESSION_ID=%ld",
			session_id);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return QString();
  }
  if(q->value(1).toString()!=addr.toString()) {
    delete q;
    return QString();
  }
  QString name=q->value(0).toString();
  delete q;

  //
  // Update Session
  //
  sql=QString().sprintf("update WEB_CONNECTIONS set TIME_STAMP=\"%s\" \
                         where SESSION_ID=%ld",
			(const char *)current_datetime.
			toString("yyyy-MM-dd hh:mm:dd"),
			session_id);
  q=new RDSqlQuery(sql);
  delete q;

  return name;
}


void RDLogoutSession(long int session_id,const QHostAddress &addr)
{
  QString sql=QString().sprintf("select IP_ADDRESS from WEB_CONNECTIONS \
                         where SESSION_ID=%ld",
			session_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return;
  }
  if(q->value(0).toString()!=addr.toString()) {
    delete q;
    return;
  }
  delete q;
  sql=QString().sprintf("delete from WEB_CONNECTIONS where SESSION_ID=%ld",
			session_id);
  q=new RDSqlQuery(sql);
  delete q;
}


bool RDParsePost(std::map<QString,QString> *vars)
{
  std::map<QString,QString> headers;
  bool header=true;
  FILE *f=NULL;
  char *data=NULL;
  ssize_t n=0;
  QString sep;
  QString name;
  QString filename;
  QString tempdir;
  int fd=-1;

  //
  // Initialize Temp Directory Path
  //
  if(getenv("TMPDIR")!=NULL) {
    tempdir=getenv("TMPDIR");
  }
  else {
    tempdir="/tmp";
  }
  tempdir+="/rivendellXXXXXX";

  //
  // Get message part separator
  //
  if(getenv("REQUEST_METHOD")==NULL) {
    return false;
  }
  if(QString(getenv("REQUEST_METHOD")).lower()!="post") {
    return false;
  }
  if((f=fdopen(0,"r"))==NULL) {
    return false;
  }
  if((n=getline(&data,(size_t *)&n,f))<=0) {
    return false;
  }
  sep=QString(data).stripWhiteSpace();

  //
  // Get message parts
  //
  while((n=getline(&data,(size_t *)&n,f))>0) {
    if(QString(data).stripWhiteSpace().contains(sep)>0) {  // End of part
      if(fd>=0) {
	ftruncate(fd,lseek(fd,0,SEEK_CUR)-2);  // Remove extraneous final CR/LF
	::close(fd);
	fd=-1;
      }
      name="";
      filename="";
      headers.clear();
      header=true;
      continue;
    }
    if(header) {  // Read header
      if(QString(data).stripWhiteSpace().isEmpty()) {
	if(!headers["content-disposition"].isNull()) {
	  QStringList fields;
	  fields=fields.split(";",headers["content-disposition"]);
	  if(fields.size()>0) {
	    if(fields[0].lower().stripWhiteSpace()=="form-data") {
	      for(unsigned i=1;i<fields.size();i++) {
		QStringList pairs;
		pairs=pairs.split("=",fields[i]);
		if(pairs[0].lower().stripWhiteSpace()=="name") {
		  name=pairs[1].stripWhiteSpace();
		  name.replace("\"","");
		}
		if(pairs[0].lower().stripWhiteSpace()=="filename") {
		  if(tempdir.right(6)=="XXXXXX") {
		    char dir[PATH_MAX];
		    strcpy(dir,tempdir);
		    mkdtemp(dir);
		    tempdir=dir;
		  }
		  filename=tempdir+"/"+pairs[1].stripWhiteSpace();
		  filename.replace("\"","");
		  fd=open(filename,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
		}
	      }
	    }
	  }
	}
	header=false;
      }
      else {
	QStringList hdr;
	hdr=hdr.split(":",QString(data).stripWhiteSpace());
	headers[hdr[0].lower()]=hdr[1];
      }
    }
    else {  // Read data
      if(filename.isEmpty()) {
	(*vars)[name]+=QString(data);
      }
      else {
	(*vars)[name]=filename;
	write(fd,data,n);
      }
    }
  }

  free(data);

  return true;
}


QString RDXmlField(const QString &tag,const QString &value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  return QString("<")+tag+str+">"+RDXmlEscape(value)+"</"+tag+">\n";
}


QString RDXmlField(const QString &tag,const char *value,const QString &attrs)
{
  return RDXmlField(tag,QString(value),attrs);
}


QString RDXmlField(const QString &tag,const int value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  return QString("<")+tag+str+">"+QString().sprintf("%d",value)+"</"+tag+">\n";
}


QString RDXmlField(const QString &tag,const unsigned value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  return QString("<")+tag+str+">"+QString().sprintf("%u",value)+"</"+tag+">\n";
}


QString RDXmlField(const QString &tag,const bool value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  if(value) {
    return QString("<")+tag+str+">true</"+tag+">\n";
  }
  return QString("<")+tag+str+">false</"+tag+">\n";
}


QString RDXmlField(const QString &tag,const QDateTime &value,
		   const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  if(value.isValid()) {
    return QString("<")+tag+str+">"+RDWebDateTime(value)+"</"+tag+">\n";
  }
  return RDXmlField(tag);
}


QString RDXmlField(const QString &tag,const QTime &value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  if(value.isValid()&&(!value.isNull())) {
    return QString("<")+tag+str+">"+value.toString("hh:mm:ss")+"</"+tag+">\n";
  }
  return RDXmlField(tag);
}


QString RDXmlField(const QString &tag)
{
  return QString("<")+tag+"/>\n";
}


QString RDXmlEscape(const QString &str)
{
  /*
   * Escape a string in accordance with XML-1.0
   */
  QString ret=str;
  ret.replace("&","&amp;");
  ret.replace("<","&lt;");
  ret.replace(">","&gt;");
  ret.replace("'","&apos;");
  ret.replace("\"","&quot;");
  return ret;
}


QString RDXmlUnescape(const QString &str)
{
  /*
   * Unescape a string in accordance with XML-1.0
   */
  QString ret=str;
  ret.replace("&amp;","&");
  ret.replace("&lt;","<");
  ret.replace("&gt;",">");
  ret.replace("&apos;","'");
  ret.replace("&quot;","\"");
  return ret;
}


QString RDUrlEscape(const QString &str)
{
  /*
   * Escape a string in accordance with RFC 2396 Section 2.4
   */
  QString ret=str;

  ret.replace("%","%25");
  ret.replace(" ","%20");
  ret.replace("<","%3C");
  ret.replace(">","%3E");
  ret.replace("#","%23");
  ret.replace("\"","%22");
  ret.replace("{","%7B");
  ret.replace("}","%7D");
  ret.replace("|","%7C");
  ret.replace("\\","%5C");
  ret.replace("^","%5E");
  ret.replace("[","%5B");
  ret.replace("]","%5D");
  ret.replace("~","%7E");

  return ret;
}


QString RDUrlUnescape(const QString &str)
{
  /*
   * Unescape a string in accordance with RFC 2396 Section 2.4
   */
  QString ret="";

  for(unsigned i=0;i<str.length();i++) {
    if((str.at(i)=="%")&&(i<str.length()-2)) {
      ret+=QString().sprintf("%c",str.mid(i+1,2).toInt(NULL,16));
      i+=2;
    }
    else {
      ret+=str.at(i);
    }
  }

  return ret;
}


QString RDWebDateTime(const QDateTime &datetime)
{
  //
  // Generate an RFC 822/1123 compliant date/time string
  //
  if(!datetime.isValid()) {
    return QString();
  }
  return RDLocalToUtc(datetime).toString("ddd, dd MMM yyyy hh:mm:ss")+" GMT";
}


QDateTime RDGetWebDateTime(const QString &str)
{
  QDateTime ret;
  QStringList list;
  QStringList dlist;

  list=list.split(" ",str);
  switch(list.size()) {
  case 4:   // RFC 850 Style
    dlist=dlist.split("-",list[1]);
    if(dlist.size()==3) {
      ret=QDateTime(QDate(dlist[2].toInt()+2000,RDGetWebMonth(dlist[1]),
			  dlist[0].toInt()),
		    RDGetWebTime(list[2]+" "+list[3]));
    }
    break;

  case 5:   // ANSI C asctime() Style
    ret=QDateTime(QDate(list[4].toInt(),RDGetWebMonth(list[1]),list[2].toInt()),
		  RDGetWebTime(list[3]+" GMT"));
    break;

  case 6:   // RFC 822/1123 Style
    ret=QDateTime(QDate(list[3].toInt(),RDGetWebMonth(list[2]),list[1].toInt()),
		  RDGetWebTime(list[4]+" "+list[5]));
    break;
  }
  return ret;
}


QTime RDGetWebTime(const QString &str)
{
  QTime ret;
  QStringList list;

  list=list.split(" ",str);
  if(list.size()==2) {
    if(list[1].lower()=="gmt") {
      list=list.split(":",list[0]);
      if(list.size()==3) {
	ret=
	  RDUtcToLocal(QTime(list[0].toInt(),list[1].toInt(),list[2].toInt()));
      }
    }
  }

  return ret;
}


int RDGetWebMonth(const QString &str)
{
  int ret=0;

  if(str.lower()=="jan") {
    ret=1;
  }
  if(str.lower()=="feb") {
    ret=2;
  }
  if(str.lower()=="mar") {
    ret=3;
  }
  if(str.lower()=="apr") {
    ret=4;
  }
  if(str.lower()=="may") {
    ret=5;
  }
  if(str.lower()=="jun") {
    ret=6;
  }
  if(str.lower()=="jul") {
    ret=7;
  }
  if(str.lower()=="aug") {
    ret=8;
  }
  if(str.lower()=="sep") {
    ret=9;
  }
  if(str.lower()=="oct") {
    ret=10;
  }
  if(str.lower()=="nov") {
    ret=11;
  }
  if(str.lower()=="dec") {
    ret=12;
  }

  return ret;
}
