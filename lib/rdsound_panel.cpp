// rdsound_panel.cpp
//
// The sound panel widget for RDAirPlay
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdsound_panel.cpp,v 1.62.6.13.2.3 2014/05/20 22:39:35 cvs Exp $
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

#include <qsignalmapper.h>
#include <rddb.h>
#include <rdlog_line.h>
#include <rdsound_panel.h>
#include <rdbutton_dialog.h>
#include <rdmacro.h>
#include <rdcut.h>
#include <rdedit_panel_name.h>
#include <rdescape_string.h>
#include <rdconf.h>

RDSoundPanel::RDSoundPanel(int cols,int rows,int station_panels,
			   int user_panels,bool flash,
			   const QString &label_template,bool extended,
			   RDEventPlayer *player,RDRipc *ripc,RDCae *cae,
			   RDStation *station,RDCartDialog *cart_dialog,
			   QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  panel_playmode_box=NULL;
  panel_button_columns=cols;
  panel_button_rows=rows;
  panel_cue_port=-1;
  if(extended) {
    panel_tablename="EXTENDED_PANELS";
    panel_name_tablename="EXTENDED_PANEL_NAMES";
  }
  else {
    panel_tablename="PANELS";
    panel_name_tablename="PANEL_NAMES";
  }
  panel_label_template=label_template;

  panel_type=RDAirPlayConf::StationPanel;
  panel_number=0;
  panel_setup_mode=false;
  panel_reset_mode=false;
  panel_parent=parent;
  panel_cae=cae;
  panel_user=NULL;
  panel_ripc=ripc;
  panel_station=station;
  panel_station_panels=station_panels;
  panel_user_panels=user_panels;
  panel_event_player=player;
  panel_action_mode=RDAirPlayConf::Normal;
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    panel_active_buttons[i]=NULL;
  }
  panel_flash=flash;
  panel_flash_count=0;
  panel_flash_state=false;
  panel_config_panels=false;
  panel_pause_enabled=false;
  for(unsigned i=0;i<PANEL_MAX_OUTPUTS;i++) {
    panel_card[i]=-1;
    panel_port[i]=-1;
  }
  panel_cart_dialog=cart_dialog;
  for(int i=0;i<PANEL_MAX_OUTPUTS;i++) {
    panel_timescaling_supported[i]=false;
  }
  panel_onair_flag=false;

  //
  // Create Fonts
  //
  QFont button_font=QFont("Helvetica",14,QFont::Bold);
  button_font.setPixelSize(14);

  //
  // Load Buttons
  //
  panel_mapper=new QSignalMapper(this,"panel_mapper");
  connect(panel_mapper,SIGNAL(mapped(int)),this,SLOT(buttonMapperData(int)));

  LoadPanels();

  //
  // Panel Selector
  //
  panel_selector_box=new RDComboBox(this,"panel_selector_box");
  panel_selector_box->setFont(button_font);
  panel_selector_box->addIgnoredKey(Qt::Key_Space);
  panel_selector_box->
    setGeometry((15+PANEL_BUTTON_SIZE_X)*(panel_button_columns-5),
		(15+PANEL_BUTTON_SIZE_Y)*panel_button_rows,
		2*PANEL_BUTTON_SIZE_X+15,50);
  connect(panel_selector_box,SIGNAL(activated(int)),
	  this,SLOT(panelActivatedData(int)));
  connect(panel_selector_box,SIGNAL(setupClicked()),
	  this,SLOT(panelSetupData()));

  if(panel_station_panels>0) {
    panel_number=0;
    panel_type=RDAirPlayConf::StationPanel;
    panel_buttons[0]->show();
  }
  else {
    if(panel_user_panels>0) {
      panel_number=0;
      panel_type=RDAirPlayConf::UserPanel;
      panel_buttons[0]->show();
    }
    else {
      setDisabled(true);
    }
  }
  
  //
  // Play Mode Box
  //
  panel_playmode_box=new QComboBox(this,"panel_playmode_box");
  panel_playmode_box->setFont(button_font);
  panel_playmode_box->
    setGeometry((15+PANEL_BUTTON_SIZE_X)*(panel_button_columns-3)-5,
		(15+PANEL_BUTTON_SIZE_Y)*panel_button_rows,
		PANEL_BUTTON_SIZE_X+10,50);
  connect(panel_playmode_box,SIGNAL(activated(int)),
	  this,SLOT(playmodeActivatedData(int)));
  panel_playmode_box->insertItem(tr("Play All"));
  panel_playmode_box->insertItem(tr("Play Hook"));

  //
  // Reset Button
  //
  panel_reset_button=new RDPushButton(this,"reset_button");
  panel_reset_button->
    setGeometry((15+PANEL_BUTTON_SIZE_X)*(panel_button_columns-2),
		(15+PANEL_BUTTON_SIZE_Y)*panel_button_rows,
		PANEL_BUTTON_SIZE_X,50);
  panel_reset_button->setFont(button_font);
  panel_reset_button->setText(tr("Reset"));
  panel_reset_button->setFlashColor(QColor(RDPANEL_RESET_FLASH_COLOR));
  panel_reset_button->setFocusPolicy(QWidget::NoFocus);
  connect(panel_reset_button,SIGNAL(clicked()),this,SLOT(resetClickedData()));

  //
  // All Button
  //
  panel_all_button=new RDPushButton(this,"all_button");
  panel_all_button->
    setGeometry((15+PANEL_BUTTON_SIZE_X)*(panel_button_columns-1),
		(15+PANEL_BUTTON_SIZE_Y)*panel_button_rows,
		PANEL_BUTTON_SIZE_X,50);
  panel_all_button->setFont(button_font);
  panel_all_button->setText(tr("All"));
  panel_all_button->setFlashColor(QColor(RDPANEL_RESET_FLASH_COLOR));
  panel_all_button->setFocusPolicy(QWidget::NoFocus);
  panel_all_button->hide();
  connect(panel_all_button,SIGNAL(clicked()),this,SLOT(allClickedData()));

  //
  // Setup Button
  //
  panel_setup_button=new RDPushButton(this,"setup_button");
  panel_setup_button->
    setGeometry((15+PANEL_BUTTON_SIZE_X)*(panel_button_columns-1),
		(15+PANEL_BUTTON_SIZE_Y)*panel_button_rows,
		PANEL_BUTTON_SIZE_X,50);
  panel_setup_button->setFont(button_font);
  panel_setup_button->setText(tr("Setup"));
  panel_setup_button->setFlashColor(QColor(RDPANEL_SETUP_FLASH_COLOR));
  panel_setup_button->setFocusPolicy(QWidget::NoFocus);
  connect(panel_setup_button,SIGNAL(clicked()),this,SLOT(setupClickedData()));

  //
  // Button Dialog Box
  //
  panel_button_dialog=
    new RDButtonDialog(panel_station->name(),panel_label_template,
		       panel_cart_dialog,panel_svcname,this,
		       "panel_button_dialog");

  //
  // CAE Setup
  //
  connect(panel_cae,SIGNAL(timescalingSupported(int,bool)),
	  this,SLOT(timescalingSupportedData(int,bool)));

  //
  // RIPC Setup
  //
  connect(panel_ripc,SIGNAL(onairFlagChanged(bool)),
	  this,SLOT(onairFlagChangedData(bool)));

  //
  // Load Panel Names
  //

  QString sql;
  sql=QString("select PANEL_NO,NAME from ")+panel_name_tablename+" where "+
    QString().sprintf("(TYPE=%d)&&",RDAirPlayConf::StationPanel)+
    "(OWNER=\""+RDEscapeString(panel_station->name())+"\") "+
    "order by PANEL_NO";
  RDSqlQuery *q=new RDSqlQuery(sql);
  q->first();
  for(int i=0;i<panel_station_panels;i++) {
    if(q->isValid()&&(q->value(0).toInt()==i)) {
      panel_selector_box->
	insertItem(QString().sprintf("[S:%d] ",i+1)+q->value(1).toString());
      q->next();
    }
    else {
      panel_selector_box->insertItem(QString().sprintf("[S:%d] Panel S:%d",
						       i+1,i+1));
    }
  }
  delete q;
  for(int i=0;i<panel_user_panels;i++) {
    panel_selector_box->insertItem(QString().sprintf("[U:%d] Panel U:%d",
						     i+1,i+1));
  }
  panel_selector_box->setFocus();

  panel_scan_timer=new QTimer(this);
  connect(panel_scan_timer,SIGNAL(timeout()),this,SLOT(scanPanelData()));
  panel_scan_timer->start(PANEL_SCAN_INTERVAL);
}


