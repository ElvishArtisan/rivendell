// list_log.cpp
//
// The full log list for RDAirPlay
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_log.cpp,v 1.105.6.11.2.1 2014/05/21 18:19:42 cvs Exp $
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

#include <qtimer.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <rddb.h>
#include <rdconf.h>
#include <rdlistviewitem.h>
#include <rddebug.h>
#include <rdlog.h>
#include <rdcreate_log.h>

#include <list_log.h>
#include <button_log.h>
#include <colors.h>
#include <globals.h>

#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/chain.xpm"
#include "../icons/track_cart.xpm"
#include "../icons/mic16.xpm"
#include "../icons/notemarker.xpm"
#include "../icons/traffic.xpm"
#include "../icons/music.xpm"


ListLog::ListLog(LogPlay *log,int id,bool allow_pause,
		 QWidget *parent,const char *name)
  : QWidget(parent,name)
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
  // Create Fonts
  //
  QFont list_font=QFont("Helvetica",12,QFont::Normal);
  list_font.setPixelSize(12);
  setFont(list_font);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont font=QFont("Helvetica",14,QFont::Bold);
  font.setPixelSize(14);
  QFont name_font=QFont("Helvetica",18,QFont::Bold);
  name_font.setPixelSize(18);

  //
  // Create Icons
  //
  list_playout_map=new QPixmap(play_xpm);
  list_macro_map=new QPixmap(rml5_xpm);
  list_chain_map=new QPixmap(chain_xpm);
  list_track_cart_map=new QPixmap(track_cart_xpm);
  list_mic16_map=new QPixmap(mic16_xpm);
  list_notemarker_map=new QPixmap(notemarker_xpm);
  list_traffic_map=new QPixmap(traffic_xpm);
  list_music_map=new QPixmap(music_xpm);

  //
  // Create Palettes
  //
  list_from_color=
    QPalette(QColor(BUTTON_FROM_BACKGROUND_COLOR),QColor(lightGray));
  list_from_color.
    setColor(QColorGroup::ButtonText,QColor(BUTTON_FROM_TEXT_COLOR));

  list_to_color=
    QPalette(QColor(BUTTON_TO_BACKGROUND_COLOR),QColor(lightGray));
  list_to_color.
    setColor(QColorGroup::ButtonText,QColor(BUTTON_TO_TEXT_COLOR));
  list_list_to_color=palette();
  list_list_to_color.setColor(QPalette::Active,QColorGroup::Highlight,
			      BUTTON_TO_BACKGROUND_COLOR);
  list_list_to_color.setColor(QPalette::Active,QColorGroup::HighlightedText,
			      BUTTON_TO_TEXT_COLOR);
  list_list_to_color.setColor(QPalette::Inactive,QColorGroup::Highlight,
			      BUTTON_TO_BACKGROUND_COLOR);
  list_list_to_color.setColor(QPalette::Inactive,QColorGroup::HighlightedText,
			      BUTTON_TO_TEXT_COLOR);
  list_list_from_color=palette();
  list_list_from_color.setColor(QPalette::Active,QColorGroup::Highlight,
				BUTTON_FROM_BACKGROUND_COLOR);
  list_list_from_color.setColor(QPalette::Active,QColorGroup::HighlightedText,
				BUTTON_FROM_TEXT_COLOR);
  list_list_from_color.setColor(QPalette::Inactive,QColorGroup::Highlight,
				BUTTON_FROM_BACKGROUND_COLOR);
  list_list_from_color.
    setColor(QPalette::Inactive,QColorGroup::HighlightedText,
	     BUTTON_FROM_TEXT_COLOR);
  list_scroll_color[0]=palette();
  list_scroll_color[0].setColor(QPalette::Active,QColorGroup::ButtonText,
			BUTTON_LOG_ACTIVE_TEXT_COLOR);
  list_scroll_color[0].setColor(QPalette::Active,QColorGroup::Button,
			BUTTON_LOG_ACTIVE_BACKGROUND_COLOR);
  list_scroll_color[0].setColor(QPalette::Active,QColorGroup::Background,
			lightGray);
  list_scroll_color[0].setColor(QPalette::Inactive,QColorGroup::ButtonText,
			BUTTON_LOG_ACTIVE_TEXT_COLOR);
  list_scroll_color[0].setColor(QPalette::Inactive,QColorGroup::Button,
			BUTTON_LOG_ACTIVE_BACKGROUND_COLOR);
  list_scroll_color[0].setColor(QPalette::Inactive,QColorGroup::Background,
			lightGray);
  list_scroll_color[1]=QPalette(backgroundColor(),lightGray);

  //
  // Hour Selector
  //
  list_hour_selector=new HourSelector(this);
  list_hour_selector->setTimeMode(list_time_mode);
  connect(list_hour_selector,SIGNAL(hourSelected(int)),
	  this,SLOT(selectHour(int)));
  list_hour_selector->hide();

  //
  // Log List
  //
  list_log_list=new LibListView(this,"list_log_list");
  list_log_list->setFont(list_font);
  int y=0;
  int h=sizeHint().height()-60;
  if(rdairplay_conf->showCounters()) {
    h-=60;
  }
  if(rdairplay_conf->hourSelectorEnabled()) {
    y+=80;
    h-=80;
    list_hour_selector->setGeometry(0,0,sizeHint().width(),80);
    list_hour_selector->show();
  }
  list_log_list->setGeometry(0,y,sizeHint().width(),h);
  list_log_list->setAllColumnsShowFocus(true);
  list_log_list->setSelectionMode(QListView::Extended);
  list_log_list->setItemMargin(5);
  list_log_list->addColumn("");
  list_log_list->setColumnAlignment(0,Qt::AlignHCenter);
  list_log_list->addColumn(tr("EST TIME"));
  list_log_list->setColumnAlignment(1,Qt::AlignRight);
  list_log_list->addColumn(tr("LEN"));
  list_log_list->setColumnAlignment(2,Qt::AlignRight);
  list_log_list->addColumn(tr("TRANS"));
  list_log_list->setColumnAlignment(3,Qt::AlignHCenter);
  list_log_list->addColumn(tr("CART"));
  list_log_list->setColumnAlignment(4,Qt::AlignHCenter);
  list_log_list->addColumn(tr("TITLE"));
  list_log_list->setColumnAlignment(5,Qt::AlignLeft);
  list_log_list->addColumn(tr("ARTIST"));
  list_log_list->setColumnAlignment(6,Qt::AlignLeft);
  list_log_list->addColumn(tr("GROUP"));
  list_log_list->setColumnAlignment(7,Qt::AlignHCenter);
  list_log_list->addColumn(tr("TIME"));
  list_log_list->setColumnAlignment(8,Qt::AlignHCenter);
  list_log_list->addColumn(tr("ALBUM"));
  list_log_list->setColumnAlignment(9,Qt::AlignLeft);
  list_log_list->addColumn(tr("LABEL"));
  list_log_list->setColumnAlignment(10,Qt::AlignLeft);
  list_log_list->addColumn(tr("CLIENT"));
  list_log_list->setColumnAlignment(11,Qt::AlignLeft);
  list_log_list->addColumn(tr("AGENCY"));
  list_log_list->setColumnAlignment(12,Qt::AlignLeft);
  list_log_list->addColumn(tr("MARKER"));
  list_log_list->setColumnAlignment(13,Qt::AlignHCenter);
  list_log_list->addColumn(tr("LINE ID"));
  list_log_list->setColumnAlignment(14,Qt::AlignHCenter);
  list_log_list->addColumn(tr("COUNT"));
  list_log_list->setColumnAlignment(15,Qt::AlignHCenter);
  list_log_list->addColumn(tr("STATUS"));
  list_log_list->setColumnAlignment(16,Qt::AlignHCenter);
  list_log_list->setHardSortColumn(15);
  list_log_list->setFocusPolicy(QWidget::NoFocus);
  connect(list_log_list,SIGNAL(selectionChanged()),
	  this,SLOT(selectionChangedData()));
  connect(list_log_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleclickedData(QListViewItem *,const QPoint &,int)));
  connect(list_log_list,SIGNAL(cartDropped(int,RDLogLine *)),
	  this,SLOT(cartDroppedData(int,RDLogLine *)));

  //
  // List Logs Dialog
  //
  list_logs_dialog=new ListLogs(list_log,this,"list_logs");

  //
  // Time Counter Section
  //
  QLabel *label=new QLabel(tr("Run Length"),this);
  label->setGeometry(372,sizeHint().height()-120,75,20);
  label->setFont(label_font);
  label->setAlignment(AlignCenter);  
  label->setBackgroundColor(QColor(lightGray));
  if(!rdairplay_conf->showCounters()) {
    label->hide();
  }

  //
  // Stop Time Counter
  //
  list_stoptime_edit=new QLineEdit(this,"list_stoptime_edit");
  list_stoptime_edit->setGeometry(407,sizeHint().height()-100,70,18);
  list_stoptime_label=new QLabel(list_stoptime_edit,tr("Next Stop:"),this);
  list_stoptime_label->setGeometry(337,sizeHint().height()-100,65,18);
  list_stoptime_label->setFont(label_font);
  list_stoptime_label->setAlignment(AlignRight|AlignVCenter);  
  list_stoptime_label->setBackgroundColor(QColor(lightGray));
  if(!rdairplay_conf->showCounters()) {
    list_stoptime_edit->hide();
    list_stoptime_label->hide();
  }

  //
  // End Time Counter
  //
  list_endtime_edit=new QLineEdit(this,"list_endtime_edit");
  list_endtime_edit->setGeometry(407,sizeHint().height()-80,70,18);
  label=new QLabel(list_endtime_edit,tr("Log End:"),this);
  label->setGeometry(337,sizeHint().height()-80,65,18);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);  
  label->setBackgroundColor(QColor(lightGray));
  if(!rdairplay_conf->showCounters()) {
    list_endtime_edit->hide();
    label->hide();
  }

  //
  // Select Button
  //
  list_take_button=new QPushButton(this,"list_take_button");
  list_take_button->setGeometry(10,sizeHint().height()-55,80,50);
  list_take_button->setFont(font);
  list_take_button->setPalette(QPalette(backgroundColor(),QColor(lightGray)));
  list_take_button->setText(tr("Select"));
  list_take_button->setFocusPolicy(QWidget::NoFocus);
  connect(list_take_button,SIGNAL(clicked()),this,SLOT(takeButtonData()));
  list_take_button->hide();

  //
  // Audition Head Button
  //
  list_head_button=new QPushButton(this,"list_head_button");
  list_head_button->setGeometry(10,sizeHint().height()-113,80,50);
  list_head_button->setFont(font);
  list_head_button->setPalette(QPalette(backgroundColor(),QColor(lightGray)));
  list_head_button->setText(tr("Audition\nHead"));
  list_head_button->setFocusPolicy(QWidget::NoFocus);
  connect(list_head_button,SIGNAL(clicked()),this,SLOT(headButtonData()));
  if(!rdairplay_conf->showCounters()) {
    list_head_button->hide();
  }

  //
  // Audition Tail Button
  //
  list_tail_button=new QPushButton(this,"list_tail_button");
  list_tail_button->setGeometry(90,sizeHint().height()-113,80,50);
  list_tail_button->setFont(font);
  list_tail_button->setPalette(QPalette(backgroundColor(),QColor(lightGray)));
  list_tail_button->setText(tr("Audition\nTail"));
  list_tail_button->setFocusPolicy(QWidget::NoFocus);
  connect(list_tail_button,SIGNAL(clicked()),this,SLOT(tailButtonData()));
  if(!rdairplay_conf->showCounters()) {
    list_tail_button->hide();
  }

  //
  // Play Button
  //
  list_play_button=new QPushButton(this,"list_play_button");
  list_play_button->setGeometry(10,sizeHint().height()-55,80,50);
  list_play_button->setFont(font);
  list_play_button->setPalette(QPalette(backgroundColor(),QColor(lightGray)));
  list_play_button->setText(tr("Start"));
  list_play_button->setDisabled(true);
  list_play_button->setFocusPolicy(QWidget::NoFocus);
  connect(list_play_button,SIGNAL(clicked()),this,SLOT(playButtonData()));

  //
  // Next Button
  //
  list_next_button=new QPushButton(this,"list_next_button");
  list_next_button->setGeometry(90,sizeHint().height()-55,80,50);
  list_next_button->setFont(font);
  list_next_button->setPalette(QPalette(backgroundColor(),
					  QColor(lightGray)));
  list_next_button->setText(tr("Make\nNext"));
  list_next_button->setDisabled(true);
  list_next_button->setFocusPolicy(QWidget::NoFocus);
  connect(list_next_button,SIGNAL(clicked()),this,SLOT(nextButtonData()));

  //
  // Modify Button
  //
  list_modify_button=new QPushButton(this,"list_modify_button");
  list_modify_button->setGeometry(170,sizeHint().height()-55,80,50);
  list_modify_button->setFont(font);
  list_modify_button->setPalette(QPalette(backgroundColor(),
					  QColor(lightGray)));
  list_modify_button->setText(tr("Modify"));
  list_modify_button->setDisabled(true);
  list_modify_button->setFocusPolicy(QWidget::NoFocus);
  connect(list_modify_button,SIGNAL(clicked()),this,SLOT(modifyButtonData()));

  //
  // Scroll Button
  //
  list_scroll_button=new QPushButton(this,"list_scroll_button");
  list_scroll_button->setGeometry(250,sizeHint().height()-55,80,50);
  list_scroll_button->setFont(font);
  list_scroll_button->setPalette(QPalette(backgroundColor(),
					  QColor(lightGray)));
  list_scroll_button->setText(tr("Scroll"));
  list_scroll_button->setFocusPolicy(QWidget::NoFocus);
  connect(list_scroll_button,SIGNAL(clicked()),this,SLOT(scrollButtonData()));
  list_scroll_button->setPalette(list_scroll_color[0]);

  //
  // Refresh Button
  //
  list_refresh_button=new QPushButton(this,"list_refresh_button");
  list_refresh_button->setGeometry(330,sizeHint().height()-55,80,50);
  list_refresh_button->setFont(font);
  list_refresh_button->
    setPalette(QPalette(backgroundColor(),QColor(lightGray)));
  list_refresh_button->setText(tr("Refresh\nLog"));
  list_refresh_button->setDisabled(true);
  list_refresh_button->setFocusPolicy(QWidget::NoFocus);
  connect(list_refresh_button,SIGNAL(clicked()),
	  this,SLOT(refreshButtonData()));

  //
  // Log Load   
  //
  list_load_button=new QPushButton(this,"list_load_button");
  list_load_button->setGeometry(sizeHint().width()-90,sizeHint().height()-55,
				80,50);
  list_load_button->setFont(font);
  list_load_button->setPalette(QPalette(backgroundColor(),QColor(lightGray)));
  list_load_button->setText(tr("Select\nLog"));
  list_load_button->setFocusPolicy(QWidget::NoFocus);
  connect(list_load_button,SIGNAL(clicked()),this,SLOT(loadButtonData()));

  //
  // Edit Event Dialog
  //
  list_event_edit=new EditEvent(list_log,this,"list_event_edit");

  //
  // Map Slots
  //
  connect(list_log,SIGNAL(reloaded()),this,SLOT(logReloadedData()));
  connect(list_log,SIGNAL(played(int)),this,SLOT(logPlayedData(int)));
  connect(list_log,SIGNAL(paused(int)),this,SLOT(logPausedData(int)));
  connect(list_log,SIGNAL(stopped(int)),this,SLOT(logStoppedData(int)));
  connect(list_log,SIGNAL(inserted(int)),this,SLOT(logInsertedData(int)));
  connect(list_log,SIGNAL(removed(int,int,bool)),
	  this,SLOT(logRemovedData(int,int,bool)));
  connect(list_log,SIGNAL(transportChanged()),
	  this,SLOT(transportChangedData()));
  connect(list_log,SIGNAL(modified(int)),this,SLOT(modifiedData(int)));
  connect(list_log,SIGNAL(refreshabilityChanged(bool)),
	  this,SLOT(refreshabilityChangedData(bool)));
  connect(list_log,SIGNAL(auditionHeadPlayed(int)),
	  this,SLOT(auditionHeadData(int)));
  connect(list_log,SIGNAL(auditionTailPlayed(int)),
	  this,SLOT(auditionTailData(int)));
  connect(list_log,SIGNAL(auditionStopped(int)),
	  this,SLOT(auditionStoppedData(int)));

  setBackgroundColor(QColor(lightGray));

  RefreshList();
  UpdateTimes();
}


