// add_feed.cpp
//
// Add a Rivendell Service
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: add_feed.cpp,v 1.7 2010/07/29 19:32:34 cvs Exp $
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
#include <rdcreateauxfieldstable.h>
#include <rdfeedlog.h>

#include <edit_feed.h>
#include <add_feed.h>


AddFeed::AddFeed(unsigned *id,QString *keyname,QWidget *parent,
		 const char *name)
  : QDialog(parent,name,true)
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
  feed_users_box=new QCheckBox(this,"feed_users_box");
  feed_users_box->setGeometry(40,40,15,15);
  feed_users_box->setChecked(true);
  QLabel *label=new QLabel(feed_users_box,tr("Enable Feed for All Users"),
			   this,"feed_users_label");
  label->setGeometry(60,38,sizeHint().width()-60,19);
  label->setFont(user_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Feed Name
  //
  feed_keyname_edit=new QLineEdit(this,"feed_keyname_edit");
  feed_keyname_edit->setGeometry(145,11,sizeHint().width()-150,19);
  feed_keyname_edit->setMaxLength(8);
  feed_keyname_edit->setValidator(validator);
  label=new QLabel(feed_keyname_edit,tr("&New Feed Name:"),
				      this,"feed_name_label");
  label->setGeometry(10,11,130,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

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
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString().sprintf("select KEY_NAME from FEEDS where KEY_NAME=\"%s\"",
			(const char *)feed_keyname_edit->text());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QMessageBox::warning(this,tr("Add Feed Error"),
			 tr("A feed with that key name already exists!"));
    delete q;
    return;
  }
  delete q;

  //
  // Create Default Feed Perms
  //
  if(feed_users_box->isChecked()) {
    sql="select LOGIN_NAME from USERS \
         where (ADMIN_USERS_PRIV='N')&&(ADMIN_CONFIG_PRIV='N')";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into FEED_PERMS set USER_NAME=\"%s\",\
                             KEY_NAME=\"%s\"",
			    (const char *)q->value(0).toString(),
			    (const char *)feed_keyname_edit->text());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  //
  // Create Feed
  //
  sql=QString().sprintf("insert into FEEDS set KEY_NAME=\"%s\",\
                         ORIGIN_DATETIME=\"%s %s\",HEADER_XML=\"%s\",\
                         CHANNEL_XML=\"%s\",ITEM_XML=\"%s\"",
			(const char *)feed_keyname_edit->text(),
			(const char *)QDate::currentDate().
			toString("yyyy-MM-dd"),
			(const char *)QTime::currentTime().
			toString("hh:mm:ss"),
			(const char *)RDEscapeString(DEFAULT_HEADER_XML),
			(const char *)RDEscapeString(DEFAULT_CHANNEL_XML),
			(const char *)RDEscapeString(DEFAULT_ITEM_XML));
  q=new RDSqlQuery(sql);
  delete q;
  RDCreateFeedLog(feed_keyname_edit->text());
  RDCreateAuxFieldsTable(feed_keyname_edit->text());
  sql=QString().sprintf("select ID from FEEDS where KEY_NAME=\"%s\"",
			(const char *)feed_keyname_edit->text());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    *feed_id=q->value(0).toUInt();
  }
  delete q;
  *feed_keyname=feed_keyname_edit->text();
  done(0);
}


void AddFeed::cancelData()
{
  done(-1);
}
