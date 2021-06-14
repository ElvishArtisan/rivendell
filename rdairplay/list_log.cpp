// list_log.cpp
//
// The full log widget for RDAirPlay
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
#include <rdconf.h>

#include "colors.h"
#include "list_log.h"

ListLog::ListLog(RDLogPlay *log,int id,bool allow_pause,
		 QWidget *parent)
  : RDWidget(parent)
{
  list_id=id;
  list_log=log;
  list_op_mode=RDAirPlayConf::LiveAssist;
  list_action_mode=RDAirPlayConf::Normal;
  list_time_mode=RDAirPlayConf::TwentyFourHour;
  list_scroll=true;
  list_pause_allowed=allow_pause;
  list_playbutton_mode=ListLog::ButtonDisabled;
  list_audition_head_playing=false;
  list_audition_tail_playing=false;

  //
  // Create Palettes
  //
  QColor system_button_text_color = palette().buttonText().color();
  QColor system_button_color = palette().button().color();
  QColor system_mid_color = palette().mid().color();
  list_from_color=
    QPalette(QColor(BUTTON_FROM_BACKGROUND_COLOR),QColor(system_mid_color));
  list_from_color.
    setColor(QPalette::ButtonText,QColor(BUTTON_FROM_TEXT_COLOR));

  list_to_color=
    QPalette(QColor(BUTTON_TO_BACKGROUND_COLOR),QColor(system_mid_color));
  list_to_color.
    setColor(QPalette::ButtonText,QColor(BUTTON_TO_TEXT_COLOR));
  list_list_to_color=palette();
  list_list_to_color.setColor(QPalette::Active,QPalette::Highlight,
			      BUTTON_TO_BACKGROUND_COLOR);
  list_list_to_color.setColor(QPalette::Active,QPalette::HighlightedText,
			      BUTTON_TO_TEXT_COLOR);
  list_list_to_color.setColor(QPalette::Inactive,QPalette::Highlight,
			      BUTTON_TO_BACKGROUND_COLOR);
  list_list_to_color.setColor(QPalette::Inactive,QPalette::HighlightedText,
			      BUTTON_TO_TEXT_COLOR);
  list_list_from_color=palette();
  list_list_from_color.setColor(QPalette::Active,QPalette::Highlight,
				BUTTON_FROM_BACKGROUND_COLOR);
  list_list_from_color.setColor(QPalette::Active,QPalette::HighlightedText,
				BUTTON_FROM_TEXT_COLOR);
  list_list_from_color.setColor(QPalette::Inactive,QPalette::Highlight,
				BUTTON_FROM_BACKGROUND_COLOR);
  list_list_from_color.
    setColor(QPalette::Inactive,QPalette::HighlightedText,
	     BUTTON_FROM_TEXT_COLOR);
  list_scroll_color[0]=palette();
  list_scroll_color[0].setColor(QPalette::Active,QPalette::ButtonText,
			BUTTON_LOG_ACTIVE_TEXT_COLOR);
  list_scroll_color[0].setColor(QPalette::Active,QPalette::Button,
			BUTTON_LOG_ACTIVE_BACKGROUND_COLOR);
  list_scroll_color[0].setColor(QPalette::Active,QPalette::Background,
			system_mid_color);
  list_scroll_color[0].setColor(QPalette::Inactive,QPalette::ButtonText,
			BUTTON_LOG_ACTIVE_TEXT_COLOR);
  list_scroll_color[0].setColor(QPalette::Inactive,QPalette::Button,
			BUTTON_LOG_ACTIVE_BACKGROUND_COLOR);
  list_scroll_color[0].setColor(QPalette::Inactive,QPalette::Background,
			system_mid_color);
  list_scroll_color[1]=palette();
  list_scroll_color[1].setColor(QPalette::Active,QPalette::ButtonText,
                       system_button_text_color);
  list_scroll_color[1].setColor(QPalette::Active,QPalette::Button,
                       system_button_color);
  list_scroll_color[1].setColor(QPalette::Active,QPalette::Background,
			system_mid_color);
  list_scroll_color[1].setColor(QPalette::Inactive,QPalette::ButtonText,
                       system_button_text_color);
  list_scroll_color[1].setColor(QPalette::Inactive,QPalette::Button,
                       system_button_color);
  list_scroll_color[1].setColor(QPalette::Inactive,QPalette::Background,
			system_mid_color);

  //
  // Hour Selector
  //
  list_hour_selector=new HourSelector(this);
  list_hour_selector->setTimeMode(list_time_mode);
  connect(list_hour_selector,SIGNAL(hourSelected(int)),
	  this,SLOT(selectHour(int)));
  connect(list_log,SIGNAL(hourChanged(int,bool)),
  	  list_hour_selector,SLOT(updateHour(int,bool)));
  list_hour_selector->hide();

  int y=0;
  int h=sizeHint().height()-60;
  if(rda->airplayConf()->showCounters()) {
    h-=60;
  }
  if(rda->airplayConf()->hourSelectorEnabled()) {
    y+=80;
    h-=80;
    list_hour_selector->show();
  }

  //
  // Log View
  //
  list_log_view=new LogTableView(this);
  list_log_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  list_log_view->setSelectionMode(QAbstractItemView::ContiguousSelection);
  list_log_view->setShowGrid(false);
  list_log_view->setSortingEnabled(false);
  list_log_view->setWordWrap(false);
  list_log->setFont(defaultFont());
  list_log->setPalette(palette());
  list_log_view->setModel(list_log);
  list_log_view->resizeColumnsToContents();
  list_log->setTimeMode(list_time_mode);
  connect(list_log_view->selectionModel(),
       SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
       this,
       SLOT(selectionChangedData(const QItemSelection &,const QItemSelection)));
  connect(list_log_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_log,SIGNAL(modelReset()),this,SLOT(modelResetData()));
  connect(list_log_view,SIGNAL(cartDropped(int,RDLogLine *)),
	  this,SLOT(cartDroppedData(int,RDLogLine *)));
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  list_log,SLOT(processNotification(RDNotification *)));

  //
  // List Logs Dialog
  //
  list_logs_dialog=new ListLogs(list_log,this);

  //
  // Time Counter Section
  //
  list_groupbox=new QGroupBox(tr("Run Length"),this);
  list_groupbox->setFont(labelFont());
  if(!rda->airplayConf()->showCounters()) {
    list_groupbox->hide();
  }

  //
  // Stop Time Counter
  //
  list_stoptime_edit=new QLineEdit(this);
  list_stoptime_label=new QLabel(tr("Next Stop:"),this);
  list_stoptime_label->setFont(labelFont());
  list_stoptime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);  
  //  list_stoptime_label->setBackgroundColor(QColor(system_mid_color));
  if(!rda->airplayConf()->showCounters()) {
    list_stoptime_edit->hide();
    list_stoptime_label->hide();
  }

  //
  // End Time Counter
  //
  list_endtime_edit=new QLineEdit(this);
  list_endtime_label=new QLabel(tr("Log End:"),this);
  list_endtime_label->setFont(labelFont());
  list_endtime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);  
  //  list_endtime_label->setBackgroundColor(QColor(system_mid_color));
  if(!rda->airplayConf()->showCounters()) {
    list_endtime_edit->hide();
    list_endtime_label->hide();
  }

  //
  // Select Button
  //
  list_take_button=new QPushButton(this);
  list_take_button->setFont(bigButtonFont());
  list_take_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  list_take_button->setText(tr("Select"));
  list_take_button->setFocusPolicy(Qt::NoFocus);
  connect(list_take_button,SIGNAL(clicked()),this,SLOT(takeButtonData()));
  list_take_button->hide();

  //
  // Audition Head Button
  //
  list_head_button=new QPushButton(this);
  list_head_button->setFont(bigButtonFont());
  list_head_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  list_head_button->setText(tr("Audition\nHead"));
  list_head_button->setFocusPolicy(Qt::NoFocus);
  connect(list_head_button,SIGNAL(clicked()),this,SLOT(headButtonData()));
  if(!rda->airplayConf()->showCounters()) {
    list_head_button->hide();
  }

  //
  // Audition Tail Button
  //
  list_tail_button=new QPushButton(this);
  list_tail_button->setFont(bigButtonFont());
  list_tail_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  list_tail_button->setText(tr("Audition\nTail"));
  list_tail_button->setFocusPolicy(Qt::NoFocus);
  connect(list_tail_button,SIGNAL(clicked()),this,SLOT(tailButtonData()));
  if(!rda->airplayConf()->showCounters()) {
    list_tail_button->hide();
  }

  //
  // Play Button
  //
  list_play_button=new QPushButton(this);
  list_play_button->setFont(bigButtonFont());
  list_play_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  list_play_button->setText(tr("Start"));
  list_play_button->setDisabled(true);
  list_play_button->setFocusPolicy(Qt::NoFocus);
  connect(list_play_button,SIGNAL(clicked()),this,SLOT(playButtonData()));

  //
  // Next Button
  //
  list_next_button=new QPushButton(this);
  list_next_button->setFont(bigButtonFont());
  list_next_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  list_next_button->setText(tr("Make\nNext"));
  list_next_button->setDisabled(true);
  list_next_button->setFocusPolicy(Qt::NoFocus);
  connect(list_next_button,SIGNAL(clicked()),this,SLOT(nextButtonData()));

  //
  // Modify Button
  //
  list_modify_button=new QPushButton(this);
  list_modify_button->setFont(bigButtonFont());
  list_modify_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  list_modify_button->setText(tr("Modify"));
  list_modify_button->setDisabled(true);
  list_modify_button->setFocusPolicy(Qt::NoFocus);
  connect(list_modify_button,SIGNAL(clicked()),this,SLOT(modifyButtonData()));

  //
  // Scroll Button
  //
  list_scroll_button=new QPushButton(this);
  list_scroll_button->setFont(bigButtonFont());
  list_scroll_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  list_scroll_button->setText(tr("Scroll"));
  list_scroll_button->setFocusPolicy(Qt::NoFocus);
  connect(list_scroll_button,SIGNAL(clicked()),this,SLOT(scrollButtonData()));
  list_scroll_button->setPalette(list_scroll_color[0]);

  //
  // Refresh Button
  //
  list_refresh_button=new QPushButton(this);
  list_refresh_button->setFont(bigButtonFont());
  list_refresh_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  list_refresh_button->setText(tr("Refresh\nLog"));
  list_refresh_button->setDisabled(true);
  list_refresh_button->setFocusPolicy(Qt::NoFocus);
  connect(list_refresh_button,SIGNAL(clicked()),
	  this,SLOT(refreshButtonData()));

  //
  // Log Load   
  //
  list_load_button=new QPushButton(this);
  list_load_button->setFont(bigButtonFont());
  list_load_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  list_load_button->setText(tr("Select\nLog"));
  list_load_button->setFocusPolicy(Qt::NoFocus);
  connect(list_load_button,SIGNAL(clicked()),this,SLOT(loadButtonData()));

  //
  // Edit Event Dialog
  //
  list_event_edit=new EditEvent(list_log,this);

  //
  // Map Slots
  //
  connect(list_log,SIGNAL(transportChanged()),
	  this,SLOT(transportChangedData()));
  connect(list_log,SIGNAL(refreshabilityChanged(bool)),
	  this,SLOT(refreshabilityChangedData(bool)));
  connect(list_log,SIGNAL(auditionHeadPlayed(int)),
	  this,SLOT(auditionHeadData(int)));
  connect(list_log,SIGNAL(auditionTailPlayed(int)),
	  this,SLOT(auditionTailData(int)));
  connect(list_log,SIGNAL(auditionStopped(int)),
	  this,SLOT(auditionStoppedData(int)));

  //  setBackgroundColor(QColor(system_mid_color));
}


