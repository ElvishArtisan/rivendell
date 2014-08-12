// edit_channelgpios.cpp
//
// Edit Rivendell Channel GPIO Settings
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_channelgpios.cpp,v 1.1.2.3 2013/03/13 15:18:05 cvs Exp $
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

#include <edit_channelgpios.h>

EditChannelGpios::EditChannelGpios(RDAirPlayConf *conf,
				   RDAirPlayConf::Channel chan,
				   QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  edit_airplay_conf=conf;
  edit_channel=chan;

  setCaption(QString("RDAdmin - ")+tr("Edit Channel GPIOs"));

  //
  // Fonts
  //
  QFont title_font("helvetica",14,QFont::Bold);
  title_font.setPixelSize(14);
  QFont label_font("helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  //
  // Title
  //
  edit_title_label=new QLabel(RDAirPlayConf::channelText(chan),this);
  edit_title_label->setFont(title_font);
  edit_title_label->setAlignment(Qt::AlignCenter);

  //
  // Start GPI
  //
  edit_start_gpi_label=new QLabel(tr("Start GPI:"),this);
  edit_start_gpi_label->setFont(label_font);
  edit_start_gpi_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  edit_start_gpi_matrix_spin=new QSpinBox(this);
  edit_start_gpi_matrix_spin->setRange(-1,MAX_MATRICES);
  edit_start_gpi_matrix_spin->setSpecialValueText(tr("None"));
  connect(edit_start_gpi_matrix_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(startMatrixGpiChangedData(int)));

  edit_start_gpi_line_spin=new QSpinBox(this);
  edit_start_gpi_line_spin->setRange(1,MAX_GPIO_PINS);
  if((chan==RDAirPlayConf::SoundPanel1Channel)||
     (chan==RDAirPlayConf::SoundPanel2Channel)||
     (chan==RDAirPlayConf::SoundPanel3Channel)||
     (chan==RDAirPlayConf::SoundPanel4Channel)||
     (chan==RDAirPlayConf::SoundPanel5Channel)||
     (chan==RDAirPlayConf::SoundPanel2Channel)||
     (chan==RDAirPlayConf::CueChannel)) {
    edit_start_gpi_label->setDisabled(true);
    edit_start_gpi_matrix_spin->setDisabled(true);
    edit_start_gpi_line_spin->setDisabled(true);
  }

  //
  // Start GPO
  //
  edit_start_gpo_label=new QLabel(tr("Start GPO:"),this);
  edit_start_gpo_label->setFont(label_font);
  edit_start_gpo_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  edit_start_gpo_matrix_spin=new QSpinBox(this);
  edit_start_gpo_matrix_spin->setRange(-1,MAX_MATRICES);
  edit_start_gpo_matrix_spin->setSpecialValueText(tr("None"));
  connect(edit_start_gpo_matrix_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(startMatrixGpoChangedData(int)));

  edit_start_gpo_line_spin=new QSpinBox(this);
  edit_start_gpo_line_spin->setRange(1,MAX_GPIO_PINS);

  //
  // Stop GPI
  //
  edit_stop_gpi_label=new QLabel(tr("Stop GPI:"),this);
  edit_stop_gpi_label->setFont(label_font);
  edit_stop_gpi_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  edit_stop_gpi_matrix_spin=new QSpinBox(this);
  edit_stop_gpi_matrix_spin->setRange(-1,MAX_MATRICES);
  edit_stop_gpi_matrix_spin->setSpecialValueText(tr("None"));
  connect(edit_stop_gpi_matrix_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(stopMatrixGpiChangedData(int)));

  edit_stop_gpi_line_spin=new QSpinBox(this);
  edit_stop_gpi_line_spin->setRange(1,MAX_GPIO_PINS);

  //
  // Stop GPO
  //
  edit_stop_gpo_label=new QLabel(tr("Stop GPO:"),this);
  edit_stop_gpo_label->setFont(label_font);
  edit_stop_gpo_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  edit_stop_gpo_matrix_spin=new QSpinBox(this);
  edit_stop_gpo_matrix_spin->setRange(-1,MAX_MATRICES);
  edit_stop_gpo_matrix_spin->setSpecialValueText(tr("None"));
  connect(edit_stop_gpo_matrix_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(stopMatrixGpoChangedData(int)));

  edit_stop_gpo_line_spin=new QSpinBox(this);
  edit_stop_gpo_line_spin->setRange(1,MAX_GPIO_PINS);

  //
  // Signaling Type
  //
  edit_gpio_type_label=new QLabel(tr("Signalling Type:"),this);
  edit_gpio_type_label->setFont(label_font);
  edit_gpio_type_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  edit_gpio_type_box=new QComboBox(this);
  edit_gpio_type_box->insertItem(tr("Edge"));
  edit_gpio_type_box->insertItem(tr("Level"));

  //
  // Buttons
  //
  edit_ok_button=new QPushButton(tr("OK"),this);
  edit_ok_button->setFont(label_font);
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  edit_cancel_button=new QPushButton(tr("Cancel"),this);
  edit_cancel_button->setFont(label_font);
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Values
  //
  edit_start_gpi_matrix_spin->
    setValue(edit_airplay_conf->startGpiMatrix(edit_channel));
  startMatrixGpiChangedData(edit_start_gpi_matrix_spin->value());
  edit_start_gpi_line_spin->
    setValue(edit_airplay_conf->startGpiLine(edit_channel));

  edit_start_gpo_matrix_spin->
    setValue(edit_airplay_conf->startGpoMatrix(edit_channel));
  startMatrixGpoChangedData(edit_start_gpo_matrix_spin->value());
  edit_start_gpo_line_spin->
    setValue(edit_airplay_conf->startGpoLine(edit_channel));

  edit_stop_gpi_matrix_spin->
    setValue(edit_airplay_conf->stopGpiMatrix(edit_channel));
  stopMatrixGpiChangedData(edit_stop_gpi_matrix_spin->value());
  edit_stop_gpi_line_spin->
    setValue(edit_airplay_conf->stopGpiLine(edit_channel));

  edit_stop_gpo_matrix_spin->
    setValue(edit_airplay_conf->stopGpoMatrix(edit_channel));
  stopMatrixGpoChangedData(edit_stop_gpo_matrix_spin->value());
  edit_stop_gpo_line_spin->
    setValue(edit_airplay_conf->stopGpoLine(edit_channel));

  edit_gpio_type_box->setCurrentItem(edit_airplay_conf->gpioType(edit_channel));
}


QSize EditChannelGpios::sizeHint() const
{
  return QSize(300,227);
}


void EditChannelGpios::resizeEvent(QResizeEvent *e)
{
  edit_title_label->setGeometry(10,10,size().width()-20,20);

  edit_start_gpi_label->setGeometry(10,35,120,20);
  edit_start_gpi_matrix_spin->setGeometry(135,35,60,20);
  edit_start_gpi_line_spin->setGeometry(205,35,60,20);

  edit_start_gpo_label->setGeometry(10,57,120,20);
  edit_start_gpo_matrix_spin->setGeometry(135,57,60,20);
  edit_start_gpo_line_spin->setGeometry(205,57,60,20);

  edit_stop_gpi_label->setGeometry(10,79,120,20);
  edit_stop_gpi_matrix_spin->setGeometry(135,79,60,20);
  edit_stop_gpi_line_spin->setGeometry(205,79,60,20);

  edit_stop_gpo_label->setGeometry(10,101,120,20);
  edit_stop_gpo_matrix_spin->setGeometry(135,101,60,20);
  edit_stop_gpo_line_spin->setGeometry(205,101,60,20);

  edit_gpio_type_label->setGeometry(10,128,120,20);
  edit_gpio_type_box->setGeometry(135,128,80,20);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditChannelGpios::startMatrixGpiChangedData(int n)
{
  edit_start_gpi_line_spin->setEnabled(n>=0);
}


void EditChannelGpios::startMatrixGpoChangedData(int n)
{
  edit_start_gpo_line_spin->setEnabled(n>=0);
}


void EditChannelGpios::stopMatrixGpiChangedData(int n)
{
  edit_stop_gpi_line_spin->setEnabled(n>=0);
}


void EditChannelGpios::stopMatrixGpoChangedData(int n)
{
  edit_stop_gpo_line_spin->setEnabled(n>=0);
}


void EditChannelGpios::okData()
{
  edit_airplay_conf->
    setStartGpiMatrix(edit_channel,edit_start_gpi_matrix_spin->value());
  edit_airplay_conf->
    setStartGpiLine(edit_channel,edit_start_gpi_line_spin->value());

  edit_airplay_conf->
    setStartGpoMatrix(edit_channel,edit_start_gpo_matrix_spin->value());
  edit_airplay_conf->
    setStartGpoLine(edit_channel,edit_start_gpo_line_spin->value());

  edit_airplay_conf->
    setStopGpiMatrix(edit_channel,edit_stop_gpi_matrix_spin->value());
  edit_airplay_conf->
    setStopGpiLine(edit_channel,edit_stop_gpi_line_spin->value());

  edit_airplay_conf->
    setStopGpoMatrix(edit_channel,edit_stop_gpo_matrix_spin->value());
  edit_airplay_conf->
    setStopGpoLine(edit_channel,edit_stop_gpo_line_spin->value());

  edit_airplay_conf->setGpioType(edit_channel,
		  (RDAirPlayConf::GpioType)edit_gpio_type_box->currentItem());

  done(0);
}


void EditChannelGpios::cancelData()
{
  done(-1);
}
