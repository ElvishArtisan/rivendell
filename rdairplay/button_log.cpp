// button_log.cpp
//
// The button log widget for RDAirPlay
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

#include <QPainter>

#include "button_log.h"
#include "colors.h"

ButtonLog::ButtonLog(RDLogPlay *log,int id,RDAirPlayConf *conf,bool allow_pause,
		     QWidget *parent)
  : RDWidget(parent)
{
  log_id=id;
  log_log=log;
  log_action_mode=RDAirPlayConf::Normal;
  log_op_mode=RDAirPlayConf::LiveAssist;
  log_time_mode=RDAirPlayConf::TwentyFourHour;
  log_pause_enabled=allow_pause;

  //
  // Set Mappings
  //
  connect(log_log,SIGNAL(transportChanged()),
	  this,SLOT(transportChangedData()));
  connect(log_log,SIGNAL(modified(int)),this,SLOT(modifiedData(int)));
  connect(log_log,SIGNAL(played(int)),this,SLOT(playedData(int)));
  connect(log_log,SIGNAL(stopped(int)),this,SLOT(stoppedData(int)));
  connect(log_log,SIGNAL(paused(int)),this,SLOT(pausedData(int)));
  connect(log_log,SIGNAL(position(int,int)),this,SLOT(positionData(int,int)));

  //
  // Edit Event Dialog
  //
  log_event_edit=new EditEvent(log_log,this);

  //
  // Top Widgets
  //
  log_pie_counter_widget=
    new PieCounter(rda->airplayConf()->pieCountLength(),this);
  log_pie_counter_widget->setCountLength(rda->airplayConf()->pieCountLength());
  log_pie_counter_widget->setFocusPolicy(Qt::NoFocus);

  log_post_counter_widget=new PostCounter(this);
  log_post_counter_widget->setPostPoint(QTime(),0,false,false);
  log_post_counter_widget->setFocusPolicy(Qt::NoFocus);

  log_stop_counter_widget=new StopCounter(this);
  log_stop_counter_widget->setTime(QTime(0,0,0));
  log_stop_counter_widget->setFocusPolicy(Qt::NoFocus);

  //
  // Line Boxes / Start Buttons
  //
  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),
	  this,SLOT(startButton(int)));
  for(int i=0;i<BUTTON_PLAY_BUTTONS;i++) {
    log_line_box[i]=new LogLineBox(conf,this);
    log_line_box[i]->setMode(LogLineBox::Full);
    log_line_box[i]->setAcceptDrops(rda->station()->enableDragdrop());
    log_line_box[i]->setAllowDrags(rda->station()->enableDragdrop());
    connect(log_line_box[i],SIGNAL(doubleClicked(int)),
	    this,SLOT(boxDoubleClickedData(int)));
    connect(log_line_box[i],SIGNAL(cartDropped(int,RDLogLine *)),
	    this,SLOT(cartDroppedData(int,RDLogLine *)));
    log_start_button[i]=new StartButton(allow_pause,this);
    mapper->setMapping(log_start_button[i],i);
    connect(log_start_button[i],SIGNAL(clicked()),
	    mapper,SLOT(map()));
  }

  for(int i=BUTTON_PLAY_BUTTONS;i<BUTTON_TOTAL_BUTTONS;i++) {
    log_line_box[i]=new LogLineBox(conf,this);
    log_line_box[i]->setMode(LogLineBox::Half);
    log_line_box[i]->setAcceptDrops(rda->station()->enableDragdrop());
    log_line_box[i]->setAllowDrags(rda->station()->enableDragdrop());
    connect(log_line_box[i],SIGNAL(doubleClicked(int)),
	    this,SLOT(boxDoubleClickedData(int)));
    connect(log_line_box[i],SIGNAL(cartDropped(int,RDLogLine *)),
	    this,SLOT(cartDroppedData(int,RDLogLine *)));
    log_start_button[i]=new StartButton(allow_pause,this);
    mapper->setMapping(log_start_button[i],i);
    connect(log_start_button[i],SIGNAL(clicked()),
	    mapper,SLOT(map()));
  }
}


QSize ButtonLog::sizeHint() const
{
  return QSize(492,800);
}


QSizePolicy ButtonLog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


RDAirPlayConf::OpMode ButtonLog::opMode() const
{
  return log_op_mode;
}


