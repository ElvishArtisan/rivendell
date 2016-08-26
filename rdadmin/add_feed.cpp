// add_feed.cpp
//
// Add a Rivendell Service
//
//   (C) Copyright 2002,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <qlabel.h>
#include <qmessagebox.h>
#include <qdatetime.h>

#include <rddb.h>
#include <rdtextvalidator.h>
#include <rdescape_string.h>

#include <edit_feed.h>
#include <add_feed.h>

AddFeed::AddFeed(unsigned *id,QString *keyname,QWidget *parent)
  : QDialog(parent,"",true)
{
  feed_keyname=keyname;
  feed_id=id;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Add Feed"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont user_font=QFont("Helvetica",12,QFont::Normal);
  user_font.setPixelSize(12);

  //
  // Enable Users Checkbox
  //
  feed_users_box=new QCheckBox(this);
  feed_users_box->setGeometry(40,40,15,15);
  feed_users_box->setChecked(true);
  QLabel *label=new QLabel(feed_users_box,tr("Enable Feed for All Users"),this);
  label->setGeometry(60,38,sizeHint().width()-60,19);
  label->setFont(user_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Feed Name
  //
  feed_keyname_edit=new QLineEdit(this);
  feed_keyname_edit->setGeometry(145,11,sizeHint().width()-150,19);
  feed_keyname_edit->setMaxLength(8);
  feed_keyname_edit->setValidator(validator);
  label=new QLabel(feed_keyname_edit,tr("&New Feed Name:"),this);
  label->setGeometry(10,11,130,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

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
}


AddFeed::~AddFeed()
{
  delete feed_keyname_edit;
}


QSize AddFeed::sizeHint() const
{
  return QSize(250,124);
} 


QSizePolicy AddFeed::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddFeed::okData()
{
  if((*feed_id=
     RDFeed::create(feed_keyname_edit->text(),feed_users_box->isChecked()))<0) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Add Feed Error"),
			 tr("A feed with that key name already exists!"));
    return;
  }
  *feed_keyname=feed_keyname_edit->text();
  done(0);
}


void AddFeed::cancelData()
{
  done(-1);
}
