// edit_ttys.cpp
//
// Edit a Rivendell TTY Configuration
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

#include <QMessageBox>

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdmatrix.h>
#include <rdstation.h>
#include <rdtextvalidator.h>

#include "edit_ttys.h"
#include "globals.h"

EditTtys::EditTtys(QString station,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  edit_station=station;
  for(int i=0;i<MAX_TTYS;i++) {
    edit_port_modified[i]=false;
  }
  setWindowTitle("RDAdmin- "+tr("Edit Serial Ports"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Port Selector
  //
  edit_port_box=new QComboBox(this);
  edit_port_box->setGeometry(75,10,100,24);
  connect(edit_port_box,SIGNAL(activated(int)),this,SLOT(idSelectedData()));
  QLabel *label=new QLabel(tr("Port ID:"),this);
  label->setGeometry(10,10,60,24);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Enable Button
  //
  edit_enable_button=new QCheckBox(this);
  edit_enable_button->setGeometry(265,14,15,15);
  connect(edit_enable_button,SIGNAL(stateChanged(int)),
	  this,SLOT(enableButtonData(int)));
  label=new QLabel(tr("Enabled"),this);
  label->setGeometry(200,10,60,22);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // The TTY Port
  //
  edit_port_edit=new QLineEdit(this);
  edit_port_edit->setGeometry(145,54,100,20);
  edit_port_edit->setValidator(validator);
  edit_port_label=new QLabel(tr("TTY Device:"),this);
  edit_port_label->setGeometry(20,54,120,20);
  edit_port_label->setFont(labelFont());
  edit_port_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  
  //
  // Baudrate Selector
  //
  edit_baudrate_box=new QComboBox(this);
  edit_baudrate_box->setGeometry(145,80,90,24);
  edit_baudrate_label=new QLabel(tr("Baud Rate:"),this);
  edit_baudrate_label->setGeometry(20,80,120,24);
  edit_baudrate_label->setFont(labelFont());
  edit_baudrate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Parity Selector
  //
  edit_parity_box=new QComboBox(this);
  edit_parity_box->setGeometry(145,108,90,24); 
  edit_parity_label=new QLabel(tr("Parity:"),this);
  edit_parity_label->setGeometry(20,108,120,24);
  edit_parity_label->setFont(labelFont());
  edit_parity_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Data Bits Selector
  //
  edit_databits_box=new QComboBox(this);
  edit_databits_box->setGeometry(145,136,60,24);
  edit_databits_label=new QLabel(tr("Data Bits:"),this);
  edit_databits_label->setGeometry(20,136,120,24);
  edit_databits_label->setFont(labelFont());
  edit_databits_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Stop Bits Selector
  //
  edit_stopbits_box=new QComboBox(this);
  edit_stopbits_box->setGeometry(145,164,60,24);
  edit_stopbits_label=new QLabel(tr("Stop Bits:"),this);
  edit_stopbits_label->setGeometry(20,164,120,24);
  edit_stopbits_label->setFont(labelFont());
  edit_stopbits_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Termination Character Selector
  //
  edit_termination_box=new QComboBox(this);
  edit_termination_box->setGeometry(145,192,90,24);
  edit_termination_label=new QLabel(tr("Terminator:"),this);
  edit_termination_label->setGeometry(20,192,120,24);
  edit_termination_label->setFont(labelFont());
  edit_termination_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this);
  close_button->setGeometry(210,230,80,50);
  close_button->setFont(buttonFont());
  close_button->setText(tr("Close"));
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Populate Data
  //
  for(int i=0;i<MAX_TTYS;i++) {
    edit_port_box->
      insertItem(edit_port_box->count(),tr("Serial")+QString().sprintf("%d",i));
  }
  edit_baudrate_box->insertItem(0,"50");
  edit_baudrate_box->insertItem(1,"75");
  edit_baudrate_box->insertItem(2,"110");
  edit_baudrate_box->insertItem(3,"134");
  edit_baudrate_box->insertItem(4,"150");
  edit_baudrate_box->insertItem(5,"200");
  edit_baudrate_box->insertItem(6,"300");
  edit_baudrate_box->insertItem(7,"600");
  edit_baudrate_box->insertItem(8,"1200");
  edit_baudrate_box->insertItem(9,"1800");
  edit_baudrate_box->insertItem(10,"2400");
  edit_baudrate_box->insertItem(11,"4800");
  edit_baudrate_box->insertItem(12,"9600");
  edit_baudrate_box->insertItem(13,"19200");
  edit_baudrate_box->insertItem(14,"38400");
  edit_baudrate_box->insertItem(15,"57600");
  edit_baudrate_box->insertItem(16,"115200");
  edit_baudrate_box->insertItem(17,"230400");

  edit_parity_box->insertItem(0,tr("None"));
  edit_parity_box->insertItem(1,tr("Even"));
  edit_parity_box->insertItem(2,tr("Odd"));

  edit_databits_box->insertItem(0,"5");
  edit_databits_box->insertItem(1,"6");
  edit_databits_box->insertItem(2,"7");
  edit_databits_box->insertItem(3,"8");

  edit_stopbits_box->insertItem(0,"1");
  edit_stopbits_box->insertItem(1,"2");

  edit_termination_box->insertItem(0,tr("None"));
  edit_termination_box->insertItem(1,tr("CR"));
  edit_termination_box->insertItem(2,tr("LF"));
  edit_termination_box->insertItem(3,tr("CR/LF"));

  edit_id=edit_port_box->currentIndex();
  edit_tty=NULL;
  ReadRecord(edit_id);
}


EditTtys::~EditTtys()
{
   delete edit_port_label;
   delete edit_port_edit;
   delete edit_baudrate_label;
   delete edit_baudrate_box;
   delete edit_databits_label;
   delete edit_databits_box;
   delete edit_stopbits_label;
   delete edit_stopbits_box;
   delete edit_parity_label;
   delete edit_parity_box;
   delete edit_termination_label;
   delete edit_termination_box;
   delete edit_tty;
}


QSize EditTtys::sizeHint() const
{
  return QSize(300,290);
} 


QSizePolicy EditTtys::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditTtys::idSelectedData()
{
  WriteRecord(edit_id);
  edit_id=edit_port_box->currentIndex();
  ReadRecord(edit_id);
}


void EditTtys::enableButtonData(int state)
{
  QString sql;
  RDSqlQuery *q;

  if(!state) {
    sql=QString("select ")+
      "NAME,"+    // 00
      "MATRIX,"+  // 01
      "TYPE "+    // 02
      "from MATRICES where "+
      "STATION_NAME=\""+RDEscapeString(edit_station)+"\" && "+
      QString().sprintf("(PORT_TYPE=%d && PORT=%d) || ",
			RDMatrix::TtyPort,edit_port_box->currentIndex())+
      QString().sprintf("(PORT_TYPE_2=%d && PORT_2=%d)",
			RDMatrix::TtyPort,edit_port_box->currentIndex());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      QMessageBox::information(this,"RDAdmin - "+tr("Error"),
			       tr("This port is currently in use by the following Switcher/GPIO device")+":\n"+
			       "\t"+tr("Matrix")+QString().sprintf(": %d.\n",q->value(1).toInt())+
			       "\t"+tr("Type")+": "+RDMatrix::typeString((RDMatrix::Type)q->value(2).toInt())+"\n"+
			       "\t"+tr("Description")+": "+q->value(0).toString());
      delete q;
      edit_enable_button->setChecked(true);
      return;
    }
    delete q;
  }

  if(state==0) {  // Off
    SetEnable(false);
  }

  if(state==2) {  // On
    SetEnable(true);
  }
}


