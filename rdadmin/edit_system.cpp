// edit_system.cpp
//
// Edit Rivendell System-Wide Configuration
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QFileDialog>
#include <QMultiMap>
#include <QMessageBox>
#include <QProgressDialog>

#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "edit_system.h"
#include "globals.h"

EditSystem::EditSystem(QWidget *parent)
  : RDDialog(parent)
{
  y_pos=0;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumHeight(sizeHint().height());

  edit_system=new RDSystem();

  setWindowTitle("RDAdmin - "+tr("System-Wide Settings"));

  edit_system=new RDSystem();
  edit_encoders_dialog=new ListEncoders(this);
  edit_station_list_model=new RDStationListModel(true,"",this);

  //
  // Dialogs
  //
  edit_test_datetimes_dialog=new TestDatetimes(this);

  //
  // System Sample Rate
  //
  edit_sample_rate_box=new QComboBox(this);
  edit_sample_rate_box->insertItem(0,"32000");
  edit_sample_rate_box->insertItem(1,"44100");
  edit_sample_rate_box->insertItem(2,"48000");
  edit_sample_rate_label=new QLabel(tr("System Sample Rate:"),this);
  edit_sample_rate_label->setFont(labelFont());
  edit_sample_rate_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_sample_rate_unit_label=new QLabel(tr("samples/second"),this);
  edit_sample_rate_unit_label->setGeometry(325,10,sizeHint().width()-285,20);
  edit_sample_rate_unit_label->setFont(labelFont());
  edit_sample_rate_unit_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Allow Duplicate Cart Titles Box
  //
  edit_duplicate_carts_box=new QCheckBox(this);
  connect(edit_duplicate_carts_box,SIGNAL(toggled(bool)),
	  this,SLOT(duplicatesCheckedData(bool)));
  edit_duplicate_label=
    new QLabel(tr("Allow Duplicate Cart Titles"),this);
  edit_duplicate_label->setFont(labelFont());
  edit_duplicate_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  edit_fix_duplicate_carts_box=new QCheckBox(this);
  edit_fix_duplicate_carts_label=
    new QLabel(tr("Auto-Correct Duplicate Cart Titles"),this);
  edit_fix_duplicate_carts_label->setFont(labelFont());
  edit_fix_duplicate_carts_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // ISCI Cross Reference Path
  //
  edit_isci_path_edit=new QLineEdit(this);
  edit_isci_path_label=new QLabel(tr("ISCI Cross Reference Path:"),this);
  edit_isci_path_label->setFont(labelFont());
  edit_isci_path_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Origin Email Address
  //
  edit_origin_email_addr_edit=new QLineEdit(this);
  edit_origin_email_addr_edit->setMaxLength(64);
  edit_origin_email_addr_label=new QLabel(tr("Origin E-Mail Address")+":",this);
  edit_origin_email_addr_label->setFont(labelFont());
  edit_origin_email_addr_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Notification Address
  //
  edit_notification_address_edit=new QLineEdit(this);
  edit_notification_address_label=
    new QLabel(tr("Multicast Address for Notifications"),this);
  edit_notification_address_label->setFont(labelFont());
  edit_notification_address_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Maximum POST Size
  //
  edit_maxpost_spin=new QSpinBox(this);
  edit_maxpost_spin->setRange(1,1000);
  edit_maxpost_label=
    new QLabel(tr("Maximum Remote Post Length:"),this);
  edit_maxpost_label->setFont(labelFont());
  edit_maxpost_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_maxpost_unit_label=new QLabel(tr("Mbytes"),this);
  edit_maxpost_unit_label->setFont(labelFont());
  edit_maxpost_unit_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Temporary Cart Group
  //
  edit_temp_cart_group_box=new RDComboBox(this);
  edit_temp_cart_group_model=new RDGroupListModel(false,true,this);
  edit_temp_cart_group_box->setModel(edit_temp_cart_group_model);
  edit_temp_cart_group_label=new QLabel(tr("Temporary Cart Group:"),this);
  edit_temp_cart_group_label->setFont(labelFont());
  edit_temp_cart_group_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Show User List
  //
  edit_show_user_list_box=new QCheckBox(this);
  edit_show_user_list_label=new QLabel(tr("Show User List in RDLogin"),this);
  edit_show_user_list_label->setFont(labelFont());
  edit_show_user_list_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // RSS Processor Host
  //
  edit_rss_processor_box=new QComboBox(this);
  edit_rss_processor_box->setModel(edit_station_list_model);
  edit_rss_processor_label=new QLabel(tr("Process RSS Updates On")+":",this);
  edit_rss_processor_label->setFont(labelFont());
  edit_rss_processor_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Duplicate List (initially hidden)
  //
  edit_duplicate_hidden_label=new QLabel(this);
  edit_duplicate_hidden_label->setText(tr("The following duplicate titles must be corrected before \"Allow Duplicate Values\" can be turned off."));
  edit_duplicate_hidden_label->setWordWrap(true);
  edit_duplicate_hidden_label->setFont(subLabelFont());
  edit_duplicate_hidden_label->hide();

  edit_duplicate_view=new RDTableView(this);
  edit_duplicate_model=new RDLibraryModel(this);
  edit_duplicate_model->setFont(defaultFont());
  edit_duplicate_model->setPalette(palette());
  edit_duplicate_view->setModel(edit_duplicate_model);
  connect(edit_duplicate_model,SIGNAL(modelReset()),
	  edit_duplicate_view,SLOT(resizeColumnsToContents()));
  edit_duplicate_view->hide();
  edit_save_button=new QPushButton(this);
  edit_save_button->setFont(buttonFont());
  edit_save_button->setText(tr("Save List"));
  connect(edit_save_button,SIGNAL(clicked()),this,SLOT(saveData()));
  edit_save_button->hide();

  //
  // Date/Time Formats
  //
  edit_datetime_group=new QGroupBox(tr("Date/Time Formats"),this);
  edit_datetime_group->setFont(labelFont());

  edit_long_date_label=
    new QLabel(tr("Long Date Format")+":",edit_datetime_group);
  edit_long_date_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_long_date_label->setFont(labelFont());
  edit_long_date_edit=new QLineEdit(edit_datetime_group);
  edit_long_date_edit->setFont(defaultFont());
  edit_long_date_edit->setMaxLength(32);

  edit_short_date_label=
    new QLabel(tr("Short Date Format")+":",edit_datetime_group);
  edit_short_date_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_short_date_label->setFont(labelFont());
  edit_short_date_edit=new QLineEdit(edit_datetime_group);
  edit_short_date_edit->setFont(defaultFont());
  edit_short_date_edit->setMaxLength(11);

  edit_time_label=
    new QLabel(tr("Time Format")+":",edit_datetime_group);
  edit_time_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_time_label->setFont(labelFont());
  edit_time_box=new QComboBox(edit_datetime_group);
  edit_time_box->setFont(defaultFont());
  edit_time_box->insertItem(0,tr("Twenty Four Hour"));
  edit_time_box->insertItem(1,tr("Twelve Hour"));

  edit_datetime_test_button=new QPushButton(tr("Test"),edit_datetime_group);
  connect(edit_datetime_test_button,SIGNAL(clicked()),
	  this,SLOT(datetimeTestData()));

  edit_datetime_defaults_button=
    new QPushButton(tr("Restore\nDefaults"),edit_datetime_group);
  connect(edit_datetime_defaults_button,SIGNAL(clicked()),
	  this,SLOT(datetimeDefaultsData()));

  //
  //  Encoders Button
  //
  edit_encoders_button=new QPushButton(this);
  edit_encoders_button->setFont(buttonFont());
  edit_encoders_button->setText(tr("Edit Encoder\nList"));
  connect(edit_encoders_button,SIGNAL(clicked()),this,SLOT(encodersData()));

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Values
  //
  edit_duplicate_carts_box->setChecked(edit_system->allowDuplicateCartTitles());
  edit_fix_duplicate_carts_box->
    setChecked(edit_system->fixDuplicateCartTitles());
  duplicatesCheckedData(edit_system->allowDuplicateCartTitles());
  edit_maxpost_spin->setValue(edit_system->maxPostLength()/1000000);
  edit_isci_path_edit->setText(edit_system->isciXreferencePath());
  edit_origin_email_addr_edit->setText(edit_system->originEmailAddress());
  edit_notification_address_edit->
    setText(edit_system->notificationAddress().toString());
  edit_show_user_list_box->setChecked(edit_system->showUserList());
  edit_long_date_edit->setText(edit_system->longDateFormat());
  edit_short_date_edit->setText(edit_system->shortDateFormat());
  if(edit_system->showTwelveHourTime()) {
    edit_time_box->setCurrentIndex(1);
  }
  else {
    edit_time_box->setCurrentIndex(0);
  }

  QString station=edit_system->rssProcessorStation();
  for(int i=0;i<edit_rss_processor_box->count();i++) {
    if(edit_rss_processor_box->itemText(i)==station) {
      edit_rss_processor_box->setCurrentIndex(i);
    }
  }

  for(int i=0;i<edit_sample_rate_box->count();i++) {
    if(edit_sample_rate_box->itemText(i).toUInt()==
       edit_system->sampleRate()) {
      edit_sample_rate_box->setCurrentIndex(i);
    }
  }
  edit_temp_cart_group_box->setCurrentText(edit_system->tempCartGroup());
}