RDSoundPanel::~RDSoundPanel()
{
  for(unsigned i=0;i<panel_buttons.size();i++) {
    delete panel_buttons[i];
  }
}


QSize RDSoundPanel::sizeHint() const
{
  return QSize(panel_button_columns*(PANEL_BUTTON_SIZE_X+15),
	       panel_button_rows*(PANEL_BUTTON_SIZE_Y+15)+50);
}


QSizePolicy RDSoundPanel::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDSoundPanel::card(int outnum) const
{
  return panel_card[outnum];
}


void RDSoundPanel::setCard(int outnum,int card)
{
  panel_card[outnum]=card;
  panel_cae->requestTimescale(card);
}


int RDSoundPanel::port(int outnum) const
{
  return panel_port[outnum];
}


void RDSoundPanel::setPort(int outnum,int port)
{
  panel_port[outnum]=port;
}


QString RDSoundPanel::outputText(int outnum) const
{
  return panel_output_text[outnum];
}


void RDSoundPanel::setOutputText(int outnum,const QString &text)
{
  panel_output_text[outnum]=text;
}


void RDSoundPanel::setRmls(int outnum,const QString &start_rml,
			   const QString &stop_rml)
{
  panel_start_rml[outnum]=start_rml;
  panel_stop_rml[outnum]=stop_rml;
}


void RDSoundPanel::setLogName(const QString &logname)
{
  panel_logname=logname;
}


void RDSoundPanel::setSvcName(const QString &svcname)
{
  panel_svcname=svcname;
  panel_svcname.replace(" ","_");
}


void RDSoundPanel::setLogfile(QString filename)
{
  panel_logfile=filename;
}


void RDSoundPanel::play(RDAirPlayConf::PanelType type,int panel,
			int row, int col,RDLogLine::StartSource src,int mport,
                        bool pause_when_finished)
{
  PlayButton(type,panel,row,col,src,panel_playmode_box->currentItem()==1,
	     mport,pause_when_finished);
}


bool RDSoundPanel::pause(RDAirPlayConf::PanelType type,int panel,
			 int row,int col,int mport)
{
  if(panel_pause_enabled) {
    PauseButton(type,panel,row,col,mport);
    return true;
  }
  return false;
}


void RDSoundPanel::stop(RDAirPlayConf::PanelType type,int panel,
			int row,int col,
                        int mport,bool pause_when_finished,int fade_out)
{
  StopButton(type,panel,row,col,mport,pause_when_finished,fade_out);
}


void RDSoundPanel::channelStop(int mport)
{
  RDPanelButton *button=NULL;
  RDPlayDeck *deck=NULL;
  for(unsigned i=0;i<RD_MAX_STREAMS;i++) {
    if((button=panel_active_buttons[i])!=NULL) {
      if(button->outputText().toInt()==(mport+1)) {
	if((deck=button->playDeck())!=NULL) {
	  if(deck->state()==RDPlayDeck::Playing) {
	    deck->stop();
	  }
	}
      }
    }
  }
}


void RDSoundPanel::setText(RDAirPlayConf::PanelType type,int panel,int row,
			   int col,const QString &str)
{
  RDPanelButton *button=
    panel_buttons[PanelOffset(type,panel)]->panelButton(row,col);
  button->setText(str);
  SaveButton(type,panel,row,col);
}


void RDSoundPanel::setColor(RDAirPlayConf::PanelType type,int panel,int row,
			    int col,const QColor &color)
{
  RDPanelButton *button=
    panel_buttons[PanelOffset(type,panel)]->panelButton(row,col);
  button->setDefaultColor(color);
  SaveButton(type,panel,row,col);
}


void RDSoundPanel::duckVolume(RDAirPlayConf::PanelType type,int panel,int row,int col,
		  int level,int fade,int mport)
{
  int edit_mport=mport;
  if (edit_mport==0) {
    edit_mport=-1;
    }
    for(int i=0;i<panel_button_columns;i++) {
	    for(int j=0;j<panel_button_rows;j++) {
      RDPlayDeck *deck=
        panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->playDeck();
      if((row==j || row==-1) && (col==i || col==-1)) {
	if(mport==-1) {
	  panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->setDuckVolume(level);
	}    
        if(deck!=NULL) {
          if(edit_mport==-1 || 
             edit_mport==panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->
                     outputText().toInt()) {
	    deck->duckVolume(level,fade);
          }
        }
      }
    }
  }
}


RDAirPlayConf::ActionMode RDSoundPanel::actionMode() const
{
  return panel_action_mode;
}


