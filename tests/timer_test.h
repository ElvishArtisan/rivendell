// timer_test.h
//
// Test QTimer Accuracy
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: timer_test.h,v 1.1.2.1 2013/02/07 18:22:06 cvs Exp $
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

#ifndef TIMER_TEST_H
#define TIMER_TEST_H

#include <sys/time.h>

#include <qobject.h>
#include <qtimer.h>

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private slots:
  void timeoutData();

 private:
  double Time(struct timeval *tv);
  QTimer *test_timer;
  struct timeval test_start_tv;
  struct timeval test_end_tv;
  int test_interval;
};


#endif  // TIMER_TEST_H
