// edit_settings.cpp
//
// Edit Rivendell System-Wide Configuration
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

#include <q3filedialog.h>
#include <q3progressdialog.h>

#include <qapplication.h>
#include <qmessagebox.h>

#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "edit_settings.h"
#include "globals.h"

EditSettings::EditSettings(QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  QString sql;
  RDSqlQuery *q;

  y_pos=0;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  edit_system=new RDSystem();

  setWindowTitle("RDAdmin - "+tr("System-Wide Settings"));

  edit_system=new RDSystem();

  //
  // System Sample Rate
  //
  edit_sample_rate_box=new QComboBox(this);
  edit_sample_rate_box->setGeometry(250,10,70,20);
  edit_sample_rate_box->insertItem("32000");
  edit_sample_rate_box->insertItem("44100");
  edit_sample_rate_box->insertItem("48000");
  QLabel *label=new QLabel(edit_sample_rate_box,tr("System Sample Rate:"),this);
  label->setGeometry(10,10,235,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  label=new QLabel(tr("samples/second"),this);
  label->setGeometry(325,10,sizeHint().width()-285,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Allow Duplicate Cart Titles Box
  //
  edit_duplicate_carts_box=new QCheckBox(this);
  edit_duplicate_carts_box->setGeometry(20,32,15,15);
  connect(edit_duplicate_carts_box,SIGNAL(toggled(bool)),
	  this,SLOT(duplicatesCheckedData(bool)));
  label=
    new QLabel(edit_duplicate_carts_box,tr("Allow Duplicate Cart Titles"),this);
  label->setGeometry(40,30,sizeHint().width()-50,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  edit_fix_duplicate_carts_box=new QCheckBox(this);
  edit_fix_duplicate_carts_box->setGeometry(30,52,15,15);
  edit_fix_duplicate_carts_label=new QLabel(edit_fix_duplicate_carts_box,
			 tr("Auto-Correct Duplicate Cart Titles"),this);
  edit_fix_duplicate_carts_label->setGeometry(50,50,sizeHint().width()-60,20);
  edit_fix_duplicate_carts_label->setFont(labelFont());
  edit_fix_duplicate_carts_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // ISCI Cross Reference Path
  //
  edit_isci_path_edit=new QLineEdit(this);
  edit_isci_path_edit->setGeometry(250,76,sizeHint().width()-260,20);
  label=new QLabel(edit_isci_path_edit,tr("ISCI Cross Reference Path:"),this);
  label->setGeometry(10,76,235,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Notification Address
  //
  edit_notification_address_edit=new QLineEdit(this);
  edit_notification_address_edit->setGeometry(250,98,150,20);
  label=new QLabel(edit_notification_address_edit,tr("Multicast Address for Notifications"),this);
  label->setGeometry(10,98,235,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Maximum POST Size
  //
  edit_maxpost_spin=new QSpinBox(this);
  edit_maxpost_spin->setGeometry(250,120,60,20);
  edit_maxpost_spin->setRange(1,1000);
  label=new QLabel(edit_maxpost_spin,tr("Maximum Remote Post Length:"),this);
  label->setGeometry(10,120,235,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  label=new QLabel(tr("Mbytes"),this);
  label->setGeometry(315,120,60,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Temporary Cart Group
  //
  edit_temp_cart_group_box=new QComboBox(this);
  edit_temp_cart_group_box->setGeometry(250,141,100,20);
  sql="select NAME from GROUPS order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_temp_cart_group_box->insertItem(q->value(0).toString());
  }
  delete q;
  label=new QLabel(edit_temp_cart_group_box,tr("Temporary Cart Group:"),this);
  label->setGeometry(10,141,235,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Show User List
  //
  edit_show_user_list_box=new QCheckBox(this);
  edit_show_user_list_box->setGeometry(20,165,15,15);
  connect(edit_show_user_list_box,SIGNAL(toggled(bool)),
	  this,SLOT(duplicatesCheckedData(bool)));
  label=
    new QLabel(edit_show_user_list_box,tr("Show User List in RDLogin"),this);
  label->setGeometry(40,163,sizeHint().width()-50,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Duplicate List (initially hidden)
  //
  edit_duplicate_label=new QLabel(this);
  edit_duplicate_label->setText(tr("The following duplicate titles must be corrected before \"Allow Duplicate Values\" can be turned off."));
  edit_duplicate_label->setWordWrap(true);
  edit_duplicate_label->setGeometry(15,186,sizeHint().width()-30,50);
  edit_duplicate_label->setFont(subLabelFont());
  edit_duplicate_label->hide();
  edit_duplicate_list=new Q3ListView(this);
  edit_duplicate_list->setGeometry(10,234,sizeHint().width()-20,215);
  edit_duplicate_list->setItemMargin(5);
  edit_duplicate_list->setAllColumnsShowFocus(true);
  edit_duplicate_list->addColumn(tr("Cart"));
  edit_duplicate_list->setColumnAlignment(0,Qt::AlignCenter);
  edit_duplicate_list->addColumn(tr("Title"));
  edit_duplicate_list->setColumnAlignment(1,Qt::AlignLeft);
  edit_duplicate_list->hide();
  edit_save_button=new QPushButton(this);
  edit_save_button->
    setGeometry(sizeHint().width()-85,454,70,25);
  edit_save_button->setFont(buttonFont());
  edit_save_button->setText(tr("&Save List"));
  connect(edit_save_button,SIGNAL(clicked()),this,SLOT(saveData()));
  edit_save_button->hide();

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,
				  80,50);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
				  80,50);
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


EditSettings::~EditSettings()
{
  delete edit_system;
  delete edit_duplicate_carts_box;
  delete edit_duplicate_label;
  delete edit_duplicate_list;
}


QSize EditSettings::sizeHint() const
{
  return QSize(500,262+y_pos);
} 


QSizePolicy EditSettings::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditSettings::duplicatesCheckedData(bool state)
{
  edit_fix_duplicate_carts_box->setDisabled(state);
  edit_fix_duplicate_carts_label->setDisabled(state);
}


void EditSettings::saveData()
{
  QString filename=RDGetHomeDir();
  filename=Q3FileDialog::getSaveFileName(filename,"Text Files *.txt",this);
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


void EditSettings::okData()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  std::map<unsigned,QString> dups;

  if(edit_duplicate_carts_box->isChecked()!=
     edit_system->allowDuplicateCartTitles()) {
    QLabel *msg=new QLabel(this);
    Q3ProgressDialog *pd=new Q3ProgressDialog(this);
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
      pd->setProgress(0,10);
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
	  pd->setProgress(++step);
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
	edit_duplicate_label->show();
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
      sql="alter table CART modify column TITLE char(255) unique";
      q=new RDSqlQuery(sql);
      delete q;
      edit_system->setAllowDuplicateCartTitles(false);
    }
    else {
      sql="alter table CART drop index TITLE";
      q=new RDSqlQuery(sql);
      delete q;
      sql="alter table CART modify column TITLE char(255)";
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

  done(0);
}


void EditSettings::cancelData()
{
  done(-1);
}


void EditSettings::BuildDuplicatesList(std::map<unsigned,QString> *dups)
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
