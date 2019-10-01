//   rdcddblookup.cpp
//
//   A Qt class for accessing the FreeDB CD Database.
//
//   (C) Copyright 2003-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdcddblookup.h>
#include <rdprofile.h>

RDCddbLookup::RDCddbLookup(const QString &caption,FILE *profile_msgs,
			   QWidget *parent)
  : RDDialog(parent)
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

  setWindowTitle(caption+" - "+tr("CDDB Query"));

  lookup_titles_label=new QLabel(tr("Multiple Matches Found!"),this);
  lookup_titles_label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
  lookup_titles_label->setFont(labelFont());

  lookup_titles_box=new QComboBox(this);

  lookup_ok_button=new QPushButton(tr("OK"),this);
  lookup_ok_button->setFont(buttonFont());
  connect(lookup_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  lookup_cancel_button=new QPushButton(tr("Cancel"),this);
  lookup_cancel_button->setFont(buttonFont());
  connect(lookup_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

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


QSize RDCddbLookup::sizeHint() const
{
  return QSize(400,120);
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
  if(lookup_record->tracks()==0) {
    return;
  }

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
  QStringList f0;
  bool ok=false;

  while(lookup_socket->canReadLine()) {
    line=QString::fromUtf8(lookup_socket->readLine());
    Profile("recevied from server: \""+line+"\"");
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
		lookup_record->discId(),lookup_record->tracks());
	for(int i=0;i<lookup_record->tracks();i++) {
	  snprintf(offset,256," %d",lookup_record->trackOffset(i));
	  strcat(buffer,offset);
	}
	snprintf(offset,256," %d",lookup_record->discLength()/75);
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
	  lookup_record->setDiscId(f0[2].toUInt(&ok,16));
	  if(!ok) {
	    FinishCddbLookup(RDCddbLookup::ProtocolError);
	  }
	  lookup_record->setDiscGenre(f0[1]);
	  f0.erase(f0.begin());
	  f0.erase(f0.begin());
	  f0.erase(f0.begin());
	  lookup_record->setDiscTitle(f0.join(" "));
	  snprintf(buffer,2048,"cddb read %s %08x\n",
		   (const char *)lookup_record->discGenre().utf8(),
		   lookup_record->discId());
	  SendToServer(buffer);
	  lookup_state=5;		
	}
	else {
	  FinishCddbLookup(RDCddbLookup::ProtocolError);
	}
	break;

      case 210:   // Multiple Exact Matches
	lookup_titles_box->clear();
	lookup_titles_key.clear();
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
	Profile("Match list complete, showing chooser dialog...");
	if(exec()) {
	  f0=lookup_titles_key.at(lookup_titles_box->currentItem()).
	    split(" ",QString::SkipEmptyParts);
	  if(f0.size()!=2) {
	    FinishCddbLookup(RDCddbLookup::ProtocolError);
	  }
	  lookup_record->setDiscId(f0.at(1).toUInt(&ok,16));
	  if(!ok) {
	    FinishCddbLookup(RDCddbLookup::ProtocolError);
	  }
	  lookup_record->setDiscGenre(f0.at(0));
	  f0=lookup_titles_box->currentText().split("/");
	  if(f0.size()==2) {
	    lookup_record->setDiscTitle(f0.at(1).trimmed());
	  }
	  else {
	    lookup_record->setDiscTitle(lookup_titles_box->currentText().trimmed());
	  }
	  snprintf(buffer,2048,"cddb read %s %08x\n",
		   (const char *)lookup_record->discGenre().utf8(),
		   lookup_record->discId());
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
	lookup_titles_key.push_back(f0.at(0).trimmed()+" "+
				    f0.at(1).trimmed());
	f0.removeFirst();
	f0.removeFirst();
	lookup_titles_box->insertItem(lookup_titles_box->count(),f0.join(" ").trimmed());
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


void RDCddbLookup::okData()
{
  done(true);
}


void RDCddbLookup::cancelData()
{
  done(false);
}


void RDCddbLookup::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  lookup_titles_label->setGeometry(15,2,w-30,20);

  lookup_titles_box->setGeometry(10,24,w-20,20);

  lookup_ok_button->setGeometry(w-180,h-60,80,50);
  lookup_cancel_button->setGeometry(w-90,h-60,80,50);
}


void RDCddbLookup::FinishCddbLookup(RDCddbLookup::Result res)
{
  SendToServer("quit");
  lookup_socket->close();
  lookup_state=0;
  emit lookupDone(res);
  Profile("CDDB lookup finished");
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


bool RDCddbLookup::ReadCdText(const QString &cdda_dir,const QString &cdda_dev)
{
  RDProfile *title_profile=new RDProfile();
  bool ret=false;
  QString str;
  QString cmd;

  //
  // Write the Track Title Data to a Temp File
  //
  QByteArray output;
  Q3Process *proc=new Q3Process(this);
  proc->addArgument("cdda2wav");
  proc->addArgument("-D");
  proc->addArgument(cdda_dev);
  proc->addArgument("--info-only");
  proc->addArgument("-v");
  proc->addArgument("titles");
  proc->setWorkingDirectory(cdda_dir);
  if(!proc->start()) {
    delete proc;
    return false;
  }
  while(proc->isRunning()) {
    output=proc->readStderr();
    if(output.size()>0) {  // Work around icedax(1)'s idiotic user prompt
      if(strncmp(output,"load cdrom please and press enter",33)==0) {
	proc->kill();
	delete proc;
	return false;
      }
    }
  }
  if((!proc->normalExit())||(proc->exitStatus()!=0)) {
    delete proc;
    return false;
  }
  delete proc;

  //
  // Read the Track Title Data File
  //
  for(int i=0;i<lookup_record->tracks();i++) {
    title_profile->setSource(cdda_dir+QString().sprintf("/audio_%02d.inf",i+1));
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
  cmd=QString("CURDIR=`pwd`;cd ")+
    cdda_dir+";cdda2wav -D "+cdda_dev+
    " --info-only -v trackid 2> /dev/null;cd $CURDIR";
  if((err=system(cmd))!=0) {
    return false;
  }

  //
  // Read the ISRC Data File
  //
  for(int i=0;i<lookup_record->tracks();i++) {
    isrc_profile->setSource(cdda_dir+QString().sprintf("/audio_%02d.inf",i+1));
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
	   (const char *)msg.toUtf8());
  }
}
