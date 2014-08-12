// edit_perms.cpp
//
// Edit RDLogManager Service Associations
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_perms.cpp,v 1.10.8.1 2012/04/23 17:22:47 cvs Exp $
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

#include <rduser.h>
#include <rdpasswd.h>
#include <rddb.h>
#include <edit_perms.h>


EditPerms::EditPerms(QString object_name,ObjectType type,
		     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  RDSqlQuery *q;
  QString str;

  sel_type=type;
  sel_name=object_name;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  str=QString(tr("Service Associations"));
  setCaption(QString().sprintf("%s - %s",
			       (const char *)sel_name,(const char *)str));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Services Selector
  //
  svc_object_sel=new RDListSelector(this,"svc_object_sel");
  svc_object_sel->setGeometry(10,10,380,130);
  svc_object_sel->sourceSetLabel(tr("Available Services"));
  svc_object_sel->destSetLabel(tr("Enabled Services"));

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
  //
  switch(sel_type) {
      case EditPerms::ObjectEvent:
	object_type="EVENT";
	break;

      case EditPerms::ObjectClock:
	object_type="CLOCK";
	break;
  }
  // FIXME: should this be filtered based on the user if usersec is enabled?
  sql=QString().sprintf("select SERVICE_NAME from %s_PERMS \
                         where %s_NAME=\"%s\"",
			(const char *)object_type,
			(const char *)object_type,
			(const char *)object_name);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    svc_object_sel->destInsertItem(q->value(0).toString());
  }
  delete q;
  sql=QString().sprintf("select NAME from SERVICES");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(svc_object_sel->destFindItem(q->value(0).toString())==0) {
      svc_object_sel->sourceInsertItem(q->value(0).toString());
    }
  }
  delete q;
}


EditPerms::~EditPerms()
{
}


QSize EditPerms::sizeHint() const
{
  return QSize(400,212);
} 


QSizePolicy EditPerms::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditPerms::okData()
{
  RDSqlQuery *q;
  QString sql;

  //
  // Add New Objects
  //
  for(unsigned i=0;i<svc_object_sel->destCount();i++) {
    sql=QString().sprintf("select %s_NAME from %s_PERMS \
                           where SERVICE_NAME=\"%s\" && %s_NAME=\"%s\"",
			  (const char *)object_type,
			  (const char *)object_type,
			  (const char *)svc_object_sel->destText(i),
			  (const char *)object_type,
			  (const char *)sel_name);
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString().
	sprintf("insert into %s_PERMS (SERVICE_NAME,%s_NAME) \
                 values (\"%s\",\"%s\")",
		(const char *)object_type,
		(const char *)object_type,
		(const char *)svc_object_sel->destText(i),
		(const char *)sel_name);
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Hosts
  //
  sql=QString().sprintf("delete from %s_PERMS where %s_NAME=\"%s\"",
			(const char *)object_type,
			(const char *)object_type,
			(const char *)sel_name);
  for(unsigned i=0;i<svc_object_sel->destCount();i++) {
    sql+=QString().sprintf(" && SERVICE_NAME<>\"%s\"",
			   (const char *)svc_object_sel->destText(i));
  }
  q=new RDSqlQuery(sql);
  delete q;

  done(0);
}


void EditPerms::cancelData()
{
  done(-1);
}


void EditPerms::closeEvent(QCloseEvent *e)
{
  cancelData();
}