void ButtonLog::setOpMode(RDAirPlayConf::OpMode mode)
{
  if(mode==log_op_mode) {
    return;
  }
  log_op_mode=mode;
  UpdateButtons();
}


RDAirPlayConf::ActionMode ButtonLog::actionMode() const
{
  return log_action_mode;
}


void ButtonLog::setActionMode(RDAirPlayConf::ActionMode mode,int *cartnum)
{
  int lines[TRANSPORT_QUANTITY];
  RDLogLine *logline=NULL;
  bool end_button=false;

  log_cart=cartnum;

  log_log->transportEvents(lines);

  for(int i=0;i<TRANSPORT_QUANTITY;i++) {
    if((lines[i]!=-1)&&((logline=log_log->logLine(lines[i]))!=NULL)) {
      switch(logline->status()) {
      case RDLogLine::Playing:
      case RDLogLine::Finishing:
	if(mode==RDAirPlayConf::CopyFrom) {
	  log_start_button[i]->
	    setMode(StartButton::CopyFrom,logline->cartType());
	}
	else {
	  log_start_button[i]->
	    setMode(StartButton::Play,logline->cartType());
	}
	break;

      case RDLogLine::Paused:
	switch(mode) {
	case RDAirPlayConf::DeleteFrom:
	  log_start_button[i]->
	    setMode(StartButton::DeleteFrom,logline->cartType());
	  break;

	case RDAirPlayConf::CopyFrom:
	  log_start_button[i]->
	    setMode(StartButton::CopyFrom,logline->cartType());
	  break;

	default:
	  log_start_button[i]->
	    setMode(StartButton::Pause,logline->cartType());
	}
	break;

      default:
	switch(mode) {
	case RDAirPlayConf::Normal:
	  log_start_button[i]->
	    setMode(StartButton::Stop,logline->cartType());
	  break;
		  
	case RDAirPlayConf::AddTo:
	  log_start_button[i]->
	    setMode(StartButton::AddTo,logline->cartType());
	  break;
		  
	case RDAirPlayConf::DeleteFrom:
	  log_start_button[i]->
	    setMode(StartButton::DeleteFrom,logline->cartType());
	  break;
		  
	case RDAirPlayConf::MoveFrom:
	  log_start_button[i]->
	    setMode(StartButton::MoveFrom,logline->cartType());
	  break;
		  
	case RDAirPlayConf::MoveTo:
	  log_start_button[i]->
	    setMode(StartButton::MoveTo,logline->cartType());
	  break;
		  
	case RDAirPlayConf::CopyFrom:
	  switch(logline->type()) {
	  case RDLogLine::Marker:
	  case RDLogLine::OpenBracket:
	  case RDLogLine::CloseBracket:
	  case RDLogLine::Chain:
	  case RDLogLine::Track:
	  case RDLogLine::MusicLink:
	  case RDLogLine::TrafficLink:
	  case RDLogLine::UnknownType:
	    log_start_button[i]->
	      setMode(StartButton::Stop,logline->cartType());
	    break;

	  case RDLogLine::Cart:
	  case RDLogLine::Macro:
	    log_start_button[i]->
	      setMode(StartButton::CopyFrom,logline->cartType());
	    break;
	  }
	  break;
		  
	case RDAirPlayConf::CopyTo:
	  log_start_button[i]->
	    setMode(StartButton::CopyTo,logline->cartType());
	  break;

	default:
	  break;
	}
      }	    
    }
    else {
      if(!end_button) {
	switch(mode) {
	case RDAirPlayConf::AddTo:
	  log_start_button[i]->
	    setMode(StartButton::AddTo,RDCart::All);
	  end_button=true;
	  break;

	case RDAirPlayConf::MoveTo:
	  log_start_button[i]->
	    setMode(StartButton::MoveTo,RDCart::All);
	  end_button=true;
	  break;

	case RDAirPlayConf::CopyTo:
	  log_start_button[i]->
	    setMode(StartButton::CopyTo,RDCart::All);
	  end_button=true;
	  break;

	default:
	  log_start_button[i]->
	    setMode(StartButton::Disabled,RDCart::All);
	  break;
	}
	end_button=true;
      }
      else {
	log_start_button[i]->
	  setMode(StartButton::Disabled,RDCart::All);
      }
    }
  }
  log_action_mode=mode;
}


