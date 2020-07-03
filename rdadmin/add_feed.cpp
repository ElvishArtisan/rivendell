// add_feed.cpp
//
// Add a Rivendell RSS Feed
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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
#include <qmessagebox.h>
#include <qdatetime.h>

#include <rddb.h>
#include <rdapplication.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "add_feed.h"
#include "edit_feed.h"
#include "globals.h"

AddFeed::AddFeed(unsigned *id,QString *keyname,QWidget *parent)
  : RDDialog(parent)
{
  feed_keyname=keyname;
  feed_id=id;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDADmin - "+tr("Add RSS Feed"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);
  validator->addBannedChar(' ');

  //
  // Feed Name
  //
  feed_keyname_edit=new QLineEdit(this);
  feed_keyname_edit->setMaxLength(8);
  feed_keyname_edit->setValidator(validator);
  connect(feed_keyname_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(keynameChangedData(const QString &)));
  feed_keyname_label=new QLabel(feed_keyname_edit,tr("&New Feed Name:"),this);
  feed_keyname_label->setFont(labelFont());
  feed_keyname_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Enable Users Checkbox
  //
  feed_users_box=new QCheckBox(this);
  feed_users_box->setChecked(true);
  feed_users_label=
    new QLabel(feed_users_box,tr("Enable Feed for All Users"),this);
  feed_users_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  feed_ok_button=new QPushButton(this);
  feed_ok_button->setDefault(true);
  feed_ok_button->setFont(buttonFont());
  feed_ok_button->setText(tr("&OK"));
  feed_ok_button->setDisabled(true);
  connect(feed_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  feed_cancel_button=new QPushButton(this);
  feed_cancel_button->setFont(buttonFont());
  feed_cancel_button->setText(tr("&Cancel"));
  connect(feed_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddFeed::~AddFeed()
{
  delete feed_keyname_edit;
}


QSize AddFeed::sizeHint() const
{
  return QSize(290,123);
} 


QSizePolicy AddFeed::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddFeed::keynameChangedData(const QString &str)
{
  feed_ok_button->setDisabled(str.isEmpty());
}


void AddFeed::okData()
{
  QString err_msg;

  *feed_id=RDFeed::create(feed_keyname_edit->text(),feed_users_box->isChecked(),
			  &err_msg);
  if(*feed_id!=0) {
    *feed_keyname=feed_keyname_edit->text();
    done(0);
  }
  else {
    QMessageBox::warning(this,"RDAdmin - "+tr("Add Feed Error"),err_msg);
  }
}


void AddFeed::cancelData()
{
  done(-1);
}


void AddFeed::resizeEvent(QResizeEvent *e)
{
  feed_keyname_label->setGeometry(10,11,120,19);
  feed_keyname_edit->setGeometry(135,11,sizeHint().width()-140,19);

  feed_users_box->setGeometry(40,35,15,15);
  feed_users_label->setGeometry(60,33,sizeHint().width()-60,19);

  feed_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  feed_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
