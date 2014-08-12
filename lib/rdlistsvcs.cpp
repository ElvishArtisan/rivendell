// rdlistsvcs.cpp
//
// Service Picker dialog
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlistsvcs.cpp,v 1.1.2.2 2012/11/16 18:10:40 cvs Exp $
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

#include <qcolordialog.h>

#include <rddb.h>
#include <rdcart.h>
#include <rdcart_dialog.h>
#include <rd.h>
#include <rdconf.h>

#include "rdlistsvcs.h"

RDListSvcs::RDListSvcs(const QString &caption,QWidget *parent)
  : QDialog(parent)
{
  edit_caption=caption;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(caption+" - "+tr("Rivendell Services"));

  //
  // Fonts
  //
  QFont label_font("helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  //
  // Services
  //
  edit_svc_list=new QListBox(this);
  connect(edit_svc_list,SIGNAL(doubleClicked(QListBoxItem *)),
	  this,SLOT(doubleClickedData(QListBoxItem *)));

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(label_font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(label_font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDListSvcs::~RDListSvcs()
{
}


QSize RDListSvcs::sizeHint() const
{
  return QSize(300,240);
} 


QSizePolicy RDListSvcs::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDListSvcs::exec(QString *svcname)
{
  QString sql;
  RDSqlQuery *q;

  edit_svcname=svcname;
  edit_svc_list->clear();
  sql="select NAME from SERVICES order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_svc_list->insertItem(q->value(0).toString());
    if(q->value(0).toString()==*edit_svcname) {
      edit_svc_list->setCurrentItem(edit_svc_list->count()-1);
    }
  }
  delete q;

  return QDialog::exec();
}


void RDListSvcs::doubleClickedData(QListBoxItem *item)
{
  okData();
}


void RDListSvcs::okData()
{
  if(edit_svc_list->currentItem()>=0) {
    *edit_svcname=edit_svc_list->currentText();
  }
  done(0);
}


void RDListSvcs::cancelData()
{
  done(-1);
}


void RDListSvcs::resizeEvent(QResizeEvent *e)
{
  edit_svc_list->setGeometry(10,10,size().width()-20,size().height()-80);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