void RDSoundPanel::setActionMode(RDAirPlayConf::ActionMode mode)
{
  if(panel_setup_mode) {
    return;
  }
  switch(mode) {
      case RDAirPlayConf::CopyFrom:
	mode=RDAirPlayConf::CopyFrom;
	break;

      case RDAirPlayConf::CopyTo:
	mode=RDAirPlayConf::CopyTo;
	break;

      case RDAirPlayConf::AddTo:
	mode=RDAirPlayConf::AddTo;
	break;

      case RDAirPlayConf::DeleteFrom:
	mode=RDAirPlayConf::DeleteFrom;
	break;

      default:
	mode=RDAirPlayConf::Normal;
	break;
  }
  if(mode!=panel_action_mode) {
    panel_action_mode=mode;
    panel_setup_button->setEnabled(panel_action_mode==RDAirPlayConf::Normal);
    for(unsigned i=0;i<panel_buttons.size();i++) {
      if(i<(unsigned)panel_station_panels &&
          (!panel_config_panels) &&   
          (mode==RDAirPlayConf::AddTo || mode==RDAirPlayConf::CopyTo || mode==RDAirPlayConf::DeleteFrom)) {
        panel_buttons[i]->setActionMode(RDAirPlayConf::Normal);
        }
      else {
        panel_buttons[i]->setActionMode(panel_action_mode);
        }
      }
    }
}


bool RDSoundPanel::pauseEnabled() const
{
  return panel_pause_enabled;
}


void RDSoundPanel::setPauseEnabled(bool state)
{
  if(state) {
    panel_reset_button->show();
  }
  else {
    panel_reset_button->hide();
  }
  panel_pause_enabled=state;
}


int RDSoundPanel::currentNumber() const
{
   return panel_number;
}


RDAirPlayConf::PanelType RDSoundPanel::currentType() const
{
   return panel_type;
}


void RDSoundPanel::setButton(RDAirPlayConf::PanelType type,int panel,
			     int row,int col,unsigned cartnum,
			     const QString &title)
{
  QString str;

  RDPanelButton *button=
    panel_buttons[PanelOffset(type,panel)]->panelButton(row,col);
  if(button->playDeck()!=NULL) {
    return;
  }
  button->clear();
  if(cartnum>0) {
    button->setCart(cartnum);
    RDCart *cart=new RDCart(cartnum);
    if(cart->exists()) {
      if(title.isEmpty()) {
	button->
	  setText(RDLogLine::resolveWildcards(cartnum,panel_label_template));
      }
      else {
	button->setText(title);
      }
      button->setLength(false,cart->forcedLength());
      if(cart->averageHookLength()>0) {
	button->setLength(true,cart->averageHookLength());
      }
      else {
	button->setLength(true,cart->forcedLength());
      }
      button->setHookMode(panel_playmode_box->currentItem()==1);
      switch(cart->type()) {
      case RDCart::Audio:
	if(button->length(button->hookMode())==0) {
	  button->setActiveLength(-1);
	}
	else {
	  button->setActiveLength(button->length(button->hookMode()));
	}
	break;

      case RDCart::Macro:
	button->setActiveLength(cart->forcedLength());
	break;

      case RDCart::All:
	break;
      }
    }
    else {
      if(title.isEmpty()) {
	str=QString(tr("Cart"));
	button->setText(str+QString().sprintf(" %06u",cartnum));
      }
      else {
	button->setText(title);
      }
    }
    delete cart;
  }
  SaveButton(type,panel,row,col);
}


void RDSoundPanel::acceptCartDrop(int row,int col,unsigned cartnum,
				  const QColor &color,const QString &title)
{
  setButton(panel_type,panel_number,row,col,cartnum,title);
  if(color.isValid()&&(color.name()!="#000000")) {
    setColor(panel_type,panel_number,row,col,color);
  }
}


void RDSoundPanel::changeUser()
{
  if(panel_user!=NULL) {
    delete panel_user;
  }
  panel_user=new RDUser(panel_ripc->user());
  panel_config_panels=panel_user->configPanels();
  LoadPanels();
  panel_buttons[PanelOffset(panel_type,panel_number)]->show();

  //
  // Remove Old Panel Names
  //
  int current_item=panel_selector_box->currentItem();
  for(int i=0;i<panel_user_panels;i++) {
    panel_selector_box->removeItem(panel_station_panels);
  }

  //
  // Load New Panel Names
  //
  QString sql;
  sql=QString("select PANEL_NO,NAME from ")+panel_name_tablename+" where "+
    QString().sprintf("(TYPE=%d)&&",RDAirPlayConf::UserPanel)+
    "(OWNER=\""+RDEscapeString(panel_user->name())+"\") "+
    "order by PANEL_NO";
  RDSqlQuery *q=new RDSqlQuery(sql);
  q->first();
  for(int i=0;i<panel_user_panels;i++) {
    if(q->isValid()&&(q->value(0).toInt()==i)) {
      panel_selector_box->
	insertItem(QString().sprintf("[U:%d] ",i+1)+q->value(1).toString());
      q->next();
    }
    else {
      panel_selector_box->insertItem(QString().sprintf("[U:%d] Panel U:%d",
						       i+1,i+1));
    }
  }
  delete q;
  panel_selector_box->setCurrentItem(current_item);
}


void RDSoundPanel::tickClock()
{
  emit tick();
  if(panel_flash) {
    if(panel_flash_count++>1) {
      emit buttonFlash(panel_flash_state);
      panel_flash_state=!panel_flash_state;
      panel_flash_count=0;
    }
  }
}


void RDSoundPanel::panelActivatedData(int n)
{
  panel_buttons[PanelOffset(panel_type,panel_number)]->hide();
  if(n<panel_station_panels) {
    panel_type=RDAirPlayConf::StationPanel;
    panel_number=n;
  }
  else {
    panel_type=RDAirPlayConf::UserPanel;
    panel_number=n-panel_station_panels;
  }
  panel_buttons[PanelOffset(panel_type,panel_number)]->show();
}


void RDSoundPanel::resetClickedData()
{
  if(panel_reset_mode) {
    panel_reset_mode=false;
    panel_reset_button->setFlashingEnabled(false);
    panel_all_button->hide();
    panel_setup_button->show();
  }
  else {
    panel_reset_mode=true;
    panel_reset_button->setFlashingEnabled(true);
    panel_setup_button->hide();
    panel_all_button->show();
  }
}


void RDSoundPanel::allClickedData()
{
    StopButton(panel_type,panel_number,-1,-1);
}


void RDSoundPanel::playmodeActivatedData(int n)
{
  LoadPanel(panel_type,panel_number);
}


void RDSoundPanel::setupClickedData()
{
  if(panel_setup_mode) {
    panel_setup_mode=false;
    panel_setup_button->setFlashingEnabled(false);
    panel_reset_button->setEnabled(true);
    panel_playmode_box->setEnabled(true);
  }
  else {
    panel_setup_mode=true;
    panel_setup_button->setFlashingEnabled(true);
    panel_reset_button->setDisabled(true);
    panel_playmode_box->setDisabled(true);
  }
  if(panel_station->enableDragdrop()&&(panel_station->enforcePanelSetup())) {
    for(unsigned i=0;i<panel_buttons.size();i++) {
      if(panel_buttons[i]!=NULL) {
	panel_buttons[i]->setAcceptDrops(panel_setup_mode);
      }
    }
  }
  panel_selector_box->setSetupMode(panel_setup_mode);
}


