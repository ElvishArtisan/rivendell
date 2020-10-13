// edit_system.cpp
//
// Edit Rivendell System-Wide Configuration
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

#include <qapplication.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>

#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "edit_system.h"
#include "globals.h"

EditSystem::EditSystem(QWidget *parent)
  : RDDialog(parent)
{
  QString sql;
  RDSqlQuery *q;

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

  //
  // System Sample Rate
  //
  edit_sample_rate_box=new QComboBox(this);
  edit_sample_rate_box->insertItem("32000");
  edit_sample_rate_box->insertItem("44100");
  edit_sample_rate_box->insertItem("48000");
  edit_sample_rate_label=
    new QLabel(edit_sample_rate_box,tr("System Sample Rate:"),this);
  edit_sample_rate_label->setFont(labelFont());
  edit_sample_rate_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  edit_sample_rate_unit_label=new QLabel(tr("samples/second"),this);
  edit_sample_rate_unit_label->setGeometry(325,10,sizeHint().width()-285,20);
  edit_sample_rate_unit_label->setFont(labelFont());
  edit_sample_rate_unit_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Allow Duplicate Cart Titles Box
  //
  edit_duplicate_carts_box=new QCheckBox(this);
  connect(edit_duplicate_carts_box,SIGNAL(toggled(bool)),
	  this,SLOT(duplicatesCheckedData(bool)));
  edit_duplicate_label=
    new QLabel(edit_duplicate_carts_box,tr("Allow Duplicate Cart Titles"),this);
  edit_duplicate_label->setFont(labelFont());
  edit_duplicate_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  edit_fix_duplicate_carts_box=new QCheckBox(this);
  edit_fix_duplicate_carts_label=new QLabel(edit_fix_duplicate_carts_box,
			 tr("Auto-Correct Duplicate Cart Titles"),this);
  edit_fix_duplicate_carts_label->setFont(labelFont());
  edit_fix_duplicate_carts_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // ISCI Cross Reference Path
  //
  edit_isci_path_edit=new QLineEdit(this);
  edit_isci_path_label=
    new QLabel(edit_isci_path_edit,tr("ISCI Cross Reference Path:"),this);
  edit_isci_path_label->setFont(labelFont());
  edit_isci_path_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Notification Address
  //
  edit_notification_address_edit=new QLineEdit(this);
  edit_notification_address_label=
    new QLabel(edit_notification_address_edit,
	       tr("Multicast Address for Notifications"),this);
  edit_notification_address_label->setFont(labelFont());
  edit_notification_address_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Maximum POST Size
  //
  edit_maxpost_spin=new QSpinBox(this);
  edit_maxpost_spin->setRange(1,1000);
  edit_maxpost_label=
    new QLabel(edit_maxpost_spin,tr("Maximum Remote Post Length:"),this);
  edit_maxpost_label->setFont(labelFont());
  edit_maxpost_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  edit_maxpost_unit_label=new QLabel(tr("Mbytes"),this);
  edit_maxpost_unit_label->setFont(labelFont());
  edit_maxpost_unit_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Temporary Cart Group
  //
  edit_temp_cart_group_box=new QComboBox(this);
  sql="select NAME from GROUPS order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_temp_cart_group_box->insertItem(q->value(0).toString());
  }
  delete q;
  edit_temp_cart_group_label=
    new QLabel(edit_temp_cart_group_box,tr("Temporary Cart Group:"),this);
  edit_temp_cart_group_label->setFont(labelFont());
  edit_temp_cart_group_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Show User List
  //
  edit_show_user_list_box=new QCheckBox(this);
  connect(edit_show_user_list_box,SIGNAL(toggled(bool)),
	  this,SLOT(duplicatesCheckedData(bool)));
  edit_show_user_list_label=
    new QLabel(edit_show_user_list_box,tr("Show User List in RDLogin"),this);
  edit_show_user_list_label->setFont(labelFont());
  edit_show_user_list_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // RSS Processor Host
  //
  edit_rss_processor_box=new QComboBox(this);
  sql=QString("select NAME from STATIONS order by NAME");
  q=new RDSqlQuery(sql);
  edit_rss_processor_box->insertItem(0,tr("[none]"));
  while(q->next()) {
    edit_rss_processor_box->insertItem(edit_rss_processor_box->count(),
				       q->value(0).toString());
  }
  delete q;
  edit_rss_processor_label=
    new QLabel(edit_rss_processor_box,tr("Process RSS Updates On")+":",this);
  edit_rss_processor_label->setFont(labelFont());
  edit_rss_processor_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Duplicate List (initially hidden)
  //
  edit_duplicate_hidden_label=new QLabel(this);
  edit_duplicate_hidden_label->setText(tr("The following duplicate titles must be corrected before \"Allow Duplicate Values\" can be turned off."));
  edit_duplicate_hidden_label->setWordWrap(true);
  edit_duplicate_hidden_label->setFont(subLabelFont());
  edit_duplicate_hidden_label->hide();
  edit_duplicate_list=new Q3ListView(this);
  edit_duplicate_list->setItemMargin(5);
  edit_duplicate_list->setAllColumnsShowFocus(true);
  edit_duplicate_list->addColumn(tr("Cart"));
  edit_duplicate_list->setColumnAlignment(0,Qt::AlignCenter);
  edit_duplicate_list->addColumn(tr("Title"));
  edit_duplicate_list->setColumnAlignment(1,Qt::AlignLeft);
  edit_duplicate_list->hide();
  edit_save_button=new QPushButton(this);
  edit_save_button->setFont(buttonFont());
  edit_save_button->setText(tr("&Save List"));
  connect(edit_save_button,SIGNAL(clicked()),this,SLOT(saveData()));
  edit_save_button->hide();

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
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  edit_duplicate_carts_box->setChecked(edit_system->allowDuplicateCartTitles());
  edit_fix_duplicate_carts_box->
    setChecked(edit_system->fixDuplicateCartTitles());
  duplicatesCheckedData(edit_system->allowDuplicateCartTitles());
  edit_maxpost_spin->setValue(edit_system->maxPostLength()/1000000);
  edit_isci_path_edit->setText(edit_system->isciXreferencePath());
  edit_notification_address_edit->
    setText(edit_system->notificationAddress().toString());
  edit_show_user_list_box->setChecked(edit_system->showUserList());



  QString station=edit_system->rssProcessorStation();
  for(int i=0;i<edit_rss_processor_box->count();i++) {
    if(edit_rss_processor_box->text(i)==station) {
      edit_rss_processor_box->setCurrentIndex(i);
    }
  }


  for(int i=0;i<edit_sample_rate_box->count();i++) {
    if(edit_sample_rate_box->text(i).toUInt()==edit_system->sampleRate()) {
      edit_sample_rate_box->setCurrentItem(i);
    }
  }
  for(int i=0;i<edit_temp_cart_group_box->count();i++) {
    if(edit_temp_cart_group_box->text(i)==edit_system->tempCartGroup()) {
      edit_temp_cart_group_box->setCurrentItem(i);
    }
  }
}


