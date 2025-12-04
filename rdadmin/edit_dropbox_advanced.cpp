// edit_dropbox_advanced.cpp
//
// Edit a Rivendell Dropbox Advanced Configuration
//
//   (C) Copyright 2002-2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>

#include "globals.h"
#include "edit_dropbox_advanced.h"

EditDropboxAdvanced::EditDropboxAdvanced(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  box_dropbox=NULL;

  //
  // Scan Count
  //
  box_scan_count_spin=new QSpinBox(this);
  box_scan_count_spin->setRange(3,20);
  box_scan_count_spin->setSpecialValueText(tr("[default]"));
  box_scan_count_label=new QLabel(tr("Dropbox Scan Count")+":",this);
  box_scan_count_label->setFont(labelFont());
  box_scan_count_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Scan Interval
  //
  box_scan_interval_label=new QLabel(tr("Dropbox Scan Interval")+":",this);
  box_scan_interval_label->setFont(labelFont());
  box_scan_interval_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  box_scan_interval_spin=new QSpinBox(this);
  box_scan_interval_spin->setRange(5,60);
  box_scan_interval_spin->setSpecialValueText(tr("[default]"));
  box_scan_interval_unit=new QLabel(tr("seconds"),this);
  box_scan_interval_unit->setFont(labelFont());
  box_scan_interval_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  box_ok_button=new QPushButton(this);
  box_ok_button->setDefault(true);
  box_ok_button->setFont(buttonFont());
  box_ok_button->setText(tr("OK"));
  connect(box_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  box_cancel_button=new QPushButton(this);
  box_cancel_button->setFont(buttonFont());
  box_cancel_button->setText(tr("Cancel"));
  connect(box_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize EditDropboxAdvanced::sizeHint() const
{
  return QSize(350,110);
} 


QSizePolicy EditDropboxAdvanced::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditDropboxAdvanced::exec(RDDropbox *dropbox)
{
  box_dropbox=dropbox;

  setWindowTitle("RDAdmin - "+tr("Dropbox Advanced Settings")+" ["+
		 tr("ID")+QString::asprintf(": %d]",dropbox->id()));
  box_scan_count_spin->setValue(dropbox->dropBoxScanCount());
  box_scan_interval_spin->setValue(dropbox->dropBoxScanInterval());

  return QDialog::exec();
}


void EditDropboxAdvanced::okData()
{
  box_dropbox->setDropBoxScanCount(box_scan_count_spin->value());
  box_dropbox->setDropBoxScanInterval(box_scan_interval_spin->value());
  done(true);
}


void EditDropboxAdvanced::cancelData()
{
  done(false);
}


void EditDropboxAdvanced::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();
  
  box_scan_count_label->setGeometry(10,3,150,20);
  box_scan_count_spin->setGeometry(215-50,3,80,20);

  box_scan_interval_label->setGeometry(10,23,150,20);
  box_scan_interval_spin->setGeometry(215-50,23,80,20);
  box_scan_interval_unit->setGeometry(320-50-20,23,100,20);

  box_ok_button->setGeometry(w-180,h-60,80,50);
  box_cancel_button->setGeometry(w-90,h-60,80,50);
}