void RDSoundPanel::buttonMapperData(int id)
{
  int row=id/panel_button_columns;
  int col=id-row*panel_button_columns;
  unsigned cartnum;

  switch(panel_action_mode) {
      case RDAirPlayConf::CopyFrom:
	if((cartnum=panel_buttons[PanelOffset(panel_type,panel_number)]->
	    panelButton(row,col)->cart())>0) {
	  emit selectClicked(cartnum,0,0);
	}
	break;
	
      case RDAirPlayConf::CopyTo:
         if(panel_buttons[PanelOffset(panel_type,panel_number)]->
	    panelButton(row,col)->playDeck()==NULL
             && ((panel_type==RDAirPlayConf::UserPanel) || 
		 panel_config_panels)) { 
	   emit selectClicked(0,row,col);
           }
	break;
	
      case RDAirPlayConf::AddTo:
         if(panel_buttons[PanelOffset(panel_type,panel_number)]->
	    panelButton(row,col)->playDeck()==NULL
             && ((panel_type==RDAirPlayConf::UserPanel) || 
		 panel_config_panels)) { 
	   emit selectClicked(0,row,col);
           }
	break;
	
      case RDAirPlayConf::DeleteFrom:
         if(panel_buttons[PanelOffset(panel_type,panel_number)]->
	    panelButton(row,col)->playDeck()==NULL
             && ((panel_type==RDAirPlayConf::UserPanel) || 
		 panel_config_panels)) { 
	   emit selectClicked(0,row,col);
           }
	break;
	
      default:
	if(panel_setup_mode) {
	  if((panel_type==RDAirPlayConf::StationPanel)&&
	     (!panel_config_panels)) {
	    ClearReset();
	    return;
	  }
	  if(panel_button_dialog->
	     exec(panel_buttons[PanelOffset(panel_type,panel_number)]->
		  panelButton(row,col),panel_playmode_box->currentItem()==1,
		  panel_user->name(),panel_user->password())
	     ==0) {
	    SaveButton(panel_type,panel_number,row,col);
	  }
	}
	else {
	  RDPanelButton *button=
	    panel_buttons[PanelOffset(panel_type,panel_number)]->
	    panelButton(row,col);
	  RDPlayDeck *deck=button->playDeck();
	  if(panel_reset_mode) {
	    StopButton(panel_type,panel_number,row,col);
	  }
	  else {
	    if(deck==NULL) {
	      PlayButton(panel_type,panel_number,row,col,
			 RDLogLine::StartManual,
			 panel_playmode_box->currentItem()==1);
	    }
	    else {
	      if(panel_pause_enabled) {
		if(deck->state()!=RDPlayDeck::Paused) {
		  PauseButton(panel_type,panel_number,row,col);
		}
		else {
		  PlayButton(panel_type,panel_number,row,col,
			     RDLogLine::StartManual,button->hookMode());
		}
	      }
	      else {
		StopButton(panel_type,panel_number,row,col);
	      }
	    }
	  }
	}
  }
  ClearReset();
}


void RDSoundPanel::stateChangedData(int id,RDPlayDeck::State state)
{
  switch(state) {
      case RDPlayDeck::Playing:
	Playing(id);
	break;

      case RDPlayDeck::Stopped:
      case RDPlayDeck::Finished:
        Stopped(id);
	break;

      case RDPlayDeck::Paused:
	Paused(id);
	break;

      default:
	break;
  }
}


void RDSoundPanel::hookEndData(int id)
{
  RDPanelButton *button=panel_active_buttons[id];
  if(!button->hookMode()) {
    return;
  }
  RDPlayDeck *deck=button->playDeck();
  if(deck!=NULL) {
    switch(deck->state()) {
	case RDPlayDeck::Playing:
	case RDPlayDeck::Paused:
	  StopButton(id);
	  break;
	  
	default:
	  break;
    }
  }
}


void RDSoundPanel::timescalingSupportedData(int card,bool state)
{
  for(unsigned i=0;i<PANEL_MAX_OUTPUTS;i++) {
    if(card==panel_card[i]) {
      panel_timescaling_supported[i]=state;
    }
  }
}


void RDSoundPanel::panelSetupData()
{
  if(panel_user->configPanels()||(panel_type==RDAirPlayConf::UserPanel)) {
    QString sql;
    RDSqlQuery *q;
    int cutpt=panel_selector_box->currentText().find(" ");
    if(panel_selector_box->currentText().left(5)==tr("Panel")) {
      cutpt=-1;
    }
    QString panel_name=panel_selector_box->currentText().
      right(panel_selector_box->currentText().length()-cutpt-1);
    RDEditPanelName *edn=new RDEditPanelName(&panel_name);
    if(edn->exec()==0) {
      panel_selector_box->
	setCurrentText("["+PanelTag(panel_selector_box->currentItem())+"] "+
		       panel_name);
      sql=QString("delete from ")+panel_name_tablename+" where "+
	QString().sprintf("(TYPE=%d)&&",panel_type)+
	"(OWNER=\""+RDEscapeString(PanelOwner(panel_type))+"\")&&"+
	QString().sprintf("(PANEL_NO=%d)",panel_number);
      q=new RDSqlQuery(sql);
      delete q;
      sql=QString("insert into ")+panel_name_tablename+" set "+
	QString().sprintf("TYPE=%d,",panel_type)+
	"OWNER=\""+RDEscapeString(PanelOwner(panel_type))+"\","+
	QString().sprintf("PANEL_NO=%d,",panel_number)+
	"NAME=\""+RDEscapeString(panel_name)+"\"";
      q=new RDSqlQuery(sql);
      delete q;
    }
    delete edn;
  }
}


void RDSoundPanel::onairFlagChangedData(bool state)
{
  panel_onair_flag=state;
}


void RDSoundPanel::scanPanelData()
{
  LoadPanel(panel_type,panel_number);
}