QSize ListLog::sizeHint() const
{
  return QSize(500,530);
}


QSizePolicy ListLog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListLog::setOpMode(RDAirPlayConf::OpMode mode)
{
  if(mode==list_op_mode) {
    return;
  }
  list_op_mode=mode;
}


RDAirPlayConf::ActionMode ListLog::actionMode() const
{
  return list_action_mode;
}


void ListLog::setActionMode(RDAirPlayConf::ActionMode mode,int *cartnum)
{
  list_cart=cartnum;

  if(mode==list_action_mode) {
    return;
  }
  switch(mode) {
  case RDAirPlayConf::Normal:
    list_log_view->setSelectionMode(QAbstractItemView::ContiguousSelection);
    list_log_view->setPalette(palette());
    list_take_button->hide();
    list_play_button->show();
    list_next_button->show();
    list_modify_button->show();
    list_scroll_button->show();
    list_refresh_button->show();
    list_load_button->show();
    break;

  case RDAirPlayConf::AddTo:
    list_log_view->setSelectionMode(QAbstractItemView::SingleSelection);
    list_log_view->setPalette(list_list_to_color);
    list_take_button->setText(ADD_TO_MODE_TITLE);
    list_take_button->setPalette(list_to_color);
    list_take_button->show();
    list_play_button->hide();
    list_next_button->hide();
    list_modify_button->hide();
    list_scroll_button->hide();
    list_refresh_button->hide();
    list_load_button->hide();
    break;

  case RDAirPlayConf::DeleteFrom:
    list_log_view->setSelectionMode(QAbstractItemView::SingleSelection);
    list_log_view->setPalette(list_list_from_color);
    list_take_button->setText(DELETE_FROM_MODE_TITLE);
    list_take_button->setPalette(list_from_color);
    list_take_button->show();
    list_play_button->hide();
    list_next_button->hide();
    list_modify_button->hide();
    list_scroll_button->hide();
    list_refresh_button->hide();
    list_load_button->hide();
    break;

  case RDAirPlayConf::MoveFrom:
    list_log_view->setSelectionMode(QAbstractItemView::SingleSelection);
    list_log_view->setPalette(list_list_from_color);
    list_take_button->setText(MOVE_FROM_MODE_TITLE);
    list_take_button->setPalette(list_from_color);
    list_take_button->show();
    list_play_button->hide();
    list_next_button->hide();
    list_modify_button->hide();
    list_scroll_button->hide();
    list_refresh_button->hide();
    list_load_button->hide();
    break;

  case RDAirPlayConf::MoveTo:
    list_log_view->setSelectionMode(QAbstractItemView::SingleSelection);
    list_log_view->setPalette(list_list_to_color);
    list_take_button->setText(MOVE_TO_MODE_TITLE);
    list_take_button->setPalette(list_to_color);
    list_take_button->show();
    list_play_button->hide();
    list_next_button->hide();
    list_modify_button->hide();
    list_scroll_button->hide();
    list_refresh_button->hide();
    list_load_button->hide();
    break;

  case RDAirPlayConf::CopyFrom:
    list_log_view->setSelectionMode(QAbstractItemView::SingleSelection);
    list_log_view->setPalette(list_list_from_color);
    list_take_button->setText(COPY_FROM_MODE_TITLE);
    list_take_button->setPalette(list_from_color);
    list_take_button->show();
    list_play_button->hide();
    list_next_button->hide();
    list_modify_button->hide();
    list_scroll_button->hide();
    list_refresh_button->hide();
    list_load_button->hide();
    break;

  case RDAirPlayConf::CopyTo:
    list_log_view->setSelectionMode(QAbstractItemView::SingleSelection);
    list_log_view->setPalette(list_list_to_color);
    list_take_button->setText(COPY_TO_MODE_TITLE);
    list_take_button->setPalette(list_to_color);
    list_take_button->show();
    list_play_button->hide();
    list_next_button->hide();
    list_modify_button->hide();
    list_scroll_button->hide();
    list_refresh_button->hide();
    list_load_button->hide();
    break;

  default:
    break;
  }
  if(mode==RDAirPlayConf::Normal) {
    list_scroll=list_suspended_scroll;
  }
  else {
    if(list_action_mode==RDAirPlayConf::Normal) {
      list_suspended_scroll=list_scroll;
      list_scroll=false;
    }
  }
  list_action_mode=mode;
}