EditSystem::~EditSystem()
{
  delete edit_station_list_model;
  delete edit_system;
  delete edit_encoders_dialog;
  delete edit_duplicate_carts_box;
  delete edit_duplicate_label;
  delete edit_duplicate_view;
  delete edit_duplicate_model;
  delete edit_test_datetimes_dialog;
}


QSize EditSystem::sizeHint() const
{
  return QSize(500,406+y_pos);
} 


QSizePolicy EditSystem::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditSystem::duplicatesCheckedData(bool state)
{
  if((!state)&&edit_system->allowDuplicateCartTitles()) {
    if(QMessageBox::warning(this,"RDAdmin - "+tr("Deprecation Warning"),
			    tr("The ability to disallow duplicate cart titles in Rivendell <strong>has been deprecated</strong> and may be removed from future versions; it is included strictly to keep existing setups working. Use of it can cause other features within Rivendell to operate unreliably. It should never be used in new Rivendell setups!")+"<p>"+
			    tr("Attempt to disallow duplicate cart titles anyway?"),
			    QMessageBox::No,QMessageBox::Yes)!=QMessageBox::Yes) {
      edit_duplicate_carts_box->setChecked(true);
      return;
    }
  }

  edit_fix_duplicate_carts_box->setDisabled(state);
  edit_fix_duplicate_carts_label->setDisabled(state);
}