void ButtonLog::setTimeMode(RDAirPlayConf::TimeMode mode)
{
  if(mode==log_time_mode) {
    return;
  }
  log_time_mode=mode;
  for(int i=0;i<BUTTON_TOTAL_BUTTONS;i++) {
    log_start_button[i]->setTimeMode(mode);
    log_line_box[i]->setTimeMode(mode);
  }
}


PieCounter *ButtonLog::pieCounterWidget() const
{
  return log_pie_counter_widget;
}


PostCounter *ButtonLog::postCounterWidget() const
{
  return log_post_counter_widget;
}


StopCounter *ButtonLog::stopCounterWidget() const
{
  return log_stop_counter_widget;
}


void ButtonLog::startButton(int id)
{
#ifdef SHOW_SLOTS
  printf("startMapperData(%d)\n",id);
#endif
  RDLogLine::Status status=RDLogLine::Scheduled;
  int line=log_line_box[id]->line();
  RDLogLine *logline=log_log->logLine(line);
  if(line<0) {
    line=log_log->lineCount();
  }
  switch(log_start_button[id]->mode()) {
  case StartButton::Stop:
    log_log->makeNext(line,false);
    log_log->play(line,RDLogLine::StartManual);
    break;

  case StartButton::Pause:
    log_log->play(line,RDLogLine::StartManual);
    break;

  case StartButton::Play:
    if(log_pause_enabled&&(logline!=NULL)&&
       (logline->cartType()==RDCart::Audio)) {
      log_log->pause(line);
    }
    else {
      log_log->stop(line);
    }
    break;

  case StartButton::AddTo:
  case StartButton::DeleteFrom:
  case StartButton::MoveFrom:
  case StartButton::MoveTo:
  case StartButton::CopyFrom:
  case StartButton::CopyTo:
    if((logline=log_log->logLine(line))==NULL) {
      status=RDLogLine::Scheduled;
    }
    else {
      status=logline->status();
    }
    emit selectClicked(log_id,line,status);
    break;

  default:
    break;
  }
}

void ButtonLog::pauseButtonHotkey(int id)
{
  int line=log_line_box[id]->line();
  if (line<0) {
   line=log_log->lineCount();
  }
  if (log_start_button[id]->mode() == StartButton::Play) {
      log_log->pause(line);
  }
}

void ButtonLog::stopButtonHotkey(int id)
{
  int line=log_line_box[id]->line();
  if (line<0) {
   line=log_log->lineCount();
  }
  log_log->stop(line);
}

void ButtonLog::transportChangedData()
{
  UpdateEvents();
  UpdateButtons();
  if(log_action_mode!=RDAirPlayConf::Normal) {
    setActionMode(log_action_mode);
  }
}


void ButtonLog::modifiedData(int line)
{
  int lines[TRANSPORT_QUANTITY];
  RDLogLine *logline;
  RDLogLine *next_logline;

  log_log->transportEvents(lines);
  for(int i=0;i<TRANSPORT_QUANTITY;i++) {
    if(lines[i]==line) {
      if((logline=log_log->logLine(line))!=NULL) {
	if((next_logline=log_log->logLine(line+1))!=NULL) {
	  log_line_box[i]->setEvent(line,next_logline->transType(),logline);
	}
	else {
	  log_line_box[i]->setEvent(line,RDLogLine::Stop,logline);
	}
      }
      else {
	log_line_box[i]->clear();
      }
    }
  }
}


void ButtonLog::boxDoubleClickedData(int line)
{
  if(line<0) {
    return;
  }
  if(log_event_edit->exec(line)==0) {
    log_log->lineModified(line);
  }
  if(line==1) {
    return;
  }
}


void ButtonLog::playedData(int line)
{
  for(int i=0;i<BUTTON_TOTAL_BUTTONS;i++) {
    if(log_line_box[i]->line()==line) {
      log_start_button[i]->setMode(StartButton::Play,
				   log_log->logLine(line)->cartType());
      UpdateButtons();
      return;
    }
  }
  UpdateEvents();
}


void ButtonLog::stoppedData(int line)
{
  for(int i=0;i<BUTTON_TOTAL_BUTTONS;i++) {
    if(log_line_box[i]->line()==line) {
      return;
    }
  }
  UpdateEvents();
}