void ListLog::setTimeMode(RDAirPlayConf::TimeMode mode)
{
  if(mode==list_time_mode) {
    return;
  }
  list_hour_selector->setTimeMode(mode);
  list_log->setTimeMode(mode);
  list_time_mode=mode;
  list_log_view->resizeColumnToContents(0);
}


void ListLog::userChanged(bool add_allowed,bool delete_allowed,
			  bool arrange_allowed,bool playout_allowed)
{
  list_load_button->setEnabled(playout_allowed);
}


void ListLog::selectHour(int hour)
{
  int row=list_log->startOfHour(hour);
  if(row>=0) {
    list_log_view->selectRow(row);
  }
}


void ListLog::takeButtonData()
{
  int line=0;

  RDLogLine::Status status=CurrentStatus();
  switch(list_action_mode) {
  case RDAirPlayConf::AddTo:
  case RDAirPlayConf::MoveTo:
  case RDAirPlayConf::CopyTo:
    switch(status) {
    case RDLogLine::Finished:
    case RDLogLine::Playing:
    case RDLogLine::Paused:
    case RDLogLine::Finishing:
      return;
	      
    default:
      line=CurrentLine();
      break;
    }
    break;
	  
  case RDAirPlayConf::CopyFrom:
    line=CurrentLine();
    if(list_log->logLine(line)!=NULL) {
      switch(list_log->logLine(line)->type()) {
      case RDLogLine::Marker:
      case RDLogLine::OpenBracket:
      case RDLogLine::CloseBracket:
      case RDLogLine::Chain:
      case RDLogLine::Track:
      case RDLogLine::MusicLink:
      case RDLogLine::TrafficLink:
      case RDLogLine::UnknownType:
	return;
		
      default:
	break;
      }
    }
    break;

  case RDAirPlayConf::MoveFrom:
    switch(status) {
    case RDLogLine::Finished:
    case RDLogLine::Playing:
    case RDLogLine::Paused:
    case RDLogLine::Finishing:
      return;
	      
    default:
      line=CurrentLine();
      break;
    }
    break;

  case RDAirPlayConf::DeleteFrom:
    switch(status) {
    case RDLogLine::Finished:
    case RDLogLine::Playing:
    case RDLogLine::Finishing:
      return;
	      
    default:
      line=CurrentLine();
      // Don't try delete "end of log" or other invalid log entries.
      if (line<0) {
	return;
      }
      break;
    }
    break;

  default:
    break;
  }

  emit selectClicked(list_id,line,status);
}