QSize ListLog::sizeHint() const
{
  return QSize(500,530);
}


QSizePolicy ListLog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListLog::refresh()
{
  RefreshList();
  UpdateTimes();
}


void ListLog::refresh(int line)
{
  RefreshList(line);
  UpdateTimes();
}


void ListLog::setStatus(int line,RDLogLine::Status status)
{
  RDListViewItem *next=GetItem(line);
  if(next==NULL) {
    return;
  }
  next->setText(16,QString().sprintf("%d",status));
}


void ListLog::setOpMode(RDAirPlayConf::OpMode mode)
{
  if(mode==list_op_mode) {
    return;
  }
  list_op_mode=mode;
  UpdateTimes();
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
	list_log_list->setSelectionMode(QListView::Extended);
	list_log_list->setPalette(palette());
	list_take_button->hide();
	list_play_button->show();
	list_next_button->show();
	list_modify_button->show();
	list_scroll_button->show();
	list_refresh_button->show();
	list_load_button->show();
	break;

      case RDAirPlayConf::AddTo:
	list_log_list->setSelectionMode(QListView::Single);
	list_log_list->setPalette(list_list_to_color);
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
	list_log_list->setSelectionMode(QListView::Single);
	list_log_list->setPalette(list_list_from_color);
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
	list_log_list->setSelectionMode(QListView::Single);
	list_log_list->setPalette(list_list_from_color);
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
	list_log_list->setSelectionMode(QListView::Single);
	list_log_list->setPalette(list_list_to_color);
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
	list_log_list->setSelectionMode(QListView::Single);
	list_log_list->setPalette(list_list_from_color);
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
	list_log_list->setSelectionMode(QListView::Single);
	list_log_list->setPalette(list_list_to_color);
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
  list_time_mode=mode;
  UpdateTimes();
  RefreshList();
}


