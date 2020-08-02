// edit_chain.cpp
//
// Edit a Rivendell Log Chain Entry
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <qpushbutton.h>
#include <qmessagebox.h>

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdlist_logs.h>

#include <edit_chain.h>

#include "globals.h"

EditChain::EditChain(RDLogLine *line,QWidget *parent)
  : EditEvent(line,parent)
{
  setWindowTitle("RDLogEdit - "+tr("Edit Log Chain"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Label
  //
  edit_label_edit=new QLineEdit(this);
  edit_label_edit->setMaxLength(64);
  edit_label_label=new QLabel(tr("Log Name"),this);
  edit_label_label->setFont(labelFont());
  connect(edit_label_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(labelChangedData(const QString &)));

  //
  // Select Log Button
  //
  edit_select_button=new QPushButton(this);
  edit_select_button->setFont(subButtonFont());
  edit_select_button->setText(tr("Select"));
  connect(edit_select_button,SIGNAL(clicked()),this,SLOT(selectLogData()));

  //
  // Comment
  //
  edit_comment_edit=new QLineEdit(this);
  edit_comment_edit->setReadOnly(true);
  edit_comment_label=new QLabel(tr("Log Description"),this);
  edit_comment_label->setFont(labelFont());

  //
  // Populate Data
  //
  edit_label_edit->setText(logLine()->markerLabel());
  labelChangedData(edit_label_edit->text());
}


QSize EditChain::sizeHint() const
{
  return QSize(625,260);
} 


QSizePolicy EditChain::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditChain::selectLogData()
{
  QString logname;

  RDListLogs *d=
    new RDListLogs(&logname,RDLogFilter::UserFilter,this);
  if(!d->exec()) {
    delete d;
    return;
  }
  delete d;
  edit_label_edit->setText(logname);
  labelChangedData(logname);
}


void EditChain::labelChangedData(const QString &logname)
{
  QString sql=
    QString("select DESCRIPTION from LOGS where ")+
    "NAME=\""+RDEscapeString(logname)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    edit_comment_edit->clear();
    return;
  }
  edit_comment_edit->setText(q->value(0).toString());
  delete q;
}


bool EditChain::saveData()
{
  logLine()->setMarkerLabel(edit_label_edit->text());
  logLine()->setMarkerComment(edit_comment_edit->text());

  return true;
}


void EditChain::resizeEvent(QResizeEvent *e)
{
  edit_label_label->setGeometry(12,100,160,14);
  edit_label_edit->setGeometry(10,116,size().width()-90,18);
  edit_select_button->setGeometry(size().width()-70,111,60,28);

  edit_comment_label->setGeometry(12,140,160,14);
  edit_comment_edit->setGeometry(10,156,sizeHint().width()-20,18);

  EditEvent::resizeEvent(e);
}
