// rdhpiinfo.cpp
//
// A Qt-based application for display information on ASI cards.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdhpiinfo.cpp,v 1.8.6.4 2014/01/21 21:59:34 cvs Exp $
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

#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include <rdcmd_switch.h>

#include <rdhpiinfo.h>
#include <change_mode.h>

MainWidget::MainWidget(QWidget *parent,const char *name)
  :QWidget(parent,name)
{
  hpi_err_t hpi_err;

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
    printf("HERE\n");
    QMessageBox::warning(this,tr("HPI Error"),
			 tr("The ASI HPI Driver is not loaded!"));
    exit(1);
  }

  //
  // HPI Version
  //
  hpi_err=HPI_SubSysGetVersionEx(NULL,&hpi_version);
  QLabel *label=new QLabel(tr("HPI Version:"),this,"hpi_version_label");
  label->setGeometry(10,10,85,20);
  label->setFont(label_font);
  label=new QLabel(QString().sprintf("%X.%02X.%02X",
				     (unsigned)((hpi_version>>16)&0xffff),
				     (unsigned)((hpi_version>>8)&0xff),
				     (unsigned)hpi_version&0xff),
		   this,"hpi_version");
  label->setGeometry(100,10,100,20);
  label->setFont(font);

  //
  // Adapter Name
  //
  info_name_box=new QComboBox(this,"info_name_box");
  info_name_box->setGeometry(100,34,sizeHint().width()-110,20);
  info_name_box->setFont(font);
  info_name_label=new QLabel(info_name_box,tr("Adapter:"),
			     this,"info_name_label");
  info_name_label->setGeometry(10,34,85,20);
  info_name_label->setFont(label_font);
  info_name_label->setAlignment(AlignRight|AlignVCenter);
  connect(info_name_box,SIGNAL(activated(int)),
	  this,SLOT(nameActivatedData(int)));

  //
  // Adapter Index
  //
  label=new QLabel(tr("Adapter Index:"),this,"index_number_label");
  label->setGeometry(10,58,105,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  info_index_label=new QLabel(this,"info_index_label");
  info_index_label->setGeometry(120,58,100,20);
  info_index_label->setFont(font);
  info_index_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Serial Number
  //
  label=new QLabel(tr("Serial Number:"),this,"serial_number_label");
  label->setGeometry(10,78,105,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  info_serial_label=new QLabel(this,"info_serial_label");
  info_serial_label->setGeometry(120,78,100,20);
  info_serial_label->setFont(font);
  info_serial_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Output Streams
  //
  label=new QLabel(tr("Input Streams:"),this,"input_streams_label");
  label->setGeometry(10,98,105,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  info_istreams_label=new QLabel(this,"info_istreams_label");
  info_istreams_label->setGeometry(120,98,100,20);
  info_istreams_label->setFont(font);
  info_istreams_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Input Streams
  //
  label=new QLabel(tr("Output Streams:"),this,"output_streams_label");
  label->setGeometry(10,118,105,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  info_ostreams_label=new QLabel(this,"info_ostreams_label");
  info_ostreams_label->setGeometry(120,118,100,20);
  info_ostreams_label->setFont(font);
  info_ostreams_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // DSP Version
  //
  label=new QLabel(tr("DSP Version:"),this,"dsp_version_label");
  label->setGeometry(10,138,105,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  info_dsp_label=new QLabel(this,"info_dsp_label");
  info_dsp_label->setGeometry(120,138,100,20);
  info_dsp_label->setFont(font);
  info_dsp_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Adapter Version
  //
  label=new QLabel(tr("Adapter Version:"),this,"adapter_version_label");
  label->setGeometry(10,158,105,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  info_adapter_label=new QLabel(this,"info_adapter_label");
  info_adapter_label->setGeometry(120,158,100,20);
  info_adapter_label->setFont(font);
  info_adapter_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Adapter Mode
  //
  label=new QLabel(tr("Adapter Mode:"),this,"adapter_mode_label");
  label->setGeometry(10,178,105,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  info_mode_label=new QLabel(this,"info_mode_label");
  info_mode_label->setGeometry(120,178,sizeHint().width()-130,20);
  info_mode_label->setFont(font);
  info_mode_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Change Mode Button
  //
  info_changemode_button=
    new QPushButton(tr("Change Card Mode"),this,"info_changemode_button");
  info_changemode_button->setGeometry(130,200,170,30);
  info_changemode_button->setFont(label_font);
  connect(info_changemode_button,SIGNAL(clicked()),
	  this,SLOT(changeModeData()));

  //
  // Close Button
  //
  QPushButton *button=new QPushButton(tr("Close"),this,"close_button");
  button->setGeometry(sizeHint().width()-60,sizeHint().height()-40,50,30);
  button->setFont(label_font);
  connect(button,SIGNAL(clicked()),qApp,SLOT(quit()));

  LoadAdapters();
  if(info_name_box->count()>0) {
    nameActivatedData(0);
  }
}


MainWidget::~MainWidget()
{
  HPI_SubSysFree(NULL);
}


QSize MainWidget::sizeHint() const
{
  return QSize(400,250);
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
    info_mode_label->setText(tr("Standard"));
    info_changemode_button->setDisabled(true);
    break;

  case HPI_ADAPTER_MODE_4OSTREAM:
    info_mode_label->setText(tr("Four Output Streams"));
    info_changemode_button->setEnabled(true);
    break;
	
  case HPI_ADAPTER_MODE_6OSTREAM:
    info_mode_label->setText(tr("Six Output Streams"));
    info_changemode_button->setEnabled(true);
    break;
	
  case HPI_ADAPTER_MODE_8OSTREAM:
    info_mode_label->setText(tr("Eight Output Streams"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_12OSTREAM:
    info_mode_label->setText(tr("Twelve Output Streams"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_16OSTREAM:
    switch(hpi_type[card]) {
    case 0x6585:
      info_mode_label->
	setText(tr("Multichannel Surround (Two Output Streams)"));
      break;
      
    default:
      info_mode_label->setText(tr("Sixteen Output Streams"));
      break;
    }
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_1OSTREAM:
    info_mode_label->setText(tr("One Output Stream"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_1:
    info_mode_label->setText(tr("Mode 1"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_2:
    info_mode_label->setText(tr("Mode 2"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_3:
    info_mode_label->setText(tr("Mode 3"));
    info_changemode_button->setEnabled(true);
    break;

  case HPI_ADAPTER_MODE_MULTICHANNEL:
    info_mode_label->setText(tr("Surround Sound [SSX]"));
    info_changemode_button->setEnabled(true);
    break;
 
  default:
    info_mode_label->setText(tr("N/A"));
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
  ChangeMode *dialog=new ChangeMode(card,hpi_type[card],hpi_mode[card],
				    this,"change_mode_dialog");
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


void MainWidget::LoadAdapters()
{
  int num_adapters;
  hpi_err_t hpi_err;

  hpi_err=HPI_SubSysGetNumAdapters(NULL,&num_adapters);
  for(int i=0;i<num_adapters;i++) {
    hpi_err=HPI_SubSysGetAdapter(NULL,i,hpi_indexes+i,hpi_type+i);
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
    }
  }
}


void MainWidget::HpiErr(hpi_err_t err,const char *func_name) const
{
  char hpi_str[200];

  if(err==HPI_ERROR_INVALID_FUNC) {
    return;
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
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
