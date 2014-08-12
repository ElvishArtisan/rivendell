// edit_rdpanel.cpp
//
// Edit an RDPanel Configuration
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_rdpanel.cpp,v 1.9.8.2 2013/12/23 18:35:15 cvs Exp $
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qpainter.h>
#include <qfiledialog.h>

#include <rddb.h>
#include <rd.h>
#include <rdtextvalidator.h>
#include <rdlist_logs.h>

#include <edit_rdpanel.h>
#include <edit_now_next.h>


EditRDPanel::EditRDPanel(RDStation *station,RDStation *cae_station,
			 QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  RDSqlQuery *q;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  air_conf=new RDAirPlayConf(station->name(),"RDPANEL");

  //
  // Create Fonts
  //
  QFont unit_font=QFont("Helvetica",12,QFont::Normal);
  unit_font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",12,QFont::Bold);
  small_font.setPixelSize(12);
  QFont big_font=QFont("Helvetica",14,QFont::Bold);
  big_font.setPixelSize(14);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Dialog Name
  //
  setCaption(tr("RDPanel config for ")+station->name());

  //
  // Channel Assignments Section
  //
  QLabel *label=new QLabel(tr("Channel Assignments"),this,"globals_label");
  label->setFont(big_font);
  label->setGeometry(10,10,200,16);

  //
  // Sound Panel First Play Output
  //
  label=new QLabel(tr("SoundPanel First Play Output"),this,"globals_label");
  label->setFont(small_font);
//  label->setGeometry(395,32,300,16);
  label->setGeometry(20,32,300,16);
  air_card_sel[0]=new RDCardSelector(this,"air_card5_sel");
  air_card_sel[0]->setGeometry(15,50,120,117);
  air_start_rml_edit[0]=new QLineEdit(this);
  air_start_rml_edit[0]->setGeometry(205,50,160,19);
  air_start_rml_edit[0]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[0],tr("Start RML:"),this);
  label->setGeometry(135,50,65,19);
  label->setAlignment(AlignVCenter|AlignRight);
  air_stop_rml_edit[0]=new QLineEdit(this);
  air_stop_rml_edit[0]->setGeometry(205,71,160,19);
  air_stop_rml_edit[0]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[0],tr("Stop RML:"),this);
  label->setGeometry(135,71,65,19);
  label->setAlignment(AlignVCenter|AlignRight);

  //
  // Sound Panel Second Play Output
  //
  label=new QLabel(tr("SoundPanel Second Play Output"),this,"globals_label");
  label->setFont(small_font);
  label->setGeometry(20,100,300,16);
  air_card_sel[1]=new RDCardSelector(this,"air_card5_sel");
  air_card_sel[1]->setGeometry(15,118,120,117);
  air_start_rml_edit[1]=new QLineEdit(this);
  air_start_rml_edit[1]->setGeometry(205,118,160,19);
  air_start_rml_edit[1]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[1],tr("Start RML:"),this);
  label->setGeometry(135,118,65,19);
  label->setAlignment(AlignVCenter|AlignRight);
  air_stop_rml_edit[1]=new QLineEdit(this);
  air_stop_rml_edit[1]->setGeometry(205,139,160,19);
  air_stop_rml_edit[1]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[1],tr("Stop RML:"),this);
  label->setGeometry(135,139,65,19);
  label->setAlignment(AlignVCenter|AlignRight);

  //
  // Sound Panel Third Play Output
  //
  label=new QLabel(tr("SoundPanel Third Play Output"),this,"globals_label");
  label->setFont(small_font);
  label->setGeometry(20,168,300,16);
  air_card_sel[2]=new RDCardSelector(this,"air_card5_sel");
  air_card_sel[2]->setGeometry(15,186,120,117);
  air_start_rml_edit[2]=new QLineEdit(this);
  air_start_rml_edit[2]->setGeometry(205,186,160,19);
  air_start_rml_edit[2]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[2],tr("Start RML:"),this);
  label->setGeometry(135,186,65,19);
  label->setAlignment(AlignVCenter|AlignRight);
  air_stop_rml_edit[2]=new QLineEdit(this);
  air_stop_rml_edit[2]->setGeometry(205,207,160,19);
  air_stop_rml_edit[2]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[2],tr("Stop RML:"),this);
  label->setGeometry(135,207,65,19);
  label->setAlignment(AlignVCenter|AlignRight);

  //
  // Sound Panel Fourth Play Output
  //
  label=new QLabel(tr("SoundPanel Fourth Play Output"),this,"globals_label");
  label->setFont(small_font);
  label->setGeometry(20,236,300,16);
  air_card_sel[3]=new RDCardSelector(this,"air_card5_sel");
  air_card_sel[3]->setGeometry(15,254,120,117);
  air_start_rml_edit[3]=new QLineEdit(this);
  air_start_rml_edit[3]->setGeometry(205,254,160,19);
  air_start_rml_edit[3]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[3],tr("Start RML:"),this);
  label->setGeometry(135,254,65,19);
  label->setAlignment(AlignVCenter|AlignRight);
  air_stop_rml_edit[3]=new QLineEdit(this);
  air_stop_rml_edit[3]->setGeometry(205,275,160,19);
  air_stop_rml_edit[3]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[3],tr("Stop RML:"),this);
  label->setGeometry(135,275,65,19);
  label->setAlignment(AlignVCenter|AlignRight);

  //
  // Sound Panel Fifth Play Output
  //
  label=new QLabel(tr("SoundPanel Fifth and Later Play Output"),
		   this,"globals_label");
  label->setFont(small_font);
  label->setGeometry(20,304,300,16);
  air_card_sel[4]=new RDCardSelector(this,"air_card5_sel");
  air_card_sel[4]->setGeometry(15,322,120,117);
  air_start_rml_edit[4]=new QLineEdit(this);
  air_start_rml_edit[4]->setGeometry(205,322,160,19);
  air_start_rml_edit[4]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[4],tr("Start RML:"),this);
  label->setGeometry(135,322,65,19);
  label->setAlignment(AlignVCenter|AlignRight);
  air_stop_rml_edit[4]=new QLineEdit(this);
  air_stop_rml_edit[4]->setGeometry(205,343,160,19);
  air_stop_rml_edit[4]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[4],tr("Stop RML:"),this);
  label->setGeometry(135,343,65,19);
  label->setAlignment(AlignVCenter|AlignRight);

  //
  // Sound Panel Cue Play Output
  //
  label=new QLabel(tr("SoundPanel Cue Output"),this,"globals_label");
  label->setFont(small_font);
  label->setGeometry(20,372,300,16);
  air_card_sel[5]=new RDCardSelector(this,"air_card5_sel");
  air_card_sel[5]->setGeometry(15,390,120,117);
  air_start_rml_edit[5]=new QLineEdit(this);
  air_start_rml_edit[5]->setGeometry(205,390,160,19);
  air_start_rml_edit[5]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[5],tr("Start RML:"),this);
  label->setGeometry(135,390,65,19);
  label->setAlignment(AlignVCenter|AlignRight);
  air_stop_rml_edit[5]=new QLineEdit(this);
  air_stop_rml_edit[5]->setGeometry(205,411,160,19);
  air_stop_rml_edit[5]->setValidator(validator);
  label=new QLabel(air_start_rml_edit[5],tr("Stop RML:"),this);
  label->setGeometry(135,411,65,19);
  label->setAlignment(AlignVCenter|AlignRight);

  //
  // Display Settings Section
  //
  label=new QLabel(tr("Display Settings"),this,"globals_label");
  label->setFont(big_font);
  label->setGeometry(20,438,200,16);

  //
  // Skin Path
  //
  air_skin_edit=new QLineEdit(this,"air_skin_edit");
  air_skin_edit->setGeometry(135,460,180,20);
  label=new QLabel(air_skin_edit,tr("Background Image:"),
		   this,"air_skin_label");
  label->setGeometry(10,460,115,20);
  label->setAlignment(AlignRight|AlignVCenter);
  QPushButton *button=new QPushButton(tr("Select"),this,"skin_select_button");
  button->setGeometry(320,458,50,25);
  connect(button,SIGNAL(clicked()),this,SLOT(selectSkinData()));

  //
  // Sound Panel Section
  //
  label=new QLabel(tr("Sound Panel Settings"),this,"globals_label");
  label->setFont(big_font);
