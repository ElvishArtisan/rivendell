// positiondialog.cpp
//
// Dialog to set RDMonitor screen position.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: positiondialog.cpp,v 1.1.2.2 2013/11/11 20:34:27 cvs Exp $
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

#include "positiondialog.h"

PositionDialog::PositionDialog(QDesktopWidget *dw,RDMonitorConfig *config,
			       QWidget *parent)
  : QDialog(parent)
{
  pos_desktop_widget=dw;
  pos_config=config;

  setCaption("RDMonitor");

  //
  // Fonts
  //
  QFont button_font("helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  QFont list_font("helvetica",12,QFont::Normal);
  list_font.setPixelSize(12);

  //
  // Screen Number
  //
  pos_screen_number_box=new QComboBox(this);
  pos_screen_number_box->setFont(list_font);
  pos_screen_number_label=
    new QLabel(pos_screen_number_box,tr("Screen")+":",this);
  pos_screen_number_label->setFont(button_font);
  pos_screen_number_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Position
  //
  pos_position_box=new QComboBox(this);
  pos_position_box->setFont(list_font);
  pos_position_box->setFont(list_font);
  pos_position_label=
    new QLabel(pos_position_box,tr("Position")+":",this);
  pos_position_label->setFont(button_font);
  pos_position_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  for(int i=0;i<RDMonitorConfig::LastPosition;i++) {
    pos_position_box->
      insertItem(RDMonitorConfig::positionText((RDMonitorConfig::Position)i));
  }

  //
  // X Offset
  //
  pos_x_offset_spin=new QSpinBox(this);
  pos_x_offset_spin->setFont(list_font);
  pos_x_offset_spin->setRange(0,99);
  pos_x_offset_label=new QLabel(pos_x_offset_spin,tr("X Offset")+":",this);
  pos_x_offset_label->setFont(button_font);
  pos_x_offset_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Y Offset
  //
  pos_y_offset_spin=new QSpinBox(this);
  pos_y_offset_spin->setFont(list_font);
  pos_y_offset_spin->setRange(0,99);
  pos_y_offset_label=new QLabel(pos_y_offset_spin,tr("Y Offset")+":",this);
  pos_y_offset_label->setFont(button_font);
  pos_y_offset_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // OK Button
  //
  pos_ok_button=new QPushButton(tr("OK"),this);
  pos_ok_button->setFont(button_font);
  connect(pos_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  pos_cancel_button=new QPushButton(tr("Cancel"),this);
  pos_cancel_button->setFont(button_font);
  connect(pos_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  setMaximumSize(sizeHint());
  setMinimumSize(sizeHint());
}


QSize PositionDialog::sizeHint() const
{
  return QSize(240,170);
}


QSizePolicy PositionDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int PositionDialog::exec()
{
  pos_screen_number_box->clear();
  for(int i=0;i<pos_desktop_widget->numScreens();i++) {
    pos_screen_number_box->insertItem(QString().sprintf("%d",i));
    if(i==pos_config->screenNumber()) {
      pos_screen_number_box->setCurrentItem(i);
    }
  }
  pos_position_box->setCurrentItem((int)pos_config->position());
  pos_x_offset_spin->setValue(pos_config->xOffset());
  pos_y_offset_spin->setValue(pos_config->yOffset());

  return QDialog::exec();
}


void PositionDialog::okData()
{
  pos_config->setScreenNumber(pos_screen_number_box->currentItem());
  pos_config->
    setPosition((RDMonitorConfig::Position)pos_position_box->currentItem());
  pos_config->setXOffset(pos_x_offset_spin->value());
  pos_config->setYOffset(pos_y_offset_spin->value());

  done(0);
}


void PositionDialog::cancelData()
{
  done(-1);
}


void PositionDialog::resizeEvent(QResizeEvent *e)
{
  pos_screen_number_label->setGeometry(10,10,65,20);
  pos_screen_number_box->setGeometry(80,10,70,20);

  pos_position_label->setGeometry(10,32,65,20);
  pos_position_box->setGeometry(80,32,150,20);

  pos_x_offset_label->setGeometry(10,54,65,20);
  pos_x_offset_spin->setGeometry(80,54,40,20);
  
  pos_y_offset_label->setGeometry(10,76,65,20);
  pos_y_offset_spin->setGeometry(80,76,40,20);
  
  pos_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  pos_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void PositionDialog::closeEvent(QCloseEvent *e)
{
  cancelData();
}