void RDSoundPanel::PlayButton(RDAirPlayConf::PanelType type,int panel,
		int row,int col,RDLogLine::StartSource src,bool hookmode,
		int mport,bool pause_when_finished)
{
  int edit_row=row;
  int edit_col=col;
  
  for(int i=0;i<panel_button_columns;i++) {
    for(int j=0;j<panel_button_rows;j++) {
      if(panel_buttons[PanelOffset(type,panel)]->
	 panelButton(j,i)->cart()>0 && 
         panel_buttons[PanelOffset(type,panel)]->
	 panelButton(j,i)->state()==false) {
        if(edit_col==-1 || col==i) {
	  edit_col=i;
	  if(edit_row==-1) {
	    edit_row=j;
	  } 
	}
      }
    }
  }
  if(edit_row==-1 || edit_col==-1) {
    return;
  }
  
  RDPanelButton *button=
    panel_buttons[PanelOffset(type,panel)]->panelButton(edit_row,edit_col);
  RDPlayDeck *deck=button->playDeck();
  if(deck!=NULL) {
    deck->play(deck->currentPosition());
    if(button->hookMode()) {
      button->setStartTime(QTime::currentTime().
			   addMSecs(panel_station->timeOffset()).
			   addMSecs(-deck->currentPosition()+
				    deck->cut()->hookStartPoint()));
    }
    else {
      button->setStartTime(QTime::currentTime().
			   addMSecs(panel_station->timeOffset()).
			   addMSecs(-deck->currentPosition()));
    }
    return;
  }

  int cartnum=0;

  if((cartnum=button->cart())==0) {
    LogLine(QString().sprintf("Tried to start empty button.  Row=%d, Col=%d",
			      edit_row,edit_col));
    return;
  }
  RDCart *cart=new RDCart(cartnum);
  if(!cart->exists()) {
    delete cart;
    LogLine(QString().sprintf("Tried to start non-existent cart: %u",cartnum));
    return;
  }
  button->setStartSource(src);
  if(panel_pause_enabled) {
    button->setPauseWhenFinished(pause_when_finished);
    }
  else {
    button->setPauseWhenFinished(false);
    }
  switch(cart->type()) {
      case RDCart::Audio:
	PlayAudio(button,cart,hookmode,mport);
	break;

      case RDCart::Macro:
	PlayMacro(button,cart);
	break;

      default:
	break;
  }
  delete cart;
}


bool RDSoundPanel::PlayAudio(RDPanelButton *button,RDCart *cart,bool hookmode,int mport)
{
  RDLogLine logline;

  bool timescale=false;
  int button_deck=GetFreeButtonDeck();
  if(button_deck<0) {
    LogLine(QString().
	    sprintf("No button deck available, playout aborted.  Cart=%u",
		    cart->number()));
    return false;
  }
  if(mport<=0 || mport>PANEL_MAX_OUTPUTS) {
    button->setOutput(GetFreeOutput());
    }
  else {
    button->setOutput(mport-1);
    }
  button->setOutputText(panel_output_text[button->output()]);
  button->setHookMode(hookmode);
  button->setPlayDeck(new RDPlayDeck(panel_cae,button_deck,this));
  button->playDeck()->setCard(panel_card[button->output()]);
  button->playDeck()->setPort(panel_port[button->output()]);
  button->playDeck()->duckVolume(button->duckVolume(),0);
  if(panel_timescaling_supported[panel_card[button->output()]]&&
     cart->enforceLength()) {
    timescale=true;
  }
  logline.loadCart(cart->number(),RDLogLine::Play,0,timescale);
  if(!button->playDeck()->setCart(&logline,true)) {
    delete button->playDeck();
    button->setPlayDeck(NULL);
    LogLine(QString().
	    sprintf("No CAE stream available, playout aborted.  Cart=%u",
		    cart->number()));
    return false;
  }
  button->setCutName(logline.cutName());
  panel_active_buttons[button_deck]=button;

  //
  // Set Mappings
  //
  connect(button->playDeck(),SIGNAL(stateChanged(int,RDPlayDeck::State)),
	  this,SLOT(stateChangedData(int,RDPlayDeck::State)));
  connect(button->playDeck(),SIGNAL(hookEnd(int)),
	  this,SLOT(hookEndData(int)));
  connect(this,SIGNAL(tick()),button,SLOT(tickClock()));
  
  //
  // Calculate Start Parameters for Hook Playout
  //
  int start_pos=0;
  int segue_start=-1;
  int segue_end=-1;
  if(hookmode&&(logline.hookStartPoint()>=0)&&(logline.hookEndPoint()>=0)) {
    start_pos=logline.hookStartPoint()-logline.startPoint();
    segue_start=logline.hookEndPoint()-logline.startPoint();
    segue_end=logline.hookEndPoint()-logline.startPoint();
  }

  //
  // Start Playout
  //
  button->
    setStartTime(QTime::currentTime().addMSecs(panel_station->timeOffset()));
  if(hookmode&&(button->playDeck()->cut()->hookStartPoint()>=0)) {
    button->setActiveLength(button->playDeck()->cut()->hookEndPoint()-
      button->playDeck()->cut()->hookStartPoint());
  }
  else {
    if(timescale) {
      button->setActiveLength(cart->forcedLength());
    }
    else {
      button->setActiveLength(button->playDeck()->cut()->length());
    }
  }
  button->playDeck()->play(start_pos,segue_start,segue_end);
  panel_event_player->
    exec(logline.resolveWildcards(panel_start_rml[button->output()]));
  emit channelStarted(button->output(),button->playDeck()->card(),
		      button->playDeck()->port());
  return true;
}


void RDSoundPanel::PlayMacro(RDPanelButton *button,RDCart *cart)
{
  RDMacro rml;
  rml.setRole(RDMacro::Cmd);
  rml.setAddress(panel_station->address());
  rml.setEchoRequested(false);
  rml.setCommand(RDMacro::EX);
  rml.setArgQuantity(1);
  rml.setArg(0,cart->number());
  panel_ripc->sendRml(&rml);
  if(!panel_svcname.isEmpty()) {
    LogTrafficMacro(button);
  }
  if(button->pauseWhenFinished() && panel_pause_enabled) {
    button->setState(true);
    button->resetCounter();
    button->setColor(RDPANEL_PAUSED_BACKGROUND_COLOR);
  }
}


void RDSoundPanel::PauseButton(RDAirPlayConf::PanelType type,int panel,
			       int row,int col,int mport)
{
	for(int i=0;i<panel_button_columns;i++) {
		for(int j=0;j<panel_button_rows;j++) {
      RDPlayDeck *deck=
        panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->playDeck();
      if(deck!=NULL && (row==j || row==-1) && (col==i || col==-1)) {
        if(mport==-1 || 
           mport==panel_buttons[PanelOffset(type,panel)]->
	   panelButton(j,i)->outputText().toInt()) {
          deck->pause();

          panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->
             setStartTime(QTime());
          }
       }
     }
   }
}


void RDSoundPanel::StopButton(RDAirPlayConf::PanelType type,int panel,
			    int row,int col,int mport,
                            bool pause_when_finished,int fade_out)
{
  int edit_mport=mport;
  if (edit_mport==0) {
    edit_mport=-1;
    }
    for(int i=0;i<panel_button_columns;i++) {
	    for(int j=0;j<panel_button_rows;j++) {
      RDPlayDeck *deck=
        panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->playDeck();
      if((row==j || row==-1) && (col==i || col==-1)) {
        if(deck!=NULL) {
          if(edit_mport==-1 || 
             edit_mport==panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->
                     outputText().toInt()) {
            if(panel_pause_enabled) {
              panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->
                    setPauseWhenFinished(pause_when_finished);
              }
            else {
              panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->
                    setPauseWhenFinished(false);
              }
            switch(deck->state()) {
	        case RDPlayDeck::Playing:
	          deck->stop(fade_out,RD_FADE_DEPTH);
	          break;

  	        case RDPlayDeck::Paused:
	          deck->clear();
	          break;

	        default:
	          deck->clear();
	          break;
            }
          }
        }
      else {
        if(!pause_when_finished && panel_pause_enabled) {
          panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->setState(false); 
          panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->
              setPauseWhenFinished(false); 
          panel_buttons[PanelOffset(type,panel)]->panelButton(j,i)->reset(); 
          }
        }
      }
    }
  }
  panel_reset_mode=false;
  panel_reset_button->setFlashingEnabled(false);
  panel_all_button->hide();
  panel_setup_button->show();
}