void EditSystem::saveData()
{
  QString filename=RDGetHomeDir();
  filename=QFileDialog::getSaveFileName(this,"RDAdmin - "+tr("Save text file"),
					filename,
					"Text files (*.txt);;All files (*.*)");
  if(filename.isNull()) {
    return;
  }
  if(filename.indexOf(".")<0) {
    filename+=".txt";
  }
  if(QFile::exists(filename)) {
    if(QMessageBox::question(this,tr("File Exists"),
			     tr("The file \"")+filename+
			     tr("\" exists.\n\nOverwrite?"),
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
  }
  FILE *f=fopen(filename.toUtf8(),"w");
  if(f==NULL) {
    QMessageBox::warning(this,tr("File Error"),
			 tr("Unable to write file")+" \""+filename+"\".");
    return;
  }
  QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());
  fprintf(f,"Rivendell Duplicate Cart Title List\n");
  fprintf(f,"Generated %s\n",
	  dt.toString("hh:mm:ss - MM/dd/yyyy").toUtf8().constData());
  fprintf(f,"\n");
  fprintf(f,"Cart    Title\n");
  fprintf(f,"----    -----\n");

  for(int i=0;i<edit_duplicate_model->rowCount();i++) {
    fprintf(f,"%s  %s\n",
	    edit_duplicate_model->data(edit_duplicate_model->index(i,0)).toString().toUtf8().constData(),
	    edit_duplicate_model->data(edit_duplicate_model->index(i,4)).toString().toUtf8().constData());
  }
  fclose(f);
}


void EditSystem::encodersData()
{
  edit_encoders_dialog->exec();
}


void EditSystem::datetimeTestData()
{
  edit_test_datetimes_dialog->exec(edit_long_date_edit->text(),
				   edit_short_date_edit->text(),
				   edit_time_box->currentIndex());
}


void EditSystem::datetimeDefaultsData()
{
  if(QMessageBox::warning(this,"RDAdmin - "+tr("Warning"),
    tr("This will reset all date/time display formats to default values!")+
			  "\n"+
			  tr("Proceed?"),
			  QMessageBox::No,QMessageBox::Yes)!=QMessageBox::Yes) {
    return;
  }
  edit_long_date_edit->setText(RD_DEFAULT_LONG_DATE_FORMAT);
  edit_short_date_edit->setText(RD_DEFAULT_SHORT_DATE_FORMAT);
  edit_time_box->setCurrentIndex(0);
}


void EditSystem::okData()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QMultiMap<unsigned,QString> dups;

  if(edit_duplicate_carts_box->isChecked()!=
     edit_system->allowDuplicateCartTitles()) {
    QLabel *msg=new QLabel(this);
    QProgressDialog *pd=new QProgressDialog(this);
    pd->setWindowTitle("RDAdmin - "+tr("Progress"));
    pd->setLabel(msg);
    pd->setCancelButton(NULL);
    pd->setMinimumDuration(2);
    if(!edit_duplicate_carts_box->isChecked()) {
      //
      // Check for dups
      //
      msg->setText(tr("Checking the Library for duplicates."));
      sql="select NUMBER,TITLE from CART order by NUMBER";
      q=new RDSqlQuery(sql);
      int count=0;
      int step=0;
      int step_size=q->size()/10;
      pd->setMaximum(10);
      pd->setValue(0);
      while(q->next()) {
	sql=QString("select `NUMBER` from `CART` where ")+
	  "(`TITLE`='"+RDEscapeString(q->value(1).toString())+"')&&"+
	  QString().sprintf("(`NUMBER`!=%u)",q->value(0).toUInt());
	q1=new RDSqlQuery(sql);
	while(q1->next()) {
	  dups.insert(q1->value(0).toUInt(),q->value(1).toString());
	}
	delete q1;
	count++;
	if(count>=step_size) {
	  pd->setValue(++step);
	  count=0;
	  qApp->processEvents();
	}
      }
      delete q;
      pd->reset();
      if(dups.size()>0) {
	y_pos=305;
	setMinimumWidth(sizeHint().width());
	setMinimumHeight(sizeHint().height());
	edit_duplicate_carts_box->setChecked(true);
	edit_duplicate_hidden_label->show();
	edit_duplicate_view->show();
	edit_save_button->show();
	edit_ok_button->setGeometry(sizeHint().width()-180,
				    sizeHint().height()-60,
				    80,50);
	edit_cancel_button->
	  setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
	QString filter_sql="where (";
	for(QMultiMap<unsigned,QString>::const_iterator ci=dups.begin();
	    ci!=dups.end();ci++) {
	  filter_sql+=QString().sprintf("`CART`.`NUMBER`=%u||",ci.key());
	}
	filter_sql=filter_sql.left(filter_sql.length()-2)+
	  ") order by `CART`.`TITLE` ";
	edit_duplicate_model->setFilterSql(filter_sql);
	return;
      }

      //
      // All ok -- make the change
      //
      sql="alter table `CART` drop index `TITLE_IDX`";
      RDSqlQuery::apply(sql);

      sql="alter table `CART` modify column `TITLE` varchar(191) unique";
      RDSqlQuery::apply(sql);

      edit_system->setAllowDuplicateCartTitles(false);
    }
    else {
      sql="alter table `CART` drop index `TITLE`";
      RDSqlQuery::apply(sql);

      sql="alter table `CART` modify column `TITLE` varchar(191)";
      RDSqlQuery::apply(sql);

      sql="alter table `CART` add index `TITLE_IDX`(`TITLE`)";
      RDSqlQuery::apply(sql);

      edit_system->setAllowDuplicateCartTitles(true);
    }
    delete pd;
  }
  edit_system->
    setFixDuplicateCartTitles(edit_fix_duplicate_carts_box->isChecked());
  edit_system->setSampleRate(edit_sample_rate_box->currentText().toUInt());
  edit_system->setMaxPostLength(edit_maxpost_spin->value()*1000000);
  edit_system->setIsciXreferencePath(edit_isci_path_edit->text());
  edit_system->setOriginEmailAddress(edit_origin_email_addr_edit->text());
  edit_system->
    setNotificationAddress(QHostAddress(edit_notification_address_edit->text()));
  edit_system->setTempCartGroup(edit_temp_cart_group_box->currentText());
  edit_system->setShowUserList(edit_show_user_list_box->isChecked());
  if(edit_rss_processor_box->currentIndex()==0) {
    edit_system->setRssProcessorStation();
  }
  else {
    edit_system->setRssProcessorStation(edit_rss_processor_box->currentText());
  }
  edit_system->setLongDateFormat(edit_long_date_edit->text());
  edit_system->setShortDateFormat(edit_short_date_edit->text());
  edit_system->setShowTwelveHourTime(edit_time_box->currentIndex());

  done(true);
}