void EditTtys::closeData()
{
  QString sql;
  RDSqlQuery *q;

  WriteRecord(edit_id);

  RDStation *rmt_station=new RDStation(edit_station);
  QHostAddress addr=rmt_station->address();
  RDMacro macro;
  
  macro.setCommand(RDMacro::SY);
  macro.setRole(RDMacro::Cmd);
  macro.setEchoRequested(false);
  for(int i=0;i<MAX_TTYS;i++) {
    if(edit_port_modified[i]) {
      sql=QString().sprintf("select MATRIX from MATRICES where PORT=%d",i);
      q=new RDSqlQuery(sql);
      if(q->first()) {
	macro.setCommand(RDMacro::SZ);
	macro.addArg(q->value(0).toInt());
      }
      else {
	macro.setCommand(RDMacro::SY);
	macro.addArg(i);
      }
      macro.setAddress(rmt_station->address());
      rda->ripc()->sendRml(&macro);
      delete q;
    }
  }
  delete rmt_station;

  done(0);
}


void EditTtys::ReadRecord(int id)
{
  if(edit_tty!=NULL) {
    delete edit_tty;
  }
  edit_tty=new RDTty(edit_station,id,true);
  edit_port_edit->setText(edit_tty->port());
  switch(edit_tty->baudRate()) {
  case 50:
    edit_baudrate_box->setCurrentIndex(0);
    break;

  case 75:
    edit_baudrate_box->setCurrentIndex(1);
    break;

  case 110:
    edit_baudrate_box->setCurrentIndex(2);
    break;

  case 134:
    edit_baudrate_box->setCurrentIndex(3);
    break;

  case 150:
    edit_baudrate_box->setCurrentIndex(4);
    break;

  case 200:
    edit_baudrate_box->setCurrentIndex(5);
    break;

  case 300:
    edit_baudrate_box->setCurrentIndex(6);
    break;

  case 600:
    edit_baudrate_box->setCurrentIndex(7);
    break;

  case 1200:
    edit_baudrate_box->setCurrentIndex(8);
    break;

  case 1800:
    edit_baudrate_box->setCurrentIndex(9);
    break;

  case 2400:
    edit_baudrate_box->setCurrentIndex(10);
    break;

  case 4800:
    edit_baudrate_box->setCurrentIndex(11);
    break;

  case 9600:
    edit_baudrate_box->setCurrentIndex(12);
    break;

  case 19200:
    edit_baudrate_box->setCurrentIndex(13);
    break;

  case 38400:
    edit_baudrate_box->setCurrentIndex(14);
    break;

  case 57600:
    edit_baudrate_box->setCurrentIndex(15);
    break;

  case 115200:
    edit_baudrate_box->setCurrentIndex(16);
    break;

  case 230400:
    edit_baudrate_box->setCurrentIndex(17);
    break;

  default:
    edit_baudrate_box->setCurrentIndex(12);
    break;	
  }
  edit_parity_box->setCurrentIndex(edit_tty->parity());
  edit_databits_box->setCurrentIndex(edit_tty->dataBits()-5);
  edit_stopbits_box->setCurrentIndex(edit_tty->stopBits()-1);
  edit_termination_box->setCurrentIndex(edit_tty->termination());
  edit_enable_button->setChecked(edit_tty->active());
  SetEnable(edit_tty->active());
}