void RDSoundPanel::StopButton(int id)
{
  RDPlayDeck *deck=panel_active_buttons[id]->playDeck();
  StopButton(deck);
}


void RDSoundPanel::StopButton(RDPlayDeck *deck)
{
  if(deck!=NULL) {
    switch(deck->state()) {
	case RDPlayDeck::Playing:
	  deck->stop();
	  break;

	case RDPlayDeck::Paused:
	  deck->clear();
	  break;

	default:
	  break;
    }
  }    
}


void RDSoundPanel::LoadPanels()
{
  for(unsigned i=0;i<panel_buttons.size();i++) {
    delete panel_buttons[i];
  }
  panel_buttons.clear();

  //
  // Load Buttons
  //
  for(int i=0;i<panel_station_panels;i++) {
    panel_buttons.push_back(new RDButtonPanel(panel_type,i,panel_button_columns,
					      panel_button_rows,
					      panel_station,panel_flash,this));
    for(int j=0;j<panel_button_rows;j++) {
      for(int k=0;k<panel_button_columns;k++) {
	connect(panel_buttons.back()->panelButton(j,k),SIGNAL(clicked()),
		panel_mapper,SLOT(map()));
	panel_mapper->setMapping(panel_buttons.back()->panelButton(j,k),
			   j*panel_button_columns+k);
      }
    }
    LoadPanel(RDAirPlayConf::StationPanel,i);
    panel_buttons.back()->setAllowDrags(panel_station->enableDragdrop());
  }
  for(int i=0;i<panel_user_panels;i++) {
    panel_buttons.push_back(new RDButtonPanel(panel_type,i,panel_button_columns,
					      panel_button_rows,
					      panel_station,panel_flash,this));
    for(int j=0;j<panel_button_rows;j++) {
      for(int k=0;k<panel_button_columns;k++) {
	connect(panel_buttons.back()->panelButton(j,k),SIGNAL(clicked()),
		panel_mapper,SLOT(map()));
	panel_mapper->setMapping(panel_buttons.back()->panelButton(j,k),
			   j*panel_button_columns+k);
      }
    }
    panel_buttons.back()->setAllowDrags(panel_station->enableDragdrop());
    LoadPanel(RDAirPlayConf::UserPanel,i);
  }
}


void RDSoundPanel::LoadPanel(RDAirPlayConf::PanelType type,int panel)
{
  QString owner;
  int offset=0;

  switch(type) {
      case RDAirPlayConf::UserPanel:
	if(panel_user==NULL) {
	  return;
	}
	owner=panel_user->name();
	offset=panel_station_panels+panel;
	break;

      case RDAirPlayConf::StationPanel:
	owner=panel_station->name();
	offset=panel;
	break;
  }

  QString sql=QString("select ")+panel_tablename+".ROW_NO,"+
    panel_tablename+".COLUMN_NO,"+
    panel_tablename+".LABEL,"+
    panel_tablename+".CART,"+
    panel_tablename+".DEFAULT_COLOR,"+
    "CART.FORCED_LENGTH,CART.AVERAGE_HOOK_LENGTH,CART.TYPE from "+
    panel_tablename+" left join CART on "+panel_tablename+".CART=CART.NUMBER "+
    "where "+panel_tablename+QString().sprintf(".TYPE=%d && ",type)+
    panel_tablename+".OWNER=\""+RDEscapeString(owner)+"\" && "+
    panel_tablename+QString().sprintf(".PANEL_NO=%d ",panel)+
    "order by "+panel_tablename+".COLUMN_NO,"+panel_tablename+".ROW_NO";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    if(panel_buttons[offset]->panelButton(q->value(0).toInt(),
	      q->value(1).toInt())->playDeck()==NULL) {
      panel_buttons[offset]->
	panelButton(q->value(0).toInt(),q->value(1).toInt())->
	setText(q->value(2).toString());
      panel_buttons[offset]->
	panelButton(q->value(0).toInt(),q->value(1).toInt())->
	setCart(q->value(3).toInt());
      panel_buttons[offset]->
	panelButton(q->value(0).toInt(),q->value(1).toInt())->
	setLength(false,q->value(5).toInt());
      panel_buttons[offset]->
	panelButton(q->value(0).toInt(),q->value(1).toInt())->
	setLength(true,q->value(6).toInt());
      if((panel_playmode_box!=NULL)&&(panel_playmode_box->currentItem()==1)&&
	 (q->value(6).toUInt()>0)) {
	panel_buttons[offset]->
	  panelButton(q->value(0).toInt(),q->value(1).toInt())->
	  setActiveLength(q->value(6).toInt());
      }
      else {
	if(q->value(7).toInt()==RDCart::Macro) {
	  panel_buttons[offset]->
	    panelButton(q->value(0).toInt(),q->value(1).toInt())->
	    setActiveLength(q->value(5).toInt());
	}
	else {
	  if(q->value(5).toInt()>0) {
	    panel_buttons[offset]->
	      panelButton(q->value(0).toInt(),q->value(1).toInt())->
	      setActiveLength(q->value(5).toInt());
	  }
	  else {
	    panel_buttons[offset]->
	      panelButton(q->value(0).toInt(),q->value(1).toInt())->
	      setActiveLength(-1);
	  }
	}
      }
      if(q->value(4).toString().isEmpty()) {
	panel_buttons[offset]->
	  panelButton(q->value(0).toInt(),q->value(1).toInt())->
	  setColor(palette().active().background());
	panel_buttons[offset]->
	  panelButton(q->value(0).toInt(),q->value(1).toInt())->
	  setDefaultColor(palette().active().background());
      }
      else {
	panel_buttons[offset]->
	  panelButton(q->value(0).toInt(),q->value(1).toInt())->
	  setColor(QColor(q->value(4).toString()));
	panel_buttons[offset]->
	  panelButton(q->value(0).toInt(),q->value(1).toInt())->
	  setDefaultColor(QColor(q->value(4).toString()));
      }
    }
  }
  delete q;
}