void ListLog::headButtonData()
{
  if(list_audition_head_playing) {
    list_log->auditionStop();
  }
  else {
    if(!list_audition_tail_playing) {
      list_log->auditionHead(CurrentLine());
    }
  }
}


void ListLog::tailButtonData()
{
  if(list_audition_tail_playing) {
    list_log->auditionStop();
  }
  else {
    if(!list_audition_head_playing) {
      list_log->auditionTail(CurrentLine());
    }
  }
}


void ListLog::auditionHeadData(int line)
{
  list_head_button->setPalette(list_scroll_color[0]);
  list_audition_head_playing=true;
}


void ListLog::auditionTailData(int line)
{
  list_tail_button->setPalette(list_scroll_color[0]);
  list_audition_tail_playing=true;
}


void ListLog::auditionStoppedData(int line)
{
  list_head_button->setPalette(list_scroll_color[1]);
  list_tail_button->setPalette(list_scroll_color[1]);
  list_audition_head_playing=false;
  list_audition_tail_playing=false;
}


void ListLog::playButtonData()
{
  int line=CurrentLine();
  if(line<0) {
    return;
  }

  switch(list_playbutton_mode) {
  case ListLog::ButtonPlay:
    switch(CurrentStatus()) {
    case RDLogLine::Scheduled:
    case RDLogLine::Paused:
      if(line<0) {
	return;
      }
      list_log->play(line,RDLogLine::StartManual);
      ClearSelection();
      break;
	      
    default:
      break;
    }
    break;

  case ListLog::ButtonStop:
    if(list_pause_allowed) {
      list_log->pause(line);
    }
    else {
      list_log->stop(line);
    }
    ClearSelection();
    break;

  case ListLog::ButtonDisabled:
    break;
  }
}


