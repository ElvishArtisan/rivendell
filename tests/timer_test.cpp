// timer_test.cpp
//
// Test QTimer Accuracy
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: timer_test.cpp,v 1.1.2.1 2013/02/07 18:22:06 cvs Exp $
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

#include <errno.h>

#include <qapplication.h>

#include <timer_test.h>

MainObject::MainObject(QObject *parent,const char *name)
  : QObject(parent,name)
{
  //
  // Timer
  //
  test_timer=new QTimer(this);
  connect(test_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));

  //
  // Start Run
  //
  test_interval=60000;
  if(gettimeofday(&test_start_tv,NULL) !=0) {
    perror("gettimeofday");
  }
  test_timer->start(test_interval,true);
}


void MainObject::timeoutData()
{
  //
  // Calculate Result
  //
  if(gettimeofday(&test_end_tv,NULL) !=0) {
    perror("gettimeofday");
  }
  double expected=(double)test_interval/1000.0;
  double got=Time(&test_end_tv)-Time(&test_start_tv);

  printf("Expected: %12.3lf  Got: %12.3lf  Diff: %8.3lf%%\n",
	 expected,got,100.0*got/expected);

  //
  // Start Next Run
  //
  test_interval*=2;
  if(gettimeofday(&test_start_tv,NULL) !=0) {
    perror("gettimeofday");
  }
  test_timer->start(test_interval,true);
}


double MainObject::Time(struct timeval *tv)
{
  return (double)tv->tv_sec+(double)tv->tv_usec/1000000.0;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