void RDSoundPanel::SaveButton(RDAirPlayConf::PanelType type,
			    int panel,int row,int col)
{
  QString sql;
  QString sql1;
  RDSqlQuery *q;
  QString owner;
  int offset=0;

  switch(type) {
      case RDAirPlayConf::UserPanel:
	owner=panel_user->name();
	offset=panel_station_panels+panel;
	break;

      case RDAirPlayConf::StationPanel:
	owner=panel_station->name();
	offset=panel;
	break;
  }

  //
  // Determine if the button exists
  //
  sql=QString("select LABEL from ")+panel_tablename+" where "+
    QString().sprintf("TYPE=%d && ",type)+
    "OWNER=\""+RDEscapeString(owner)+"\" && "+
    QString().sprintf("PANEL_NO=%d && ",panel)+
    QString().sprintf("ROW_NO=%d && ",row)+
    QString().sprintf("COLUMN_NO=%d",col);
  q=new RDSqlQuery(sql);
  if(q->size()>0) {
    //
    // If so, update the record
    //
    delete q;
    sql1=QString("update ")+panel_tablename+" set "+
      "LABEL=\""+RDEscapeString(panel_buttons[offset]->panelButton(row,col)->
				text())+"\","+
      QString().sprintf("CART=%d,",
			panel_buttons[PanelOffset(panel_type,panel_number)]->
			panelButton(row,col)->cart())+
      "DEFAULT_COLOR=\""+panel_buttons[offset]->panelButton(row,col)->
      defaultColor().name()+"\" where "+
      QString().sprintf("(TYPE=%d)&&",type)+
      "(OWNER=\""+RDEscapeString(owner)+"\")&&"+
      QString().sprintf("(PANEL_NO=%d)&&",panel)+
      QString().sprintf("(ROW_NO=%d)&&",row)+
      QString().sprintf("(COLUMN_NO=%d)",col);
    q=new RDSqlQuery(sql1);
    if(q->isActive()) {
      delete q;
      return;
    }
    delete q;
  }
  else {
    delete q;
    
    //
    // Otherwise, insert a new one
    //
    sql1=QString("insert into ")+panel_tablename+
      " (TYPE,OWNER,PANEL_NO,ROW_NO,COLUMN_NO,LABEL,CART,DEFAULT_COLOR) "+
      QString().sprintf("values (%d,",type)+
      "\""+RDEscapeString(owner)+"\","+
      QString().sprintf("%d,%d,%d,",panel,row,col)+
      "\""+RDEscapeString(panel_buttons[offset]->
			  panelButton(row,col)->text())+"\","+
      QString().sprintf("%d,",
			panel_buttons[PanelOffset(panel_type,panel_number)]->
			panelButton(row,col)->cart())+
      "\""+RDEscapeString(panel_buttons[offset]->
			  panelButton(row,col)->defaultColor().name())+"\")";
    q=new RDSqlQuery(sql1);
    delete q;
  }
}


int RDSoundPanel::PanelOffset(RDAirPlayConf::PanelType type,int panel)
{
  switch(type) {
      case RDAirPlayConf::StationPanel:
	return panel;
	break;

      case RDAirPlayConf::UserPanel:
	return panel_station_panels+panel;
	break;
  }
  return 0;
}


int RDSoundPanel::GetFreeButtonDeck()
{
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(panel_active_buttons[i]==NULL) {
      return i;
    }
  }
  return -1;
}


int RDSoundPanel::GetFreeOutput()
{
  bool active=false;

  for(int i=0;i<PANEL_MAX_OUTPUTS;i++) {
    active=false;
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      if((panel_active_buttons[j]!=NULL)&&
	 (panel_active_buttons[j]->output()==i)) {
	active=true;
      }
    }
    if(!active) {
      return i;
    }
  }
  return PANEL_MAX_OUTPUTS-1;
}


void RDSoundPanel::LogPlayEvent(unsigned cartnum,int cutnum)
{
  RDCut *cut=new RDCut(QString().sprintf("%06u_%03d",cartnum,cutnum));
  cut->logPlayout();
  delete cut;
}


void RDSoundPanel::LogTraffic(RDPanelButton *button)
{
  if(panel_svcname.isEmpty()) {
    return;
  }

  QString sql;
  RDSqlQuery *q;
  QDateTime datetime(QDate::currentDate(),QTime::currentTime());

  sql=QString("select CART.TITLE,CART.ARTIST,CART.PUBLISHER,")+
    "CART.COMPOSER,CART.USAGE_CODE,CUTS.ISRC,"+
    "CART.ALBUM,CART.LABEL,CUTS.ISCI,CART.CONDUCTOR,CART.USER_DEFINED,"
    "CART.SONG_ID from CART left join CUTS "+
    "on CART.NUMBER=CUTS.CART_NUMBER where "+
    "CUTS.CUT_NAME=\""+RDEscapeString(button->cutName())+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString("insert into `")+panel_svcname+"_SRT` set "+
      QString().sprintf("LENGTH=%d,",button->startTime().
			msecsTo(datetime.time()))+
      QString().sprintf("CART_NUMBER=%u,",button->cart())+
      "STATION_NAME=\""+RDEscapeString(panel_station->name().utf8())+"\","+
      "EVENT_DATETIME=\""+datetime.toString("yyyy-MM-dd")+" "+
      button->startTime().toString("hh:mm:ss")+"\","+
      QString().sprintf("EVENT_TYPE=%d,",RDAirPlayConf::TrafficStop)+
      QString().sprintf("EVENT_SOURCE=%d,",RDLogLine::SoundPanel)+
      QString().sprintf("PLAY_SOURCE=%d,",RDLogLine::SoundPanel)+
      QString().sprintf("CUT_NUMBER=%d,",button->cutName().right(3).toInt())+
      "TITLE=\""+RDEscapeString(q->value(0).toString().utf8())+"\","+
      "ARTIST=\""+RDEscapeString(q->value(1).toString().utf8())+"\","+
      "PUBLISHER=\""+RDEscapeString(q->value(2).toString().utf8())+"\","+
      "COMPOSER=\""+RDEscapeString(q->value(3).toString().utf8())+"\","+
      QString().sprintf("USAGE_CODE=%d,",q->value(4).toInt())+
      "ISRC=\""+RDEscapeString(q->value(5).toString().utf8())+"\","+
      QString().sprintf("START_SOURCE=%d,",button->startSource())+
      "ALBUM=\""+RDEscapeString(q->value(6).toString().utf8())+"\","+
      "LABEL=\""+RDEscapeString(q->value(7).toString().utf8())+"\","+
      "ISCI=\""+RDEscapeString(q->value(8).toString().utf8())+"\","+
      "CONDUCTOR=\""+RDEscapeString(q->value(9).toString().utf8())+"\","+
      "USER_DEFINED=\""+RDEscapeString(q->value(10).toString().utf8())+"\","+
      "SONG_ID=\""+RDEscapeString(q->value(11).toString().utf8())+"\","+
      "ONAIR_FLAG=\""+RDYesNo(panel_onair_flag)+"\"";
    delete q;
    q=new RDSqlQuery(sql);
  }
  delete q;
}


