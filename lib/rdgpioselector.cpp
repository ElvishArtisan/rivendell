// rdgpioselector.cpp
//
// GPIO Pin selector widget for Rivendell
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

#include <math.h>

#include "rd.h"
#include "rdgpioselector.h"


RDGpioSelector::RDGpioSelector(QWidget *parent)
  : QWidget(parent)
{
  //
  // Pin
  //
  gpio_pin_box=new QSpinBox(this);
  gpio_pin_box->setGeometry(60,22,50,19);
  gpio_pin_box->setSpecialValueText("None");
  gpio_pin_box->setMinimum(-1);
  gpio_pin_box->setMaximum(MAX_GPIO_PINS-1);
  gpio_pin_box->setValue(-1);
  connect(gpio_pin_box,SIGNAL(valueChanged(int)),this,SLOT(pinData(int)));
  QLabel *gpio_pin_label=new QLabel(tr("Pin:"),this);
  gpio_pin_label->setGeometry(0,24,55,19);
  gpio_pin_label->setAlignment(Qt::AlignRight);
}


RDGpioSelector::~RDGpioSelector()
{
  delete gpio_pin_box;
}


QSize RDGpioSelector::sizeHint() const
{
  return QSize(110,87);
} 


QSizePolicy RDGpioSelector::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDGpioSelector::pin() const
{
  return gpio_pin_box->value();
}


void RDGpioSelector::setPin(int pinno)
{
  gpio_pin_box->setValue(pinno);
}


void RDGpioSelector::pinData(int pinno)
{
  emit pinChanged(pinno);
}