void ListLog::modifyButtonData()
{
  QModelIndexList rows=list_log_view->selectionModel()->selectedRows();

  if(rows.size()==0) {
    return;
  }
  if(rows.size()==1) {
    RDLogLine *ll=list_log->logLine(rows.first().row());
    if(ll!=NULL) {
      if((ll->status()!=RDLogLine::Scheduled)&&
	 (ll->status()!=RDLogLine::Paused)&&
	 (ll->state()!=RDLogLine::NoCart)&&
	 (ll->state()!=RDLogLine::NoCut)) {
	return;
      }
    }
  }
  if(list_event_edit->exec(rows.first().row())==0) {
    list_log->lineModified(rows.first().row());
  }
  if(rows.first().row()==1) {
    return;
  }
  ClearSelection();
}


void ListLog::doubleClickedData(const QModelIndex &index)
{
  modifyButtonData();
}


void ListLog::scrollButtonData()
{
  if(list_scroll) {
    list_scroll_button->setPalette(list_scroll_color[1]);
    list_scroll=false;
}
  else {
    list_scroll_button->setPalette(list_scroll_color[0]);
    list_scroll=true;
    ScrollTo(list_log->nextLine());
  }
}


void ListLog::refreshButtonData()
{
  list_log->refresh();
}


void ListLog::nextButtonData()
{
  RDLogLine *ll=NULL;
  QModelIndexList rows=list_log_view->selectionModel()->selectedRows();

  if(rows.size()==1) {
    if((ll=list_log->logLine(rows.first().row()))!=NULL) {
      if(ll->status()==RDLogLine::Scheduled) {
	list_log->makeNext(rows.first().row());
	ClearSelection();
      }
    }
  }
}


