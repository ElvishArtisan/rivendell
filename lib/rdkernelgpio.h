// rdkernelgpio.h
//
// Control Class for the Linux SysFS GPIO Interface
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDKERNELGPIO_H
#define RDKERNELGPIO_H

#include <stdio.h>

#include <vector>

#include <qobject.h>
#include <qtimer.h>

//
// See https://www.kernel.org/doc/Documentation/gpio/sysfs.txt
// for an explanation of this interface.
//
#define KERNELGPIO_SYS_FILE QString("/sys/class/gpio")
#define KERNELGPIO_POLL_INTERVAL 20

class RDKernelGpio : public QObject
{
  Q_OBJECT;
 public:
  enum Direction {In=0,Out=1};
  enum Edge {None=0,Rising=1,Falling=2,Both=3};
  RDKernelGpio(QObject *parent=0);
  ~RDKernelGpio();
  bool addGpio(int gpio);
  bool removeGpio(int gpio);
  Direction direction(int gpio, bool *ok=NULL) const;
  bool setDirection(int gpio,Direction dir) const;
  bool activeLow(int gpio, bool *ok=NULL) const;
  bool setActiveLow(int gpio,bool state) const;
  bool value(int gpio,bool *ok=NULL) const;

 public slots:
  bool setValue(int gpio,bool state) const;

 private slots:
  void pollData();

 signals:
  void valueChanged(int gpio,bool state);

 private:
  FILE *OpenNode(const QString &name,const char *mode,int gpio=-1) const;
  std::vector<int> gpio_gpios;
  std::vector<bool> gpio_states;
  QTimer *gpio_poll_timer;
};


#endif  // RDKERNELGPIO_H
