// rdsendmail.cpp
//
// Send an e-mail message using sendmail(1)
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <QObject>

#include "rdsendmail.h"
#include "rduser.h"

#include <QProcess>

bool __RDSendMail_IsAscii(const QString &str)
{
  for(int i=0;i<str.length();i++) {
    QChar ch=str.at(i);
    if((ch.cell()>127)||(ch.row()>0)) {
      return false;
    }
  }

  return true;
}


QByteArray __RDSendMail_EncodeBody(QString *charset,QString *encoding,
				   const QString &str)
{
  if(__RDSendMail_IsAscii(str)) {
    *charset="";
    *encoding="";
    return str.toAscii();
  }
  *charset=";charset=utf8";
  *encoding="Content-Transfer-Encoding: base64\r\n";
  QByteArray ret;
  QByteArray raw=str.toUtf8();
  for(int i=0;i<raw.length();i+=48) {
    ret+=raw.mid(i,48).toBase64()+"\r\n";
  }
  return ret;
}


QByteArray __RDSendMail_EncodeHeader(const QString &str)
{
  if(__RDSendMail_IsAscii(str)) {
    return str.toAscii();
  }
  return QByteArray("=?utf-8?B?")+str.toUtf8().toBase64()+"?=";
}

//
// This implements a basic email sending capability using the system's
// sendmail(1) interface.
//
bool RDSendMail(QString *err_msg,const QString &subject,const QString &body,
		const QString &from_addr,const QStringList &to_addrs,
		const QStringList &cc_addrs,const QStringList &bcc_addrs)
{
  QStringList args;
  QProcess *proc=NULL;
  QString msg="";

  *err_msg="";

  //
  // Validate Addresses
  //
  if(from_addr.isEmpty()) {
    *err_msg+=QObject::tr("You must supply a \"from\" address")+"\n";
  }
  else {
    if(!RDUser::emailIsValid(from_addr)) {
      *err_msg+=QObject::tr("address")+" \""+from_addr+"\" "+
	QObject::tr("is invalid")+"\n";
    }
  }
  for(int i=0;i<to_addrs.size();i++) {
    if(!RDUser::emailIsValid(to_addrs.at(i))) {
      *err_msg+=QObject::tr("address")+" \""+to_addrs.at(i)+"\" "+
	QObject::tr("is invalid")+"\n";
    }
  }
  for(int i=0;i<cc_addrs.size();i++) {
    if(!RDUser::emailIsValid(cc_addrs.at(i))) {
      *err_msg+=QObject::tr("address")+" \""+cc_addrs.at(i)+"\" "+
	QObject::tr("is invalid")+"\n";
    }
  }
  for(int i=0;i<bcc_addrs.size();i++) {
    if(!RDUser::emailIsValid(bcc_addrs.at(i))) {
      *err_msg+=QObject::tr("address")+" \""+bcc_addrs.at(i)+"\" "+
	QObject::tr("is invalid")+"\n";
    }
  }
  if(!err_msg->isEmpty()) {
    return false;
  }

  //
  // Compose Message
  //
  QString charset;
  QString encoding;
  QByteArray raw=__RDSendMail_EncodeBody(&charset,&encoding,body);

  msg+="From: "+from_addr+"\r\n";

  //  msg+="Content-Type: text/plain;charset=utf-8\r\n";
  //  msg+="Content-Transfer-Encoding: base64\r\n";
  msg+="Content-Type: text/plain"+charset+"\r\n";
  msg+=encoding;

  if(to_addrs.size()>0) {
    msg+="To: ";
    for(int i=0;i<to_addrs.size();i++) {
      msg+=to_addrs.at(i)+", ";
    }
    msg=msg.left(msg.length()-2);
    msg+="\r\n";
  }
  if(cc_addrs.size()>0) {
    msg+="Cc: ";
    for(int i=0;i<cc_addrs.size();i++) {
      msg+=cc_addrs.at(i)+", ";
    }
    msg=msg.left(msg.length()-2);
    msg+="\r\n";
  }
  if(bcc_addrs.size()>0) {
    msg+="Bcc: ";
    for(int i=0;i<bcc_addrs.size();i++) {
      msg+=bcc_addrs.at(i)+", ";
    }
    msg=msg.left(msg.length()-2);
    msg+="\r\n";
  }
  msg+="Subject: "+__RDSendMail_EncodeHeader(subject)+"\r\n";

  msg+="\r\n";
  msg+=raw;

  //
  // Send message
  //
  args.clear();
  args.push_back("-bm");
  args.push_back("-t");
  proc=new QProcess();
  proc->start("sendmail",args);
  if(!proc->waitForStarted()) {
    *err_msg=QObject::tr("unable to start sendmail")+"\n";
    delete proc;
    return false;
  }
  proc->write(msg.toUtf8());
  proc->closeWriteChannel();
  proc->waitForFinished();
  if(proc->exitStatus()!=QProcess::NormalExit) {
    *err_msg=QObject::tr("sendmail crashed")+"\r\n";
    delete proc;
    return false;
  }
  if(proc->exitCode()!=0) {
    *err_msg=QObject::tr("sendmail returned non-zero exit code")+
      QString().sprintf(": %d [",proc->exitCode())+
      QString::fromUtf8(proc->readAllStandardError())+"]\n";
    delete proc;
    return false;
  }
  delete proc;

  *err_msg=QObject::tr("ok");

  return true;
}


bool RDSendMail(QString *err_msg,const QString &subject,const QString &body,
		const QString &from_addr,const QString &to_addrs,
		const QString &cc_addrs,const QString &bcc_addrs)
{
  return RDSendMail(err_msg,subject,body,from_addr,
		    to_addrs.split(",",QString::SkipEmptyParts),
		    cc_addrs.split(",",QString::SkipEmptyParts),
		    bcc_addrs.split(",",QString::SkipEmptyParts));
}