void ListLog::userChanged(bool add_allowed,bool delete_allowed,
			  bool arrange_allowed,bool playout_allowed)
{
  list_load_button->setEnabled(playout_allowed);
}


void ListLog::selectHour(int hour)
{
  RDListViewItem *item=(RDListViewItem *)list_log_list->firstChild();
  while(item!=NULL) {
    if(PredictedStartHour(item)==hour) {
      list_log_list->clearSelection();
      list_log_list->ensureItemVisible(item);
      list_log_list->setSelected(item,true);
      return;
    }
    item=(RDListViewItem *)item->nextSibling();
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
	      line=list_log_list->currentItem()->text(15).toInt();
	      break;
	}
	break;
	  
      case RDAirPlayConf::CopyFrom:
	line=list_log_list->currentItem()->text(15).toInt();
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
	      line=list_log_list->currentItem()->text(15).toInt();
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
	      line=list_log_list->currentItem()->text(15).toInt();
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
  RDListViewItem *item=(RDListViewItem *)list_log_list->currentItem();
  if((item==NULL)||
     ((item->text(16).toInt()!=RDLogLine::Scheduled)&&
      (item->text(16).toInt()!=RDLogLine::Paused)&&
      (item->text(16).toInt()!=RDLogLine::NoCart)&&
      (item->text(16).toInt()!=RDLogLine::NoCut))) {
    return;
  }
  int line=item->text(15).toInt();
  if(list_event_edit->exec(line)==0) {
    list_log->lineModified(line);
  }
  if(line==1) {
    return;
  }
  refresh(line);
  ClearSelection();
}


