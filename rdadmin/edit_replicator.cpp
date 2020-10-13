// edit_replicator.cpp
//
// Edit a Rivendell Replicator
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

#include <math.h>

#include <rdapplication.h>
#include <rdescape_string.h>
#include <rdexport_settings_dialog.h>

#include "edit_replicator.h"
#include "globals.h"

EditReplicator::EditReplicator(const QString &repl_name,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  QString sql;
  RDSqlQuery *q;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  repl_replicator=new RDReplicator(repl_name);
  repl_settings=new RDSettings();

  setWindowTitle("RDAdmin - "+tr("Replicator: ")+repl_name);

  //
  // Replicator Name
  //
  repl_name_edit=new QLineEdit(this);
  repl_name_edit->setGeometry(105,11,sizeHint().width()-115,19);
  repl_name_edit->setMaxLength(32);
  repl_name_edit->setReadOnly(true);
  QLabel *repl_name_label=new QLabel(repl_name_edit,tr("Name:"),this);
  repl_name_label->setFont(labelFont());
  repl_name_label->setGeometry(10,11,90,19);
  repl_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Replicator Description
  //
  repl_description_edit=new QLineEdit(this);
  repl_description_edit->setGeometry(105,33,sizeHint().width()-115,19);
  repl_description_edit->setMaxLength(64);
  QLabel *repl_description_label=
    new QLabel(repl_description_edit,tr("Description:"),this);
  repl_description_label->setFont(labelFont());
  repl_description_label->setGeometry(10,33,90,19);
  repl_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Replicator Type
  //
  repl_type_box=new QComboBox(this);
  repl_type_box->setGeometry(105,55,sizeHint().width()-115,19);
  for(unsigned i=0;i<(int)RDReplicator::TypeLast;i++) {
    repl_type_box->insertItem(RDReplicator::typeString((RDReplicator::Type)i));
    if(repl_replicator->type()==(RDReplicator::Type)i) {
      repl_type_box->setCurrentItem(i);
    }
  }
  QLabel *repl_type_label=new QLabel(repl_type_box,tr("Type:"),this);
  repl_type_label->setFont(labelFont());
  repl_type_label->setGeometry(10,55,90,19);
  repl_type_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Host System
  //
  repl_station_box=new QComboBox(this);
  repl_station_box->setGeometry(155,77,sizeHint().width()-165,19);
  sql="select NAME from STATIONS order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    repl_station_box->insertItem(q->value(0).toString());
    if(repl_replicator->stationName()==q->value(0).toString()) {
      repl_station_box->setCurrentItem(repl_station_box->count()-1);
    }
  }
  delete q;
  QLabel *repl_station_label=
    new QLabel(repl_station_box,tr("Host System:"),this);
  repl_station_label->setFont(labelFont());
  repl_station_label->setGeometry(10,77,140,19);
  repl_station_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Upload Audio URL
  //
  repl_url_edit=new QLineEdit(this);
  repl_url_edit->setGeometry(155,99,335,19);
  repl_url_edit->setMaxLength(255);
  repl_url_label=new QLabel(repl_url_edit,tr("Audio Upload URL:"),this);
  repl_url_label->setFont(labelFont());
  repl_url_label->setGeometry(20,99,130,19);
  repl_url_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Upload Username
  //
  repl_username_edit=new QLineEdit(this);
  repl_username_edit->setGeometry(225,121,95,19);
  repl_username_edit->setMaxLength(64);
  repl_username_label=new QLabel(repl_username_edit,tr("Username:"),this);
  repl_username_label->setFont(labelFont());
  repl_username_label->setGeometry(40,121,180,19);
  repl_username_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Upload Password
  //
  repl_password_edit=new QLineEdit(this);
  repl_password_edit->setGeometry(395,121,95,19);
  repl_password_edit->setMaxLength(64);
  repl_password_edit->setEchoMode(QLineEdit::Password);
  repl_password_label=new QLabel(repl_password_edit,tr("Password:"),this);
  repl_password_label->setFont(labelFont());
  repl_password_label->setGeometry(320,121,70,19);
  repl_password_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Audio Format
  //
  repl_format_edit=new QLineEdit(this);
  repl_format_edit->setGeometry(155,143,285,20);
  repl_format_edit->setReadOnly(true);
  repl_format_label=new QLabel(repl_format_edit,tr("Upload Format:"),this);
  repl_format_label->setFont(labelFont());
  repl_format_label->setGeometry(5,143,145,20);
  repl_format_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  repl_format_button=new QPushButton(this);
  repl_format_button->setGeometry(450,142,40,24);
  repl_format_button->setFont(subButtonFont());
  repl_format_button->setText(tr("S&et"));
  connect(repl_format_button,SIGNAL(clicked()),this,SLOT(setFormatData()));

  //
  // Normalize Check Box
  //
  repl_normalize_box=new QCheckBox(this);
  repl_normalize_box->setGeometry(155,167,15,15);
  repl_normalize_box->setChecked(true);
  repl_normalize_check_label=
    new QLabel(repl_normalize_box,tr("Normalize"),this);
  repl_normalize_check_label->setFont(labelFont());
  repl_normalize_check_label->setGeometry(175,165,83,20);
  repl_normalize_check_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(repl_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeCheckData(bool)));

  //
  // Normalize Level
  //
  repl_normalize_spin=new QSpinBox(this);
  repl_normalize_spin->setGeometry(295,165,40,20);
  repl_normalize_spin->setRange(-30,-1);
  repl_normalize_label=new QLabel(repl_normalize_spin,tr("Level:"),this);
  repl_normalize_label->setFont(labelFont());
  repl_normalize_label->setGeometry(245,165,45,20);
  repl_normalize_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  repl_normalize_unit_label=new QLabel(tr("dBFS"),this);
  repl_normalize_unit_label->setFont(labelFont());
  repl_normalize_unit_label->setGeometry(340,165,40,20);
  repl_normalize_unit_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Groups Selector
  //
  repl_groups_sel=new RDListSelector(this);
  repl_groups_sel->setGeometry(60,192,380,130);
  repl_groups_sel->sourceSetLabel(tr("Available Groups"));
  repl_groups_sel->destSetLabel(tr("Active Groups"));

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Values
  //
  repl_name_edit->setText(repl_replicator->name());
  repl_description_edit->setText(repl_replicator->description());
  repl_url_edit->setText(repl_replicator->url());
  repl_username_edit->setText(repl_replicator->urlUsername());
  repl_password_edit->setText(repl_replicator->urlPassword());
  repl_settings->setFormat(repl_replicator->format());
  repl_settings->setChannels(repl_replicator->channels());
  repl_settings->setSampleRate(repl_replicator->sampleRate());
  repl_settings->setBitRate(repl_replicator->bitRate());
  repl_settings->setQuality(repl_replicator->quality());
  repl_format_edit->setText(repl_settings->description());
  if(repl_replicator->normalizeLevel()>0) {
    repl_normalize_box->setChecked(false);
  }
  else {
    repl_normalize_box->setChecked(true);
    repl_normalize_spin->setValue(repl_replicator->normalizeLevel()/1000);
  }
  sql=QString("select GROUP_NAME from REPLICATOR_MAP where ")+
    "REPLICATOR_NAME=\""+RDEscapeString(repl_name_edit->text())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    repl_groups_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString().sprintf("select NAME from GROUPS");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(repl_groups_sel->destFindItem(q->value(0).toString())==0) {
      repl_groups_sel->sourceInsertItem(q->value(0).toString());
    }
  }
  delete q;
}


