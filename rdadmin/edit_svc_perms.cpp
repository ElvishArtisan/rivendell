// edit_svc_perms.cpp
//
// Edit Rivendell Service Permissions
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>
#include <rdescape_string.h>

#include "edit_svc_perms.h"

EditSvcPerms::EditSvcPerms(RDSvc *svc,QWidget *parent)
  : QDialog(parent)
{
  setModal(true);

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

  setWindowTitle("RDAdmin - "+tr("Service: ")+svc_svc->name());

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Services Selector
  //
  svc_host_sel=new RDListSelector(this);
  svc_host_sel->sourceSetLabel(tr("Available Hosts"));
  svc_host_sel->destSetLabel(tr("Enabled Hosts"));
  svc_host_sel->setGeometry(10,10,380,130);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  sql=QString("select STATION_NAME from SERVICE_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(svc_svc->name())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    svc_host_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString("select NAME from STATIONS");
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
    sql=QString("select STATION_NAME from SERVICE_PERMS where ")+
      "SERVICE_NAME=\""+RDEscapeString(svc_svc->name())+"\" && "+
      "STATION_NAME=\""+RDEscapeString(svc_host_sel->destText(i))+"\"";
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString("insert into SERVICE_PERMS (SERVICE_NAME,STATION_NAME) ")+
	"values (\""+RDEscapeString(svc_svc->name())+"\","+
	"\""+RDEscapeString(svc_host_sel->destText(i))+"\")";
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Hosts
  //
  sql=QString("delete from SERVICE_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(svc_svc->name())+"\"";
  for(unsigned i=0;i<svc_host_sel->destCount();i++) {
    sql+=QString(" && STATION_NAME<>\"")+
      RDEscapeString(svc_host_sel->destText(i))+"\"";
  }
  q=new RDSqlQuery(sql);
  delete q;
  done(0);
}


void EditSvcPerms::cancelData()
{
  done(1);
}