//  label->setGeometry(430,179,200,16);
  label->setGeometry(430,10,200,16);

  //
  // # of Station Panels
  //
  air_station_box=new QSpinBox(this,"air_station_box");
  air_station_box->setGeometry(510,35,50,20);
  air_station_box->setRange(0,MAX_PANELS);
  air_station_box->setSpecialValueText(tr("None"));
  air_station_label=new QLabel(air_station_box,tr("Host Panels:"),
			     this,"air_station_label");
  air_station_label->setGeometry(405,35,100,20);
  air_station_label->setAlignment(AlignRight|AlignVCenter);

  //
  // # of User Panels
  //
  air_user_box=new QSpinBox(this,"air_user_box");
  air_user_box->setGeometry(510,57,50,20);
  air_user_box->setRange(0,MAX_PANELS);
  air_user_box->setSpecialValueText(tr("None"));
  air_user_label=new QLabel(air_user_box,tr("User Panels:"),
			     this,"air_user_label");
  air_user_label->setGeometry(405,57,100,20);
  air_user_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Flash Active Button
  //
  air_flash_box=new QCheckBox(this,"air_flash_box");
  air_flash_box->setGeometry(435,85,15,15);
  label=new QLabel(air_flash_box,tr("Flash Active Buttons"),
			     this,"air_flash_label");
  label->setGeometry(455,85,150,15);
  label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Pause Panel Button
  //
  air_panel_pause_box=new QCheckBox(this,"air_panel_pause_box");
  air_panel_pause_box->setGeometry(435,107,15,15);
  label=new QLabel(air_panel_pause_box,tr("Enable Button Pausing"),
			     this,"air_panel_pause_label");
  label->setGeometry(455,107,150,15);
  label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Clear Cart Filter
  //
  air_clearfilter_box=new QCheckBox(this,"air_clearfilter_box");
  air_clearfilter_box->setGeometry(435,129,15,15);
  label=new QLabel(air_clearfilter_box,tr("Clear Cart Search Filter"),
		   this,"air_clearfilter_label");
  label->setGeometry(455,129,150,15);
  label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Default Service
  //
  air_defaultsvc_box=new QComboBox(this,"air_defaultsvc_box");
  air_defaultsvc_box->setGeometry(520,151,100,20);
  label=new QLabel(air_defaultsvc_box,tr("Default Service:"),
		   this,"air_defaultsvc_label");
  label->setGeometry(385,151,130,20);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Label Template
  //
  air_label_template_edit=new QLineEdit(this,"air_label_template_edit");
  air_label_template_edit->setGeometry(520,173,sizeHint().width()-530,20);
  label=new QLabel(air_label_template_edit,tr("Label Template:"),
		   this,"air_label_template_label");
  label->setGeometry(415,173,100,20);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  //  Ok Button
  //
  button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(small_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(small_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  if(station->scanned()) {
    for(int i=0;i<6;i++) {
      air_card_sel[i]->setMaxCards(cae_station->cards());
      for(int j=0;j<air_card_sel[i]->maxCards();j++) {
	air_card_sel[i]->setMaxPorts(j,cae_station->cardOutputs(j));
      }
    }
  }
  else {
    QMessageBox::information(this,tr("No Audio Configuration Data"),
			    tr("Channel assignments will not be available for this host, as audio resource data\nhas not yet been generated.  Please start the Rivendell daemons on this host\n(by executing, as user 'root',  the command \"/etc/init.d/rivendell start\")\nin order to populate the audio resources database."));
    for(int i=0;i<6;i++) {
      air_card_sel[i]->setDisabled(true);
    }
  }
  air_card_sel[0]->setCard(air_conf->card(RDAirPlayConf::SoundPanel1Channel));
  air_card_sel[0]->setPort(air_conf->port(RDAirPlayConf::SoundPanel1Channel));

  air_card_sel[1]->setCard(air_conf->card(RDAirPlayConf::SoundPanel2Channel));
  air_card_sel[1]->setPort(air_conf->port(RDAirPlayConf::SoundPanel2Channel));

  air_card_sel[2]->setCard(air_conf->card(RDAirPlayConf::SoundPanel3Channel));
  air_card_sel[2]->setPort(air_conf->port(RDAirPlayConf::SoundPanel3Channel));

  air_card_sel[3]->setCard(air_conf->card(RDAirPlayConf::SoundPanel4Channel));
  air_card_sel[3]->setPort(air_conf->port(RDAirPlayConf::SoundPanel4Channel));

  air_card_sel[4]->setCard(air_conf->card(RDAirPlayConf::SoundPanel5Channel));
  air_card_sel[4]->setPort(air_conf->port(RDAirPlayConf::SoundPanel5Channel));

  air_card_sel[5]->setCard(air_conf->card(RDAirPlayConf::CueChannel));
  air_card_sel[5]->setPort(air_conf->port(RDAirPlayConf::CueChannel));

  air_defaultsvc_box->insertItem(tr("[none]"));
  QString defaultsvc=air_conf->defaultSvc();
  sql=QString().sprintf("select SERVICE_NAME from SERVICE_PERMS \
                         where STATION_NAME=\"%s\"",
			(const char *)air_conf->station());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    air_defaultsvc_box->insertItem(q->value(0).toString());
    if(defaultsvc==q->value(0).toString()) {
      air_defaultsvc_box->setCurrentItem(air_defaultsvc_box->count()-1);
    }
  }
  delete q;
  air_skin_edit->setText(air_conf->skinPath());
  air_station_box->setValue(air_conf->panels(RDAirPlayConf::StationPanel));
  air_user_box->setValue(air_conf->panels(RDAirPlayConf::UserPanel));
  air_clearfilter_box->setChecked(air_conf->clearFilter());
  air_flash_box->setChecked(air_conf->flashPanel());
  air_panel_pause_box->setChecked(air_conf->panelPauseEnabled());
  air_label_template_edit->setText(air_conf->buttonLabelTemplate());
  air_start_rml_edit[0]->
    setText(air_conf->startRml(RDAirPlayConf::SoundPanel1Channel));
  air_stop_rml_edit[0]->
    setText(air_conf->stopRml(RDAirPlayConf::SoundPanel1Channel));

  air_start_rml_edit[1]->
    setText(air_conf->startRml(RDAirPlayConf::SoundPanel2Channel));
  air_stop_rml_edit[1]->
    setText(air_conf->stopRml(RDAirPlayConf::SoundPanel2Channel));

  air_start_rml_edit[2]->
    setText(air_conf->startRml(RDAirPlayConf::SoundPanel3Channel));
  air_stop_rml_edit[2]->
    setText(air_conf->stopRml(RDAirPlayConf::SoundPanel3Channel));

  air_start_rml_edit[3]->
    setText(air_conf->startRml(RDAirPlayConf::SoundPanel4Channel));
  air_stop_rml_edit[3]->
    setText(air_conf->stopRml(RDAirPlayConf::SoundPanel4Channel));

  air_start_rml_edit[4]->
    setText(air_conf->startRml(RDAirPlayConf::SoundPanel5Channel));
  air_stop_rml_edit[4]->
    setText(air_conf->stopRml(RDAirPlayConf::SoundPanel5Channel));

  air_start_rml_edit[5]->setText(air_conf->startRml(RDAirPlayConf::CueChannel));
  air_stop_rml_edit[5]->setText(air_conf->stopRml(RDAirPlayConf::CueChannel));
}


