//   rdcddblookup.cpp
//
//   A Qt class for accessing the FreeDB CD Database.
//
//   (C) Copyright 2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdcddblookup.cpp,v 1.5.8.3 2014/01/14 17:35:31 cvs Exp $
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
//

#include <stdlib.h>
#include <string.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qdatetime.h>

#include <rdcddblookup.h>
#include <rdprofile.h>

RDCddbLookup::RDCddbLookup(FILE *profile_msgs,QObject *parent,const char *name) 
  : QObject(parent,name)
{
  lookup_state=0;
  lookup_profile_msgs=profile_msgs;

  //
  // Get the Hostname
  //
  if(getenv("HOSTNAME")==NULL) {
    lookup_hostname=RDCDDBLOOKUP_DEFAULT_HOSTNAME;
  }
  else {
    lookup_hostname=getenv("HOSTNAME");
  }

  //
  // Socket
  //
  lookup_socket=new QSocket(this,"lookup_socket");
  connect(lookup_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(lookup_socket,SIGNAL(error(int)),this,SLOT(errorData(int)));
}


RDCddbLookup::~RDCddbLookup()
{
  delete lookup_socket;
}


void RDCddbLookup::setCddbRecord(RDCddbRecord *rec)
{
  lookup_record=rec;
}


void RDCddbLookup::lookupRecord(const QString &cdda_dir,const QString &cdda_dev,
			       const QString &hostname,Q_UINT16 port,
			       const QString &username,const QString &appname,
				const QString &appver)
{
  lookup_username=username;
  lookup_appname=appname;
  lookup_appver=appver;

  Profile("starting CD-TEXT lookup");
  if(!cdda_dir.isEmpty()) {
    if(ReadCdText(cdda_dir,cdda_dev)) {
      emit done(RDCddbLookup::ExactMatch);
      Profile("CD-TEXT lookup success");
      return;
    }
  }
  Profile("CD-TEXT lookup failure");

  Profile("starting CDDB lookup");
  if(!hostname.isEmpty()) {
    //
    // Set Up Default User
    //
    if(lookup_username.isEmpty()) {
      if(getenv("USER")==NULL) {
	lookup_username=RDCDDBLOOKUP_DEFAULT_USER;
      }
      else {
	lookup_username=getenv("USER");
      }
    }
    
    //
    // Get the Hostname
    //
    if(getenv("HOSTNAME")==NULL) {
      lookup_hostname=RDCDDBLOOKUP_DEFAULT_HOSTNAME;
    }
    else {
      lookup_hostname=getenv("HOSTNAME");
    }
    lookup_socket->connectToHost(hostname,port);
  }
}


bool RDCddbLookup::readIsrc(const QString &cdda_dir,const QString &cdda_dev)
{
  return ReadIsrcs(cdda_dir,cdda_dev);
}


void RDCddbLookup::readyReadData()
{
  QString line;
  QString tag;
  QString value;
  int index;
  int code;
  char buffer[2048];
  char offset[256];
  int hex;
  int start;

  while(lookup_socket->canReadLine()) {
    line=lookup_socket->readLine();
    Profile("recevied from server: \""+line+"\"");
    sscanf((const char *)line,"%d",&code);
    switch(lookup_state) {
    case 0:    // Login Banner
      if((code==200)||(code==201)) {
	sprintf(buffer,"cddb hello %s %s %s %s",
		(const char *)lookup_username,
		(const char *)lookup_hostname,
		(const char *)lookup_appname,
		(const char *)lookup_appver);
	SendToServer(buffer);
	lookup_state=1;
      }
      else {
	FinishCddbLookup(RDCddbLookup::ProtocolError);
      }
      break;

    case 1:    // Handshake Response
      if((code==200)||(code==402)) {
	sprintf(buffer,"cddb query %08x %d",
		lookup_record->discId(),lookup_record->tracks());
	for(int i=0;i<lookup_record->tracks();i++) {
	  sprintf(offset," %d",lookup_record->trackOffset(i));
	  strcat(buffer,offset);
	}
	sprintf(offset," %d",lookup_record->discLength()/75);
	strcat(buffer,offset);
	SendToServer(buffer);
	lookup_state=2;
      }
      else {
	FinishCddbLookup(RDCddbLookup::ProtocolError);
      }
      break;

    case 2:    // Query Response
      switch(code) {
      case 200:   // Exact Match
	start=4;
	if(sscanf((const char *)line+start,"%s",offset)==1) {
	  lookup_record->setDiscGenre(offset);
	  start+=lookup_record->discGenre().length()+1;
	}
	if(sscanf((const char *)line+start,"%x",&hex)==1) {
	  lookup_record->setDiscId(hex);
	  start+=9;
	}
	lookup_record->setDiscTitle((const char *)line+start);
	sprintf(buffer,"cddb read %s %08x\n",
		(const char *)lookup_record->discGenre(),
		lookup_record->discId());
	SendToServer(buffer);
	lookup_state=3;		
	break;

      case 211:   // Inexact Match
	FinishCddbLookup(RDCddbLookup::PartialMatch);
	break;

      default:
	FinishCddbLookup(RDCddbLookup::ProtocolError);
	break;
      }
      break;

    case 3:    // Read Response
      if((code==210)) {
	lookup_state=4;
      }
      else {
	FinishCddbLookup(RDCddbLookup::ProtocolError);
      }
      break;
	  
    case 4:    // Record Lines
      if(line[0]!='#') {   // Ignore Comments
	if(line[0]=='.') {  // Done
	  FinishCddbLookup(RDCddbLookup::ExactMatch);
	}
	ParsePair(&line,&tag,&value,&index);
	if(tag=="DTITLE") {
	  lookup_record->setDiscTitle(value.left(value.length()-1));
	}
	if(tag=="DYEAR") {
	  lookup_record->setDiscYear(value.toUInt());
	}
	if(tag=="EXTD") {
	  lookup_record->
	    setDiscExtended(lookup_record->discExtended()+
			    DecodeString(value));
	}
	if(tag=="PLAYORDER") {
	  lookup_record->setDiscPlayOrder(value);
	}
	if((tag=="TTITLE")&&(index!=-1)) {
	  lookup_record->setTrackTitle(index,value.left(value.length()-1));
	}
	if((tag=="EXTT")&&(index!=-1)) {
	  lookup_record->
	    setTrackExtended(index,
			     lookup_record->trackExtended(index)+value);
	}
      }
      break;
    }
  }
}


void RDCddbLookup::errorData(int err)
{
  switch(err) {
      case QSocket::ErrConnectionRefused:
	printf("CDDB: Connection Refused!\n");
	break;
      case QSocket::ErrHostNotFound:
	printf("CDDB: Host Not Found!\n");
	break;
      case QSocket::ErrSocketRead:
	printf("CDDB: Socket Read Error!\n");
	break;
  }
  lookup_state=0;
  emit done(RDCddbLookup::NetworkError);
}


void RDCddbLookup::FinishCddbLookup(RDCddbLookup::Result res)
{
  SendToServer("quit");
  lookup_socket->close();
  lookup_state=0;
  emit done(res);
  Profile("CDDB lookup finished");
}


QString RDCddbLookup::DecodeString(QString &str)
{
  QString outstr;
  QChar ch;

  for(unsigned i=0;i<str.length();i++) {
    if((ch=str.at(i).latin1())=='\\') {
      outstr+=QString("\n");
      i++;
    }
    else {
      outstr+=QString(ch);
    }
  }
  return outstr;
}


void RDCddbLookup::ParsePair(QString *line,QString *tag,QString *value,
			    int *index)
{
  for(unsigned i=0;i<line->length();i++) {
    if(line->at(i)=='=') {
      *tag=line->left(i);
      *value=line->right(line->length()-i-1);
      *value=value->left(value->length()-1);   // Lose the silly linefeed
      *index=GetIndex(tag);
      return;
    }
  }
}


int RDCddbLookup::GetIndex(QString *tag)
{
  int index;

  for(unsigned i=0;i<tag->length();i++) {
    if(tag->at(i).isDigit()) {
      index=tag->right(tag->length()-i).toInt();
      *tag=tag->left(i);
      return index;
    }
  }
  return -1;
}


bool RDCddbLookup::ReadCdText(const QString &cdda_dir,const QString &cdda_dev)
{
  int err=0;
  RDProfile *title_profile=new RDProfile();
  bool ret=false;
  QString str;
  QString cmd;

  //
  // Write the Track Title Data to a Temp File
  //
  cmd=QString().sprintf("CURDIR=`pwd`;cd %s;cdda2wav -D %s --info-only -v titles 2> /dev/null;cd $CURDIR",
				(const char *)cdda_dir,
				(const char *)cdda_dev);
  if((err=system(cmd))!=0) {
    return false;
  }

  //
  // Read the Track Title Data File
  //
  for(int i=0;i<lookup_record->tracks();i++) {
    title_profile->setSource(QString().sprintf("%s/audio_%02d.inf",
					 (const char *)cdda_dir,i+1));
    str=title_profile->stringValue("","Albumtitle","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setDiscTitle(str);
      ret=true;
    }

    str=title_profile->stringValue("","Albumperformer","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setDiscArtist(str);
      ret=true;
    }

    str=title_profile->stringValue("","Tracktitle","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setTrackTitle(i,str);
      ret=true;
    }

    str=title_profile->stringValue("","Performer","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setTrackArtist(i,str);
      ret=true;
    }
  }
  return ret;
}


bool RDCddbLookup::ReadIsrcs(const QString &cdda_dir,const QString &cdda_dev)
{
  int err=0;
  RDProfile *title_profile=new RDProfile();
  RDProfile *isrc_profile=new RDProfile();
  bool ret=false;
  QString str;
  QString cmd;

  //
  // Write the ISRC Data to a Temp File
  //
  cmd=QString().sprintf("CURDIR=`pwd`;cd %s;cdda2wav -D %s --info-only -v trackid 2> /dev/null;cd $CURDIR",
				(const char *)cdda_dir,
				(const char *)cdda_dev);
  if((err=system(cmd))!=0) {
    return false;
  }

  //
  // Read the ISRC Data File
  //
  for(int i=0;i<lookup_record->tracks();i++) {
    isrc_profile->setSource(QString().sprintf("%s/audio_%02d.inf",
					      (const char *)cdda_dir,i+1));
    str=isrc_profile->stringValue("","ISRC","");
    str.remove("'");
    str.remove("-");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setIsrc(i,str);
      ret=true;
    }
  }
  delete title_profile;
  delete isrc_profile;

  return ret;
}


void RDCddbLookup::SendToServer(const QString &msg)
{
  lookup_socket->writeBlock(msg+"\n",msg.length()+1);
  Profile("sent to server: \""+msg+"\"");
}


void RDCddbLookup::Profile(const QString &msg)
{
  if(lookup_profile_msgs!=NULL) {
    printf("%s | RDCddbLookup::%s\n",
	    (const char *)QTime::currentTime().toString("hh:mm:ss.zzz"),
	    (const char *)msg);
  }
}