void EditTtys::WriteRecord(int id)
{
  int baud;

  edit_tty->setActive(edit_enable_button->isChecked());
  edit_tty->setPort(edit_port_edit->text());
  baud=edit_baudrate_box->currentText().toInt();
  edit_tty->setBaudRate(baud);
  edit_tty->setParity((RDTTYDevice::Parity)edit_parity_box->currentIndex());
  edit_tty->setDataBits(edit_databits_box->currentIndex()+5);
  edit_tty->setStopBits(edit_stopbits_box->currentIndex()+1);
  edit_tty->
    setTermination((RDTty::Termination)edit_termination_box->currentIndex());
  edit_port_modified[id]=true;
}


void EditTtys::SetEnable(bool state)
{
   edit_port_label->setEnabled(state);
   edit_port_edit->setEnabled(state);
   edit_baudrate_label->setEnabled(state);
   edit_baudrate_box->setEnabled(state);
   edit_databits_label->setEnabled(state);
   edit_databits_box->setEnabled(state);
   edit_stopbits_label->setEnabled(state);
   edit_stopbits_box->setEnabled(state);
   edit_parity_label->setEnabled(state);
   edit_parity_box->setEnabled(state);
   edit_termination_label->setEnabled(state);
   edit_termination_box->setEnabled(state);
}