EditSystem::~EditSystem()
{
  delete edit_system;
  delete edit_encoders_dialog;
  delete edit_duplicate_carts_box;
  delete edit_duplicate_label;
  delete edit_duplicate_list;
}


QSize EditSystem::sizeHint() const
{
  return QSize(500,284+y_pos);
} 


QSizePolicy EditSystem::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditSystem::duplicatesCheckedData(bool state)
{
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
  if(filename.find(".")<0) {
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
  FILE *f=fopen(filename,"w");
  if(f==NULL) {
    QMessageBox::warning(this,tr("File Error"),
			 tr("Unable to write file \""+filename+"\"."));
    return;
  }
  QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());
  fprintf(f,"Rivendell Duplicate Cart Title List\n");
  fprintf(f,"Generated %s\n",
	  (const char *)dt.toString("hh:mm:ss - MM/dd/yyyy"));
  fprintf(f,"\n");
  fprintf(f,"Cart    Title\n");
  fprintf(f,"----    -----\n");
  Q3ListViewItem *item=edit_duplicate_list->firstChild();
  while(item!=NULL) {
    fprintf(f,"%s  %s\n",(const char *)item->text(0),
	    (const char *)item->text(1));
    item=item->nextSibling();
  }
  fclose(f);
}


void EditSystem::encodersData()
{
  edit_encoders_dialog->exec();
}