void RDSoundPanel::LogTrafficMacro(RDPanelButton *button)
{
  QString sql;
  RDSqlQuery *q;
  QDateTime datetime(QDate::currentDate(),QTime::currentTime());

  sql=QString("select TITLE,ARTIST,PUBLISHER,COMPOSER,USAGE_CODE,")+
    "FORCED_LENGTH,ALBUM,LABEL from CART where "+
    QString().sprintf("NUMBER=%u",button->cart());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString("insert into `")+panel_svcname+"_SRT` set "+
      QString().sprintf("LENGTH=%d,",q->value(5).toUInt())+
      QString().sprintf("CART_NUMBER=%u,",button->cart())+
      "STATION_NAME=\""+RDEscapeString(panel_station->name().utf8())+"\","+
      "EVENT_DATETIME=\""+datetime.toString("yyyy-MM-dd hh:mm:ss")+"\","+
      QString().sprintf("EVENT_TYPE=%d,",RDAirPlayConf::TrafficMacro)+
      QString().sprintf("EVENT_SOURCE=%d,",RDLogLine::SoundPanel)+
      QString().sprintf("PLAY_SOURCE=%d,",RDLogLine::SoundPanel)+
      "TITLE=\""+RDEscapeString(q->value(0).toString().utf8())+"\","+
      "ARTIST=\""+RDEscapeString(q->value(1).toString().utf8())+"\","+
      "PUBLISHER=\""+RDEscapeString(q->value(2).toString().utf8())+"\","+
      "COMPOSER=\""+RDEscapeString(q->value(3).toString().utf8())+"\","+
      QString().sprintf("USAGE_CODE=%d,",q->value(4).toInt())+
      QString().sprintf("START_SOURCE=%d,",button->startSource())+
      "ALBUM=\""+RDEscapeString(q->value(6).toString().utf8())+"\","+
      "LABEL=\""+RDEscapeString(q->value(7).toString().utf8())+"\","+
      "ONAIR_FLAG=\""+RDYesNo(panel_onair_flag)+"\"";
    delete q;
    q=new RDSqlQuery(sql);
    delete q;
  }
}


void RDSoundPanel::LogLine(QString str)
{
  FILE *file;

  if(panel_logfile.isEmpty()) {
    return;
  }

  QDateTime current=QDateTime::currentDateTime();
  if((file=fopen(panel_logfile,"a"))==NULL) {
    return;
  }
  fprintf(file,"%02d/%02d/%4d - %02d:%02d:%02d.%03d : RDSoundPanel: %s\n",
	  current.date().month(),
	  current.date().day(),
	  current.date().year(),
	  current.time().hour(),
	  current.time().minute(),
	  current.time().second(),
	  current.time().msec(),
	  (const char *)str);
  fclose(file);
}


void RDSoundPanel::Playing(int id)
{
  if(panel_active_buttons[id]==NULL) {
    LogLine(QString().sprintf("Invalid ID=%d in RDSoundPanel::Playing()",
			      id));
    return;
  }
  panel_active_buttons[id]->setState(true);
  panel_active_buttons[id]->setColor(RDPANEL_PLAY_BACKGROUND_COLOR);
  LogPlayEvent(panel_active_buttons[id]->playDeck()->cart()->number(),
	       panel_active_buttons[id]->playDeck()->cut()->cutNumber());
  LogLine(QString().
	  sprintf("Playout started: id=%d  cart=%u  cut=%d",
		  id,panel_active_buttons[id]->playDeck()->cart()->number(),
		  panel_active_buttons[id]->playDeck()->cut()->cutNumber()));
}


void RDSoundPanel::Paused(int id)
{
  if(panel_active_buttons[id]==NULL) {
    LogLine(QString().sprintf("Invalid ID=%d in RDSoundPanel::Paused()",
			      id));
    return;
  }
  panel_active_buttons[id]->setState(true);
  panel_active_buttons[id]->setColor(RDPANEL_PAUSED_BACKGROUND_COLOR);
  LogLine(QString().
	  sprintf("Playout paused: id=%d  cart=%u  cut=%d",
		  id,panel_active_buttons[id]->playDeck()->cart()->number(),
		  panel_active_buttons[id]->playDeck()->cut()->cutNumber()));
}


void RDSoundPanel::Stopped(int id)
{
  if(panel_active_buttons[id]==NULL) {
    LogLine(QString().sprintf("Invalid ID=%d in RDSoundPanel::Stopped()",
			      id));
    return;
  }
  LogTraffic(panel_active_buttons[id]);
  ClearChannel(id);
  if(panel_active_buttons[id]->pauseWhenFinished()) {
    panel_active_buttons[id]->setState(true);
    panel_active_buttons[id]->setColor(RDPANEL_PAUSED_BACKGROUND_COLOR);
    panel_active_buttons[id]->resetCounter();
    }
  else {
    panel_active_buttons[id]->setState(false);
    panel_active_buttons[id]->setHookMode(panel_playmode_box->currentItem()==1);
  }
  disconnect(this,SIGNAL(tick()),panel_active_buttons[id],SLOT(tickClock()));
  panel_active_buttons[id]->playDeck()->disconnect();
  delete panel_active_buttons[id]->playDeck();
  panel_active_buttons[id]->setPlayDeck(NULL);
  if(!panel_active_buttons[id]->pauseWhenFinished()) {
    panel_active_buttons[id]->reset();
    }
  panel_active_buttons[id]->setDuckVolume(0);
  panel_active_buttons[id]=NULL;
  LogLine(QString().sprintf("Playout stopped: id=%d",id));
}


void RDSoundPanel::ClearChannel(int id)
{
  RDPlayDeck *playdeck=panel_active_buttons[id]->playDeck();
  if(panel_cae->
     playPortActive(playdeck->card(),playdeck->port(),playdeck->stream())) {
    return;
  }
  panel_event_player->exec(panel_stop_rml[panel_active_buttons[id]->output()]);
  emit channelStopped(panel_active_buttons[id]->output(),
		      playdeck->card(),playdeck->port());
}


void RDSoundPanel::ClearReset()
{
  panel_reset_mode=false;
  panel_reset_button->setFlashingEnabled(false);
  panel_setup_button->setEnabled(true);
}


QString RDSoundPanel::PanelTag(int index)
{
  if(index<panel_station_panels) {
    return QString().sprintf("S:%d",index+1);
  }
  return QString().sprintf("U:%d",index-panel_station_panels+1);
}


QString RDSoundPanel::PanelOwner(RDAirPlayConf::PanelType type)
{
  switch(type) {
      case RDAirPlayConf::StationPanel:
	return panel_station->name();

      case RDAirPlayConf::UserPanel:
	if(panel_user!=NULL) {
	  return panel_user->name();
	}
  }
  return QString();
}
