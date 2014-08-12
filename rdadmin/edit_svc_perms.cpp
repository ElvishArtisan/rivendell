// edit_svc_perms.cpp
//
// Edit Rivendell Service Permissions
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_svc_perms.cpp,v 1.9 2010/07/29 19:32:34 cvs Exp $
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

#include <rddb.h>
#include <edit_svc_perms.h>
#include <rduser.h>
#include <rdpasswd.h>


EditSvcPerms::EditSvcPerms(RDSvc *svc,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  RDSqlQuery *q;

  svc_svc=svc;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Service: ")+svc_svc->name());

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Services Selector
  //
  svc_host_sel=new RDListSelector(this,"svc_host_sel");
  svc_host_sel->sourceSetLabel(tr("Available Hosts"));
  svc_host_sel->destSetLabel(tr("Enabled Hosts"));
  svc_host_sel->setGeometry(10,10,380,130);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  sql=QString().sprintf("select STATION_NAME from SERVICE_PERMS \
                         where SERVICE_NAME=\"%s\"",
			(const char *)svc_svc->name());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    svc_host_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString().sprintf("select NAME from STATIONS");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(svc_host_sel->destFindItem(q->value(0).toString())==0) {
      svc_host_sel->sourceInsertItem(q->value(0).toString());
    }
  }
  delete q;
}


EditSvcPerms::~EditSvcPerms()
{
}


QSize EditSvcPerms::sizeHint() const
{
  return QSize(400,212);
} 


QSizePolicy EditSvcPerms::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditSvcPerms::okData()
{
  RDSqlQuery *q;
  QString sql;

  //
  // Add New Hosts
  //
  for(unsigned i=0;i<svc_host_sel->destCount();i++) {
    sql=QString().sprintf("select STATION_NAME from SERVICE_PERMS \
                           where SERVICE_NAME=\"%s\" && STATION_NAME=\"%s\"",
			  (const char *)svc_svc->name(),
			  (const char *)svc_host_sel->destText(i));
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString().
	sprintf("insert into SERVICE_PERMS (SERVICE_NAME,STATION_NAME) \
                 values (\"%s\",\"%s\")",
		(const char *)svc_svc->name(),
		(const char *)svc_host_sel->destText(i));
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Hosts
  //
  sql=QString().sprintf("delete from SERVICE_PERMS where SERVICE_NAME=\"%s\"",
			(const char *)svc_svc->name());
  for(unsigned i=0;i<svc_host_sel->destCount();i++) {
    sql+=QString().sprintf(" && STATION_NAME<>\"%s\"",
			   (const char *)svc_host_sel->destText(i));
  }
  q=new RDSqlQuery(sql);
  delete q;
  done(0);
}


void EditSvcPerms::cancelData()
{
  done(1);
}