void ListLog::loadButtonData()
{
  QString name=list_log->logName();
  QString svcname=list_log->serviceName();
  QString err_msg;
  RDLog *edit_log;
  RDLogLock *log_lock=NULL;

  switch((ListLogs::Operation)list_logs_dialog->exec(&name,&svcname,&log_lock)) {
  case ListLogs::Load:
    list_log->setLogName(name);
    list_log->load();
    break;

  case ListLogs::Save:
    list_log->save();
    edit_log=
      new RDLog(list_log->logName().left(list_log->logName().length()-4));
    delete edit_log;
    break;
    
  case ListLogs::SaveAs:
    if(!RDLog::create(name,svcname,QDate(),rda->ripc()->user(),&err_msg,
		      rda->config())) {
      QMessageBox::warning(this,"RDAirPlay - "+tr("Error"),err_msg);
      return;
    }
    list_log->setServiceName(svcname);
    list_log->setLogName(name);
    list_log->save();
    break;

  case ListLogs::Unload:
    list_log->clear();
    break;

  case ListLogs::Cancel:
    break;
  }
  if(log_lock!=NULL) {
    delete log_lock;
  }
}


void ListLog::selectionChangedData(const QItemSelection &new_sel,
				   const QItemSelection &old_sel)
{
  QModelIndexList rows=list_log_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    list_endtime_edit->setText("");
    list_stoptime_label->setText(tr("Selected:"));
    SetPlaybuttonMode(ListLog::ButtonDisabled);
    list_modify_button->setDisabled(true);
    list_next_button->setDisabled(true);
    if(rows.size()>0) {
      int last_line=rows.last().row();
      if(last_line>=list_log->lineCount()) {
	last_line=list_log->lineCount()-1;
      }
      list_stoptime_edit->
	setText(RDGetTimeLength(list_log->
				length(rows.first().row(),last_line+1),
				true,false));
    }
    return;
  }  
  switch(CurrentStatus()) {
  case RDLogLine::Scheduled:
  case RDLogLine::Paused:
    if(rows.first().row()>=list_log->lineCount()) {  //End marker!
      SetPlaybuttonMode(ListLog::ButtonDisabled);
      list_modify_button->setDisabled(true);
      list_next_button->setDisabled(true);
    }
    else {
      SetPlaybuttonMode(ListLog::ButtonPlay);
      list_modify_button->setEnabled(true);
      list_next_button->setEnabled(true);
    }
    break;

  case RDLogLine::Playing:
    SetPlaybuttonMode(ListLog::ButtonStop);
    list_modify_button->setEnabled(true);
    list_next_button->setDisabled(true);
    break;

  case RDLogLine::Finished:
    SetPlaybuttonMode(ListLog::ButtonDisabled);
    list_modify_button->setDisabled(true);
    list_next_button->setDisabled(true);
    break;

  default:
    break;
  }
  if((rows.size()==1)&&(rows.first().row()<list_log->lineCount())) {
    list_endtime_edit->
      setText(RDGetTimeLength(list_log->
	       length(rows.last().row(),list_log->lineCount()),true,false));
    list_stoptime_label->setText(tr("Next Stop:"));
    int stoplen=list_log->lengthToStop(rows.last().row());
    if(stoplen>=0) {
      list_stoptime_edit->setText(RDGetTimeLength(stoplen,true,false));
    }
    else {
      list_stoptime_edit->setText("");
    }
  }
  else {
    list_endtime_edit->setText("");
    list_stoptime_edit->setText("");
  }
}