EditReplicator::~EditReplicator()
{
  delete repl_settings;
  delete repl_replicator;
}


QSize EditReplicator::sizeHint() const
{
  return QSize(500,409);
} 


QSizePolicy EditReplicator::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditReplicator::setFormatData()
{
  RDExportSettingsDialog *dialog=new RDExportSettingsDialog("RDAdmin",this);
  dialog->exec(repl_settings);
  delete dialog;
  repl_format_edit->setText(repl_settings->description());
}


void EditReplicator::normalizeCheckData(bool state)
{
  repl_normalize_label->setEnabled(state);
  repl_normalize_spin->setEnabled(state);
  repl_normalize_unit_label->setEnabled(state);
}


void EditReplicator::okData()
{
  QString sql;
  RDSqlQuery *q;

  repl_replicator->setDescription(repl_description_edit->text());
  repl_replicator->setType((RDReplicator::Type)repl_type_box->currentItem());
  repl_replicator->setStationName(repl_station_box->currentText());
  repl_replicator->setUrl(repl_url_edit->text());
  repl_replicator->setUrlUsername(repl_username_edit->text());
  repl_replicator->setUrlPassword(repl_password_edit->text());
  repl_replicator->setFormat(repl_settings->format());
  repl_replicator->setChannels(repl_settings->channels());
  repl_replicator->setSampleRate(repl_settings->sampleRate());
  repl_replicator->setBitRate(repl_settings->bitRate());
  repl_replicator->setQuality(repl_settings->quality());
  if(repl_normalize_box->isChecked()) {
    repl_replicator->setNormalizeLevel(repl_normalize_spin->value()*1000);
  }
  else {
    repl_replicator->setNormalizeLevel(1);
  }

  //
  // Add New Groups
  //
  for(unsigned i=0;i<repl_groups_sel->destCount();i++) {
    sql=QString("select GROUP_NAME from REPLICATOR_MAP where ")+
      "REPLICATOR_NAME=\""+RDEscapeString(repl_name_edit->text())+"\" && "+
      "GROUP_NAME=\""+RDEscapeString(repl_groups_sel->destText(i))+"\"";
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString("insert into REPLICATOR_MAP (REPLICATOR_NAME,GROUP_NAME) ")+
	"values (\""+RDEscapeString(repl_name_edit->text())+"\","+
	"\""+RDEscapeString(repl_groups_sel->destText(i))+"\")";
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Groups
  //
  sql=QString("delete from REPLICATOR_MAP where ")+
    "REPLICATOR_NAME=\""+RDEscapeString(repl_name_edit->text())+"\"";
  for(unsigned i=0;i<repl_groups_sel->destCount();i++) {
    sql+=QString(" && GROUP_NAME<>\"")+
      RDEscapeString(repl_groups_sel->destText(i))+"\"";
  }
  q=new RDSqlQuery(sql);
  delete q;

  done(0);
}


void EditReplicator::cancelData()
{
  done(-1);
}
