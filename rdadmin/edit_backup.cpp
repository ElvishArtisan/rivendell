// edit_backup.cpp
//
// Edit an automatic backup configuration.
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <QPushButton>

#include <rd.h>

#include "edit_backup.h"

EditBackup::EditBackup(RDStation *station,QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont small_font=QFont("Helvetica",12,QFont::Bold);
  small_font.setPixelSize(12);
  QFont big_font=QFont("Helvetica",14,QFont::Bold);
  big_font.setPixelSize(14);

  //
  // Dialog Name
  //
  edit_station=station;
  setWindowTitle("RDAdmin - "+tr("Backup config for ")+station->name());

  //
  // Backup Life
  //
  edit_life_box=new QSpinBox(this);
  edit_life_box->setGeometry(155,10,40,19);
  edit_life_box->setMinValue(0);
  edit_life_box->setMaxValue(30);
  QLabel *edit_life_box_label=
    new QLabel(edit_life_box,tr("Keep Backups For:"),this);
  edit_life_box_label->setGeometry(10,10,140,19);
  edit_life_box_label->setFont(small_font);
  edit_life_box_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  QLabel *edit_life_box_unit=new QLabel(tr("days"),this);
  edit_life_box_unit->setGeometry(200,10,120,19);
  edit_life_box_unit->setFont(small_font);
  edit_life_box_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);
  connect(edit_life_box,SIGNAL(valueChanged(int)),
	  this,SLOT(lifeChangedData(int)));

  //
  // Backup Directory
  //
  edit_path_edit=new QLineEdit(this);
  edit_path_edit->setGeometry(155,35,sizeHint().width()-165,19);
  edit_path_label=new QLabel(edit_life_box,tr("Backup Directory:"),this);
  edit_path_label->setGeometry(10,35,140,19);
  edit_path_label->setFont(small_font);
  edit_path_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(small_font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(small_font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  edit_life_box->setValue(edit_station->backupLife());
  edit_path_edit->setText(edit_station->backupPath());
  lifeChangedData(edit_life_box->value());
}


EditBackup::~EditBackup()
{
}


QSize EditBackup::sizeHint() const
{
  return QSize(375,130);
} 


QSizePolicy EditBackup::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditBackup::lifeChangedData(int days)
{
  if(days==0) {
    edit_path_label->setDisabled(true);
    edit_path_edit->setDisabled(true);
  }
  else {
    edit_path_label->setEnabled(true);
    edit_path_edit->setEnabled(true);
  }
}


void EditBackup::okData()
{
  if((edit_life_box->value()>0)&&(edit_path_edit->text().isEmpty())) {
    QMessageBox::warning(this,tr("Path Missing"),
			 tr("You must supply a backup path!"));
    return;
  }
  edit_station->setBackupLife(edit_life_box->value());
  edit_station->setBackupPath(edit_path_edit->text());
  done(0);
}


void EditBackup::cancelData()
{
  done(-1);
}