void ListLog::transportChangedData()
{
  int transport_line[TRANSPORT_QUANTITY];

  list_log->transportEvents(transport_line);
  if(list_scroll&&(transport_line[0]>=0)) {
    ScrollTo(transport_line[0]);
  }
}


void ListLog::modelResetData()
{
  list_log_view->resizeColumnsToContents();
}


void ListLog::refreshabilityChangedData(bool state)
{
  list_refresh_button->setEnabled(state);
}


void ListLog::cartDroppedData(int line,RDLogLine *ll)
{
  emit cartDropped(list_id,line,ll);
}


void ListLog::resizeEvent(QResizeEvent *e)
{
  //
  // Hour Selector
  //
  if(rda->airplayConf()->hourSelectorEnabled()) {
    list_hour_selector->setGeometry(0,0,size().width(),80);
  }

  //
  // Log Items
  //
  int list_y=0;
  int list_h=size().height()-60;
  if(rda->airplayConf()->showCounters()) {
    list_h-=60;
  }

  if(size().width()>=850) {
    //
    // Audition Head/Tail Buttons
    //
    list_head_button->setGeometry(510,size().height()-55,90,50);
    list_tail_button->setGeometry(600,size().height()-55,90,50);

    //
    // Counters
    //
    list_groupbox->setGeometry(695,size().height()-61,153,58);
    list_stoptime_edit->setGeometry(773,size().height()-45,70,18);
    list_stoptime_label->setGeometry(703,size().height()-45,65,18);
    list_endtime_edit->setGeometry(773,size().height()-25,70,18);
    list_endtime_label->setGeometry(703,size().height()-25,65,18);

    //
    // Bottom Buttons
    //
    list_take_button->setGeometry(10,size().height()-55,80,50);
    list_play_button->setGeometry(10,size().height()-55,80,50);
    list_next_button->setGeometry(90,size().height()-55,80,50);
    list_modify_button->setGeometry(170,size().height()-55,80,50);
    list_scroll_button->setGeometry(250,size().height()-55,80,50);
    list_refresh_button->setGeometry(330,size().height()-55,80,50);
    list_load_button->setGeometry(410,size().height()-55,
				  80,50);
    list_h+=60;
  }
  else {
    //
    // Audition Head/Tail Buttons
    //
    list_head_button->setGeometry(10,size().height()-113,90,50);
    list_tail_button->setGeometry(100,size().height()-113,90,50);

    //
    // Counters
    //
    list_groupbox->setGeometry(333,size().height()-116,158,58);
    list_stoptime_edit->setGeometry(412,size().height()-100,70,18);
    list_stoptime_label->setGeometry(342,size().height()-100,65,18);
    list_endtime_edit->setGeometry(412,size().height()-80,70,18);
    list_endtime_label->setGeometry(342,size().height()-80,65,18);

    //
    // Bottom Buttons
    //
    list_take_button->setGeometry(10,size().height()-55,80,50);
    list_play_button->setGeometry(10,size().height()-55,80,50);
    list_next_button->setGeometry(90,size().height()-55,80,50);
    list_modify_button->setGeometry(170,size().height()-55,80,50);
    list_scroll_button->setGeometry(250,size().height()-55,80,50);
    list_refresh_button->setGeometry(330,size().height()-55,80,50);
    list_load_button->setGeometry(size().width()-90,size().height()-55,80,50);
  }

  if(rda->airplayConf()->hourSelectorEnabled()) {
    if(size().width()>=(24*HOURSELECTOR_BUTTON_EDGE)) {
      list_y+=HOURSELECTOR_BUTTON_EDGE;
      list_h-=HOURSELECTOR_BUTTON_EDGE;
    }
    else {
      list_y+=2*HOURSELECTOR_BUTTON_EDGE;
      list_h-=2*HOURSELECTOR_BUTTON_EDGE;
    }
  }
  list_log_view->setGeometry(0,list_y,size().width(),list_h);

}