EditRDPanel::~EditRDPanel()
{
}


QSize EditRDPanel::sizeHint() const
{
  return QSize(630,496);
} 


QSizePolicy EditRDPanel::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditRDPanel::selectSkinData()
{
 QString filename=air_skin_edit->text();
  filename=QFileDialog::getOpenFileName(filename,RD_IMAGE_FILE_FILTER,this,"",
					tr("Select Image File"));
  if(!filename.isNull()) {
    air_skin_edit->setText(filename);
  }
}


void EditRDPanel::okData()
{
  air_conf->setStartRml(RDAirPlayConf::SoundPanel1Channel,
			air_start_rml_edit[0]->text());
  air_conf->setStopRml(RDAirPlayConf::SoundPanel1Channel,
		       air_stop_rml_edit[0]->text());
  air_conf->setCard(RDAirPlayConf::SoundPanel1Channel,air_card_sel[0]->card());
  air_conf->setPort(RDAirPlayConf::SoundPanel1Channel,air_card_sel[0]->port());

  air_conf->setStartRml(RDAirPlayConf::SoundPanel2Channel,
			air_start_rml_edit[1]->text());
  air_conf->setStopRml(RDAirPlayConf::SoundPanel2Channel,
		       air_stop_rml_edit[1]->text());
  air_conf->setCard(RDAirPlayConf::SoundPanel2Channel,air_card_sel[1]->card());
  air_conf->setPort(RDAirPlayConf::SoundPanel2Channel,air_card_sel[1]->port());

  air_conf->setStartRml(RDAirPlayConf::SoundPanel3Channel,
			air_start_rml_edit[2]->text());
  air_conf->setStopRml(RDAirPlayConf::SoundPanel3Channel,
		       air_stop_rml_edit[2]->text());
  air_conf->setCard(RDAirPlayConf::SoundPanel3Channel,air_card_sel[2]->card());
  air_conf->setPort(RDAirPlayConf::SoundPanel3Channel,air_card_sel[2]->port());

  air_conf->setStartRml(RDAirPlayConf::SoundPanel4Channel,
			air_start_rml_edit[3]->text());
  air_conf->setStopRml(RDAirPlayConf::SoundPanel4Channel,
		       air_stop_rml_edit[3]->text());
  air_conf->setCard(RDAirPlayConf::SoundPanel4Channel,air_card_sel[3]->card());
  air_conf->setPort(RDAirPlayConf::SoundPanel4Channel,air_card_sel[3]->port());

  air_conf->setStartRml(RDAirPlayConf::SoundPanel5Channel,
			air_start_rml_edit[4]->text());
  air_conf->setStopRml(RDAirPlayConf::SoundPanel5Channel,
		       air_stop_rml_edit[4]->text());
  air_conf->setCard(RDAirPlayConf::SoundPanel5Channel,air_card_sel[4]->card());
  air_conf->setPort(RDAirPlayConf::SoundPanel5Channel,air_card_sel[4]->port());

  air_conf->setStartRml(RDAirPlayConf::CueChannel,
			air_start_rml_edit[5]->text());
  air_conf->setStopRml(RDAirPlayConf::CueChannel,air_stop_rml_edit[5]->text());
  air_conf->setCard(RDAirPlayConf::CueChannel,air_card_sel[5]->card());
  air_conf->setPort(RDAirPlayConf::CueChannel,air_card_sel[5]->port());
  air_conf->setSkinPath(air_skin_edit->text());
  if(air_defaultsvc_box->currentItem()==0) {
    air_conf->setDefaultSvc("");
  }
  else {
    air_conf->setDefaultSvc(air_defaultsvc_box->currentText());
  }
  air_conf->setPanels(RDAirPlayConf::StationPanel,air_station_box->value());
  air_conf->setPanels(RDAirPlayConf::UserPanel,air_user_box->value());
  air_conf->setClearFilter(air_clearfilter_box->isChecked());
  air_conf->setFlashPanel(air_flash_box->isChecked());
  air_conf->setPanelPauseEnabled(air_panel_pause_box->isChecked());
  air_conf->setButtonLabelTemplate(air_label_template_edit->text());

  done(0);
}


void EditRDPanel::cancelData()
{
  done(1);
}
