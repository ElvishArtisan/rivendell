// meterstrip_test.cpp
//
// Test harness for RDWaveWidget
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QMessageBox>

#include <rdapplication.h>

#include "meterstrip_test.h"

MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{
  QString err_msg;
  bool ok=false;

  //
  // Open the database
  //
  rda=new RDApplication("meterstrip_test","meterstrip_test",METERSTRIP_TEST_USAGE,
			this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"meterstrip_test - "+tr("Error"),err_msg);
    exit(RDApplication::ExitNoDb);
  }

  //
  // Get Command Switches
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--input-meter") {
      QStringList f0=rda->cmdSwitch()->value(i).split(":",QString::KeepEmptyParts);
      if(f0.size()<3) {
	fprintf(stderr,"meterstrip_test: invalid --input-meter arguments\n");
	exit(RDApplication::ExitInvalidOption);
      }
      d_types.push_back(MainWidget::Input);
      d_card_numbers.push_back(f0.at(0).toUInt(&ok));
      if(!ok) {
	fprintf(stderr,"meterstrip_test: invalid card value\n");
	exit(RDApplication::ExitInvalidOption);
      }
      d_port_numbers.push_back(f0.at(1).toUInt(&ok));
      if(!ok) {
	fprintf(stderr,"meterstrip_test: invalid port value\n");
	exit(RDApplication::ExitInvalidOption);
      }
      f0.removeFirst();
      f0.removeFirst();
      d_labels.push_back(f0.join(":"));
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--output-meter") {
      QStringList f0=rda->cmdSwitch()->value(i).split(":",QString::KeepEmptyParts);
      if(f0.size()<3) {
	fprintf(stderr,"meterstrip_test: invalid --output-meter arguments\n");
	exit(RDApplication::ExitInvalidOption);
      }
      d_types.push_back(MainWidget::Output);
      d_card_numbers.push_back(f0.at(0).toUInt(&ok));
      if(!ok) {
	fprintf(stderr,"meterstrip_test: invalid card value\n");
	exit(RDApplication::ExitInvalidOption);
      }
      d_port_numbers.push_back(f0.at(1).toUInt(&ok));
      if(!ok) {
	fprintf(stderr,"meterstrip_test: invalid port value\n");
	exit(RDApplication::ExitInvalidOption);
      }
      f0.removeFirst();
      f0.removeFirst();
      d_labels.push_back(f0.join(":"));
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"meterstrip_test: unknown option \"%s\"\"\n",
	      rda->cmdSwitch()->key(i).toUtf8().constData());
      exit(RDApplication::ExitInvalidOption);
    }
  }

  if(d_types.size()==0) {
    fprintf(stderr,"at least one meter must be specified\n");
    exit(RDApplication::ExitInvalidOption);
  }
  
  d_meter_stack=new RDMeterStrip(this);

  for(int i=0;i<d_types.size();i++) {
    switch(d_types.at(i)) {
    case MainWidget::Output:
      d_meter_stack->addOutputMeter(d_card_numbers.at(i),d_port_numbers.at(i),
				    d_labels.at(i));
      break;

    case MainWidget::Input:
      d_meter_stack->
	addInputMeter(d_card_numbers.at(i),d_port_numbers.at(i),d_labels.at(i));
      break;
    }
  }  
  setMinimumSize(d_meter_stack->sizeHint());
  
  //
  // Connect to caed(8)
  //
  connect(rda->cae(),SIGNAL(isConnected(bool)),
	  this,SLOT(caeConnectedData(bool)));
  rda->cae()->connectHost();
}


QSize MainWidget::sizeHint() const
{
  return QSize(d_meter_stack->sizeHint());
}


void MainWidget::caeConnectedData(bool state)
{
  rda->cae()->enableMetering(&d_card_numbers);
  //  resize(d_meter_stack->sizeHint());
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  d_meter_stack->setGeometry(0,0,w,h);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);

  MainWidget *w=new MainWidget();
  w->show();

  return a.exec();
}