int ListLog::CurrentLine()
{
  QModelIndexList rows=list_log_view->selectionModel()->selectedRows();

  if(rows.size()==1) {
    return rows.first().row();
  }

  return -1;
}


RDLogLine::Status ListLog::CurrentStatus()
{
  QModelIndexList rows=list_log_view->selectionModel()->selectedRows();

  if(rows.size()==1) {
    RDLogLine *ll=list_log->logLine(rows.first().row());
    if(ll==NULL) {
      return RDLogLine::Scheduled;
    }
    else {
      return ll->status();
    }
  }
  return RDLogLine::Finished;
}


RDLogLine::State ListLog::CurrentState()
{
  QModelIndexList rows=list_log_view->selectionModel()->selectedRows();

  if(rows.size()==1) {
    RDLogLine *ll=list_log->logLine(rows.first().row());
    if(ll!=NULL) {
      return ll->state();
    }    
  }
  return RDLogLine::NoCart;
}


void ListLog::ClearSelection()
{
  list_log_view->clearSelection();
  SetPlaybuttonMode(ListLog::ButtonDisabled);
  list_modify_button->setDisabled(true);
  list_next_button->setDisabled(true);
}


void ListLog::ScrollTo(int line)
{
  list_log_view->
    scrollTo(list_log->index(line,0),QAbstractItemView::PositionAtCenter);
  list_log_view->clearSelection();
}


void ListLog::SetPlaybuttonMode(ListLog::PlayButtonMode mode)
{
  if(mode==list_playbutton_mode) {
    return;
  }
  switch(mode) {
  case ListLog::ButtonPlay:
    list_play_button->setEnabled(true);
    list_play_button->setText(tr("Start"));
    break;

  case ListLog::ButtonStop:
    list_play_button->setEnabled(true);
    if(list_pause_allowed) {
      list_play_button->setText(tr("Pause"));
    }
    else {
      list_play_button->setText(tr("Stop"));
    }
    break;

  case ListLog::ButtonDisabled:
    list_play_button->setDisabled(true);
    list_play_button->setText(tr("Start"));
    break;
  }
  list_playbutton_mode=mode;
}


QString ListLog::TimeString(const QTime &time) const
{
  QString ret;
  switch(list_time_mode) {
  case RDAirPlayConf::TwelveHour:
    ret=time.toString("h:mm:ss.zzz");
    ret=ret.left(ret.length()-2);
    ret+=(" "+time.toString("ap"));
    break;

  case RDAirPlayConf::TwentyFourHour:
    ret=time.toString("hh:mm:ss.zzz").left(10);
    break;
  }
  return ret;
}
