// journal.cpp
//
// E-mail file importation actions
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

#include <rdapplication.h>
#include <rdgroup.h>
#include <rdsendmail.h>

#include "journal.h"

Journal::Journal(bool send_immediately)
{
  c_send_immediately=send_immediately;
}


void Journal::addSuccess(const QString &groupname,QString filename,
			 unsigned cartnum,const QString &title)
{
  QString errors;
  QString subject;
  QString body;
  RDGroup *group=new RDGroup(groupname);
  QStringList addrs=
    group->notifyEmailAddress().split(",",QString::SkipEmptyParts);

  if(addrs.size()>0) {
    filename=filename.split("/",QString::SkipEmptyParts).last();
    if(c_send_immediately) {
      subject=QObject::tr("Rivendell import for file")+": "+filename;
      body+=QObject::tr("Rivendell File Import Report")+"\n";
      body+="\n";
      body+=QObject::tr("Import Success")+"\n";
      body+="\n";
      body+=QObject::tr("Filename")+": "+filename+"\n";
      body+=QObject::tr("Submitted by")+": ";
      if(rda->user()->emailAddress().isEmpty()) {
	body+=rda->user()->name()+"\n";
      }
      else {
	body+=rda->user()->emailAddress()+"\n";
      }
      body+=QObject::tr("Group")+": "+groupname+"\n";
      body+=QObject::tr("Cart Number")+QString().sprintf(": %06u",cartnum)+"\n";
      body+=QObject::tr("Cart Title")+": "+title+"\n";

      if(!RDSendMail(&errors,subject,body,rda->system()->originEmailAddress(),
		     addrs)) {
	rda->syslog(LOG_WARNING,"email send failed [%s]",
		    errors.toUtf8().constData());
      }
    }
    else {
      c_good_groups.push_back(groupname);
      c_good_filenames.push_back(filename);
      c_good_cart_numbers.push_back(cartnum);
      c_good_titles.push_back(title);
    }
  }
  delete group;
}


void Journal::addFailure(const QString &groupname,QString filename,
			 const QString &err_msg)
{
  QString errors;
  QString subject;
  QString body;
  RDGroup *group=new RDGroup(groupname);
  QStringList addrs=
    group->notifyEmailAddress().split(",",QString::SkipEmptyParts);

  if(addrs.size()>0) {
    filename=filename.split("/",QString::SkipEmptyParts).last();
    if(c_send_immediately) {
      subject=QObject::tr("Rivendell import FAILURE for file")+": "+filename;
      body+=QObject::tr("Rivendell File Import Report")+"\n";
      body+="\n";
      body+=QObject::tr("IMPORT FAILED!")+"\n";
      body+="\n";
      body+=QObject::tr("Filename")+": "+filename+"\n";
      body+=QObject::tr("Submitted by")+": ";
      if(rda->user()->emailAddress().isEmpty()) {
	body+=rda->user()->name()+"\n";
      }
      else {
	body+=rda->user()->emailAddress()+"\n";
      }
      body+=QObject::tr("Group")+": "+groupname+"\n";
      body+=QObject::tr("Reason")+": "+err_msg+"\n";

      if(!RDSendMail(&errors,subject,body,rda->system()->originEmailAddress(),
		     addrs)) {
	rda->syslog(LOG_WARNING,"email send failed [%s]",
		    errors.toUtf8().constData());
      }
    }
    else {
      c_bad_groups.push_back(groupname);
      c_bad_filenames.push_back(filename);
      c_bad_errors.push_back(err_msg);
    }
  }

  delete group;
}


void Journal::sendAll()
{
  QStringList used_addrs;

  //
  // Successful imports
  //
  used_addrs.clear();
  if(c_good_groups.size()>0) {
    QMultiMap<QString,QString> grp_map=GroupsByAddress(c_good_groups);
    for(QMap<QString,QString>::const_iterator it=grp_map.begin();
	it!=grp_map.end();it++) {
      if(!used_addrs.contains(it.key())) {
	QString errors;
	QString subject;
	QString body;
	QString from_addr;
	QStringList to_addrs;

	from_addr=rda->system()->originEmailAddress();
	to_addrs=it.key().split(",",QString::SkipEmptyParts);
	subject=QObject::tr("Rivendell import report")+"\n";

	body+=QObject::tr("Rivendell File Import Report")+"\n";
	body+="\n";
	body+=QObject::tr("-Group---- -Cart- -Title------------------------ -Filename-------------")+"\n";
	QStringList grps=grp_map.values(it.key());
	for(int i=0;i<grps.size();i++) {
	  for(int j=0;j<c_good_groups.size();j++) {
	    if(c_good_groups.at(j)==grps.at(i)) {
	      body+=QString().sprintf("%-10s %06u %-30s %-22s\n",
			grps.at(i).left(10).toUtf8().constData(),
			c_good_cart_numbers.at(j),
			c_good_titles.at(j).left(30).toUtf8().constData(),
			c_good_filenames.at(j).left(22).toUtf8().constData());
	    }
	  }
	}
	if(!RDSendMail(&errors,subject,body,rda->system()->originEmailAddress(),
		       to_addrs)) {
	  rda->syslog(LOG_WARNING,"email send failed [%s]",
		      errors.toUtf8().constData());
	}
	used_addrs.push_back(it.key());
      }
    }
  }

  //
  // Failed imports
  //
  used_addrs.clear();
  if(c_bad_groups.size()>0) {
    QMultiMap<QString,QString> grp_map=GroupsByAddress(c_bad_groups);
    for(QMap<QString,QString>::const_iterator it=grp_map.begin();
	it!=grp_map.end();it++) {
      if(!used_addrs.contains(it.key())) {
	QString errors;
	QString subject;
	QString body;
	QString from_addr;
	QStringList to_addrs;

	from_addr=rda->system()->originEmailAddress();
	to_addrs=it.key().split(",",QString::SkipEmptyParts);
	subject=QObject::tr("Rivendell import FAILURE report")+"\n";

	body+=QObject::tr("Rivendell File Import FAILURE Report")+"\n";
	body+="\n";
	body+=QObject::tr("-Group---- -Reason------------------------------ -Filename-------------")+"\n";
	QStringList grps=grp_map.values(it.key());
	for(int i=0;i<grps.size();i++) {
	  for(int j=0;j<c_bad_groups.size();j++) {
	    if(c_bad_groups.at(j)==grps.at(i)) {
	      body+=QString().sprintf("%-10s %-37s %-22s\n",
			grps.at(i).left(10).toUtf8().constData(),
			c_bad_errors.at(j).left(37).toUtf8().constData(),
			c_bad_filenames.at(j).left(22).toUtf8().constData());
	    }
	  }
	}
	if(!RDSendMail(&errors,subject,body,rda->system()->originEmailAddress(),
		       to_addrs)) {
	  rda->syslog(LOG_WARNING,"email send failed [%s]",
		      errors.toUtf8().constData());
	}
	used_addrs.push_back(it.key());
      }
    }
  }
}


QMultiMap<QString,QString> Journal::GroupsByAddress(QStringList groups) const
{
  RDGroup *grp;
  QMultiMap<QString,QString> ret;

  for(int i=0;i<groups.size();i++) {
    grp=new RDGroup(groups.at(i));
    if(!grp->notifyEmailAddress().isEmpty()) {
      if(!ret.contains(grp->notifyEmailAddress(),grp->name())) {
	ret.insert(grp->notifyEmailAddress(),grp->name());
      }
    }
    delete grp;
  }

  return ret;
}
