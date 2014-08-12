// edit_ttys.cpp
//
// Edit a Rivendell TTY Configuration
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_ttys.cpp,v 1.16 2010/07/29 19:32:34 cvs Exp $
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

#include <qstring.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

#include <rddb.h>
#include <edit_ttys.h>
#include <rdstation.h>
#include <rdtextvalidator.h>
#include <rdmacro.h>
#include <globals.h>


EditTtys::EditTtys(QString station,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString str;

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
  setCaption(tr("Edit TTYs"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Port Selector
  //
  edit_port_box=new QComboBox(this,"edit_port_box");
  edit_port_box->setGeometry(75,10,100,24);
  edit_port_box->setInsertionPolicy(QComboBox::NoInsertion);
  connect(edit_port_box,SIGNAL(activated(int)),this,SLOT(idSelectedData()));
  QLabel *label=new QLabel(edit_port_box,tr("Port ID:"),
			   this,"edit_port_label");
  label->setGeometry(10,14,60,22);
  label->setFont(font);
  label->setAlignment(AlignRight);

  //
  // Enable Button
  //
  edit_enable_button=new QCheckBox(this,"edit_enable_button");
  edit_enable_button->setGeometry(265,14,15,15);
  connect(edit_enable_button,SIGNAL(stateChanged(int)),
	  this,SLOT(enableButtonData(int)));
  label=new QLabel(edit_enable_button,tr("Enabled"),this,"edit_enable_button");
  label->setGeometry(200,14,60,22);
  label->setFont(font);
  label->setAlignment(AlignRight);

  //
  // The TTY Port
  //
  edit_port_edit=new QLineEdit(this,"edit_port_edit");
  edit_port_edit->setGeometry(145,54,100,20);
  edit_port_edit->setValidator(validator);
  edit_port_label=new QLabel(edit_port_edit,tr("TTY Device:"),this,
			     "edit_port_edit");
  edit_port_label->setGeometry(20,56,120,22);
  edit_port_label->setFont(font);
  edit_port_label->setAlignment(AlignRight);
  
  //
  // Baudrate Selector
  //
  edit_baudrate_box=new QComboBox(this,"edit_baudrate_box");
  edit_baudrate_box->setGeometry(145,80,90,24);
  edit_baudrate_box->setInsertionPolicy(QComboBox::NoInsertion);
  edit_baudrate_label=new QLabel(edit_baudrate_box,tr("Baud Rate:"),this,
			   "edit_baudrate_label");
  edit_baudrate_label->setGeometry(20,84,120,22);
  edit_baudrate_label->setFont(font);
  edit_baudrate_label->setAlignment(AlignRight);

  //
  // Parity Selector
  //
  edit_parity_box=new QComboBox(this,"edit_parity_box");
  edit_parity_box->setGeometry(145,108,90,24); 
  edit_parity_box->setInsertionPolicy(QComboBox::NoInsertion);
  edit_parity_label=new QLabel(edit_parity_box,tr("Parity:"),this,
			   "edit_parity_label");
  edit_parity_label->setGeometry(20,110,120,22);
  edit_parity_label->setFont(font);
  edit_parity_label->setAlignment(AlignRight);

  //
  // Data Bits Selector
  //
  edit_databits_box=new QComboBox(this,"edit_databits_box");
  edit_databits_box->setGeometry(145,136,60,24);
  edit_databits_box->setInsertionPolicy(QComboBox::NoInsertion);
  edit_databits_label=new QLabel(edit_databits_box,tr("Data Bits:"),this,
			   "edit_databits_label");
  edit_databits_label->setGeometry(20,138,120,22);
  edit_databits_label->setFont(font);
  edit_databits_label->setAlignment(AlignRight);

  //
  // Stop Bits Selector
  //
  edit_stopbits_box=new QComboBox(this,"edit_stopbits_box");
  edit_stopbits_box->setGeometry(145,164,60,24);
  edit_stopbits_box->setInsertionPolicy(QComboBox::NoInsertion);
  edit_stopbits_label=new QLabel(edit_stopbits_box,tr("Stop Bits:"),this,
			   "edit_stopbits_label");
  edit_stopbits_label->setGeometry(20,166,120,22);
  edit_stopbits_label->setFont(font);
  edit_stopbits_label->setAlignment(AlignRight);

  //
  // Termination Character Selector
  //
  edit_termination_box=new QComboBox(this,"edit_termination_box");
  edit_termination_box->setGeometry(145,192,90,24);
  edit_termination_box->setInsertionPolicy(QComboBox::NoInsertion);
  edit_termination_label=new QLabel(edit_termination_box,tr("Terminator:"),
				    this,"edit_termination_label");
  edit_termination_label->setGeometry(20,194,120,22);
  edit_termination_label->setFont(font);
  edit_termination_label->setAlignment(AlignRight);

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this,"close_button");
  close_button->setGeometry(210,230,80,50);
  close_button->setFont(font);
  close_button->setText(tr("&Close"));
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Populate Data
  //
  str=QString(tr("Serial"));
  for(int i=0;i<MAX_TTYS;i++) {
    edit_port_box->insertItem(QString().sprintf("%s%d",(const char *)str,i));
  }
  edit_baudrate_box->insertItem("50");
  edit_baudrate_box->insertItem("75");
  edit_baudrate_box->insertItem("110");
  edit_baudrate_box->insertItem("134");
  edit_baudrate_box->insertItem("150");
  edit_baudrate_box->insertItem("200");
  edit_baudrate_box->insertItem("300");
  edit_baudrate_box->insertItem("600");
  edit_baudrate_box->insertItem("1200");
  edit_baudrate_box->insertItem("1800");
  edit_baudrate_box->insertItem("2400");
  edit_baudrate_box->insertItem("4800");
  edit_baudrate_box->insertItem("9600");
  edit_baudrate_box->insertItem("19200");
  edit_baudrate_box->insertItem("38400");
  edit_baudrate_box->insertItem("57600");
  edit_baudrate_box->insertItem("115200");
  edit_baudrate_box->insertItem("230400");

  edit_parity_box->insertItem(tr("None"));
  edit_parity_box->insertItem(tr("Even"));
  edit_parity_box->insertItem(tr("Odd"));

  edit_databits_box->insertItem("5");
  edit_databits_box->insertItem("6");
  edit_databits_box->insertItem("7");
  edit_databits_box->insertItem("8");

  edit_stopbits_box->insertItem("1");
  edit_stopbits_box->insertItem("2");

  edit_termination_box->insertItem(tr("None"));
  edit_termination_box->insertItem(tr("CR"));
  edit_termination_box->insertItem(tr("LF"));
  edit_termination_box->insertItem(tr("CR/LF"));

  edit_id=edit_port_box->currentItem();
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
  edit_id=edit_port_box->currentItem();
  ReadRecord(edit_id);
}


void EditTtys::enableButtonData(int state)
{
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
  macro.setArgQuantity(1);
  for(int i=0;i<MAX_TTYS;i++) {
    if(edit_port_modified[i]) {
      sql=QString().sprintf("select MATRIX from MATRICES where PORT=%d",i);
      q=new RDSqlQuery(sql);
      if(q->first()) {
	macro.setCommand(RDMacro::SZ);
	macro.setArg(0,q->value(0).toInt());
      }
      else {
	macro.setCommand(RDMacro::SY);
	macro.setArg(0,i);
      }
      macro.setAddress(rmt_station->address());
      rdripc->sendRml(&macro);
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
	edit_baudrate_box->setCurrentItem(0);
	break;
      case 75:
	edit_baudrate_box->setCurrentItem(1);
	break;
      case 110:
	edit_baudrate_box->setCurrentItem(2);
	break;
      case 134:
	edit_baudrate_box->setCurrentItem(3);
	break;
      case 150:
	edit_baudrate_box->setCurrentItem(4);
	break;
      case 200:
	edit_baudrate_box->setCurrentItem(5);
	break;
      case 300:
	edit_baudrate_box->setCurrentItem(6);
	break;
      case 600:
	edit_baudrate_box->setCurrentItem(7);
	break;
      case 1200:
	edit_baudrate_box->setCurrentItem(8);
	break;
      case 1800:
	edit_baudrate_box->setCurrentItem(9);
	break;
      case 2400:
	edit_baudrate_box->setCurrentItem(10);
	break;
      case 4800:
	edit_baudrate_box->setCurrentItem(11);
	break;
      case 9600:
	edit_baudrate_box->setCurrentItem(12);
	break;
      case 19200:
	edit_baudrate_box->setCurrentItem(13);
	break;
      case 38400:
	edit_baudrate_box->setCurrentItem(14);
	break;
      case 57600:
	edit_baudrate_box->setCurrentItem(15);
	break;
      case 115200:
	edit_baudrate_box->setCurrentItem(16);
	break;
      case 230400:
	edit_baudrate_box->setCurrentItem(17);
	break;
      default:
	edit_baudrate_box->setCurrentItem(12);
	break;	
  }
  edit_parity_box->setCurrentItem(edit_tty->parity());
  edit_databits_box->setCurrentItem(edit_tty->dataBits()-5);
  edit_stopbits_box->setCurrentItem(edit_tty->stopBits()-1);
  edit_termination_box->setCurrentItem(edit_tty->termination());
  edit_enable_button->setChecked(edit_tty->active());
  SetEnable(edit_tty->active());
}


void EditTtys::WriteRecord(int id)
{
  int baud;

  edit_tty->setActive(edit_enable_button->isChecked());
  edit_tty->setPort(edit_port_edit->text());
  sscanf((const char *)edit_baudrate_box->currentText(),"%d",&baud);
  edit_tty->setBaudRate(baud);
  edit_tty->setParity((RDTTYDevice::Parity)edit_parity_box->currentItem());
  edit_tty->setDataBits(edit_databits_box->currentItem()+5);
  edit_tty->setStopBits(edit_stopbits_box->currentItem()+1);
  edit_tty->
    setTermination((RDTty::Termination)edit_termination_box->currentItem());
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