void ListLog::doubleclickedData(QListViewItem *,const QPoint &,int)
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
  if((list_log_list->currentItem()==NULL)||
     (list_log_list->currentItem()->text(16).toInt()!=RDLogLine::Scheduled)) {
    return;
  }
  int line=list_log_list->currentItem()->text(15).toInt();
  list_log->makeNext(line);
  ClearSelection();
}


void ListLog::loadButtonData()
{
  QString name=list_log->logName().left(list_log->logName().length()-4);
  QString svcname=list_log->serviceName();

  RDLog *edit_log;
  QString sql;
  RDSqlQuery *q;

  switch(list_logs_dialog->exec(&name,&svcname)) {
      case 0:
	list_log->setLogName(RDLog::tableName(name));
	list_log->load();
	break;

      case 2:
	list_log->save();
	edit_log=new RDLog(list_log->logName().
			   left(list_log->logName().length()-4));
	delete edit_log;
	break;

      case 3:
        sql=QString().sprintf("insert into LOGS set \
                                       NAME=\"%s\",TYPE=0,\
                                       DESCRIPTION=\"%s log\",\
                                       ORIGIN_USER=\"%s\",\
                                       ORIGIN_DATETIME=NOW(),\
                                       LINK_DATETIME=NOW(),\
                                       MODIFIED_DATETIME=NOW(),\
                                       SERVICE=\"%s\"",
			      (const char *)name,
			      (const char *)name,
			      (const char *)rdripc->user(),
			      (const char *)svcname);
	q=new RDSqlQuery(sql);
	if(!q->isActive()) {
	  QMessageBox::warning(this,tr("Log Exists"),
			       tr("Log Already Exists!"));
	  delete q;
	  return;
	}
	delete q;
	edit_log=new RDLog(name,true);
	RDCreateLogTable(RDLog::tableName(name));
	list_log->setServiceName(svcname);
	list_log->setLogName(RDLog::tableName(name));
	list_log->save();
	edit_log->setModifiedDatetime(QDateTime(QDate::currentDate(),
						QTime::currentTime()));
	delete edit_log;
	break;

      case -1:
	list_log->clear();
	break;
  } 
}


