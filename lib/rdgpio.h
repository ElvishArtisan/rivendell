//   rdgpio.h
//
//   A driver for General-Purpose I/O devices.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdgpio.h,v 1.5 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#ifndef RDGPIO_H
#define RDGPIO_H

#include <linux/input.h>

#include <qobject.h>
#include <qtimer.h>
#include <qsignalmapper.h>

#include <gpio.h>

#define GPIO_CLOCK_INTERVAL 100
#define GPIO_MAX_LINES 24

class RDGpio : public QObject
{
 Q_OBJECT
 public:
 enum Mode {Auto=GPIO_MODE_AUTO,Input=GPIO_MODE_INPUT,Output=GPIO_MODE_OUTPUT};
  RDGpio(QObject *parent=0,const char *name=0);
  QString device() const;
  void setDevice(QString dev);
  QString description() const;
  RDGpio::Mode mode();
  void setMode(RDGpio::Mode mode);
  bool open();
  void close();
  int inputs() const;
  int outputs() const;
  unsigned inputMask();
  bool inputState(int line);
  unsigned outputMask() const;

 signals:
  void inputChanged(int line,bool state);
  void outputChanged(int line,bool state);

 public slots:
  void gpoSet(int line,unsigned interval=0);
  void gpoReset(int line,unsigned interval=0);

 private slots:
  void inputTimerData();
  void revertData(int);

 private:
  enum Api {ApiGpio=0,ApiInput=1};
  void RemapTimers();
  void SetReversion(int,unsigned);
  void Clear();
  void InitGpio();
  void InitInput();
  Api gpio_api;
  int gpio_fd;
  QString gpio_device;
  bool gpio_open;
  struct gpio_info gpio_info;
  QTimer *gpio_input_timer;
  unsigned gpio_input_mask;
  unsigned gpio_output_mask;
  QSignalMapper *gpio_revert_mapper;
  QTimer *gpio_revert_timer[GPIO_MAX_LINES];
  int gpio_key_map[KEY_MAX];
  QString gpio_description;
};


#endif  // RDGPIO_H