void EditSystem::cancelData()
{
  done(false);
}


void EditSystem::BuildDuplicatesList(std::map<unsigned,QString> *dups)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql="select `NUMBER`,`TITLE` from `CART` order by `NUMBER`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("select `NUMBER` from `CART` where ")+
      "(`TITLE`='"+RDEscapeString(q->value(1).toString())+"')&&"+
      QString().sprintf("(`NUMBER`!=%u)",q->value(0).toUInt());
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      (*dups)[q1->value(0).toUInt()]=q->value(1).toString();
    }
    delete q1;
  }
  delete q;
}


void EditSystem::resizeEvent(QResizeEvent *e)
{
  edit_sample_rate_box->setGeometry(250,10,70,20);
  edit_sample_rate_label->setGeometry(10,10,235,20);

  edit_duplicate_carts_box->setGeometry(20,32,15,15);
  edit_duplicate_label->setGeometry(40,30,size().width()-50,20);
  edit_fix_duplicate_carts_box->setGeometry(30,52,15,15);
  edit_fix_duplicate_carts_label->setGeometry(50,50,size().width()-60,20);

  edit_show_user_list_box->setGeometry(20,74,15,15);
  edit_show_user_list_label->setGeometry(40,72,size().width()-50,20);

  edit_isci_path_edit->setGeometry(250,98,size().width()-260,20);
  edit_isci_path_label->setGeometry(10,98,235,20);

  edit_origin_email_addr_edit->setGeometry(250,120,size().width()-260,20);
  edit_origin_email_addr_label->setGeometry(10,120,235,20);

  edit_notification_address_edit->setGeometry(250,142,150,20);
  edit_notification_address_label->setGeometry(10,142,235,20);

  edit_maxpost_spin->setGeometry(250,164,60,20);
  edit_maxpost_label->setGeometry(10,164,235,20);
  edit_maxpost_unit_label->setGeometry(315,164,60,20);

  edit_temp_cart_group_box->setGeometry(250,185,140,20);
  edit_temp_cart_group_label->setGeometry(10,185,235,20);

  edit_rss_processor_label->setGeometry(10,207,235,20);
  edit_rss_processor_box->setGeometry(250,207,200,20);

  edit_datetime_group->setGeometry(10,229,size().width()-20,100);
  edit_datetime_test_button->setGeometry(5,22,80,35);
  edit_datetime_defaults_button->setGeometry(5,60,80,35);
  edit_long_date_label->setGeometry(110,27,120,20);
  edit_long_date_edit->setGeometry(235,27,edit_datetime_group->width()-240,20);
  edit_short_date_label->setGeometry(110,49,120,20);
  edit_short_date_edit->setGeometry(235,49,edit_datetime_group->width()-240,20);
  edit_time_label->setGeometry(110,71,120,20);
  edit_time_box->setGeometry(235,71,edit_time_box->sizeHint().width(),20);

  edit_duplicate_hidden_label->setGeometry(15,329,size().width()-30,50);
  edit_duplicate_view->setGeometry(10,377,size().width()-20,215);
  edit_save_button->setGeometry(size().width()-85,597,70,25);

  edit_encoders_button->setGeometry(10,size().height()-60,120,50);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
