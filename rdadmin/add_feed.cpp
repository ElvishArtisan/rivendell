// add_feed.cpp
//
// Add a Rivendell RSS Feed
//
//   (C) Copyright 2002-2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <QMessageBox>

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "add_feed.h"
#include "edit_feed.h"
#include "globals.h"

AddFeed::AddFeed(unsigned *id,QString *keyname,QStringList *usernames,
		 QWidget *parent)
  : RDDialog(parent)
{
  feed_keyname=keyname;
  feed_id=id;
  feed_usernames=usernames;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumHeight(sizeHint().height());

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
  feed_keyname_label=new QLabel(tr("New Feed Name:"),this);
  feed_keyname_label->setFont(labelFont());
  feed_keyname_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Enable User Selector
  //
  feed_usernames_groupbox=new QGroupBox(tr("Enable Users For Feed"),this);

  feed_usernames_group=new QButtonGroup(this);
  connect(feed_usernames_group,SIGNAL(buttonToggled(QAbstractButton *,bool)),
	  this,SLOT(buttonToggledData(QAbstractButton *,bool)));

  feed_none_radio=
    new QRadioButton(tr("No Users"),this);
  feed_usernames_group->addButton(feed_none_radio);

  feed_all_radio=
    new QRadioButton(tr("All Users"),this);
  feed_usernames_group->addButton(feed_all_radio);

  feed_some_radio=
    new QRadioButton(tr("Users from Feed")+":",this);
  feed_usernames_group->addButton(feed_some_radio);
  feed_keyname_box=new QComboBox(this);
  feed_keyname_model=new RDFeedListModel(true,false,this);
  feed_keyname_model->setPalette(palette());
  feed_keyname_model->setFont(font());
  feed_keyname_box->setModel(feed_keyname_model);
  feed_some_radio->setDisabled(feed_keyname_model->rowCount()==0);
  feed_none_radio->setChecked(true);
  buttonToggledData(feed_usernames_group->checkedButton(),true);

  //
  //  Ok Button
  //
  feed_ok_button=new QPushButton(this);
  feed_ok_button->setDefault(true);
  feed_ok_button->setFont(buttonFont());
  feed_ok_button->setText(tr("OK"));
  feed_ok_button->setDisabled(true);
  connect(feed_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  feed_cancel_button=new QPushButton(this);
  feed_cancel_button->setFont(buttonFont());
  feed_cancel_button->setText(tr("Cancel"));
  connect(feed_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddFeed::~AddFeed()
{
  delete feed_keyname_edit;
}


QSize AddFeed::sizeHint() const
{
  return QSize(310,205);
} 


QSizePolicy AddFeed::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddFeed::keynameChangedData(const QString &str)
{
  feed_ok_button->setDisabled(str.isEmpty());
}


void AddFeed::buttonToggledData(QAbstractButton *rbutton,bool checked)
{
  feed_keyname_box->setEnabled((rbutton==feed_some_radio)&&checked);
}


void AddFeed::okData()
{
  QString err_msg;
  QString sql;
  RDSqlQuery *q=NULL;

  *feed_id=RDFeed::create(feed_keyname_edit->text(),false,&err_msg);
  if(*feed_id!=0) {
    *feed_keyname=feed_keyname_edit->text();
    if(feed_usernames_group->checkedButton()==feed_all_radio) {
      sql=QString("select ")+
	"`LOGIN_NAME` "+  // 00
	"from `USERS` where "+
	"(`ADMIN_CONFIG_PRIV`='N')&&(ADMIN_RSS_PRIV='N')";
      q=new RDSqlQuery(sql);
      while(q->next()) {
	AuthorizeUser(*feed_keyname,q->value(0).toString());
      }
      delete q;
    }
    if(feed_usernames_group->checkedButton()==feed_some_radio) {
      sql=QString("select ")+
	"`USER_NAME` "+  // 00
	"from `FEED_PERMS` where "+
	"`KEY_NAME`='"+RDEscapeString(feed_keyname_box->currentText())+"'";
      q=new RDSqlQuery(sql);
      while(q->next()) {
	AuthorizeUser(*feed_keyname,q->value(0).toString());
      }
      delete q;
    }
    done(true);
  }
  else {
    QMessageBox::warning(this,"RDAdmin - "+tr("Add Feed Error"),err_msg);
  }
}


void AddFeed::cancelData()
{
  done(false);
}


void AddFeed::resizeEvent(QResizeEvent *e)
{
  feed_keyname_label->setGeometry(10,11,120,19);
  feed_keyname_edit->setGeometry(135,11,sizeHint().width()-145,19);

  feed_usernames_groupbox->setGeometry(10,35,size().width()-20,97);
  feed_none_radio->setGeometry(30,45,160,30);
  feed_all_radio->setGeometry(30,70,160,30);
  feed_some_radio->setGeometry(30,95,160,30);
  feed_keyname_box->setGeometry(150,95,size().width()-170,30);
  
  feed_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  feed_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void AddFeed::AuthorizeUser(const QString &keyname,
			    const QString &login_name) const
{
  QString sql=QString("insert into `FEED_PERMS` set ")+
    "`KEY_NAME`='"+RDEscapeString(keyname)+"',"+
    "`USER_NAME`='"+RDEscapeString(login_name)+"'";
  RDSqlQuery::apply(sql);
}
