// rdhpiinfo.cpp
//
// A Qt-based application for display information on ASI cards.
//
//   (C) Copyright 2002-2014,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include <rdcmd_switch.h>

#include "rdhpiinfo.h"
#include "change_mode.h"
#include "virtdetails.h"

MainWidget::MainWidget(QWidget *parent)
  :QWidget(parent)
{
  setCaption(tr("RDHPIInfo")+" v"+VERSION);

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Load the command-line arguments
  //
  RDCmdSwitch *cmd=new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdhpiinfo",
				   RDHPIINFO_USAGE);
  delete cmd;

  //
  // Generate Fonts
  //
  QFont font("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);
  QFont label_font("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  //
  // Open HPI
  //
  if(HPI_SubSysCreate()==NULL) {
    QMessageBox::warning(this,tr("HPI Error"),
			 tr("The ASI HPI Driver is not loaded!"));
    exit(1);
  }

  //
  // HPI Version
  //
  HpiErr(HPI_SubSysGetVersionEx(NULL,&hpi_version),"HPI_SubSysGetVersionEx");
  QLabel *label=new QLabel(tr("HPI Version:"),this);
  label->setGeometry(10,10,85,20);
  label->setFont(label_font);
  label=new QLabel(QString().sprintf("%d.%02d.%02d",
				     (unsigned)((hpi_version>>16)&0xffff),
				     (unsigned)((hpi_version>>8)&0xff),
				     (unsigned)hpi_version&0xff),this);
  label->setGeometry(100,10,100,20);
  label->setFont(font);

  //
  // Adapter Name
  //
  info_name_box=new QComboBox(this);
  info_name_box->setGeometry(100,34,sizeHint().width()-110,20);
  info_name_box->setFont(font);
  info_name_label=new QLabel(info_name_box,tr("Adapter:"),this);
  info_name_label->setGeometry(10,34,85,20);
  info_name_label->setFont(label_font);
  info_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(info_name_box,SIGNAL(activated(int)),
	  this,SLOT(nameActivatedData(int)));

  //
  // Adapter Index
  //
  label=new QLabel(tr("Adapter Index:"),this);
  label->setGeometry(10,58,105,20);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  info_index_label=new QLabel(this);
  info_index_label->setGeometry(120,58,100,20);
  info_index_label->setFont(font);
  info_index_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Serial Number
  //
  label=new QLabel(tr("Serial Number:"),this);
  label->setGeometry(10,78,105,20);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  info_serial_label=new QLabel(this);
  info_serial_label->setGeometry(120,78,100,20);
  info_serial_label->setFont(font);
  info_serial_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Output Streams
  //
  label=new QLabel(tr("Input Streams:"),this);
  label->setGeometry(10,98,105,20);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  info_istreams_label=new QLabel(this);
  info_istreams_label->setGeometry(120,98,100,20);
  info_istreams_label->setFont(font);
  info_istreams_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Input Streams
  //
  label=new QLabel(tr("Output Streams:"),this);
  label->setGeometry(10,118,105,20);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  info_ostreams_label=new QLabel(this);
  info_ostreams_label->setGeometry(120,118,100,20);
  info_ostreams_label->setFont(font);
  info_ostreams_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // DSP Version
  //
  label=new QLabel(tr("DSP Version:"),this);
  label->setGeometry(10,138,105,20);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  info_dsp_label=new QLabel(this);
  info_dsp_label->setGeometry(120,138,100,20);
  info_dsp_label->setFont(font);
  info_dsp_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Adapter Version
  //
  label=new QLabel(tr("Adapter Version:"),this);
  label->setGeometry(10,158,105,20);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  info_adapter_label=new QLabel(this);
  info_adapter_label->setGeometry(120,158,100,20);
  info_adapter_label->setFont(font);
  info_adapter_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // DSP Utilization
  //
  info_utilization_label=new QLabel(tr("DSP Utilization")+":",this);
  info_utilization_label->setGeometry(10,183,105,20);
  info_utilization_label->setFont(label_font);
  info_utilization_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  info_utilization_label->setDisabled(true);
  info_utilization_edit=new QLineEdit(this);
  info_utilization_edit->setGeometry(120,183,60,20);
  info_utilization_edit->setFont(font);
  info_utilization_edit->setReadOnly(true);
  info_utilization_edit->setDisabled(true);
  info_utilization_button=new QPushButton(tr("Details"),this);
  info_utilization_button->setGeometry(190,180,70,26);
  info_utilization_button->setFont(font);
  info_utilization_button->setDisabled(true);
  connect(info_utilization_button,SIGNAL(clicked()),
	  this,SLOT(utilizationData()));

  //
  // Adapter Mode
  //
  label=new QLabel(tr("Adapter Mode:"),this);
  label->setGeometry(10,213,105,20);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  info_mode_edit=new QLineEdit(this);
  info_mode_edit->setGeometry(120,213,sizeHint().width()-210,20);
  info_mode_edit->setReadOnly(true);
  info_mode_edit->setFont(font);
  info_mode_edit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  info_changemode_button=
    new QPushButton(tr("Change"),this);
  info_changemode_button->setGeometry(sizeHint().width()-80,210,70,26);
  info_changemode_button->setFont(font);
  connect(info_changemode_button,SIGNAL(clicked()),
	  this,SLOT(changeModeData()));

  //
  // Close Button
  //
  QPushButton *button=new QPushButton(tr("Close"),this);
  button->setGeometry(sizeHint().width()-60,sizeHint().height()-40,50,30);
  button->setFont(label_font);
  connect(button,SIGNAL(clicked()),qApp,SLOT(quit()));

  LoadAdapters();

  hpi_profile_timer=new QTimer(this);
  connect(hpi_profile_timer,SIGNAL(timeout()),
	  this,SLOT(updateDspUtilization()));

  if(info_name_box->count()>0) {
    nameActivatedData(0);
    hpi_profile_timer->start(1000);
  }
}


MainWidget::~MainWidget()
{
  HPI_SubSysFree(NULL);
}


QSize MainWidget::sizeHint() const
{
  return QSize(400,290);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::nameActivatedData(int id)
{
  QString str;
  int card=name_map[id];
  info_index_label->
    setText(QString().sprintf("%u",(unsigned)hpi_indexes[card]+1));
  info_serial_label->
    setText(QString().sprintf("%u",(unsigned)hpi_serial[card]));
  info_istreams_label->
    setText(QString().sprintf("%d",hpi_istreams[card]));
  info_ostreams_label->
    setText(QString().sprintf("%d",hpi_ostreams[card]));
  info_dsp_label->setText(QString().sprintf("%d.%d",
					    hpi_card_version[card]>>13,
					    (hpi_card_version[card]>>7)&63));
  info_adapter_label->
    setText(QString().sprintf("%c%d",
			      ((hpi_card_version[card]>>3)&15)+'A',
			      hpi_card_version[card]&7));
  switch(hpi_mode[card]) {
  case 0:  // No mode support
    info_mode_edit->setText(tr("Standard"));
    info_changemode_button->setDisabled(true);
    break;

  case HPI_ADAPTER_MODE_4OSTREAM:
    info_mode_edit->setText(tr("Four Output Streams"));
    info_changemode_button->setEnabled(true);
    break;
	
  case HPI_ADAPTER_MODE_6OSTREAM:
    info_mode_edit->setText(tr("Six Output Streams"));
    info_changemode_button->setEnabled(true);
    break;
	
  case HPI_ADAPTER_MODE_8OSTREAM:
    info_mode_edit->setText(tr("Eight Output Streams"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_12OSTREAM:
    info_mode_edit->setText(tr("Twelve Output Streams"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_16OSTREAM:
    info_mode_edit->setText(tr("Sixteen Output Streams"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_1OSTREAM:
    info_mode_edit->setText(tr("One Output Stream"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_1:
    info_mode_edit->setText(tr("Mode 1"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_2:
    info_mode_edit->setText(tr("Mode 2"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_3:
    info_mode_edit->setText(tr("Mode 3"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_MULTICHANNEL:
    info_mode_edit->setText(tr("Surround Sound [SSX]"));
    info_changemode_button->setEnabled(true);
    break;
 
  case HPI_ADAPTER_MODE_9OSTREAM:
    info_mode_edit->setText(tr("Nine Output Stream"));
    info_changemode_button->setEnabled(true);
    break;
	    
  case HPI_ADAPTER_MODE_MONO:
    info_mode_edit->setText(tr("Mono Mode"));
    info_changemode_button->setEnabled(true);
    break;
	    
  case HPI_ADAPTER_MODE_LOW_LATENCY:
    info_mode_edit->setText(tr("Low Latency Mode"));
    info_changemode_button->setEnabled(true);
    break;
	    
  default:
    info_mode_edit->setText(tr("N/A"));
    info_changemode_button->setDisabled(true);
    if(hpi_mode[card]!=hpi_serial[card]) {
      str=QString(tr("rdhpiinfo: unknown adapter mode"));
      fprintf(stderr,"%s %d\n",(const char *)str,hpi_mode[card]);
    }
    break;
  }
}


void MainWidget::changeModeData()
{
  int card=name_map[info_name_box->currentItem()];
  int mode;
  QString str;
  hpi_err_t hpi_err;
  char hpi_text[100];
  ChangeMode *dialog=new ChangeMode(card,hpi_type[card],hpi_mode[card],this);
  if((mode=dialog->exec())<0) {
    delete dialog;
    return;
  }
  delete dialog;
  hpi_err=HPI_AdapterOpen(NULL,card);
  if((hpi_err=HPI_AdapterSetMode(NULL,card,mode))==0) {
    QMessageBox::information(this,tr("RdhpiInfo"),
			     tr("The adapter mode has been changed.\nYou must now restart the HPI driver!"));
  }
  else {
    HPI_GetErrorText(hpi_err,hpi_text);
    str=QString(tr("HPI Error"));
    QMessageBox::warning(this,tr("RdhpiInfo"),
			 QString().sprintf("%s %d:\n\"%s\"",(const char *)str,
					   (int)hpi_err,hpi_text));
  }
  hpi_err=HPI_AdapterClose(NULL,card);
}


void MainWidget::utilizationData()
{
  VirtDetails *d=new VirtDetails(hpi_indexes[info_name_box->currentItem()],
				 hpi_profile[info_name_box->currentItem()],
				 hpi_profile_quan[info_name_box->currentItem()],
				 this);
  hpi_profile_timer->stop();
  d->exec();
  hpi_profile_timer->start(1000);
  delete d;
}


void MainWidget::updateDspUtilization()
{
  uint32_t util=0;

  if(HpiErr(HPI_ProfileGetUtilization(NULL,
				      hpi_profile[info_name_box->currentItem()],
				      &util))==0) {
    info_utilization_edit->setText(QString().sprintf("%5.1lf%%",
						     (double)util/100.0));
    info_utilization_label->setEnabled(true);
    info_utilization_edit->setEnabled(true);
    info_utilization_button->setEnabled(true);
  }
  else {
    info_utilization_edit->setText("xx.x");
    info_utilization_label->setDisabled(true);
    info_utilization_edit->setDisabled(true);
    info_utilization_button->setDisabled(true);
  }
}


void MainWidget::LoadAdapters()
{
  int num_adapters;

  HpiErr(HPI_SubSysGetNumAdapters(NULL,&num_adapters),
	 "HPI_SubSysGetNumAdapters");
  for(int i=0;i<num_adapters;i++) {
    HpiErr(HPI_SubSysGetAdapter(NULL,i,hpi_indexes+i,hpi_type+i),
	   "HPI_SubSysGetAdapter");
    hpi_ostreams[i]=0;
    hpi_istreams[i]=0;
    hpi_card_version[i]=0;
    hpi_serial[i]=0;
    hpi_mode[i]=0;
    if(hpi_type[i]!=0) {
      info_name_box->insertItem(QString().sprintf("AudioScience %X [%d]",
						  hpi_type[i],i+1));
      name_map[info_name_box->count()-1]=i;
      HpiErr(HPI_AdapterOpen(NULL,hpi_indexes[i]),"HPI_AdapterOpen");
      HpiErr(HPI_AdapterGetInfo(NULL,hpi_indexes[i],&hpi_ostreams[i],&hpi_istreams[i],
				&hpi_card_version[i],hpi_serial+i,
				&hpi_type[i]),"HPI_AdapterGetInfo");
      HpiErr(HPI_AdapterGetMode(NULL,hpi_indexes[i],&hpi_mode[i]),
	     "HPI_AdapterGetMode");
      HpiErr(HPI_AdapterClose(NULL,hpi_indexes[i]),"HPI_AdapterClose");
      HpiErr(HPI_ProfileOpenAll(NULL,hpi_indexes[i],0,&hpi_profile[i],
				&hpi_profile_quan[i]));
    }
  }
}


hpi_err_t MainWidget::HpiErr(hpi_err_t err,const char *func_name) const
{
  char hpi_str[200];

  if(err==HPI_ERROR_INVALID_FUNC) {
    return err;
  }
  if(err!=0) {
    HPI_GetErrorText(err,hpi_str);
    if(func_name==NULL) {
      fprintf(stderr,"rdhpiinfo: %s\n",hpi_str);
    }
    else {
      fprintf(stderr,"rdhpiinfo[%s]: %s\n",func_name,hpi_str);
    }
  }
  return err;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget();
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
