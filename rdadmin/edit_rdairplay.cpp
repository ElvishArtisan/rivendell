// edit_rdairplay.cpp
//
// Edit an RDAirPlay Configuration
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <q3buttongroup.h>
#include <q3listbox.h>
#include <q3textedit.h>

#include <qcheckbox.h>
#include <qdialog.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qstring.h>

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdlist_logs.h>
#include <rdtextvalidator.h>

#include "edit_rdairplay.h"
#include "edit_hotkeys.h"
#include "edit_channelgpios.h"
#include "globals.h"

EditRDAirPlay::EditRDAirPlay(RDStation *station,RDStation *cae_station,
			     QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  QString sql;
  RDSqlQuery *q;

  air_exitpasswd_changed=false;
  air_logmachine=0;
  air_virtual_logmachine=0;
  air_virtual_logstartmachine=0;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  air_conf=new RDAirPlayConf(station->name(),"RDAIRPLAY");

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Dialog Name
  //
  setWindowTitle("RDAdmin - "+tr("Configure RDAirPlay"));

  //
  // Channel Assignments Section
  //
  QLabel *label=new QLabel(tr("Channel Assignments"),this);
  label->setFont(sectionLabelFont());
  label->setGeometry(10,10,200,16);

  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(editGpiosData(int)));

  //
  // Main Log Output 1
  //
  label=new QLabel(tr("Main Log Output 1"),this);
  label->setFont(labelFont());
  label->setGeometry(25,32,200,16);
  air_card_sel[0]=new RDCardSelector(this);
  air_card_sel[0]->setId(0);
  air_card_sel[0]->setGeometry(20,50,120,117);
  air_start_rml_edit[0]=new QLineEdit(this);
  air_start_rml_edit[0]->setGeometry(210,50,95,19);
  air_start_rml_edit[0]->setValidator(validator);
  air_start_rml_label[0]=
    new QLabel(air_start_rml_edit[0],tr("Start RML:"),this);
  air_start_rml_label[0]->setFont(subLabelFont());
  air_start_rml_label[0]->setGeometry(140,50,65,19);
  air_start_rml_label[0]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  air_stop_rml_edit[0]=new QLineEdit(this);
  air_stop_rml_edit[0]->setGeometry(210,71,95,19);
  air_stop_rml_edit[0]->setValidator(validator);
  air_stop_rml_label[0]=new QLabel(air_start_rml_edit[0],tr("Stop RML:"),this);
  air_stop_rml_label[0]->setFont(subLabelFont());
  air_stop_rml_label[0]->setGeometry(140,71,65,19);
  air_stop_rml_label[0]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  air_channel_button[0]=new QPushButton(tr("Edit\nGPIOs"),this);
  air_channel_button[0]->setGeometry(310,46,60,50);
  air_channel_button[0]->setFont(buttonFont());
  mapper->setMapping(air_channel_button[0],RDAirPlayConf::MainLog1Channel);
  connect(air_channel_button[0],SIGNAL(clicked()),mapper,SLOT(map()));
  connect(air_card_sel[0],SIGNAL(settingsChanged(int,int,int)),
	  this,SLOT(audioSettingsChangedData(int,int,int)));

  //
  // Main Log Output 2
  //
  label=new QLabel(tr("Main Log Output 2"),this);
  label->setFont(labelFont());
  label->setGeometry(25,100,200,16);
  air_card_sel[1]=new RDCardSelector(this);
  air_card_sel[1]->setId(1);
  air_card_sel[1]->setGeometry(20,118,120,117);
  air_start_rml_edit[1]=new QLineEdit(this);
  air_start_rml_edit[1]->setGeometry(210,118,95,19);
  air_start_rml_edit[1]->setValidator(validator);
  air_start_rml_label[1]=
    new QLabel(air_start_rml_edit[1],tr("Start RML:"),this);
  air_start_rml_label[1]->setFont(subLabelFont());
  air_start_rml_label[1]->setGeometry(140,118,65,19);
  air_start_rml_label[1]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  air_stop_rml_edit[1]=new QLineEdit(this);
  air_stop_rml_edit[1]->setGeometry(210,139,95,19);
  air_stop_rml_edit[1]->setValidator(validator);
  air_stop_rml_label[1]=new QLabel(air_start_rml_edit[1],tr("Stop RML:"),this);
  air_stop_rml_label[1]->setFont(subLabelFont());
  air_stop_rml_label[1]->setGeometry(140,139,65,19);
  air_stop_rml_label[1]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  air_channel_button[1]=new QPushButton(tr("Edit\nGPIOs"),this);
  air_channel_button[1]->setGeometry(310,114,60,50);
  air_channel_button[1]->setFont(buttonFont());
  mapper->setMapping(air_channel_button[1],RDAirPlayConf::MainLog2Channel);
  connect(air_channel_button[1],SIGNAL(clicked()),mapper,SLOT(map()));
  connect(air_card_sel[1],SIGNAL(settingsChanged(int,int,int)),
	  this,SLOT(audioSettingsChangedData(int,int,int)));

  //
  // Aux Log 1 Output
  //
  label=new QLabel(tr("Aux Log 1 Output"),this);
  label->setFont(labelFont());
  label->setGeometry(25,168,200,16);
  air_card_sel[4]=new RDCardSelector(this);
  air_card_sel[4]->setId(4);
  air_card_sel[4]->setGeometry(20,186,120,117);
  air_start_rml_edit[4]=new QLineEdit(this);
  air_start_rml_edit[4]->setGeometry(210,186,95,19);
  air_start_rml_edit[4]->setValidator(validator);
  air_start_rml_label[4]=
    new QLabel(air_start_rml_edit[4],tr("Start RML:"),this);
  air_start_rml_label[4]->setFont(subLabelFont());
  air_start_rml_label[4]->setGeometry(140,186,65,19);
  air_start_rml_label[4]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  air_stop_rml_edit[4]=new QLineEdit(this);
  air_stop_rml_edit[4]->setGeometry(210,207,95,19);
  air_stop_rml_edit[4]->setValidator(validator);
  air_stop_rml_label[4]=new QLabel(air_start_rml_edit[4],tr("Stop RML:"),this);
  air_stop_rml_label[4]->setFont(subLabelFont());
  air_stop_rml_label[4]->setGeometry(140,207,65,19);
  air_stop_rml_label[4]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  air_channel_button[4]=new QPushButton(tr("Edit\nGPIOs"),this);
  air_channel_button[4]->setGeometry(310,182,60,50);
  air_channel_button[4]->setFont(buttonFont());
  mapper->setMapping(air_channel_button[4],RDAirPlayConf::AuxLog1Channel);
  connect(air_channel_button[4],SIGNAL(clicked()),mapper,SLOT(map()));
  connect(air_card_sel[4],SIGNAL(settingsChanged(int,int,int)),
	  this,SLOT(audioSettingsChangedData(int,int,int)));

  //
  // Aux Log 2 Output
  //
  label=new QLabel(tr("Aux Log 2 Output"),this);
  label->setFont(labelFont());
  label->setGeometry(25,236,200,16);
  air_card_sel[5]=new RDCardSelector(this);
  air_card_sel[5]->setId(5);
  air_card_sel[5]->setGeometry(20,254,120,117);
  air_start_rml_edit[5]=new QLineEdit(this);
  air_start_rml_edit[5]->setGeometry(210,254,95,19);
  air_start_rml_edit[5]->setValidator(validator);
  air_start_rml_label[5]=
    new QLabel(air_start_rml_edit[5],tr("Start RML:"),this);
  air_start_rml_label[5]->setFont(subLabelFont());
  air_start_rml_label[5]->setGeometry(140,254,65,19);
  air_start_rml_label[5]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  air_stop_rml_edit[5]=new QLineEdit(this);
  air_stop_rml_edit[5]->setGeometry(210,275,95,19);
  air_stop_rml_edit[5]->setValidator(validator);
  air_stop_rml_label[5]=new QLabel(air_start_rml_edit[5],tr("Stop RML:"),this);
  air_stop_rml_label[5]->setFont(subLabelFont());
  air_stop_rml_label[5]->setGeometry(140,275,65,19);
  air_stop_rml_label[5]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  air_channel_button[5]=new QPushButton(tr("Edit\nGPIOs"),this);
  air_channel_button[5]->setGeometry(310,250,60,50);
  air_channel_button[5]->setFont(buttonFont());
  mapper->setMapping(air_channel_button[5],RDAirPlayConf::AuxLog2Channel);
  connect(air_channel_button[5],SIGNAL(clicked()),mapper,SLOT(map()));
  connect(air_card_sel[5],SIGNAL(settingsChanged(int,int,int)),
	  this,SLOT(audioSettingsChangedData(int,int,int)));

  //
  // Virtual Log Output
  //
  label=new QLabel(tr("Virtual Log Outputs"),this);
  label->setFont(labelFont());
  label->setGeometry(25,309,190,20);
  air_virtual_machine_box=new QComboBox(this);
  air_virtual_machine_box->setGeometry(270,309,100,20);
  for(int i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
    air_virtual_machine_box->
      insertItem(QString().sprintf("vLog %d",i+RD_RDVAIRPLAY_LOG_BASE+1));
  }
  connect(air_virtual_machine_box,SIGNAL(activated(int)),
	  this,SLOT(virtualLogActivatedData(int)));
  label=new QLabel(air_virtual_machine_box,tr("Log Machine")+":",this);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  label->setGeometry(165,309,100,20);
  air_virtual_card_sel=new RDCardSelector(this);
  air_virtual_card_sel->setGeometry(20,332,120,117);
  air_virtual_start_rml_edit=new QLineEdit(this);
  air_virtual_start_rml_edit->setGeometry(210,332,95,19);
  air_virtual_start_rml_edit->setValidator(validator);
  air_virtual_start_rml_label=
    new QLabel(air_virtual_start_rml_edit,tr("Start RML:"),this);
  air_virtual_start_rml_label->setFont(subLabelFont());
  air_virtual_start_rml_label->setGeometry(140,332,65,19);
  air_virtual_start_rml_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  air_virtual_stop_rml_edit=new QLineEdit(this);
  air_virtual_stop_rml_edit->setGeometry(210,353,95,19);
  air_virtual_stop_rml_edit->setValidator(validator);
  air_virtual_stop_rml_label=
    new QLabel(air_virtual_start_rml_edit,tr("Stop RML:"),this);
  air_virtual_stop_rml_label->setFont(subLabelFont());
  air_virtual_stop_rml_label->setGeometry(140,353,65,19);
  air_virtual_stop_rml_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  // **** INACTIVE INACTIVE INACTIVE ****
  //
  // Audition/Cue Output
  //
  air_card_sel[3]=new RDCardSelector(this);
  air_card_sel[3]->setId(3);
  air_card_sel[3]->setGeometry(20,322,120,117);
  air_start_rml_edit[3]=new QLineEdit(this);
  air_start_rml_edit[3]->setGeometry(210,322,160,19);
  air_start_rml_edit[3]->setValidator(validator);
  air_start_rml_label[3]=new QLabel(air_start_rml_edit[3],tr("Start RML:"),this);
  air_start_rml_label[3]->setFont(subLabelFont());
  air_start_rml_label[3]->setGeometry(140,322,65,19);
  air_start_rml_label[3]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  air_stop_rml_edit[3]=new QLineEdit(this);
  air_stop_rml_edit[3]->setGeometry(210,343,160,19);
  air_stop_rml_edit[3]->setValidator(validator);
  air_stop_rml_label[3]=new QLabel(air_start_rml_edit[3],tr("Stop RML:"),this);
  air_stop_rml_label[3]->setFont(subLabelFont());
  air_stop_rml_label[3]->setGeometry(140,343,65,19);
  air_stop_rml_label[3]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  air_channel_button[3]=NULL;
  connect(air_card_sel[3],SIGNAL(settingsChanged(int,int,int)),
	  this,SLOT(audioSettingsChangedData(int,int,int)));
  air_card_sel[3]->hide();
  air_start_rml_edit[3]->hide();
  air_start_rml_label[3]->hide();
  air_stop_rml_edit[3]->hide();
  air_stop_rml_label[3]->hide();
  // **** INACTIVE INACTIVE INACTIVE ****

  //
  // HotKeys Configuration Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(10,385,180,50);
  button->setFont(buttonFont());
  button->setText(tr("Configure Hot Keys"));
  connect(button,SIGNAL(clicked()),this,SLOT(editHotKeys()));

  //
  // Sound Panel First Play Output
  //
  label=new QLabel(tr("SoundPanel First Play Output"),this);
  label->setFont(labelFont());
  label->setGeometry(395,32,300,16);
  air_card_sel[2]=new RDCardSelector(this);
  air_card_sel[2]->setId(2);
  air_card_sel[2]->setGeometry(390,50,120,117);
  air_start_rml_edit[2]=new QLineEdit(this);
  air_start_rml_edit[2]->setGeometry(580,50,95,19);
  air_start_rml_edit[2]->setValidator(validator);
  air_start_rml_label[2]=new QLabel(air_start_rml_edit[2],tr("Start RML:"),this);
  air_start_rml_label[2]->setFont(subLabelFont());
  air_start_rml_label[2]->setGeometry(510,50,65,19);
  air_start_rml_label[2]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  air_stop_rml_edit[2]=new QLineEdit(this);
  air_stop_rml_edit[2]->setGeometry(580,71,95,19);
  air_stop_rml_edit[2]->setValidator(validator);
  air_stop_rml_label[2]=new QLabel(air_start_rml_edit[2],tr("Stop RML:"),this);
  air_stop_rml_label[2]->setFont(subLabelFont());
  air_stop_rml_label[2]->setGeometry(510,71,65,19);
  air_stop_rml_label[2]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  air_channel_button[2]=new QPushButton(tr("Edit\nGPIOs"),this);
  air_channel_button[2]->setGeometry(680,46,60,50);
  air_channel_button[2]->setFont(buttonFont());
  mapper->setMapping(air_channel_button[2],RDAirPlayConf::SoundPanel1Channel);
  connect(air_channel_button[2],SIGNAL(clicked()),mapper,SLOT(map()));
  connect(air_card_sel[2],SIGNAL(settingsChanged(int,int,int)),
	  this,SLOT(audioSettingsChangedData(int,int,int)));

  //
  // Sound Panel Second Play Output
  //
  label=new QLabel(tr("SoundPanel Second Play Output"),this);
  label->setFont(labelFont());
  label->setGeometry(395,100,300,16);
  air_card_sel[6]=new RDCardSelector(this);
  air_card_sel[6]->setId(6);
  air_card_sel[6]->setGeometry(390,118,120,117);
  air_start_rml_edit[6]=new QLineEdit(this);
  air_start_rml_edit[6]->setGeometry(580,118,95,19);
  air_start_rml_edit[6]->setValidator(validator);
  air_start_rml_label[6]=
    new QLabel(air_start_rml_edit[6],tr("Start RML:"),this);
  air_start_rml_label[6]->setFont(subLabelFont());
  air_start_rml_label[6]->setGeometry(510,118,65,19);
  air_start_rml_label[6]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  air_stop_rml_edit[6]=new QLineEdit(this);
  air_stop_rml_edit[6]->setGeometry(580,139,95,19);
  air_stop_rml_edit[6]->setValidator(validator);
  air_stop_rml_label[6]=new QLabel(air_start_rml_edit[6],tr("Stop RML:"),this);
  air_stop_rml_label[6]->setFont(subLabelFont());
  air_stop_rml_label[6]->setGeometry(510,139,65,19);
  air_stop_rml_label[6]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  air_channel_button[6]=new QPushButton(tr("Edit\nGPIOs"),this);
  air_channel_button[6]->setGeometry(680,114,60,50);
  air_channel_button[6]->setFont(buttonFont());
  mapper->setMapping(air_channel_button[6],RDAirPlayConf::SoundPanel2Channel);
  connect(air_channel_button[6],SIGNAL(clicked()),mapper,SLOT(map()));
  connect(air_card_sel[6],SIGNAL(settingsChanged(int,int,int)),
	  this,SLOT(audioSettingsChangedData(int,int,int)));

  //
  // Sound Panel Third Play Output
  //
  label=new QLabel(tr("SoundPanel Third Play Output"),this);
  label->setFont(labelFont());
  label->setGeometry(395,168,300,16);
  air_card_sel[7]=new RDCardSelector(this);
  air_card_sel[7]->setId(7);
  air_card_sel[7]->setGeometry(390,186,120,117);
  air_start_rml_edit[7]=new QLineEdit(this);
  air_start_rml_edit[7]->setGeometry(580,186,95,19);
  air_start_rml_edit[7]->setValidator(validator);
  air_start_rml_label[7]=
    new QLabel(air_start_rml_edit[7],tr("Start RML:"),this);
  air_start_rml_label[7]->setFont(subLabelFont());
  air_start_rml_label[7]->setGeometry(510,186,65,19);
  air_start_rml_label[7]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  air_stop_rml_edit[7]=new QLineEdit(this);
  air_stop_rml_edit[7]->setGeometry(580,207,95,19);
  air_stop_rml_edit[7]->setValidator(validator);
  air_stop_rml_label[7]=new QLabel(air_start_rml_edit[7],tr("Stop RML:"),this);
  air_stop_rml_label[7]->setFont(subLabelFont());
  air_stop_rml_label[7]->setGeometry(510,207,65,19);
  air_stop_rml_label[7]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  air_channel_button[7]=new QPushButton(tr("Edit\nGPIOs"),this);
  air_channel_button[7]->setGeometry(680,182,60,50);
  air_channel_button[7]->setFont(buttonFont());
  mapper->setMapping(air_channel_button[7],RDAirPlayConf::SoundPanel3Channel);
  connect(air_channel_button[7],SIGNAL(clicked()),mapper,SLOT(map()));
  connect(air_card_sel[7],SIGNAL(settingsChanged(int,int,int)),
	  this,SLOT(audioSettingsChangedData(int,int,int)));

  //
  // Sound Panel Fourth Play Output
  //
  label=new QLabel(tr("SoundPanel Fourth Play Output"),this);
  label->setFont(labelFont());
  label->setGeometry(395,236,300,16);
  air_card_sel[8]=new RDCardSelector(this);
  air_card_sel[8]->setId(8);
  air_card_sel[8]->setGeometry(390,254,120,117);
  air_start_rml_edit[8]=new QLineEdit(this);
  air_start_rml_edit[8]->setGeometry(580,254,95,19);
  air_start_rml_edit[8]->setValidator(validator);
  air_start_rml_label[8]=
    new QLabel(air_start_rml_edit[8],tr("Start RML:"),this);
  air_start_rml_label[8]->setFont(subLabelFont());
  air_start_rml_label[8]->setGeometry(510,254,65,19);
  air_start_rml_label[8]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  air_stop_rml_edit[8]=new QLineEdit(this);
  air_stop_rml_edit[8]->setGeometry(580,275,95,19);
  air_stop_rml_edit[8]->setValidator(validator);
  air_stop_rml_label[8]=new QLabel(air_start_rml_edit[8],tr("Stop RML:"),this);
  air_stop_rml_label[8]->setFont(subLabelFont());
  air_stop_rml_label[8]->setGeometry(510,275,65,19);
  air_stop_rml_label[8]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  air_channel_button[8]=new QPushButton(tr("Edit\nGPIOs"),this);
  air_channel_button[8]->setGeometry(680,250,60,50);
  air_channel_button[8]->setFont(buttonFont());
  mapper->setMapping(air_channel_button[8],RDAirPlayConf::SoundPanel4Channel);
  connect(air_channel_button[8],SIGNAL(clicked()),mapper,SLOT(map()));
  connect(air_card_sel[8],SIGNAL(settingsChanged(int,int,int)),
	  this,SLOT(audioSettingsChangedData(int,int,int)));

  //
  // Sound Panel Fifth Play Output
  //
  label=new QLabel(tr("SoundPanel Fifth and Later Play Output"),this);
  label->setFont(labelFont());
  label->setGeometry(395,304,300,16);
  air_card_sel[9]=new RDCardSelector(this);
  air_card_sel[9]->setId(9);
  air_card_sel[9]->setGeometry(390,322,120,117);
  air_start_rml_edit[9]=new QLineEdit(this);
  air_start_rml_edit[9]->setGeometry(580,322,95,19);
  air_start_rml_edit[9]->setValidator(validator);
  air_start_rml_label[9]=
    new QLabel(air_start_rml_edit[9],tr("Start RML:"),this);
  air_start_rml_label[9]->setFont(subLabelFont());
  air_start_rml_label[9]->setGeometry(510,322,65,19);
  air_start_rml_label[9]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  air_stop_rml_edit[9]=new QLineEdit(this);
  air_stop_rml_edit[9]->setGeometry(580,343,95,19);
  air_stop_rml_edit[9]->setValidator(validator);
  air_stop_rml_label[9]=new QLabel(air_start_rml_edit[9],tr("Stop RML:"),this);
  air_stop_rml_label[9]->setFont(subLabelFont());
  air_stop_rml_label[9]->setGeometry(510,343,65,19);
  air_stop_rml_label[9]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  air_channel_button[9]=new QPushButton(tr("Edit\nGPIOs"),this);
  air_channel_button[9]->setGeometry(680,318,60,50);
  air_channel_button[9]->setFont(buttonFont());
  mapper->setMapping(air_channel_button[9],RDAirPlayConf::SoundPanel5Channel);
  connect(air_channel_button[9],SIGNAL(clicked()),mapper,SLOT(map()));
  connect(air_card_sel[9],SIGNAL(settingsChanged(int,int,int)),
	  this,SLOT(audioSettingsChangedData(int,int,int)));

  //
  // Main Log Play Section
  //
  label=new QLabel("Log Settings",this);
  label->setFont(sectionLabelFont());
  label->setGeometry(805,10,200,16);

  //
  // Segue Length
  //
  air_segue_edit=new QLineEdit(this);
  air_segue_edit->setGeometry(895,32,50,20);
  air_segue_label=new QLabel(air_segue_edit,tr("Manual Segue:"),this);
  air_segue_label->setFont(subLabelFont());
  air_segue_label->setGeometry(790,32,100,20);
  air_segue_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  air_segue_unit=new QLabel(air_segue_edit,tr("msecs"),this);
  air_segue_unit->setFont(subLabelFont());
  air_segue_unit->setGeometry(950,32,40,20);
  air_segue_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  
  //
  // Forced Transition Length
  //
  air_trans_edit=new QLineEdit(this);
  air_trans_edit->setGeometry(895,54,50,20);
  air_trans_label=new QLabel(air_trans_edit,tr("Forced Segue:"),this);
  air_trans_label->setFont(subLabelFont());
  air_trans_label->setGeometry(790,54,100,20);
  air_trans_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  air_trans_unit=new QLabel(air_trans_edit,tr("msecs"),this);
  air_trans_unit->setFont(subLabelFont());
  air_trans_unit->setGeometry(950,54,40,20);
  air_trans_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  
  //
  // Pie Countdown Length
  //
  air_piecount_box=new QSpinBox(this);
  air_piecount_box->setGeometry(895,76,50,20);
  air_piecount_box->setRange(0,60);
  air_piecount_label=new QLabel(air_piecount_box,tr("Pie Counts Last:"),this);
  air_piecount_label->setFont(subLabelFont());
  air_piecount_label->setGeometry(785,76,105,20);
  air_piecount_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  air_piecount_unit=new QLabel(tr("secs"),this);
  air_piecount_unit->setFont(subLabelFont());
  air_piecount_unit->setGeometry(950,76,40,20);
  air_piecount_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Pie Countdown To
  //
  air_countto_box=new QComboBox(this);
  air_countto_box->setGeometry(895,98,100,20);
  air_countto_label=new QLabel(air_countto_box,tr("Pie Counts To:"),this);
  air_countto_label->setFont(subLabelFont());
  air_countto_label->setGeometry(785,98,105,20);
  air_countto_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  air_countto_box->insertItem(tr("Cart End"));
  air_countto_box->insertItem(tr("Transition"));

  //
  // Default Transition Type
  //
  air_default_transtype_box=new QComboBox(this);
  air_default_transtype_box->setGeometry(895,120,100,20);
  label=new QLabel(air_default_transtype_box,tr("Default Trans. Type:"),this);
  label->setFont(subLabelFont());
  label->setGeometry(760,120,130,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  air_default_transtype_box->insertItem(tr("Play"));
  air_default_transtype_box->insertItem(tr("Segue"));
  air_default_transtype_box->insertItem(tr("Stop"));

  //
  // Default Service
  //
  air_defaultsvc_box=new QComboBox(this);
  air_defaultsvc_box->setGeometry(895,142,100,20);
  label=new QLabel(air_defaultsvc_box,tr("Default Service:"),this);
  label->setFont(subLabelFont());
  label->setGeometry(760,142,130,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Sound Panel Section
  //
  label=new QLabel(tr("Sound Panel Settings"),this);
  label->setFont(sectionLabelFont());
  label->setGeometry(805,179,200,16);

  //
  // # of Station Panels
  //
  air_station_box=new QSpinBox(this);
  air_station_box->setGeometry(895,204,50,20);
  air_station_box->setRange(0,MAX_PANELS);
  air_station_box->setSpecialValueText(tr("None"));
  air_station_label=new QLabel(air_station_box,tr("System Panels:"),this);
  air_station_label->setFont(subLabelFont());
  air_station_label->setGeometry(750,204,140,20);
  air_station_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // # of User Panels
  //
  air_user_box=new QSpinBox(this);
  air_user_box->setGeometry(895,226,50,20);
  air_user_box->setRange(0,MAX_PANELS);
  air_user_box->setSpecialValueText(tr("None"));
  air_user_label=new QLabel(air_user_box,tr("User Panels:"),this);
  air_user_label->setFont(subLabelFont());
  air_user_label->setGeometry(750,226,140,20);
  air_user_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Flash Active Button
  //
  air_flash_box=new QCheckBox(this);
  air_flash_box->setGeometry(810,254,15,15);
  label=new QLabel(air_flash_box,tr("Flash Active Buttons"),this);
  label->setFont(subLabelFont());
  label->setGeometry(830,254,150,15);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Pause Panel Button
  //
  air_panel_pause_box=new QCheckBox(this);
  air_panel_pause_box->setGeometry(810,276,15,15);
  label=new QLabel(air_panel_pause_box,tr("Enable Button Pausing"),this);
  label->setFont(subLabelFont());
  label->setGeometry(830,276,150,15);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Label Template
  //
  air_label_template_edit=new QLineEdit(this);
  air_label_template_edit->setGeometry(895,298,sizeHint().width()-910,20);
  label=new QLabel(air_label_template_edit,tr("Label Template:"),this);
  label->setFont(subLabelFont());
  label->setGeometry(790,298,100,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Miscellaneous Section
  //
  label=new QLabel(tr("Miscellaneous Settings"),this);
  label->setFont(sectionLabelFont());
  label->setGeometry(805,330,200,16);

  //
  // Check Timesync
  //
  air_timesync_box=new QCheckBox(this);
  air_timesync_box->setGeometry(810,356,15,15);
  air_timesync_label=new QLabel(air_timesync_box,tr("Check TimeSync"),this);
  air_timesync_label->setFont(subLabelFont());
  air_timesync_label->setGeometry(830,356,100,15);
  air_timesync_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Show Log Buttons
  //
  air_auxlog_box[0]=new QCheckBox(this);
  air_auxlog_box[0]->setGeometry(810,378,15,15);
  label=new QLabel(air_auxlog_box[0],tr("Show Auxlog 1 Button"),this);
  label->setFont(subLabelFont());
  label->setGeometry(830,378,150,15);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  air_auxlog_box[1]=new QCheckBox(this);
  air_auxlog_box[1]->setGeometry(810,400,15,15);
  label=new QLabel(air_auxlog_box[1],tr("Show Auxlog 2 Button"),this);
  label->setFont(subLabelFont());
  label->setGeometry(830,400,150,15);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Clear Cart Filter
  //
  air_clearfilter_box=new QCheckBox(this);
  air_clearfilter_box->setGeometry(810,422,15,15);
  label=new QLabel(air_clearfilter_box,tr("Clear Cart Search Filter"),this);
  label->setFont(subLabelFont());
  label->setGeometry(830,422,150,15);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Pause Enable Checkbox
  //
  air_pause_box=new QCheckBox(this);
  air_pause_box->setGeometry(810,444,15,15);
  label=new QLabel(air_pause_box,tr("Enable Paused Events"),this);
  label->setFont(subLabelFont());
  label->setGeometry(830,444,150,15);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Show Extra Counters/Buttons
  //
  air_show_counters_box=new QCheckBox(this);
  air_show_counters_box->setGeometry(810,466,15,15);
  label=
    new QLabel(air_show_counters_box,tr("Show Extra Buttons/Counters"),this);
  label->setFont(subLabelFont());
  label->setGeometry(830,466,170,15);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Show Hour Selector
  //
  air_hour_selector_box=new QCheckBox(this);
  air_hour_selector_box->setGeometry(810,488,15,15);
  label=new QLabel(air_hour_selector_box,tr("Show Hour Selector"),this);
  label->setFont(subLabelFont());
  label->setGeometry(830,488,170,15);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Audition Preroll
  //
  air_audition_preroll_spin=new QSpinBox(this);
  air_audition_preroll_spin->setGeometry(895,507,45,20);
  air_audition_preroll_spin->setRange(1,60);
  air_audition_preroll_label=
    new QLabel(air_audition_preroll_spin,tr("Audition Preroll:"),this);
  air_audition_preroll_label->setFont(subLabelFont());
  air_audition_preroll_label->setGeometry(800,510,90,15);
  air_audition_preroll_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  air_audition_preroll_unit=new QLabel(tr("secs"),this);
  air_audition_preroll_unit->setFont(subLabelFont());
  air_audition_preroll_unit->setGeometry(945,510,100,15);
  air_audition_preroll_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Space Bar Action
  //
  air_bar_group=new Q3ButtonGroup(1,Qt::Vertical,tr("Space Bar Action"),this);
  air_bar_group->setFont(subLabelFont());
  air_bar_group->setGeometry(805,532,sizeHint().width()-815,55);
  new QRadioButton(tr("None"),air_bar_group);
  new QRadioButton(tr("Start Next"),air_bar_group);

  //
  // Start/Stop Section
  //
  label=new QLabel(tr("Start/Stop Settings"),this);
  label->setFont(sectionLabelFont());
  label->setGeometry(10,451,200,16);

  //
  // Exit Password
  //
  air_exitpasswd_edit=new QLineEdit(this);
  air_exitpasswd_edit->setGeometry(100,474,sizeHint().width()-905,20);
  air_exitpasswd_edit->setEchoMode(QLineEdit::Password);
  air_exitpasswd_edit->setText("******");
  label=new QLabel(air_exitpasswd_edit,tr("Exit Password:"),this);
  label->setFont(subLabelFont());
  label->setGeometry(0,474,95,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(air_exitpasswd_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(exitPasswordChangedData(const QString &)));

  //
  // Log Machine Selector
  //
  air_logmachine_box=new QComboBox(this);
  air_logmachine_box->setGeometry(45,499,100,20);
  air_logmachine_box->insertItem(tr("Main Log"));
  for(unsigned i=1;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_logmachine_box->insertItem(QString().sprintf("Aux %d Log",i));
  }
  for(unsigned i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
    air_logmachine_box->insertItem(QString().sprintf("vLog %d",i+RD_RDVAIRPLAY_LOG_BASE+1));
  }
  connect(air_logmachine_box,SIGNAL(activated(int)),
	  this,SLOT(logActivatedData(int)));

  //
  // Startup Mode
  //
  air_startmode_box=new QComboBox(this);
  air_startmode_box->setGeometry(100,524,240,20);
  air_startmode_box->insertItem(tr("start with empty log"));
  air_startmode_box->insertItem(tr("load previous log"));
  air_startmode_box->insertItem(tr("load specified log"));
  label=new QLabel(air_exitpasswd_edit,tr("At Startup:"),this);
  label->setFont(subLabelFont());
  label->setGeometry(30,524,65,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(air_startmode_box,SIGNAL(activated(int)),
	  this,SLOT(startModeChangedData(int)));

  //
  // Auto Restart Checkbox
  //
  air_autorestart_box=new QCheckBox(this);
  air_autorestart_box->setGeometry(105,549,15,15);
  air_autorestart_label=
    new QLabel(air_autorestart_box,
	       tr("Restart Log After Unclean Shutdown"),this);
  air_autorestart_label->setFont(subLabelFont());
  air_autorestart_label->setGeometry(125,549,250,15);
  air_autorestart_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Startup Log
  //
  air_startlog_edit=new QLineEdit(this);
  air_startlog_edit->setGeometry(100,569,240,20);
  air_startlog_label=new QLabel(air_startlog_edit,tr("Log:"),this);
  air_startlog_label->setFont(subLabelFont());
  air_startlog_label->setGeometry(30,569,65,20);
  air_startlog_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Log Select Button
  //
  air_startlog_button=new QPushButton(this);
  air_startlog_button->setGeometry(350,567,50,24);
  air_startlog_button->setFont(subButtonFont());
  air_startlog_button->setText(tr("&Select"));
  connect(air_startlog_button,SIGNAL(clicked()),this,SLOT(selectData()));

  //
  // Display Settings Section
  //
  label=new QLabel(tr("Display Settings"),this);
  label->setFont(sectionLabelFont());
  label->setGeometry(435,381,200,16);

  //
  // Skin Path
  //
  air_skin_edit=new QLineEdit(this);
  air_skin_edit->setGeometry(555,403,180,20);
  label=new QLabel(air_skin_edit,tr("Background Image:"),this);
  label->setFont(subLabelFont());
  label->setGeometry(435,403,115,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  button=new QPushButton(tr("Select"),this);
  button->setFont(subButtonFont());
  button->setGeometry(745,400,50,25);
  connect(button,SIGNAL(clicked()),this,SLOT(selectSkinData()));

  //
  // Title Template
  //
  air_title_template_edit=new QLineEdit(this);
  air_title_template_edit->setGeometry(555,425,180,20);
  label=new QLabel(air_label_template_edit,tr("Title Template:"),this);
  label->setFont(subLabelFont());
  label->setGeometry(430,425,120,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Artist Template
  //
  air_artist_template_edit=new QLineEdit(this);
  air_artist_template_edit->setGeometry(555,447,180,20);
  label=new QLabel(air_label_template_edit,tr("Artist Template:"),this);
  label->setFont(subLabelFont());
  label->setGeometry(430,447,120,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Outcue Template
  //
  air_outcue_template_edit=new QLineEdit(this);
  air_outcue_template_edit->setGeometry(555,469,180,20);
  label=new QLabel(air_label_template_edit,tr("Outcue Template:"),this);
  label->setFont(subLabelFont());
  label->setGeometry(430,469,120,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Description Template
  //
  air_description_template_edit=new QLineEdit(this);
  air_description_template_edit->setGeometry(555,491,180,20);
  label=new QLabel(air_label_template_edit,tr("Description Template:"),this);
  label->setFont(subLabelFont());
  label->setGeometry(430,491,120,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Log Mode Control Section
  //
  label=new QLabel(tr("Log Mode Control"),this);
  label->setFont(sectionLabelFont());
  label->setGeometry(435,530,200,16);

  //
  // Mode Control Style
  //
  air_modecontrol_box=new QComboBox(this);
  air_modecontrol_box->setGeometry(560,550,110,20);
  connect(air_modecontrol_box,SIGNAL(activated(int)),
	  this,SLOT(modeControlActivatedData(int)));
  label=new QLabel(air_modecontrol_box,tr("Mode Control Style:"),this);
  label->setFont(subLabelFont());
  label->setGeometry(435,550,120,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  air_modecontrol_box->insertItem(tr("Unified"));
  air_modecontrol_box->insertItem(tr("Independent"));

  //
  // Startup Mode
  //
  for(int i=0;i<3;i++) {
    air_logstartmode_box[i]=new QComboBox(this);
    air_logstartmode_box[i]->setGeometry(615,572+i*22,110,20);
    connect(air_logstartmode_box[i],SIGNAL(activated(int)),
	    this,SLOT(logStartupModeActivatedData(int)));
    air_logstartmode_label[i]=new QLabel(air_logstartmode_box[i],"",this);
    air_logstartmode_label[i]->setFont(subLabelFont());
    air_logstartmode_label[i]->setGeometry(470,572+i*22,140,20);
    air_logstartmode_label[i]->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    air_logstartmode_box[i]->insertItem(tr("Previous"));
    air_logstartmode_box[i]->insertItem(tr("LiveAssist"));
    air_logstartmode_box[i]->insertItem(tr("Automatic"));
    air_logstartmode_box[i]->insertItem(tr("Manual"));
  }
  air_logstartmode_label[0]->setText(tr("Main Log Startup Mode:"));
  air_logstartmode_label[1]->setText(tr("Aux 1 Log Startup Mode:"));
  air_logstartmode_label[2]->setText(tr("Aux 2 Log Startup Mode:"));

  air_virtual_logstartsel_box=new QComboBox(this);
  air_virtual_logstartsel_box->setGeometry(435,638,120,20);
  connect(air_virtual_logstartsel_box,SIGNAL(activated(int)),
	  this,SLOT(virtualModeActivatedData(int)));
  for(int i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
    air_virtual_logstartsel_box->
      insertItem(QString().sprintf("vLog %d",i+RD_RDVAIRPLAY_LOG_BASE+1));
  }
  label=new QLabel(":",this);
  label->setFont(subLabelFont());
  label->setGeometry(555,638,5,20);
  label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
  air_virtual_logstartmode_box=new QComboBox(this);
  air_virtual_logstartmode_box->setGeometry(565,638,110,20);
  air_virtual_logstartmode_box->insertItem(tr("Previous"));
  air_virtual_logstartmode_box->insertItem(tr("LiveAssist"));
  air_virtual_logstartmode_box->insertItem(tr("Automatic"));
  air_virtual_logstartmode_box->insertItem(tr("Manual"));

  //
  //  Ok Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  if(cae_station->scanned()) {
    for(int i=0;i<10;i++) {
      air_card_sel[i]->setMaxCards(cae_station->cards());
      for(int j=0;j<air_card_sel[i]->maxCards();j++) {
	air_card_sel[i]->setMaxPorts(j,cae_station->cardOutputs(j));
      }
    }
    air_virtual_card_sel->setMaxCards(cae_station->cards());
    for(int j=0;j<air_virtual_card_sel->maxCards();j++) {
      air_virtual_card_sel->setMaxPorts(j,cae_station->cardOutputs(j));
    }
  }
  else {
    QMessageBox::information(this,tr("No Audio Configuration Data"),
			     tr("Channel assignments will not be available for this host as audio resource data\nhas not yet been generated.  Please start the Rivendell daemons on the host\nconfigured to run the CAE service in order to populate the audio resources database."));
    for(int i=0;i<10;i++) {
      air_card_sel[i]->setDisabled(true);
    }
  }
  for(int i=0;i<RDAirPlayConf::LastChannel;i++) {
    air_card_sel[i]->setCard(air_conf->card((RDAirPlayConf::Channel)i));
    air_card_sel[i]->setPort(air_conf->port((RDAirPlayConf::Channel)i));
  }
  air_segue_edit->setText(QString().sprintf("%d",air_conf->segueLength()));
  air_trans_edit->setText(QString().sprintf("%d",air_conf->transLength()));
  air_piecount_box->setValue(air_conf->pieCountLength()/1000);
  air_countto_box->setCurrentItem(air_conf->pieEndPoint());
  air_default_transtype_box->setCurrentItem(air_conf->defaultTransType());
  air_defaultsvc_box->insertItem(tr("[none]"));
  QString defaultsvc=air_conf->defaultSvc();
  sql=QString("select SERVICE_NAME from SERVICE_PERMS where ")+
    "STATION_NAME=\""+RDEscapeString(air_conf->station())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    air_defaultsvc_box->insertItem(q->value(0).toString());
    if(defaultsvc==q->value(0).toString()) {
      air_defaultsvc_box->setCurrentItem(air_defaultsvc_box->count()-1);
    }
  }
  delete q;
  air_station_box->setValue(air_conf->panels(RDAirPlayConf::StationPanel));
  air_user_box->setValue(air_conf->panels(RDAirPlayConf::UserPanel));
  air_timesync_box->setChecked(air_conf->checkTimesync());
  for(int i=0;i<2;i++) {
    air_auxlog_box[i]->setChecked(air_conf->showAuxButton(i));
  }
  air_clearfilter_box->setChecked(air_conf->clearFilter());
  air_bar_group->setButton((int)air_conf->barAction());
  air_flash_box->setChecked(air_conf->flashPanel());
  air_panel_pause_box->setChecked(air_conf->panelPauseEnabled());
  air_label_template_edit->setText(air_conf->buttonLabelTemplate());
  air_pause_box->setChecked(air_conf->pauseEnabled());
  air_show_counters_box->setChecked(air_conf->showCounters());
  air_hour_selector_box->setChecked(air_conf->hourSelectorEnabled());
  air_audition_preroll_spin->setValue(air_conf->auditionPreroll()/1000);
  air_title_template_edit->setText(air_conf->titleTemplate());
  air_artist_template_edit->setText(air_conf->artistTemplate());
  air_outcue_template_edit->setText(air_conf->outcueTemplate());
  air_description_template_edit->setText(air_conf->descriptionTemplate());
  for(int i=0;i<RDAirPlayConf::LastChannel;i++) {
    air_start_rml_edit[i]->setText(air_conf->
				   startRml((RDAirPlayConf::Channel)i));
    air_stop_rml_edit[i]->setText(air_conf->
				  stopRml((RDAirPlayConf::Channel)i));
  }
  air_modecontrol_box->setCurrentItem((int)air_conf->opModeStyle());
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_startmodes[i]=air_conf->startMode(i);
    air_startlogs[i]=air_conf->logName(i);
    air_autorestarts[i]=air_conf->autoRestart(i);
    air_logstartmode_box[i]->setCurrentItem(air_conf->logStartMode(i));
  }
  for(int i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
    air_startmodes[i+RD_RDVAIRPLAY_LOG_BASE]=
      air_conf->startMode(i+RD_RDVAIRPLAY_LOG_BASE);
    air_startlogs[i+RD_RDVAIRPLAY_LOG_BASE]=
      air_conf->logName(i+RD_RDVAIRPLAY_LOG_BASE);
    air_autorestarts[i+RD_RDVAIRPLAY_LOG_BASE]=
      air_conf->autoRestart(i+RD_RDVAIRPLAY_LOG_BASE);
    air_virtual_cards[i]=air_conf->virtualCard(i+RD_RDVAIRPLAY_LOG_BASE);
    air_virtual_ports[i]=air_conf->virtualPort(i+RD_RDVAIRPLAY_LOG_BASE);
    air_virtual_start_rmls[i]=
      air_conf->virtualStartRml(i+RD_RDVAIRPLAY_LOG_BASE);
    air_virtual_stop_rmls[i]=air_conf->virtualStopRml(i+RD_RDVAIRPLAY_LOG_BASE);
    air_virtual_opmodes[i]=air_conf->opMode(i+RD_RDVAIRPLAY_LOG_BASE);
    air_virtual_logstartmode_box->setCurrentItem((int)air_virtual_opmodes[0]);
  }
  air_virtual_card_sel->setCard(air_virtual_cards[air_virtual_logmachine]);
  air_virtual_card_sel->setPort(air_virtual_ports[air_virtual_logmachine]);
  air_virtual_start_rml_edit->
    setText(air_virtual_start_rmls[air_virtual_logmachine]);
  air_virtual_stop_rml_edit->
    setText(air_virtual_stop_rmls[air_virtual_logmachine]);
  air_startmode_box->setCurrentItem((int)air_startmodes[air_logmachine]);
  air_startlog_edit->setText(air_startlogs[air_logmachine]);
  air_autorestart_box->setChecked(air_autorestarts[air_logmachine]);
  air_skin_edit->setText(air_conf->skinPath());
  startModeChangedData(air_startmodes[air_logmachine]);

  for(unsigned i=0;i<RDAirPlayConf::LastChannel;i++) {
    audioSettingsChangedData(i,air_card_sel[i]->card(),air_card_sel[i]->port());
  }
}


EditRDAirPlay::~EditRDAirPlay()
{
}


QSize EditRDAirPlay::sizeHint() const
{
  return QSize(1010,680);
} 


QSizePolicy EditRDAirPlay::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditRDAirPlay::audioSettingsChangedData(int id,int card,int port)
{
  if(air_channel_button[id]!=NULL) {
    bool state=air_card_sel[id]->isDisabled();
    air_start_rml_label[id]->setDisabled(state);
    air_start_rml_edit[id]->setDisabled(state);
    air_stop_rml_label[id]->setDisabled(state);
    air_stop_rml_edit[id]->setDisabled(state);
    if((id==RDAirPlayConf::MainLog1Channel)||
       (id==RDAirPlayConf::MainLog2Channel)) {
      air_channel_button[RDAirPlayConf::MainLog1Channel]->
	setDisabled(air_card_sel[RDAirPlayConf::MainLog1Channel]->isDisabled());
      bool state2=(air_card_sel[RDAirPlayConf::MainLog2Channel]->isDisabled())
		    ||((air_card_sel[RDAirPlayConf::MainLog1Channel]->card()==
			air_card_sel[RDAirPlayConf::MainLog2Channel]->card())&&
		       (air_card_sel[RDAirPlayConf::MainLog1Channel]->port()==
			air_card_sel[RDAirPlayConf::MainLog2Channel]->port()));
      air_channel_button[RDAirPlayConf::MainLog2Channel]->setDisabled(state2);
      air_start_rml_label[RDAirPlayConf::MainLog2Channel]->setDisabled(state2);
      air_start_rml_edit[RDAirPlayConf::MainLog2Channel]->setDisabled(state2);
      air_stop_rml_label[RDAirPlayConf::MainLog2Channel]->setDisabled(state2);
      air_stop_rml_edit[RDAirPlayConf::MainLog2Channel]->setDisabled(state2);
    }
    else {
      air_channel_button[id]->setDisabled(air_card_sel[id]->isDisabled());
    }
  }
}


void EditRDAirPlay::editGpiosData(int num)
{
  EditChannelGpios *d=
    new EditChannelGpios(air_conf,(RDAirPlayConf::Channel)num,this);
  d->exec();
  delete d;
}


void EditRDAirPlay::exitPasswordChangedData(const QString &str)
{
  air_exitpasswd_changed=true;
}


void EditRDAirPlay::logActivatedData(int lognum)
{
  air_startmodes[air_logmachine]=
    (RDAirPlayConf::StartMode)air_startmode_box->currentItem();
  air_startlogs[air_logmachine]=air_startlog_edit->text();
  air_autorestarts[air_logmachine]=air_autorestart_box->isChecked();

  air_logmachine=lognum;
  if(lognum>2) {
    air_logmachine=lognum+RD_RDVAIRPLAY_LOG_BASE-3;
  }

  air_startmode_box->setCurrentItem((int)air_startmodes[air_logmachine]);
  air_startlog_edit->setText(air_startlogs[air_logmachine]);
  air_autorestart_box->setChecked(air_autorestarts[air_logmachine]);
  startModeChangedData((int)air_startmodes[air_logmachine]);
}


void EditRDAirPlay::virtualLogActivatedData(int vlognum)
{
  air_virtual_cards[air_virtual_logmachine]=air_virtual_card_sel->card();
  air_virtual_ports[air_virtual_logmachine]=air_virtual_card_sel->port();
  air_virtual_start_rmls[air_virtual_logmachine]=
    air_virtual_start_rml_edit->text();
  air_virtual_stop_rmls[air_virtual_logmachine]=
    air_virtual_stop_rml_edit->text();

  air_virtual_logmachine=vlognum;
  air_virtual_card_sel->setCard(air_virtual_cards[air_virtual_logmachine]);
  air_virtual_card_sel->setPort(air_virtual_ports[air_virtual_logmachine]);
  air_virtual_start_rml_edit->
    setText(air_virtual_start_rmls[air_virtual_logmachine]);
  air_virtual_stop_rml_edit->
    setText(air_virtual_stop_rmls[air_virtual_logmachine]);
}


void EditRDAirPlay::virtualModeActivatedData(int vlognum)
{
  air_virtual_opmodes[air_virtual_logstartmachine]=
    (RDAirPlayConf::OpMode)air_virtual_logstartmode_box->currentItem();
  air_virtual_logstartmachine=vlognum;
  air_virtual_logstartmode_box->
    setCurrentItem((int)air_virtual_opmodes[air_virtual_logstartmachine]);
}


void EditRDAirPlay::startModeChangedData(int mode)
{
  air_startlog_edit->setEnabled((RDAirPlayConf::StartMode)mode==
				RDAirPlayConf::StartSpecified);
  air_startlog_label->setEnabled((RDAirPlayConf::StartMode)mode==
				 RDAirPlayConf::StartSpecified);
  air_startlog_button->setEnabled((RDAirPlayConf::StartMode)mode==
				 RDAirPlayConf::StartSpecified);
  air_autorestart_box->setDisabled((RDAirPlayConf::StartMode)mode==
				   RDAirPlayConf::StartEmpty);
  air_autorestart_label->setDisabled((RDAirPlayConf::StartMode)mode==
				     RDAirPlayConf::StartEmpty);
}


void EditRDAirPlay::selectData()
{
  QString logname=air_startlog_edit->text();

  RDListLogs *ll=new RDListLogs(&logname,RDLogFilter::NoFilter,this);
  if(ll->exec()) {
    air_startlog_edit->setText(logname);
  }
  delete ll;
}


void EditRDAirPlay::editHotKeys()
{
  QString module_name = QString("airplay");
  EditHotkeys *edit_hotkeys=
    new EditHotkeys(air_conf->station(),(const char *)module_name,this);
  edit_hotkeys->exec();
  delete edit_hotkeys;
}


void EditRDAirPlay::selectSkinData()
{
  QString filename=air_skin_edit->text();
  filename=QFileDialog::getOpenFileName(this,"RDAdmin - "+
					tr("Select Image File"),filename,
					RD_IMAGE_FILE_FILTER);
  if(!filename.isNull()) {
    air_skin_edit->setText(filename);
  }
}


void EditRDAirPlay::modeControlActivatedData(int n)
{
  if(n==0) {
    for(int i=1;i<RDAIRPLAY_LOG_QUANTITY;i++) {
      air_logstartmode_box[i]->
	setCurrentItem(air_logstartmode_box[0]->currentItem());
    }
  }
}


void EditRDAirPlay::logStartupModeActivatedData(int n)
{
  if(air_modecontrol_box->currentItem()==0) {
    for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
      air_logstartmode_box[i]->setCurrentItem(n);
    }
  }
}


void EditRDAirPlay::okData()
{
  bool ok=false;
  int segue=air_segue_edit->text().toInt(&ok);
  if(!ok) {
    QMessageBox::warning(this,tr("Data Error"),tr("Invalid Segue Length!"));
    return;
  }
  int trans=air_trans_edit->text().toInt(&ok);
  if(!ok) {
    QMessageBox::warning(this,tr("Data Error"),
			 tr("Invalid Forced Segue Length!"));
    return;
  }
  for(int i=0;i<RDAirPlayConf::LastChannel;i++) {
    RDAirPlayConf::Channel chan=(RDAirPlayConf::Channel)i;
    air_conf->setStartRml(chan,air_start_rml_edit[i]->text());
    air_conf->setStopRml(chan,air_stop_rml_edit[i]->text());
    air_conf->setCard(chan,air_card_sel[i]->card());
    air_conf->setPort(chan,air_card_sel[i]->port());
    if(air_card_sel[i]->isDisabled()) {
      air_conf->setStartGpiMatrix(chan,-1);
      air_conf->setStartGpoMatrix(chan,-1);
      air_conf->setStopGpiMatrix(chan,-1);
      air_conf->setStopGpoMatrix(chan,-1);
    }
  }
  air_conf->setSegueLength(segue);
  air_conf->setTransLength(trans);
  air_conf->setPieCountLength(air_piecount_box->value()*1000);
  air_conf->
    setPieEndPoint((RDAirPlayConf::PieEndPoint)air_countto_box->currentItem());
  air_conf->setDefaultTransType((RDLogLine::TransType)
				air_default_transtype_box->currentItem());
  if(air_defaultsvc_box->currentItem()==0) {
    air_conf->setDefaultSvc("");
  }
  else {
    air_conf->setDefaultSvc(air_defaultsvc_box->currentText());
  }
  air_conf->setPanels(RDAirPlayConf::StationPanel,air_station_box->value());
  air_conf->setPanels(RDAirPlayConf::UserPanel,air_user_box->value());
  air_conf->setCheckTimesync(air_timesync_box->isChecked());
  for(int i=0;i<2;i++) {
    air_conf->setShowAuxButton(i,air_auxlog_box[i]->isChecked());
  }
  air_conf->setClearFilter(air_clearfilter_box->isChecked());
  air_conf->
    setBarAction((RDAirPlayConf::BarAction)air_bar_group->selectedId());
  air_conf->setFlashPanel(air_flash_box->isChecked());
  air_conf->setPanelPauseEnabled(air_panel_pause_box->isChecked());
  air_conf->setButtonLabelTemplate(air_label_template_edit->text());
  air_conf->setPauseEnabled(air_pause_box->isChecked());
  air_conf->setShowCounters(air_show_counters_box->isChecked());
  air_conf->setHourSelectorEnabled(air_hour_selector_box->isChecked());
  air_conf->setAuditionPreroll(air_audition_preroll_spin->value()*1000);
  air_conf->setTitleTemplate(air_title_template_edit->text());
  air_conf->setArtistTemplate(air_artist_template_edit->text());
  air_conf->setOutcueTemplate(air_outcue_template_edit->text());
  air_conf->setDescriptionTemplate(air_description_template_edit->text());
  if(air_exitpasswd_changed) {
    air_conf->setExitPassword(air_exitpasswd_edit->text());
  }
  air_startmodes[air_logmachine]=
    (RDAirPlayConf::StartMode)air_startmode_box->currentItem();
  air_startlogs[air_logmachine]=air_startlog_edit->text();
  air_autorestarts[air_logmachine]=air_autorestart_box->isChecked();
  air_conf->setOpModeStyle((RDAirPlayConf::OpModeStyle)
			   air_modecontrol_box->currentItem());
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_conf->setStartMode(i,air_startmodes[i]);
    air_conf->setLogName(i,air_startlogs[i]);
    air_conf->setAutoRestart(i,air_autorestarts[i]);
    air_conf->
      setLogStartMode(i,(RDAirPlayConf::OpMode)air_logstartmode_box[i]->
		      currentItem());
  }
  virtualLogActivatedData(air_virtual_logmachine);
  for(int i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
    air_conf->setStartMode(i+RD_RDVAIRPLAY_LOG_BASE,
			   air_startmodes[i+RD_RDVAIRPLAY_LOG_BASE]);
    air_conf->setLogName(i+RD_RDVAIRPLAY_LOG_BASE,
			 air_startlogs[i+RD_RDVAIRPLAY_LOG_BASE]);
    air_conf->setAutoRestart(i+RD_RDVAIRPLAY_LOG_BASE,
			     air_autorestarts[i+RD_RDVAIRPLAY_LOG_BASE]);
    air_conf->setVirtualCard(i+RD_RDVAIRPLAY_LOG_BASE,air_virtual_cards[i]);
    air_conf->setVirtualPort(i+RD_RDVAIRPLAY_LOG_BASE,air_virtual_ports[i]);
    air_conf->
      setVirtualStartRml(i+RD_RDVAIRPLAY_LOG_BASE,air_virtual_start_rmls[i]);
    air_conf->
      setVirtualStopRml(i+RD_RDVAIRPLAY_LOG_BASE,air_virtual_stop_rmls[i]);
    virtualModeActivatedData(air_virtual_logstartmachine);
    air_conf->setOpMode(i+RD_RDVAIRPLAY_LOG_BASE,air_virtual_opmodes[i]);
  }
  air_conf->setSkinPath(air_skin_edit->text());
  done(0);
}


void EditRDAirPlay::cancelData()
{
  done(1);
}


void EditRDAirPlay::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(Qt::black);
  p->drawLine(210,485,420,485);   // Top
  p->drawLine(25,495,25,580);     // Left
  p->drawLine(420,485,420,580);   // Right
  p->drawLine(25,580,65,580);     // Left Bottom
  p->drawLine(405,580,420,580);   // Right Bottom
  p->end();
  delete p;
}
