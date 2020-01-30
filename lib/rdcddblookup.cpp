//   rdcddblookup.cpp
//
//   RDDiscLookup instance class for accessing the FreeDB CD Database.
//
//   (C) Copyright 2003-2020 Fred Gleason <fredg@paravelsystems.com>
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
#include <q3process.h>

#include "rdapplication.h"
#include "rdcddblookup.h"
#include "rdprofile.h"

RDCddbLookup::RDCddbLookup(const QString &caption,FILE *profile_msgs,
			   QWidget *parent)
  : RDDiscLookup(caption,profile_msgs,parent)
{
  lookup_state=0;

  setWindowTitle(caption+" - "+tr("CDDB Query"));

  //
  // Socket
  //
  lookup_socket=new QTcpSocket(this);
  connect(lookup_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(lookup_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
}


RDCddbLookup::~RDCddbLookup()
{
  delete lookup_socket;
}


QString RDCddbLookup::sourceName() const
{
  return QString("FreeDB");
}


void RDCddbLookup::lookupRecord()
{
  lookup_username=rda->user()->name();
  lookup_hostname=rda->libraryConf()->cddbServer();
  lookup_appname="rivendell";
  lookup_appver=VERSION;

  profile("starting CDDB lookup");
  lookup_socket->connectToHost(lookup_hostname,RDCDDBLOOKUP_DEFAULT_PORT);
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
  QStringList f0;
  bool ok=false;

  while(lookup_socket->canReadLine()) {
    line=QString::fromUtf8(lookup_socket->readLine());
    profile("recevied from server: \""+line+"\"");
    code=line.split(" ").at(0).toInt();
    switch(lookup_state) {
    case 0:    // Login Banner
      if((code==200)||(code==201)) {
	snprintf(buffer,2048,"cddb hello %s %s %s %s",
		 (const char *)lookup_username.utf8(),
		 (const char *)lookup_hostname.utf8(),
		 (const char *)lookup_appname.utf8(),
		 (const char *)lookup_appver.utf8());
	SendToServer(buffer);
	lookup_state=1;
      }
      else {
	FinishCddbLookup(RDCddbLookup::ProtocolError);
      }
      break;

    case 1:    // Handshake Response
      if((code==200)||(code==402)) {
	SendToServer("proto 6");
	lookup_state=2;
      }
      else {
	FinishCddbLookup(RDCddbLookup::ProtocolError);
      }
      break;

    case 2:    // Protocol Level Response
      if(code==201) {
	snprintf(buffer,2048,"cddb query %08x %d",
		cddbRecord()->discId(),cddbRecord()->tracks());
	for(int i=0;i<cddbRecord()->tracks();i++) {
	  snprintf(offset,256," %d",cddbRecord()->trackOffset(i));
	  strcat(buffer,offset);
	}
	snprintf(offset,256," %d",cddbRecord()->discLength()/75);
	strcat(buffer,offset);
	SendToServer(buffer);
	lookup_state=3;
      }
      else {
	FinishCddbLookup(RDCddbLookup::ProtocolError);
      }
      break;

    case 3:    // Query Response
      switch(code) {
      case 200:   // Exact Match
	f0=line.split(" ");
	if(f0.size()>=4) {
	  cddbRecord()->setDiscId(f0[2].toUInt(&ok,16));
	  if(!ok) {
	    FinishCddbLookup(RDCddbLookup::ProtocolError);
	  }
	  cddbRecord()->setDiscGenre(f0[1]);
	  f0.erase(f0.begin());
	  f0.erase(f0.begin());
	  f0.erase(f0.begin());
	  cddbRecord()->setDiscTitle(f0.join(" "));
	  snprintf(buffer,2048,"cddb read %s %08x\n",
		   (const char *)cddbRecord()->discGenre().utf8(),
		   cddbRecord()->discId());
	  SendToServer(buffer);
	  lookup_state=5;		
	}
	else {
	  FinishCddbLookup(RDCddbLookup::ProtocolError);
	}
	break;

      case 210:   // Multiple Exact Matches
	titlesBox()->clear();
	titlesKey()->clear();
	lookup_state=4;
	break;

      case 211:   // Inexact Match
	FinishCddbLookup(RDCddbLookup::PartialMatch);
	break;

      default:
	FinishCddbLookup(RDCddbLookup::ProtocolError);
	break;
      }
      break;

    case 4:    // Process Multiple Matches
      if(line.trimmed()==".") {
	profile("Match list complete, showing chooser dialog...");
	if(exec()) {
	  f0=titlesKey()->at(titlesBox()->currentItem()).
	    split(" ",QString::SkipEmptyParts);
	  if(f0.size()!=2) {
	    FinishCddbLookup(RDCddbLookup::ProtocolError);
	  }
	  cddbRecord()->setDiscId(f0.at(1).toUInt(&ok,16));
	  if(!ok) {
	    FinishCddbLookup(RDCddbLookup::ProtocolError);
	  }
	  cddbRecord()->setDiscGenre(f0.at(0));
	  f0=titlesBox()->currentText().split("/");
	  if(f0.size()==2) {
	    cddbRecord()->setDiscTitle(f0.at(1).trimmed());
	  }
	  else {
	    cddbRecord()->setDiscTitle(titlesBox()->currentText().trimmed());
	  }
	  snprintf(buffer,2048,"cddb read %s %08x\n",
		   (const char *)cddbRecord()->discGenre().utf8(),
		   cddbRecord()->discId());
	  SendToServer(buffer);
	  lookup_state=5;	
	}
	else {
	  FinishCddbLookup(RDCddbLookup::NoMatch);
	}
      }
      else {
	f0.clear();
	f0=line.split(" ");
	titlesKey()->push_back(f0.at(0).trimmed()+" "+
				    f0.at(1).trimmed());
	f0.removeFirst();
	f0.removeFirst();
	titlesBox()->insertItem(titlesBox()->count(),f0.join(" ").trimmed());
      }
      break;

    case 5:    // Read Response
      if((code==210)) {
	lookup_state=6;
      }
      else {
	FinishCddbLookup(RDCddbLookup::ProtocolError);
      }
      break;
	  
    case 6:    // Record Lines
      if(line[0]!='#') {   // Ignore Comments
	if(line[0]=='.') {  // Done
	  FinishCddbLookup(RDCddbLookup::ExactMatch);
	}
	ParsePair(&line,&tag,&value,&index);
	if(tag=="DTITLE") {
	  cddbRecord()->setDiscTitle(value.left(value.length()-1));
	}
	if(tag=="DYEAR") {
	  cddbRecord()->setDiscYear(value.toUInt());
	}
	if(tag=="EXTD") {
	  cddbRecord()->
	    setDiscExtended(cddbRecord()->discExtended()+
			    DecodeString(value));
	}
	if(tag=="PLAYORDER") {
	  cddbRecord()->setDiscPlayOrder(value);
	}
	if((tag=="TTITLE")&&(index!=-1)) {
	  cddbRecord()->setTrackTitle(index,value.left(value.length()-1));
	}
	if((tag=="EXTT")&&(index!=-1)) {
	  cddbRecord()->
	    setTrackExtended(index,
			     cddbRecord()->trackExtended(index)+value);
	}
      }
      break;
    }
  }
}


void RDCddbLookup::errorData(QAbstractSocket::SocketError err)
{
  switch(err) {
      case QTcpSocket::ErrConnectionRefused:
	printf("CDDB: Connection Refused!\n");
	break;
      case QTcpSocket::ErrHostNotFound:
	printf("CDDB: Host Not Found!\n");
	break;
      case QTcpSocket::ErrSocketRead:
	printf("CDDB: Socket Read Error!\n");
	break;
      default:
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
  emit lookupDone(res);
  profile("CDDB lookup finished");
}


QString RDCddbLookup::DecodeString(QString &str)
{
  QString outstr;
  QChar ch;

  for(int i=0;i<str.length();i++) {
    if((ch=str.at(i))=='\\') {
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
  for(int i=0;i<line->length();i++) {
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

  for(int i=0;i<tag->length();i++) {
    if(tag->at(i).isDigit()) {
      index=tag->right(tag->length()-i).toInt();
      *tag=tag->left(i);
      return index;
    }
  }
  return -1;
}


void RDCddbLookup::SendToServer(const QString &msg)
{
  lookup_socket->writeBlock(msg+"\n",msg.length()+1);
  profile("sent to server: \""+msg+"\"");
}