void ListLog::logReloadedData()
{
  RefreshList();
  UpdateTimes();
  selectionChangedData();
}


void ListLog::logPlayedData(int line)
{
  setStatus(line,RDLogLine::Playing);
  UpdateTimes();
}


void ListLog::logPausedData(int line)
{
  setStatus(line,RDLogLine::Paused);
  UpdateTimes();
}


void ListLog::logStoppedData(int line)
{
  setStatus(line,RDLogLine::Finished);
  UpdateTimes();
}


void ListLog::logInsertedData(int line)
{
  bool appended=false;

  if(line>=list_log->size()) {
    line=list_log->size()-1;
  }
  if(line>=list_log->size()-1) {
    appended=true;
  }
  int count;
  RDListViewItem *item=GetItem(line+1);
  while(item!=NULL) {
    if((count=item->text(15).toInt())>=0) {
      item->setText(15,QString().sprintf("%d",count+1));
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  item=new RDListViewItem(list_log_list);
  list_log->logLine(line)->setListViewItem(item);
  RefreshItem(item,line);
  if(appended) {
    if((item=(RDListViewItem *)list_log_list->findItem("-2",13))!=NULL) {
      list_log_list->ensureItemVisible(item);
    }
  }
}


void ListLog::logRemovedData(int line,int num,bool moving)
{
  int count;
  RDListViewItem *item=GetItem(line+num);
  while(item!=NULL) {
    if((count=item->text(15).toInt())>=0) {
      item->setText(15,QString().sprintf("%d",count-num));
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  for(int i=line;i<(line+num);i++) {
    delete GetItem(i);
  }
  if(!moving) {
    UpdateTimes(line,num);
  }
}


void ListLog::selectionChangedData()
{
  int count=0;
  RDListViewItem *item=NULL;
  RDListViewItem *next=(RDListViewItem *)list_log_list->firstChild();
  int start_line=-1;
  int end_line=-1;

  while(next!=NULL) {
    if(list_log_list->isSelected(next)) {
      item=next;
      if((start_line<0)&&(next->text(14).toInt()!=END_MARKER_ID)) {
	start_line=next->text(15).toInt();
      }
      if(next->text(12).toInt()!=END_MARKER_ID) {
	end_line=next->text(15).toInt();
      }
      count++;
    }
    next=(RDListViewItem *)next->nextSibling();
  }
  if(count!=1) {
    list_endtime_edit->setText("");
    list_stoptime_label->setText(tr("Selected:"));
    SetPlaybuttonMode(ListLog::ButtonDisabled);
    list_modify_button->setDisabled(true);
    list_next_button->setDisabled(true);
    if(start_line>=0) {
      list_stoptime_edit->setText(RDGetTimeLength(list_log->
		      length(start_line,end_line+1),true,false));
    }
    return;
  }  
  switch(CurrentStatus()) {
      case RDLogLine::Scheduled:
      case RDLogLine::Paused:
	SetPlaybuttonMode(ListLog::ButtonPlay);
	list_modify_button->setEnabled(true);
	list_next_button->setEnabled(true);
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
  if(item->text(15).toInt()>=0) {
    list_endtime_edit->setText(RDGetTimeLength(list_log->
      length(item->text(15).toInt(),list_log->size()),true,false));
    list_stoptime_label->setText(tr("Next Stop:"));
    int stoplen=list_log->lengthToStop(item->text(15).toInt());
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

  SetColor();

  list_log->transportEvents(transport_line);
  for(int i=0;i<TRANSPORT_QUANTITY;i++) {
    if(transport_line[i]!=-1) {
      UpdateColor(transport_line[i],true);
    }
  }
  if(list_scroll&&(transport_line[0]>=0)) {
    ScrollTo(transport_line[0]);
  }
}


void ListLog::modifiedData(int line)
{
  RefreshList(line);
  UpdateTimes();
}


void ListLog::refreshabilityChangedData(bool state)
{
  list_refresh_button->setEnabled(state);
}


void ListLog::cartDroppedData(int line,RDLogLine *ll)
{
  emit cartDropped(list_id,line,ll);
}


void ListLog::paintEvent(QPaintEvent *e)
{
  if(!rdairplay_conf->showCounters()) {
    return;
  }
  int x=336;
  int y=sizeHint().height()-111;

  QPainter *p=new QPainter(this);
  p->setPen(black);
  p->setBrush(black);
  p->moveTo(x,y);
  p->lineTo(x+146,y);
  p->lineTo(x+146,y+53);
  p->lineTo(x,y+53);
  p->lineTo(x,y);

  p->end();
  delete p;
  
}


void ListLog::RefreshList()
{
  RDListViewItem *l;
  RDLogLine *logline;

  list_log_list->clear();
  l=new RDListViewItem(list_log_list);
  l->setText(5,tr("--- end of log ---"));
  l->setText(15,QString().sprintf("%d",END_MARKER_ID));
  l->setText(14,QString().sprintf("%d",list_log->size()));
  for(int i=list_log->size()-1;i>=0;i--) {
    if((logline=list_log->logLine(i))!=NULL) {
      l=new RDListViewItem(list_log_list);
      logline->setListViewItem(l);
    }
    RefreshItem(l,i);
  }
}


void ListLog::RefreshList(int line)
{
  RDListViewItem *next=(RDListViewItem *)list_log_list->firstChild();
  while((next!=NULL)&&next->text(15).toInt()!=line) {
    next=(RDListViewItem *)next->nextSibling();
  }
  if(next!=NULL) {
    RefreshItem(next,line);
  }
}


void ListLog::RefreshItem(RDListViewItem *l,int line)
{
  int lines[TRANSPORT_QUANTITY];
  bool is_next=false;

  RDLogLine *log_line=list_log->logLine(line);
  if(log_line==NULL) {
    return;
  }
  switch(log_line->timeType()) {
      case RDLogLine::Hard:
	l->setText(1,QString("T")+
		   TimeString(log_line->startTime(RDLogLine::Logged)));
	l->setText(8,QString("T")+
		   TimeString(log_line->startTime(RDLogLine::Logged)));
	for(int i=0;i<list_log_list->columns();i++) {
	  l->setTextColor(i,LOG_HARDTIME_TEXT_COLOR,QFont::Bold);
	}
	l->setText(14,"N");
	break;
      default:
	if(!log_line->startTime(RDLogLine::Logged).isNull()) {
	  l->setText(8,TimeString(log_line->startTime(RDLogLine::Logged)));
	}
	else {
	  l->setText(8,"");
	}
	for(int i=0;i<list_log_list->columns();i++) {
	  l->setTextColor(i,LOG_RELATIVE_TEXT_COLOR,QFont::Normal);
	}
	l->setText(14,"");
	break;
  }
  switch(log_line->transType()) {
      case RDLogLine::Play:
	l->setText(3,tr("PLAY"));
	l->setTextColor(3,l->textColor(2),QFont::Normal);
	break;
      case RDLogLine::Stop:
	l->setText(3,tr("STOP"));
	l->setTextColor(3,l->textColor(2),QFont::Normal);
	break;
      case RDLogLine::Segue:
	l->setText(3,tr("SEGUE"));
	if(log_line->hasCustomTransition()) {
	  l->setTextColor(3,RD_CUSTOM_TRANSITION_COLOR,QFont::Bold);
	}
	else {
	  if(log_line->timeType()==RDLogLine::Hard) {
	    l->setTextColor(3,l->textColor(2),QFont::Bold);
	  }
	  else {
	    l->setTextColor(3,l->textColor(2),QFont::Normal);
	  }
	}
	break;

      default:
	break;
  }
  switch(log_line->type()) {
      case RDLogLine::Cart:
	switch(log_line->source()) {
	    case RDLogLine::Tracker:
	      l->setPixmap(0,*list_track_cart_map);
	      break;

	    default:
	      l->setPixmap(0,*list_playout_map);
	      break;
	}
	l->setText(2,RDGetTimeLength(log_line->effectiveLength(),false,false));
	l->setText(4,QString().
		   sprintf("%06u",log_line->cartNumber()));
	if((log_line->source()!=RDLogLine::Tracker)||
	   log_line->originUser().isEmpty()||
	   (!log_line->originDateTime().isValid())) {
	  l->setText(5,log_line->title());
	}
	else {
	  l->setText(5,QString().
		     sprintf("%s -- %s %s",
			     (const char *)log_line->title(),
			     (const char *)log_line->originUser(),
			     (const char *)log_line->originDateTime().
			     toString("M/d hh:mm")));
	}
	l->setText(6,log_line->artist());
	l->setText(7,log_line->groupName());
	l->setTextColor(7,log_line->groupColor(),QFont::Bold);
	l->setText(9,log_line->album());
	l->setText(10,log_line->label());
	l->setText(11,log_line->client());
	l->setText(12,log_line->agency());
	break;

      case RDLogLine::Macro:
	l->setPixmap(0,*list_macro_map);
	l->setText(2,RDGetTimeLength(log_line->forcedLength(),false,false));
	l->setText(4,QString().
		   sprintf("%06u",log_line->cartNumber()));
	l->setText(5,log_line->title());
	l->setText(6,log_line->artist());
	l->setText(7,log_line->groupName());
	l->setTextColor(7,log_line->groupColor(),QFont::Bold);
	l->setText(9,log_line->album());
	l->setText(10,log_line->label());
	l->setText(11,log_line->client());
	l->setText(12,log_line->agency());
	break;

      case RDLogLine::Marker:
	l->setPixmap(0,*list_notemarker_map);
	l->setText(2,"00:00");
	l->setText(4,tr("MARKER"));
	l->setText(5,RDTruncateAfterWord(log_line->markerComment(),5,true));
	l->setText(13,log_line->markerLabel());
	break;

      case RDLogLine::Track:
	l->setPixmap(0,*list_mic16_map);
	l->setText(2,"00:00");
	l->setText(4,tr("TRACK"));
	l->setText(5,RDTruncateAfterWord(log_line->markerComment(),5,true));
	break;

      case RDLogLine::MusicLink:
	l->setPixmap(0,*list_music_map);
	l->setText(2,"00:00");
	l->setText(4,tr("LINK"));
	l->setText(5,tr("[music import]"));
	break;

      case RDLogLine::TrafficLink:
	l->setPixmap(0,*list_traffic_map);
	l->setText(2,"00:00");
	l->setText(4,tr("LINK"));
	l->setText(5,tr("[traffic import]"));
	break;

      case RDLogLine::Chain:
	l->setPixmap(0,*list_chain_map);
	l->setText(2,"");
	l->setText(4,tr("CHAIN TO"));
	l->setText(5,log_line->markerLabel());
	l->setText(6,RDTruncateAfterWord(log_line->markerComment(),5,true));
	break;

      default:
	break;
  }
  l->setText(14,QString().sprintf("%d",log_line->id()));
  l->setText(15,QString().sprintf("%d",line));
  l->setText(16,QString().sprintf("%d",log_line->status()));
  SetPlaybuttonMode(ListLog::ButtonDisabled);
  list_modify_button->setDisabled(true);
  switch(log_line->state()) {
      case RDLogLine::NoCart:
	if(log_line->type()==RDLogLine::Cart) {
	  l->setPixmap(0,NULL);
	  l->setText(8,"");
	  l->setText(3,"");
	  l->setText(5,tr("[CART NOT FOUND]"));
	}
	break;

      case RDLogLine::NoCut:
	if(log_line->type()==RDLogLine::Cart) {
	  l->setText(6,tr("[NO VALID CUT AVAILABLE]"));
	}
	break;

      default:
	if((log_line->type()==RDLogLine::Cart)&&
	   (log_line->effectiveLength()==0)) {
	  l->setText(6,tr("[NO AUDIO AVAILABLE]"));
	}
	break;
  }
  list_log->transportEvents(lines);
  for(int i=0;i<TRANSPORT_QUANTITY;i++) {
    if(line==lines[i]) {
      is_next=true;
    }
  }
  UpdateColor(line,is_next);
}


RDListViewItem *ListLog::GetItem(int line)
{
  RDLogLine *logline;
  if((logline=list_log->logLine(line))==NULL) {
    return NULL;
  }
  return logline->listViewItem();
}


int ListLog::CurrentLine() {
  RDListViewItem *item;

  if((item=(RDListViewItem *)list_log_list->currentItem())==NULL) {
    return -1;
  }
  if(!list_log_list->isSelected(item)) {
    return -1;
  }
  return list_log_list->currentItem()->text(15).toInt();
}


RDLogLine::Status ListLog::CurrentStatus() {
  if(list_log_list->currentItem()==NULL) {
    return RDLogLine::Finished;
  }
  return (RDLogLine::Status)list_log_list->currentItem()->text(16).toInt();
}


RDLogLine::State ListLog::CurrentState() {
  if(list_log_list->currentItem()==NULL) {
    return RDLogLine::NoCart;
  }
  return list_log->
    logLine(list_log_list->currentItem()->text(14).toInt())->state();
}


void ListLog::ClearSelection()
{
  list_log_list->clearSelection();
  SetPlaybuttonMode(ListLog::ButtonDisabled);
  list_modify_button->setDisabled(true);
  list_next_button->setDisabled(true);
}


void ListLog::UpdateTimes(int removed_line,int num_lines)
{
  QTime time;
  QTime end_time;
  int line;
  RDLogLine *logline;

  RDListViewItem *next=(RDListViewItem *)list_log_list->firstChild();
  for(int i=0;i<(list_log_list->childCount()-1);i++) {
    if((line=next->text(15).toInt())>=removed_line) {
      line+=num_lines;
    }
    if((logline=list_log->logLine(line))!=NULL) {
      switch((RDLogLine::Status)next->text(16).toInt()) {
	  case RDLogLine::Scheduled:
	  case RDLogLine::Paused:
	    switch(logline->timeType()) {
		case RDLogLine::Hard:
		  next->setText(1,QString(tr("T"))+
			  TimeString(logline->startTime(RDLogLine::Logged)));
		  break;
		  
		default:
		  if(!logline->startTime(RDLogLine::Predicted).isNull()) {
		    next->setText(1,
			TimeString(logline->startTime(RDLogLine::Predicted)));
		  }
		  else {
		    next->setText(1,"");
		  }
		  break;
	    }
	    break;

	  default:
	    next->setText(1,TimeString(logline->startTime(RDLogLine::Actual)));
	    break;
      }
      next=(RDListViewItem *)next->nextSibling();
    }
  }
  UpdateHourSelector();
}


void ListLog::ScrollTo(int line)
{
  RDListViewItem *item=GetItem(line);

  list_log_list->ensureVisible(0,list_log_list->itemPos(item),
			       0,list_log_list->size().height()/2);
  list_log_list->setCurrentItem(item);
  list_log_list->clearSelection();
}


void ListLog::UpdateColor(int line,bool next)
{
  RDLogLine *logline;
  if((logline=list_log->logLine(line))==NULL) {
    return;
  }
  RDListViewItem *item=GetItem(line);
  if(item==NULL) {
    return;
  }
  switch(logline->status()) {
  case RDLogLine::Scheduled:
  case RDLogLine::Auditioning:
    if((logline->type()==RDLogLine::Cart)&&
       (logline->state()==RDLogLine::NoCart)) {
      item->setBackgroundColor(QColor(LOG_ERROR_COLOR));
    }
    else {
      if(((logline->cutNumber()<0)&&(logline->type()==RDLogLine::Cart))||
	 (logline->state()==RDLogLine::NoCut)) {
	item->setBackgroundColor(QColor(LOG_ERROR_COLOR));
	item->setText(6,tr("[NO VALID CUT AVAILABLE]"));
      }
      else {
	if(next) {
	  if(logline->evergreen()) {
	    item->setBackgroundColor(QColor(LOG_EVERGREEN_COLOR));
	  }
	  else {
	    item->setBackgroundColor(QColor(LOG_NEXT_COLOR));
	  }
	}
	else {
	  if(logline->evergreen()) {
	    item->setBackgroundColor(QColor(LOG_EVERGREEN_COLOR));
	  }
	  else {
	    item->setBackgroundColor(QColor(LOG_SCHEDULED_COLOR));
	  }
	}
      }
    }
    break;
	
  case RDLogLine::Playing:
  case RDLogLine::Finishing:
    item->setBackgroundColor(QColor(LOG_PLAYING_COLOR));
    break;
	
  case RDLogLine::Paused:
    item->setBackgroundColor(QColor(LOG_PAUSED_COLOR));
    break;
	
  case RDLogLine::Finished:
    if(logline->state()==RDLogLine::Ok) {
      item->setBackgroundColor(QColor(LOG_FINISHED_COLOR));
    }
    else {
      item->setBackgroundColor(QColor(LOG_ERROR_COLOR));
    }
      break;
  }
}


void ListLog::SetColor()
{
  for(int i=0;i<list_log->size();i++) {
    UpdateColor(i);
  }
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


void ListLog::UpdateHourSelector()
{
  bool found[24]={false};
  RDListViewItem *item=(RDListViewItem *)list_log_list->firstChild();
  int hour=-1;

  while(item!=NULL) {
    if((hour=PredictedStartHour(item))>=0) {
      found[hour]=true;
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  list_hour_selector->updateHours(found);
}


int ListLog::PredictedStartHour(RDListViewItem *item)
{
  bool ok=false;

  if(item==NULL) {
    return -1;
  }
  QStringList item_fields=QStringList().split(":",item->text(1));
  if(item_fields.size()==3) {
    int item_hour=item_fields[0].replace("T","").toInt(&ok);
    if(ok) {
      return item_hour;
    }
  }
  return -1;
}