void ButtonLog::pausedData(int line)
{
  for(int i=0;i<BUTTON_TOTAL_BUTTONS;i++) {
    if(log_line_box[i]->line()==line) {
      return;
    }
  }
  UpdateEvents();
}


void ButtonLog::positionData(int line,int point)
{
  for(int i=0;i<BUTTON_PLAY_BUTTONS;i++) {
    if(log_line_box[i]->line()==line) {
      log_line_box[i]->setTimer(point);
      return;
    }
  }
}


void ButtonLog::cartDroppedData(int line,RDLogLine *ll)
{
  emit cartDropped(log_id,line,ll);
}


void ButtonLog::resizeEvent(QResizeEvent *e)
{
  log_post_counter_widget->setGeometry(5,
				15+10,
				log_post_counter_widget->sizeHint().width(),
				log_post_counter_widget->sizeHint().height());
  log_pie_counter_widget->setGeometry(10+log_post_counter_widget->size().width(),
			       10,
			       log_pie_counter_widget->sizeHint().width(),
			       log_pie_counter_widget->sizeHint().height());
  log_stop_counter_widget->setGeometry(10-5+log_pie_counter_widget->sizeHint().width()+
				log_post_counter_widget->size().width()+10,
				15+10,
				log_stop_counter_widget->sizeHint().width(),
				log_stop_counter_widget->sizeHint().height());



  for(int i=0;i<BUTTON_PLAY_BUTTONS;i++) {
    log_line_box[i]->setGeometry(10+LOGLINEBOX_FULL_HEIGHT,
				 130+(LOGLINEBOX_FULL_HEIGHT+11)*i,
				 log_line_box[i]->sizeHint().width(),
				 log_line_box[i]->sizeHint().height());
  }
  for(int i=0;i<BUTTON_PLAY_BUTTONS;i++) {
    log_start_button[i]->setGeometry(5,
				     130+(LOGLINEBOX_FULL_HEIGHT+11)*i,
				     LOGLINEBOX_FULL_HEIGHT,
				     LOGLINEBOX_FULL_HEIGHT);
  }

  QRect viewport=QRect(0,0,size().width(),size().height());
  for(int i=BUTTON_PLAY_BUTTONS;i<BUTTON_TOTAL_BUTTONS;i++) {
    log_line_box[i]->setGeometry(10+85,
			       130+(LOGLINEBOX_FULL_HEIGHT+11)*3+
			       (LOGLINEBOX_HALF_HEIGHT+11)*(i-3),
				 log_line_box[i]->sizeHint().width(),
				 log_line_box[i]->sizeHint().height());
    log_start_button[i]->setGeometry(5,
				     130+(LOGLINEBOX_FULL_HEIGHT+11)*3+
				     (LOGLINEBOX_HALF_HEIGHT+11)*(i-3),
				     LOGLINEBOX_FULL_HEIGHT,
				     LOGLINEBOX_HALF_HEIGHT);
    bool visible=viewport.contains(log_line_box[i]->geometry())&&
      viewport.contains(log_start_button[i]->geometry());
    log_line_box[i]->setVisible(visible);
    log_start_button[i]->setVisible(visible);
  }
}


void ButtonLog::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->fillRect(0,0,size().width(),size().height(),
	      QColor(AIR_WIDGET_BACKGROUND_COLOR));
  delete p;
}


void ButtonLog::UpdateEvents()
{
  RDLogLine *logline=NULL;
  RDLogLine::TransType trans_type;
  int lines[TRANSPORT_QUANTITY];
  log_log->transportEvents(lines);
  int next_line;
  for(int i=0;i<TRANSPORT_QUANTITY;i++) {
    if(lines[i]>=0) {
      if((logline=log_log->logLine(lines[i]))!=NULL) {
	log_start_button[i]->setPort(logline->portName());
	log_line_box[i]->setStatus(log_log->logLine(lines[i])->status());
	switch(log_log->logLine(lines[i])->status()) {
	case RDLogLine::Playing:
	case RDLogLine::Finishing:
	  log_start_button[i]->
	    setMode(StartButton::Play,logline->cartType());
	  break;

	case RDLogLine::Paused:
	  log_start_button[i]->
	    setMode(StartButton::Pause,logline->cartType());
	  break;

	case RDLogLine::Scheduled:
	  log_start_button[i]->
	    setMode(StartButton::Stop,logline->cartType());
	  break;

	default:
	  log_start_button[i]->
	    setMode(StartButton::Disabled,logline->cartType());
	  break;
	}
      }
      if((next_line=log_log->nextLine(lines[i]))>=0) {
	trans_type=log_log->logLine(next_line)->transType();
      }
      else {
	trans_type=RDLogLine::Stop;
      }
      log_line_box[i]->
	setEvent(lines[i],trans_type,log_log->logLine(lines[i]));
    }
    else {
      log_start_button[i]->setMode(StartButton::Disabled,RDCart::All);
      log_line_box[i]->clear();
    }
  }
}