void EditSystem::okData()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  std::map<unsigned,QString> dups;

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
	sql=QString("select NUMBER from CART where ")+
	  "(TITLE=\""+RDEscapeString(q->value(1).toString())+"\")&&"+
	  QString().sprintf("(NUMBER!=%u)",q->value(0).toUInt());
	q1=new RDSqlQuery(sql);
	while(q1->next()) {
	  dups[q1->value(0).toUInt()]=q->value(1).toString();
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
	Q3ListViewItem *item;
	y_pos=305;
	setMinimumWidth(sizeHint().width());
	setMaximumWidth(sizeHint().width());
	setMinimumHeight(sizeHint().height());
	setMaximumHeight(sizeHint().height());
	edit_duplicate_carts_box->setChecked(true);
	edit_duplicate_hidden_label->show();
	edit_duplicate_list->show();
	edit_save_button->show();
	edit_duplicate_list->clear();
	edit_ok_button->setGeometry(sizeHint().width()-180,
				    sizeHint().height()-60,
				    80,50);
	edit_cancel_button->
	  setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
	for(std::map<unsigned,QString>::const_iterator ci=dups.begin();
	    ci!=dups.end();ci++) {
	  item=new Q3ListViewItem(edit_duplicate_list);
	  item->setText(0,QString().sprintf("%06u",ci->first));
	  item->setText(1,ci->second);
	}
	return;
      }

      //
      // All ok -- make the change
      //
      sql="alter table CART drop index TITLE_IDX";
      q=new RDSqlQuery(sql);
      delete q;
      sql="alter table CART modify column TITLE varchar(191) unique";
      q=new RDSqlQuery(sql);
      delete q;
      edit_system->setAllowDuplicateCartTitles(false);
    }
    else {
      sql="alter table CART drop index TITLE";
      q=new RDSqlQuery(sql);
      delete q;
      sql="alter table CART modify column TITLE varchar(191)";
      q=new RDSqlQuery(sql);
      delete q;
      sql="alter table CART add index TITLE_IDX(TITLE)";
      q=new RDSqlQuery(sql);
      delete q;
      edit_system->setAllowDuplicateCartTitles(true);
    }
    delete pd;
  }
  edit_system->
    setFixDuplicateCartTitles(edit_fix_duplicate_carts_box->isChecked());
  edit_system->setSampleRate(edit_sample_rate_box->currentText().toUInt());
  edit_system->setMaxPostLength(edit_maxpost_spin->value()*1000000);
  edit_system->setIsciXreferencePath(edit_isci_path_edit->text());
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

  done(0);
}


void EditSystem::cancelData()
{
  done(-1);
}


void EditSystem::BuildDuplicatesList(std::map<unsigned,QString> *dups)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql="select NUMBER,TITLE from CART order by NUMBER";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("select NUMBER from CART where ")+
      "(TITLE=\""+RDEscapeString(q->value(1).toString())+"\")&&"+
      QString().sprintf("(NUMBER!=%u)",q->value(0).toUInt());
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

  edit_notification_address_edit->setGeometry(250,120,150,20);
  edit_notification_address_label->setGeometry(10,120,235,20);

  edit_maxpost_spin->setGeometry(250,142,60,20);
  edit_maxpost_label->setGeometry(10,142,235,20);
  edit_maxpost_unit_label->setGeometry(315,142,60,20);

  edit_temp_cart_group_box->setGeometry(250,163,100,20);
  edit_temp_cart_group_label->setGeometry(10,163,235,20);

  edit_rss_processor_label->setGeometry(10,185,235,20);
  edit_rss_processor_box->setGeometry(250,185,200,20);

  edit_duplicate_hidden_label->setGeometry(15,207,size().width()-30,50);
  edit_duplicate_list->setGeometry(10,255,size().width()-20,215);
  edit_save_button->setGeometry(size().width()-85,475,70,25);

  edit_encoders_button->setGeometry(10,size().height()-60,120,50);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
