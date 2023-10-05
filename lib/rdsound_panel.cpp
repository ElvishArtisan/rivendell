// rdsound_panel.cpp
//
// The sound panel widget for RDAirPlay
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdapplication.h"
#include "rdbutton_dialog.h"
#include "rdcut.h"
#include "rdconf.h"
#include "rddb.h"
#include "rdedit_panel_name.h"
#include "rdescape_string.h"
#include "rdlog_line.h"
#include "rdmacro.h"
#include "rdsound_panel.h"
#include "rdweb.h"

RDSoundPanel::RDSoundPanel(int station_panels,int user_panels,bool flash,
			   const QString &caption,const QString &label_template,
			   bool extended,RDEventPlayer *player,
			   RDCartDialog *cart_dialog,QWidget *parent)
  : RDWidget(parent)
{
  panel_started=false;
  panel_current_panel=NULL;
  panel_dump_panel_updates=false;
  panel_playmode_box=NULL;
  panel_button_columns=PANEL_MAX_BUTTON_COLUMNS;
  panel_button_rows=PANEL_MAX_BUTTON_ROWS;
  panel_cue_port=-1;
  panel_caption=caption;
  if(extended) {
    panel_tablename="`EXTENDED_PANELS`";
    panel_name_tablename="`EXTENDED_PANEL_NAMES`";
  }
  else {
    panel_tablename="`PANELS`";
    panel_name_tablename="`PANEL_NAMES`";
  }
  panel_label_template=label_template;

  panel_type=RDAirPlayConf::StationPanel;
  panel_number=0;
  panel_setup_mode=false;
  panel_reset_mode=false;
  panel_parent=parent;
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
  for(unsigned i=0;i<RD_SOUNDPANEL_MAX_OUTPUTS;i++) {
    panel_card[i]=-1;
    panel_port[i]=-1;
  }
  panel_cart_dialog=cart_dialog;
  for(int i=0;i<RD_SOUNDPANEL_MAX_OUTPUTS;i++) {
    panel_timescaling_supported[i]=false;
  }
  panel_onair_flag=false;

  //
  // Load Buttons
  //
  UpdatePanels("");  // Load Host (Station) Panels

  //
  // Panel Selector
  //
  panel_selector_box=new RDComboBox(this);
  panel_selector_box->setFont(buttonFont());
  panel_selector_box->addIgnoredKey(Qt::Key_Space);
  connect(panel_selector_box,SIGNAL(activated(int)),
	  this,SLOT(panelActivatedData(int)));
  connect(panel_selector_box,SIGNAL(setupClicked()),
	  this,SLOT(panelSetupData()));

  //
  // Play Mode Box
  //
  panel_playmode_box=new QComboBox(this);
  panel_playmode_box->setFont(buttonFont());
  connect(panel_playmode_box,SIGNAL(activated(int)),
	  this,SLOT(playmodeActivatedData(int)));
  panel_playmode_box->insertItem(panel_playmode_box->count(),tr("Play All"));
  panel_playmode_box->insertItem(panel_playmode_box->count(),tr("Play Hook"));

  //
  // Reset Button
  //
  panel_reset_button=new RDPushButton(this);
  panel_reset_button->setFont(buttonFont());
  panel_reset_button->setText(tr("Reset"));
  panel_reset_button->setFlashColor(QColor(RDPANEL_RESET_FLASH_COLOR));
  panel_reset_button->setFocusPolicy(Qt::NoFocus);
  connect(panel_reset_button,SIGNAL(clicked()),this,SLOT(resetClickedData()));

  //
  // All Button
  //
  panel_all_button=new RDPushButton(this);
  panel_all_button->setFont(buttonFont());
  panel_all_button->setText(tr("All"));
  panel_all_button->setFlashColor(QColor(RDPANEL_RESET_FLASH_COLOR));
  panel_all_button->setFocusPolicy(Qt::NoFocus);
  panel_all_button->hide();
  connect(panel_all_button,SIGNAL(clicked()),this,SLOT(allClickedData()));

  //
  // Setup Button
  //
  panel_setup_button=new RDPushButton(this);
  panel_setup_button->setFont(buttonFont());
  panel_setup_button->setText(tr("Setup"));
  panel_setup_button->setFlashColor(QColor(RDPANEL_SETUP_FLASH_COLOR));
  panel_setup_button->setFocusPolicy(Qt::NoFocus);
  connect(panel_setup_button,SIGNAL(clicked()),this,SLOT(setupClickedData()));

  //
  // Button Dialog Box
  //
  panel_button_dialog=
    new RDButtonDialog(rda->station()->name(),panel_caption,
		       panel_label_template,panel_cart_dialog,panel_svcname,
		       this);

  //
  // CAE Setup
  //
  connect(rda->cae(),SIGNAL(timescalingSupported(int,bool)),
	  this,SLOT(timescalingSupportedData(int,bool)));

  //
  // RIPC Setup
  //
  connect(rda->ripc(),SIGNAL(onairFlagChanged(bool)),
	  this,SLOT(onairFlagChangedData(bool)));

  //
  // Load Panel Names
  //
  QString sql;
  sql=QString("select ")+
    "`PANEL_NO`,"+
    "`NAME` "+
    "from "+panel_name_tablename+" where "+
    QString::asprintf("(`TYPE`=%d)&&",RDAirPlayConf::StationPanel)+
    "(`OWNER`='"+RDEscapeString(rda->station()->name())+"') "+
    "order by `PANEL_NO`";
  RDSqlQuery *q=new RDSqlQuery(sql);
  q->first();
  for(int i=0;i<panel_station_panels;i++) {
    if(q->isValid()&&(q->value(0).toInt()==i)) {
      panel_selector_box->
	insertItem(QString::asprintf("[S:%d] ",i+1)+q->value(1).toString());
      q->next();
    }
    else {
      panel_selector_box->insertItem(QString::asprintf("[S:%d] Panel S:%d",
						       i+1,i+1));
    }
  }
  delete q;
  for(int i=0;i<panel_user_panels;i++) {
    panel_selector_box->insertItem(QString::asprintf("[U:%d] Panel U:%d",
						     i+1,i+1));
  }
  panel_selector_box->setFocus();
  setDisabled((panel_station_panels==0)&&(panel_user_panels==0));
}