void ButtonLog::UpdateButtons()
{
  QTime time;
  QTime end_time;
  int lines[TRANSPORT_QUANTITY];
  RDLogLine *logline;
  RDLogLine *next_logline;
  int running;
  int longest_line=-1;

  //
  // Get Longest-Running Event
  //
  if((running=log_log->runningEvents(lines,false))>0) {
    for(int i=0;i<running;i++) {
      if((logline=log_log->logLine(lines[i]))!=NULL) {
	if((time=logline->startTime(RDLogLine::Actual).
	   addMSecs(logline->effectiveLength()))>end_time) {
	  end_time=time;
	  longest_line=lines[i];
	}
      }
    }
  }
  log_log->transportEvents(lines);

  for(int i=0;i<BUTTON_TOTAL_BUTTONS;i++) {  // The playback slots
    if(lines[i]>=0) {
      if((logline=log_log->logLine(lines[i]))!=NULL) {
	//
	// Set Buttons
	//
	switch(logline->status()) {
	case RDLogLine::Playing:
	case RDLogLine::Finishing:
	  log_start_button[i]->
	    setMode(StartButton::Play,logline->cartType());
	  log_start_button[i]->
	    setTime(logline->startTime(RDLogLine::Actual));
	  break;

	case RDLogLine::Paused:
	  log_start_button[i]->
	    setMode(StartButton::Pause,logline->cartType());
	  log_start_button[i]->
	    setTime(logline->startTime(RDLogLine::Predicted));
	  break;

	case RDLogLine::Scheduled:
	  log_start_button[i]->
	    setMode(StartButton::Stop,logline->cartType());
	  log_start_button[i]->
	    setTime(logline->startTime(RDLogLine::Predicted));
	  break;

	default:
	  log_start_button[i]->
	    setMode(StartButton::Disabled,logline->cartType());
	  break;
	}

	//
	// Set Progress Bars
	//
	if((next_logline=log_log->logLine(log_log->nextLine(lines[i])))!=
	   NULL) {
	  switch(log_log->mode()) {
	  case RDAirPlayConf::Auto:
	    switch(next_logline->transType()) {
	    case RDLogLine::Play:
	    case RDLogLine::Segue:
	      log_line_box[i]->setBarMode(LogLineBox::Transitioning);
	      break;

	    case RDLogLine::Stop:
	      if(lines[i]==longest_line) {
		log_line_box[i]->setBarMode(LogLineBox::Stopping);
	      }
	      else {
		log_line_box[i]->setBarMode(LogLineBox::Transitioning);
	      }
	      break;

	    default:
	      break;
	    }
	    break;

	  case RDAirPlayConf::LiveAssist:
	  case RDAirPlayConf::Manual:
	    switch(next_logline->status()) {
	    case RDLogLine::Playing:
	    case RDLogLine::Finishing:
	      log_line_box[i]->setBarMode(LogLineBox::Transitioning);
	      break;

	    default:
	      if(lines[i]==longest_line) {
		log_line_box[i]->setBarMode(LogLineBox::Stopping);
	      }
	      else {
		log_line_box[i]->setBarMode(LogLineBox::Transitioning);
	      }
	      break;
	    }
	    break;

	  default:
	    break;
	  }
	}
	else {
	  if(lines[i]==longest_line) {
	    log_line_box[i]->setBarMode(LogLineBox::Stopping);
	  }
	  else {
	    log_line_box[i]->setBarMode(LogLineBox::Transitioning);
	  }
	}
      }
      else {
	log_start_button[i]->
	  setMode(StartButton::Disabled,logline->cartType());
      }
    }
  }
}