RDSoundPanel::~RDSoundPanel()
{
  for(QMap<QString,QList<RDButtonPanel *> >::const_iterator it=panel_arrays.begin();it!=panel_arrays.end();it++) {
    for(int i=0;i<it.value().size();i++) {
      delete it.value().at(i);
    }
  }
  panel_arrays.clear();
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


bool RDSoundPanel::dumpPanelUpdates() const
{
  return panel_dump_panel_updates;
}


void RDSoundPanel::setDumpPanelUpdates(bool state)
{
  panel_dump_panel_updates=state;
}


int RDSoundPanel::card(int outnum) const
{
  return panel_card[outnum];
}


void RDSoundPanel::setCard(int outnum,int card)
{
  panel_card[outnum]=card;
  rda->cae()->requestTimescale(card);
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


void RDSoundPanel::setSvcName(const QString &svcname)
{
  panel_svcname=svcname;
  panel_svcname.replace(" ","_");
}


void RDSoundPanel::play(RDAirPlayConf::PanelType type,int panel,
			int row, int col,RDLogLine::StartSource src,int mport,
                        bool pause_when_finished)
{
  PlayButton(type,panel,row,col,src,panel_playmode_box->currentIndex()==1,
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
  QString username;
  if(type==RDAirPlayConf::UserPanel) {
    username=rda->user()->name();
  }
  RDPanelButton *button=
    panel_arrays.value(username).at(panel)->panelButton(row,col);
  button->setText(str);
  SaveButton(type,panel,row,col);
}


void RDSoundPanel::setColor(RDAirPlayConf::PanelType type,int panel,int row,
			    int col,const QColor &color)
{
  QString username;
  if(type==RDAirPlayConf::UserPanel) {
    username=rda->user()->name();
  }
  RDPanelButton *button=
    panel_arrays.value(username).at(panel)->panelButton(row,col);
  button->setDefaultColor(color);
  SaveButton(type,panel,row,col);
}


void RDSoundPanel::duckVolume(RDAirPlayConf::PanelType type,int panel,int row,
			      int col,int level,int fade,int mport)
{
  QString username;
  if(type==RDAirPlayConf::UserPanel) {
    username=rda->user()->name();
  }
  int edit_mport=mport;
  if (edit_mport==0) {
    edit_mport=-1;
    }
    for(int i=0;i<panel_button_columns;i++) {
	    for(int j=0;j<panel_button_rows;j++) {
      RDPlayDeck *deck=
        panel_arrays.value(username).at(panel)->panelButton(j,i)->playDeck();
      if((row==j || row==-1) && (col==i || col==-1)) {
	if(mport==-1) {
	  panel_arrays.value(username).at(panel)->
	    panelButton(j,i)->setDuckVolume(level);
	}    
        if(deck!=NULL) {
          if(edit_mport==-1 || 
             edit_mport==
	     panel_arrays.value(username).at(panel)->
	     panelButton(j,i)->outputText().toInt()) {
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
    for(QMap<QString,QList<RDButtonPanel *> >::const_iterator it=panel_arrays.begin();it!=panel_arrays.end();it++) {
      for(int i=0;i<it.value().size();i++) {
	if(i<panel_station_panels &&
	   (!panel_config_panels) &&   
	   (mode==RDAirPlayConf::AddTo || mode==RDAirPlayConf::CopyTo || mode==RDAirPlayConf::DeleteFrom)) {
	  it.value().at(i)->setActionMode(RDAirPlayConf::Normal);
        }
	else {
	  it.value().at(i)->setActionMode(panel_action_mode);
	}
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


QString RDSoundPanel::json(const QString &owner,int padding,bool final) const
{
  QString ret;
  QList<RDButtonPanel *> panels=panel_arrays.value(owner);

  ret+=RDJsonPadding(padding)+"\"array\": {\r\n";
  ret+=RDJsonField("owner",owner,4+padding);

  for(int i=0;i<(panels.size()-1);i++) {
    ret+=panels.at(i)->json(4+padding);
  }
  if(panels.size()>0) {
    ret+=panels.last()->json(4+padding,true);
  }

  ret+=RDJsonPadding(padding)+"}";
  if(!final) {
    ret+=",";
  }
  ret+="\r\n";

  return ret;
}


QString RDSoundPanel::json(int padding) const
{
  QString ret;

  int count=0;
  for(QMap<QString,QList<RDButtonPanel *> >::const_iterator it=panel_arrays.
	begin();it!=panel_arrays.end();it++) {
    ret+=json(it.key(),4,++count==panel_arrays.size());
  }

  return ret;
}


void RDSoundPanel::setButton(RDAirPlayConf::PanelType type,int panel,
			     int row,int col,unsigned cartnum,
			     const QString &title)
{
  QString str;

  QString username;
  if(type==RDAirPlayConf::UserPanel) {
    username=rda->user()->name();
  }
  RDPanelButton *button=
    panel_arrays.value(username).at(panel)->panelButton(row,col);
  if(button->isActive()) {
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
      button->setHookMode(panel_playmode_box->currentIndex()==1);
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
	button->setText(str+QString::asprintf(" %06u",cartnum));
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
  panel_config_panels=rda->user()->configPanels();
  UpdatePanels(rda->user()->name());
  if(panel_dump_panel_updates) {
    printf("{\r\n%s}\r\n",json(4).toUtf8().constData());
  }

  //
  // Remove Old Panel Names
  //
  int current_item=panel_selector_box->currentIndex();
  for(int i=0;i<panel_user_panels;i++) {
    panel_selector_box->removeItem(panel_station_panels);
  }

  //
  // Load New Panel Names
  //
  QString sql;
  sql=QString("select ")+
    "`PANEL_NO`,"+
    "`NAME` "+
    "from "+panel_name_tablename+" where "+
    QString::asprintf("(`TYPE`=%d)&&",RDAirPlayConf::UserPanel)+
    "(`OWNER`='"+RDEscapeString(rda->user()->name())+"') "+
    "order by `PANEL_NO`";
  RDSqlQuery *q=new RDSqlQuery(sql);
  q->first();
  for(int i=0;i<panel_user_panels;i++) {
    if(q->isValid()&&(q->value(0).toInt()==i)) {
      panel_selector_box->
	insertItem(QString::asprintf("[U:%d] ",i+1)+q->value(1).toString());
      q->next();
    }
    else {
      panel_selector_box->
	insertItem(QString::asprintf("[U:%d] Panel U:%d",i+1,i+1));
    }
  }
  delete q;
  panel_selector_box->setCurrentIndex(current_item);

  if(!panel_started) {
    panelActivatedData(panel_selector_box->currentIndex());
    panel_started=true;
  }
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


void RDSoundPanel::panelUp()
{
  int index=panel_selector_box->currentIndex();

  if(index<(panel_selector_box->count()-1)) {
    panelActivatedData(index+1);
    panel_selector_box->setCurrentIndex(index+1);
  }
}


void RDSoundPanel::panelDown()
{
  int index=panel_selector_box->currentIndex();

  if(index>0) {
    panelActivatedData(index-1);
    panel_selector_box->setCurrentIndex(index-1);
  }
}


void RDSoundPanel::panelActivatedData(int n)
{
  QString username;
  if(panel_type==RDAirPlayConf::UserPanel) {
    username=rda->user()->name();
  }
  if(n<panel_station_panels) {
    panel_type=RDAirPlayConf::StationPanel;
    panel_number=n;
    username="";
  }
  else {
    panel_type=RDAirPlayConf::UserPanel;
    panel_number=n-panel_station_panels;
    username=rda->user()->name();
  }

  ShowPanel(panel_type,panel_number);
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
  ShowPanel(panel_type,panel_number);
}


void RDSoundPanel::setupClickedData()
{
  QString username;
  if(panel_type==RDAirPlayConf::UserPanel) {
    username=rda->user()->name();
  }
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
  if(rda->station()->enableDragdrop()&&(rda->station()->enforcePanelSetup())) {
    for(QMap<QString,QList<RDButtonPanel *> >::const_iterator it=
	  panel_arrays.begin();it!=panel_arrays.end();it++) {
      for(int i=0;i<it.value().size();i++) {
	it.value().at(i)->setAcceptDrops(panel_setup_mode);
      }
    }
  }
  panel_selector_box->setSetupMode(panel_setup_mode);
}


void RDSoundPanel::buttonClickedData(int pnum,int col,int row)
{
  if(panel_current_panel==NULL) {
    printf("NO CURRENT PANEL!\n");
    return;
  }
  RDPanelButton *button=panel_current_panel->panelButton(row,col);
  unsigned cartnum;

  switch(panel_action_mode) {
  case RDAirPlayConf::CopyFrom:
    if((cartnum=button->cart())>0) {
      emit selectClicked(cartnum,0,0);
    }
    break;
	
  case RDAirPlayConf::CopyTo:
    if(button->playDeck()==NULL
       && ((panel_type==RDAirPlayConf::UserPanel) || 
    	   panel_config_panels)) { 
      emit selectClicked(0,button->row(),button->column());
    }
    break;
	
  case RDAirPlayConf::AddTo:
    if(button->playDeck()==NULL
       && ((panel_type==RDAirPlayConf::UserPanel) || 
	   panel_config_panels)) { 
      emit selectClicked(0,button->row(),button->column());
    }
    break;
	
  case RDAirPlayConf::DeleteFrom:
    if(button->playDeck()==NULL
       && ((panel_type==RDAirPlayConf::UserPanel) || 
	   panel_config_panels)) { 
      emit selectClicked(0,button->row(),button->column());
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
	 exec(button,panel_playmode_box->currentIndex()==1,
	      rda->user()->name(),rda->user()->password())) {
	SaveButton(panel_type,panel_number,button->row(),button->column());
      }
    }
    else {
      RDPlayDeck *deck=button->playDeck();
      if(panel_reset_mode) {
	StopButton(panel_type,panel_number,button->row(),button->column());
      }
      else {
	if(deck==NULL) {
	  PlayButton(panel_type,panel_number,button->row(),button->column(),
		     RDLogLine::StartManual,
		     panel_playmode_box->currentIndex()==1);
	}
	else {
	  if(panel_pause_enabled) {
	    if(deck->state()!=RDPlayDeck::Paused) {
	      PauseButton(panel_type,panel_number,button->row(),button->column());
	    }
	    else {
	      PlayButton(panel_type,panel_number,button->row(),button->column(),
			 RDLogLine::StartManual,button->hookMode());
	    }
	  }
	  else {
	    StopButton(panel_type,panel_number,button->row(),button->column());
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
  for(unsigned i=0;i<RD_SOUNDPANEL_MAX_OUTPUTS;i++) {
    if(card==panel_card[i]) {
      panel_timescaling_supported[i]=state;
    }
  }
}


void RDSoundPanel::panelSetupData()
{
  if(rda->user()->configPanels()||(panel_type==RDAirPlayConf::UserPanel)) {
    QString sql;
    int cutpt=panel_selector_box->currentText().indexOf(" ");
    if(panel_selector_box->currentText().left(5)==tr("Panel")) {
      cutpt=-1;
    }
    QString tag=panel_selector_box->currentText().left(cutpt);
    
    QString panel_name=panel_selector_box->currentText().
      right(panel_selector_box->currentText().length()-cutpt-1);
    RDEditPanelName *edn=new RDEditPanelName(&panel_name);
    if(edn->exec()) {
      panel_selector_box->
	setItemText(panel_selector_box->currentIndex(),tag+" "+panel_name);
      panel_selector_box->
	setCurrentIndex(panel_selector_box->
			findText("["+PanelTag(panel_selector_box->
					      currentIndex())+"] "+panel_name));
      sql=QString("delete from ")+panel_name_tablename+" where "+
	QString::asprintf("(`TYPE`=%d)&&",panel_type)+
	"(`OWNER`='"+RDEscapeString(PanelOwner(panel_type))+"')&&"+
	QString::asprintf("(`PANEL_NO`=%d)",panel_number);
      RDSqlQuery::apply(sql);

      sql=QString("insert into ")+panel_name_tablename+" set "+
	QString::asprintf("`TYPE`=%d,",panel_type)+
	"`OWNER`='"+RDEscapeString(PanelOwner(panel_type))+"',"+
	QString::asprintf("`PANEL_NO`=%d,",panel_number)+
	"`NAME`='"+RDEscapeString(panel_name)+"'";
      RDSqlQuery::apply(sql);
    }
    delete edn;
  }
}


void RDSoundPanel::onairFlagChangedData(bool state)
{
  panel_onair_flag=state;
}


void RDSoundPanel::resizeEvent(QResizeEvent *e)
{
  //  int w=size().width();
  int h=size().height();
  
  for(QMap<QString,QList<RDButtonPanel *> >::const_iterator it=panel_arrays.begin();it!=panel_arrays.end();it++) {
    for(int i=0;i<it.value().size();i++) {
      it.value().at(i)->setGeometry(0,0,size().width()-5,size().height()-60);
    }
  }
  panel_selector_box->setGeometry(0,h-50,2*PANEL_BUTTON_SIZE_X+10,50);
  panel_playmode_box->setGeometry(2*PANEL_BUTTON_SIZE_X+15,h-50,
				  PANEL_BUTTON_SIZE_X+10,50);
  panel_reset_button->setGeometry(2*PANEL_BUTTON_SIZE_X+140,h-50,
				  PANEL_BUTTON_SIZE_X,50);
  panel_all_button->setGeometry(2*PANEL_BUTTON_SIZE_X+235,h-50,
				  PANEL_BUTTON_SIZE_X,50);
  panel_setup_button->setGeometry(2*PANEL_BUTTON_SIZE_X+235,h-50,
				  PANEL_BUTTON_SIZE_X,50);
}


void RDSoundPanel::wheelEvent(QWheelEvent *e)
{
  if(e->orientation()==Qt::Vertical) {
    if(e->delta()>0) {
      panelDown();
    }
    if(e->delta()<0) {
      panelUp();
    }
  }
  e->accept();
}


void RDSoundPanel::PlayButton(RDAirPlayConf::PanelType type,int panel,
		int row,int col,RDLogLine::StartSource src,bool hookmode,
		int mport,bool pause_when_finished)
{
  int edit_row=row;
  int edit_col=col;
  QString username;
  if(type==RDAirPlayConf::UserPanel) {
    username=rda->user()->name();
  }
  
  for(int i=0;i<panel_button_columns;i++) {
    for(int j=0;j<panel_button_rows;j++) {
      if(panel_arrays.value(username).at(panel)->panelButton(j,i)->cart()>0 && 
         panel_arrays.value(username).at(panel)->
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
    panel_arrays.value(username).at(panel)->panelButton(edit_row,edit_col);
  RDPlayDeck *deck=button->playDeck();
  if(deck!=NULL) {
    deck->play(deck->currentPosition());
    if(button->hookMode()) {
      button->setStartTime(QTime::currentTime().
			   addMSecs(rda->station()->timeOffset()).
			   addMSecs(-deck->currentPosition()+
				    deck->cut()->hookStartPoint()));
    }
    else {
      button->setStartTime(QTime::currentTime().
			   addMSecs(rda->station()->timeOffset()).
			   addMSecs(-deck->currentPosition()));
    }
    return;
  }

  int cartnum=0;

  if((cartnum=button->cart())==0) {
    LogLine(QString::asprintf("Tried to start empty button.  Row=%d, Col=%d",
			      edit_row,edit_col));
    return;
  }
  RDCart *cart=new RDCart(cartnum);
  if(!cart->exists()) {
    delete cart;
    LogLine(QString::asprintf("Tried to start non-existent cart: %u",cartnum));
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
  if(mport<=0 || mport>RD_SOUNDPANEL_MAX_OUTPUTS) {
    button->setOutput(GetFreeOutput());
    }
  else {
    button->setOutput(mport-1);
    }
  button->setOutputText(panel_output_text[button->output()]);
  button->setHookMode(hookmode);
  button->setPlayDeck(new RDPlayDeck(rda->cae(),button_deck,this));
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
    setStartTime(QTime::currentTime().addMSecs(rda->station()->timeOffset()));
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
  rml.setAddress(rda->station()->address());
  rml.setEchoRequested(false);
  rml.setCommand(RDMacro::EX);
  rml.addArg(cart->number());
  rda->ripc()->sendRml(&rml);
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
  QString username;
  if(type==RDAirPlayConf::UserPanel) {
    username=rda->user()->name();
  }
  for(int i=0;i<panel_button_columns;i++) {
    for(int j=0;j<panel_button_rows;j++) {
      RDPlayDeck *deck=
        panel_arrays.value(username).at(panel)->panelButton(j,i)->playDeck();
      if(deck!=NULL && (row==j || row==-1) && (col==i || col==-1)) {
        if(mport==-1 || 
           mport==panel_arrays.value(username).at(panel)->
	   panelButton(j,i)->outputText().toInt()) {
          deck->pause();
	  
          panel_arrays.value(username).at(panel)->
	    panelButton(j,i)->setStartTime(QTime());
	}
      }
    }
  }
}


void RDSoundPanel::StopButton(RDAirPlayConf::PanelType type,int panel,
			    int row,int col,int mport,
                            bool pause_when_finished,int fade_out)
{
  QString username;
  if(type==RDAirPlayConf::UserPanel) {
    username=rda->user()->name();
  }
  int edit_mport=mport;
  if (edit_mport==0) {
    edit_mport=-1;
    }
    for(int i=0;i<panel_button_columns;i++) {
	    for(int j=0;j<panel_button_rows;j++) {
      RDPlayDeck *deck=panel_arrays.value(username).at(panel)->
	panelButton(j,i)->playDeck();
      if((row==j || row==-1) && (col==i || col==-1)) {
        if(deck!=NULL) {
          if(edit_mport==-1 || 
             edit_mport==panel_arrays.value(username).
	     at(panel)->panelButton(j,i)->outputText().toInt()) {
            if(panel_pause_enabled) {
              panel_arrays.value(username).at(panel)->
		panelButton(j,i)->setPauseWhenFinished(pause_when_finished);
              }
            else {
              panel_arrays.value(username).at(panel)->
		panelButton(j,i)->setPauseWhenFinished(false);
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
          panel_arrays.value(username).at(panel)->
	    panelButton(j,i)->setState(false); 
          panel_arrays.value(username).at(panel)->
	    panelButton(j,i)->setPauseWhenFinished(false); 
          panel_arrays.value(username).at(panel)->panelButton(j,i)->reset(); 
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


void RDSoundPanel::UpdatePanels(const QString &username)
{
  QString owner=username;
  RDAirPlayConf::PanelType type=RDAirPlayConf::UserPanel;
  int max_panels=panel_user_panels;

  if(username.isEmpty()) {
    owner=rda->station()->name();
    type=RDAirPlayConf::StationPanel;
    max_panels=panel_station_panels;
  }

  //
  // Load the array if it isn't already loaded
  //
  if(max_panels>0) {
    QList<RDButtonPanel *> list;
    if(panel_arrays.value(username).size()>0) {
      list=panel_arrays.value(username);
    }
    for(int i=panel_arrays.value(username).size();i<max_panels;i++) {
      RDButtonPanel *panel=new RDButtonPanel(type,i,this);
      panel->setGeometry(0,0,size().width()-5,size().height()-60);
      connect(panel,SIGNAL(buttonClicked(int,int,int)),
	      this,SLOT(buttonClickedData(int,int,int)));
      panel->hide();
      list.push_back(panel);
    }
    panel_arrays[username]=list;
  }

  //
  // Update button attributes
  //
  QString sql=QString("select ")+
    panel_tablename+".`PANEL_NO`,"+       // 00
    panel_tablename+".`ROW_NO`,"+         // 01
    panel_tablename+".`COLUMN_NO`,"+      // 02
    panel_tablename+".`LABEL`,"+          // 03
    panel_tablename+".`CART`,"+           // 04
    panel_tablename+".`DEFAULT_COLOR`,"+  // 05
    "`CART`.`FORCED_LENGTH`,"+            // 06
    "`CART`.`AVERAGE_HOOK_LENGTH`,"+      // 07
    "`CART`.`TYPE` "+                     // 08
    "from "+panel_tablename+" "+          // 09
    "left join `CART` on "+panel_tablename+".`CART`=`CART`.`NUMBER` "+
    "where "+panel_tablename+QString::asprintf(".`TYPE`=%d && ",type)+
    panel_tablename+".`OWNER`='"+RDEscapeString(owner)+"' && "+
    panel_tablename+QString::asprintf(".`PANEL_NO`<%d ",max_panels)+
    "order by "+
    panel_tablename+".`PANEL_NO`,"+
    panel_tablename+".`COLUMN_NO`,"+
    panel_tablename+".`ROW_NO`";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    RDPanelButton *button=panel_arrays.value(username).at(q->value(0).toInt())->
      panelButton(q->value(1).toInt(),q->value(2).toInt());
    if(!button->isActive()) {
      button->setText(q->value(3).toString());
      button->setCart(q->value(4).toInt());
      button->setLength(false,q->value(6).toInt());
      button->setLength(true,q->value(7).toInt());
      if((panel_playmode_box!=NULL)&&(panel_playmode_box->currentIndex()==1)&&
	 (q->value(7).toUInt()>0)) {
	button->setActiveLength(q->value(7).toInt());
      }
      else {
	if(q->value(8).toInt()==RDCart::Macro) {
	  button->setActiveLength(q->value(6).toInt());
	}
	else {
	  if(q->value(6).toInt()>0) {
	    button->setActiveLength(q->value(6).toInt());
	  }
	  else {
	    button->setActiveLength(-1);
	  }
	}
      }
      if(q->value(5).toString().isEmpty()) {
	button->setColor(palette().color(QPalette::Background));
	button->setDefaultColor(palette().color(QPalette::Background));
      }
      else {
	button->setColor(QColor(q->value(5).toString()));
	button->setDefaultColor(QColor(q->value(5).toString()));
      }
    }
  }
  delete q;
}


void RDSoundPanel::ShowPanel(RDAirPlayConf::PanelType type,int offset)
{
  QString username;
  if(type==RDAirPlayConf::UserPanel) {
    username=rda->user()->name();
  }
  if(panel_arrays.value(username).size()>offset) {
    if(panel_current_panel!=NULL) {
      panel_current_panel->hide();
    }
    panel_arrays.value(username).at(offset)->show();
    panel_current_panel=panel_arrays.value(username).at(offset);
  }
}


void RDSoundPanel::SaveButton(RDAirPlayConf::PanelType type,
			    int panel,int row,int col)
{
  QString sql;
  QString sql1;
  RDSqlQuery *q;
  QString owner;
  QString username;
  if(type==RDAirPlayConf::UserPanel) {
    username=rda->user()->name();
  }

  switch(type) {
  case RDAirPlayConf::UserPanel:
    owner=rda->user()->name();
    break;

  case RDAirPlayConf::StationPanel:
    owner=rda->station()->name();
    break;
  }

  RDPanelButton *button=
    panel_arrays.value(username).at(panel)->panelButton(row,col);

  //
  // Determine if the button exists
  //
  sql=QString("select `LABEL` from ")+panel_tablename+" where "+
    QString::asprintf("`TYPE`=%d && ",type)+
    "`OWNER`='"+RDEscapeString(owner)+"' && "+
    QString::asprintf("`PANEL_NO`=%d && ",panel)+
    QString::asprintf("`ROW_NO`=%d && ",row)+
    QString::asprintf("`COLUMN_NO`=%d",col);
  q=new RDSqlQuery(sql);
  if(q->size()>0) {
    //
    // If so, update the record
    //
    delete q;
    sql1=QString("update ")+panel_tablename+" set "+
      "`LABEL`='"+RDEscapeString(button->text())+"',"+
      QString::asprintf("`CART`=%d,",button->cart())+
      "`DEFAULT_COLOR`='"+button->defaultColor().name()+"' where "+
      QString::asprintf("(`TYPE`=%d)&&",type)+
      "(`OWNER`='"+RDEscapeString(owner)+"')&&"+
      QString::asprintf("(`PANEL_NO`=%d)&&",panel)+
      QString::asprintf("(`ROW_NO`=%d)&&",row)+
      QString::asprintf("(`COLUMN_NO`=%d)",col);
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
      " (`TYPE`,"+
      "`OWNER`,"+
      "`PANEL_NO`,"+
      "`ROW_NO`,"+
      "`COLUMN_NO`,"+
      "`LABEL`,"+
      "`CART`,"+
      "`DEFAULT_COLOR`) "+
      QString::asprintf("values (%d,",type)+
      "'"+RDEscapeString(owner)+"',"+
      QString::asprintf("%d,%d,%d,",panel,row,col)+
      "'"+RDEscapeString(button->text())+"',"+
      QString::asprintf("%d,",button->cart())+
      "'"+RDEscapeString(button->defaultColor().name())+"')";
    RDSqlQuery::apply(sql1);
  }
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

  for(int i=0;i<RD_SOUNDPANEL_MAX_OUTPUTS;i++) {
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
  return RD_SOUNDPANEL_MAX_OUTPUTS-1;
}


void RDSoundPanel::LogPlayEvent(unsigned cartnum,int cutnum)
{
  RDCut *cut=new RDCut(QString::asprintf("%06u_%03d",cartnum,cutnum));
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
  QDateTime now=QDateTime::currentDateTime();

  sql=QString("select ")+
    "`CART`.`TITLE`,"+         // 00
    "`CART`.`ARTIST`,"+        // 01
    "`CART`.`PUBLISHER`,"+     // 02
    "`CART`.`COMPOSER`,"+      // 03
    "`CART`.`USAGE_CODE`,"+    // 04
    "`CUTS`.`ISRC`,"+          // 05
    "`CART`.`ALBUM`,"+         // 06
    "`CART`.`LABEL`,"+         // 07
    "`CUTS`.`ISCI`,"+          // 08
    "`CART`.`CONDUCTOR`,"+     // 09
    "`CART`.`USER_DEFINED`,"+  // 10
    "`CART`.`SONG_ID`,"+       // 11
    "`CUTS`.`DESCRIPTION`,"+   // 12
    "`CUTS`.`OUTCUE` "+        // 13
    "from `CART` left join `CUTS` "+
    "on `CART`.`NUMBER`=`CUTS`.`CART_NUMBER` where "+
    "`CUTS`.`CUT_NAME`='"+RDEscapeString(button->cutName())+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString("insert into `ELR_LINES` set ")+
      "`SERVICE_NAME`='"+RDEscapeString(panel_svcname)+"',"+
      QString::asprintf("`LENGTH`=%d,",button->startTime().msecsTo(now.time()))+
      QString::asprintf("`CART_NUMBER`=%u,",button->cart())+
      "`STATION_NAME`='"+RDEscapeString(rda->station()->name().toUtf8())+"',"+
      "`EVENT_DATETIME`="+
      RDCheckDateTime(QDateTime(now.date(),button->startTime()),
		      "yyyy-MM-dd hh:mm:ss")+","+
      QString::asprintf("`EVENT_TYPE`=%d,",RDAirPlayConf::TrafficStop)+
      QString::asprintf("`EVENT_SOURCE`=%d,",RDLogLine::SoundPanel)+
      QString::asprintf("`PLAY_SOURCE`=%d,",RDLogLine::SoundPanel)+
      QString::asprintf("`CUT_NUMBER`=%d,",button->cutName().right(3).toInt())+
      "`TITLE`='"+RDEscapeString(q->value(0).toString().toUtf8())+"',"+
      "`ARTIST`='"+RDEscapeString(q->value(1).toString().toUtf8())+"',"+
      "`PUBLISHER`='"+RDEscapeString(q->value(2).toString().toUtf8())+"',"+
      "`COMPOSER`='"+RDEscapeString(q->value(3).toString().toUtf8())+"',"+
      QString::asprintf("`USAGE_CODE`=%d,",q->value(4).toInt())+
      "`ISRC`='"+RDEscapeString(q->value(5).toString().toUtf8())+"',"+
      QString::asprintf("`START_SOURCE`=%d,",button->startSource())+
      "`ALBUM`='"+RDEscapeString(q->value(6).toString().toUtf8())+"',"+
      "`LABEL`='"+RDEscapeString(q->value(7).toString().toUtf8())+"',"+
      "`ISCI`='"+RDEscapeString(q->value(8).toString().toUtf8())+"',"+
      "`DESCRIPTION`='"+RDEscapeString(q->value(12).toString().toUtf8())+"',"+
      "`OUTCUE`='"+RDEscapeString(q->value(13).toString().toUtf8())+"',"+
      "`CONDUCTOR`='"+RDEscapeString(q->value(9).toString().toUtf8())+"',"+
      "`USER_DEFINED`='"+RDEscapeString(q->value(10).toString().toUtf8())+"',"+
      "`SONG_ID`='"+RDEscapeString(q->value(11).toString().toUtf8())+"',"+
      "`ONAIR_FLAG`='"+RDYesNo(panel_onair_flag)+"'";
    RDSqlQuery::apply(sql);
  }
  delete q;
}


void RDSoundPanel::LogTrafficMacro(RDPanelButton *button)
{
  QString sql;
  RDSqlQuery *q;
  QDateTime datetime(QDate::currentDate(),QTime::currentTime());

  sql=QString("select ")+
    "`TITLE`,"+          // 00
    "`ARTIST`,"+         // 01
    "`PUBLISHER`,"+      // 02
    "`COMPOSER`,"+       // 03
    "`USAGE_CODE`,"+     // 04
    "`FORCED_LENGTH`,"+  // 05
    "`ALBUM`,"+          // 06 
    "`LABEL` "+          // 07
    "from `CART` where "+
    QString::asprintf("`NUMBER`=%u",button->cart());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString("insert into `ELR_LINES` set ")+
      "`SERVICE_NAME`='"+RDEscapeString(panel_svcname)+"',"+
      QString::asprintf("`LENGTH`=%d,",q->value(5).toUInt())+
      QString::asprintf("`CART_NUMBER`=%u,",button->cart())+
      "`STATION_NAME`='"+RDEscapeString(rda->station()->name().toUtf8())+"',"+
      "`EVENT_DATETIME`='"+datetime.toString("yyyy-MM-dd hh:mm:ss")+"',"+
      QString::asprintf("`EVENT_TYPE`=%d,",RDAirPlayConf::TrafficMacro)+
      QString::asprintf("`EVENT_SOURCE`=%d,",RDLogLine::SoundPanel)+
      QString::asprintf("`PLAY_SOURCE`=%d,",RDLogLine::SoundPanel)+
      "`TITLE`='"+RDEscapeString(q->value(0).toString().toUtf8())+"',"+
      "`ARTIST`='"+RDEscapeString(q->value(1).toString().toUtf8())+"',"+
      "`PUBLISHER`='"+RDEscapeString(q->value(2).toString().toUtf8())+"',"+
      "`COMPOSER`='"+RDEscapeString(q->value(3).toString().toUtf8())+"',"+
      QString::asprintf("`USAGE_CODE`=%d,",q->value(4).toInt())+
      QString::asprintf("`START_SOURCE`=%d,",button->startSource())+
      "`ALBUM`='"+RDEscapeString(q->value(6).toString().toUtf8())+"',"+
      "`LABEL`='"+RDEscapeString(q->value(7).toString().toUtf8())+"',"+
      "`ONAIR_FLAG`='"+RDYesNo(panel_onair_flag)+"'";
    delete q;
    RDSqlQuery::apply(sql);
  }
}


void RDSoundPanel::LogLine(QString str)
{
  FILE *file;

  if(panel_logfile.isEmpty()) {
    return;
  }

  QDateTime current=QDateTime::currentDateTime();
  if((file=fopen(panel_logfile.toUtf8(),"a"))==NULL) {
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
	  str.toUtf8().constData());
  fclose(file);
}


void RDSoundPanel::Playing(int id)
{
  if(panel_active_buttons[id]==NULL) {
    LogLine(QString::asprintf("Invalid ID=%d in RDSoundPanel::Playing()",
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
    LogLine(QString::asprintf("Invalid ID=%d in RDSoundPanel::Paused()",
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
    LogLine(QString::asprintf("Invalid ID=%d in RDSoundPanel::Stopped()",
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
    panel_active_buttons[id]->setHookMode(panel_playmode_box->currentIndex()==1);
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
  LogLine(QString::asprintf("Playout stopped: id=%d",id));
}


void RDSoundPanel::ClearChannel(int id)
{
  RDPlayDeck *playdeck=panel_active_buttons[id]->playDeck();
  if(rda->cae()->
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
    return QString::asprintf("S:%d",index+1);
  }
  return QString::asprintf("U:%d",index-panel_station_panels+1);
}


QString RDSoundPanel::PanelOwner(RDAirPlayConf::PanelType type)
{
  switch(type) {
  case RDAirPlayConf::StationPanel:
    return rda->station()->name();

  case RDAirPlayConf::UserPanel:
    return rda->user()->name();
  }
  return QString();
}


RDPanelButton *RDSoundPanel::GetVisibleButton(int row,int col) const
{
  return NULL;
}
