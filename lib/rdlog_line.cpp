// rdlog_line.cpp
//
// A container class for a Rivendell Log Line.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlog_line.cpp,v 1.113.4.13.2.2 2014/05/22 16:12:54 cvs Exp $
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

#ifndef WIN32
#include <syslog.h>
#endif  // WIN32

#include <qobject.h>

#include <rddb.h>
#include <rdconf.h>
#include <rd.h>
#include <rdlog_line.h>
#include <rdcut.h>
#include <rdmacro_event.h>
#include <rdweb.h>

RDLogLine::RDLogLine()
{
  clear();
}


RDLogLine::RDLogLine(unsigned cartnum)
{
  QString sql;
  RDSqlQuery *q;

  clear();
  log_cart_number=cartnum;
  sql=QString().sprintf("select GROUP_NAME,TITLE,ARTIST,ALBUM,YEAR,LABEL,\
                         CLIENT,AGENCY,COMPOSER,PUBLISHER,USER_DEFINED,NOTES \
                         from CART where NUMBER=%u",log_cart_number);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    log_group_name=q->value(0).toString();
    log_title=q->value(1).toString();
    log_artist=q->value(2).toString();
    log_album=q->value(3).toString();
    log_year=QDate(q->value(4).toInt(),1,1);
    log_label=q->value(5).toString();
    log_client=q->value(6).toString();
    log_agency=q->value(7).toString();
    log_composer=q->value(8).toString();
    log_publisher=q->value(9).toString();
    log_user_defined=q->value(10).toString();
    log_cart_notes=q->value(11).toString();
  }
  delete q;
}


void RDLogLine::clear()
{
  log_id=-1;
  log_status=RDLogLine::Scheduled;
  log_state=RDLogLine::Ok;
  log_validity=RDCart::AlwaysValid;
  log_pass=0;
  log_source=RDLogLine::Manual;
  log_cart_number=0;
  for(int i=0;i<5;i++) {
    log_start_time[i]=QTime();
  }
  log_time_type=RDLogLine::Relative;
  log_origin_user="";
  log_origin_datetime=QDateTime();
  log_trans_type=RDLogLine::Play;
  for(unsigned i=0;i<2;i++) {
    log_start_point[i]=-1;
    log_end_point[i]=-1;
    log_segue_start_point[i]=-1;
    log_segue_end_point[i]=-1;
    log_fadeup_point[i]=-1;
    log_fadedown_point[i]=-1;
  }
  log_segue_gain=RD_FADE_DEPTH;
  log_segue_gain_cut=RD_FADE_DEPTH;
  log_fadedown_gain=0;
  log_fadeup_gain=0;
  log_duck_up_gain=0;
  log_duck_down_gain=0;
  log_hook_mode=false;
  log_hook_start=-1;
  log_hook_end=-1;
  log_cart_type=RDCart::Audio;
  log_group_name="";
  log_group_color=QColor();
  log_title="";
  log_artist="";
  log_publisher="";
  log_composer="";
  log_isrc="";
  log_album="";
  log_year=QDate();
  log_isci="";
  log_label="";
  log_conductor="";
  log_song_id="";
  log_client="";
  log_agency="";
  log_outcue="";
  log_description="";
  log_user_defined="";
  log_usage_code=RDCart::UsageFeature;
  log_forced_length=0;
  log_cut_quantity=0;
  log_last_cut_played=0;
  log_play_order=RDCart::Sequence;
  log_enforce_length=false;
  log_preserve_pitch=false;
  log_start_datetime=QDateTime();
  log_end_datetime=QDateTime();
  log_deck=-1;
  log_port_name="";
  log_play_time=QTime();
  log_cut_number=-1;
  log_effective_length=-1;
  log_talk_start=-1;
  log_talk_end=-1;
  log_talk_length=-1;
  log_play_position=0;
  log_type=RDLogLine::Cart;
  log_marker_comment="";
  log_marker_label="";
  log_marker_post_time=QTime();
  log_listview=NULL;
  log_grace_time=0;
  log_forced_stop=false;
  log_play_position_changed=false;
  log_evergreen=false;
  log_ext_start_time=QTime();
  log_ext_length=-1;
  log_ext_cart_name="";
  log_ext_data="";
  log_ext_event_id="";
  log_ext_annc_type="";
  log_pause_card=-1;
  log_pause_port=-1;
  log_now_next_enabled=false;
  log_zombified=false;
  log_timescaling_active=false;
  log_play_deck=NULL;
  log_asyncronous=false;
  log_cut_name="";
  log_average_segue_length=0;
  log_has_custom_transition=false;
  log_use_event_length=false;
  log_event_length=-1;
  log_link_event_name="";
  log_link_start_time=QTime();
  log_link_length=0;
  log_link_start_slop=0;
  log_link_end_slop=0;
  log_link_id=-1;
  log_link_embedded=false;
  log_start_source=RDLogLine::StartUnknown;
}


void RDLogLine::clearExternalData()
{
  log_ext_start_time=QTime();
  log_ext_length=-1;
  log_ext_data="";
  log_ext_event_id="";
  log_ext_annc_type="";
}


void RDLogLine::clearTrackData(RDLogLine::TransEdge edge)
{
  if((edge==RDLogLine::LeadingTrans)||(edge==RDLogLine::AllTrans)) {
    log_start_point[RDLogLine::LogPointer]=-1;
    log_fadeup_point[RDLogLine::LogPointer]=-1;
    log_fadeup_gain=RD_FADE_DEPTH;
    log_has_custom_transition=false;
  }
  if((edge==RDLogLine::TrailingTrans)||(edge==RDLogLine::AllTrans)) {
    log_end_point[RDLogLine::LogPointer]=-1;
    log_fadedown_point[RDLogLine::LogPointer]=-1;
    log_fadedown_gain=RD_FADE_DEPTH;
    log_segue_start_point[RDLogLine::LogPointer]=-1;
    log_segue_end_point[RDLogLine::LogPointer]=-1;
    log_segue_gain=RD_FADE_DEPTH;
  }
}


int RDLogLine::id() const
{
  return log_id;
}


void RDLogLine::setId(int id)
{
  log_id=id;
}


RDLogLine::Status RDLogLine::status() const
{
  return log_status;
}


void RDLogLine::setStatus(RDLogLine::Status stat)
{
  log_status=stat;
}


RDLogLine::State RDLogLine::state() const
{
  return log_state;
}


void RDLogLine::setState(RDLogLine::State state)
{
  log_state=state;
}


RDCart::Validity RDLogLine::validity() const
{
  return log_validity;
}


RDCart::Validity RDLogLine::validity(const QDateTime &datetime) const
{
  if(datetime.isNull()||log_end_datetime.isNull()) {
    return log_validity;
  }
  if(datetime>log_end_datetime) {
    return RDCart::NeverValid;
  }
  if(datetime<log_start_datetime) {
    return RDCart::FutureValid;
  }
  return log_validity;
}


void RDLogLine::setValidity(RDCart::Validity valid)
{
  log_validity=valid;
}


unsigned RDLogLine::pass() const
{
  return log_pass;
}


void RDLogLine::incrementPass()
{
  log_pass++;
}


void RDLogLine::clearPass()
{
  log_pass=0;
}


bool RDLogLine::zombified() const
{
  return log_zombified;
}


void RDLogLine::setZombified(bool state)
{
  log_zombified=state;
}


bool RDLogLine::evergreen() const
{
  return log_evergreen;
}


void RDLogLine::setEvergreen(bool state)
{
  log_evergreen=state;
}


RDLogLine::Source RDLogLine::source() const
{
  return log_source;
}


void RDLogLine::setSource(RDLogLine::Source src)
{
  log_source=src;
}


unsigned RDLogLine::cartNumber() const
{
  return log_cart_number;
}


void RDLogLine::setCartNumber(unsigned cart)
{
  log_cart_number=cart;
}


QTime RDLogLine::startTime(RDLogLine::StartTimeType type) const
{
  return log_start_time[type];
}


void RDLogLine::setStartTime(RDLogLine::StartTimeType type,QTime time)
{
  if(type==RDLogLine::Initial) {
    log_start_time[RDLogLine::Actual]=time;
  }
  log_start_time[type]=time;
}


int RDLogLine::graceTime() const
{
  return log_grace_time;
}


void RDLogLine::setGraceTime(int time)
{
  log_grace_time=time;
}


RDLogLine::TimeType RDLogLine::timeType() const
{
  return log_time_type;
}


void RDLogLine::setTimeType(RDLogLine::TimeType type)
{
  log_time_type=type;
}


QString RDLogLine::originUser() const
{
  return log_origin_user;
}


void RDLogLine::setOriginUser(const QString &username)
{
  log_origin_user=username;
}


QDateTime RDLogLine::originDateTime() const
{
  return log_origin_datetime;
}


void RDLogLine::setOriginDateTime(const QDateTime &datetime)
{
  log_origin_datetime=datetime;
}


RDLogLine::TransType RDLogLine::transType() const
{
  return log_trans_type;
}


void RDLogLine::setTransType(RDLogLine::TransType type)
{
  log_trans_type=type;
}


int RDLogLine::startPoint(PointerSource ptr) const
{
  if(ptr==RDLogLine::AutoPointer) {
    if(log_start_point[RDLogLine::LogPointer]>=0) {
      return log_start_point[RDLogLine::LogPointer];
    }
    return log_start_point[RDLogLine::CartPointer];
  }
  return log_start_point[ptr];
}


void RDLogLine::setStartPoint(int point,PointerSource ptr)
{
  log_start_point[ptr]=point;
}


int RDLogLine::endPoint(PointerSource ptr) const
{
  if(ptr==RDLogLine::AutoPointer) {
    if(log_end_point[RDLogLine::LogPointer]>=0) {
      return log_end_point[RDLogLine::LogPointer];
    }
    return log_end_point[RDLogLine::CartPointer];
  }
  return log_end_point[ptr];
}


void RDLogLine::setEndPoint(int point,PointerSource ptr)
{
  log_end_point[ptr]=point;
}


int RDLogLine::segueStartPoint(PointerSource ptr) const
{
  if(ptr==RDLogLine::AutoPointer) {
    if(log_segue_start_point[RDLogLine::LogPointer]>=0) {
      return log_segue_start_point[RDLogLine::LogPointer];
    }
    if(log_segue_start_point[RDLogLine::CartPointer]>=0) {
      return log_segue_start_point[RDLogLine::CartPointer];
    }
    if(log_end_point[RDLogLine::LogPointer]>=0) {
      return log_end_point[RDLogLine::LogPointer];
    }
    return log_end_point[RDLogLine::CartPointer];
  }
  return log_segue_start_point[ptr];
}


void RDLogLine::setSegueStartPoint(int point,PointerSource ptr)
{
  log_segue_start_point[ptr]=point;
}


int RDLogLine::segueEndPoint(PointerSource ptr) const
{
  if(ptr==RDLogLine::AutoPointer) {
    if(log_segue_end_point[RDLogLine::LogPointer]>=0) {
      return log_segue_end_point[RDLogLine::LogPointer];
    }
    if(log_segue_end_point[RDLogLine::CartPointer]>=0) {
      return log_segue_end_point[RDLogLine::CartPointer];
    }
    if(log_end_point[RDLogLine::LogPointer]>=0) {
      return log_end_point[RDLogLine::LogPointer];
    }
    return log_end_point[RDLogLine::CartPointer];
  }
  return log_segue_end_point[ptr];
}


void RDLogLine::setSegueEndPoint(int point,PointerSource ptr)
{
  log_segue_end_point[ptr]=point;
}


int RDLogLine::segueGain() const
{
  if(log_segue_gain>log_segue_gain_cut) {
    return log_segue_gain;
  }
  else {
    return log_segue_gain_cut;
  }
}


void RDLogLine::setSegueGain(int gain)
{
  log_segue_gain=gain;
}


int RDLogLine::fadeupPoint(RDLogLine::PointerSource ptr) const
{
  if(ptr==RDLogLine::AutoPointer) {
    if(log_fadeup_point[RDLogLine::LogPointer]>=0) {
      return log_fadeup_point[RDLogLine::LogPointer];
    }
    if(log_fadeup_point[RDLogLine::CartPointer]>=0) {
      return log_fadeup_point[RDLogLine::CartPointer];
    }
    if(log_start_point[RDLogLine::LogPointer]>=0) {
      return log_start_point[RDLogLine::LogPointer];
    }
    return log_start_point[RDLogLine::CartPointer];
  }
  return log_fadeup_point[ptr];
}


void RDLogLine::setFadeupPoint(int point,RDLogLine::PointerSource ptr)
{
  log_fadeup_point[ptr]=point;
}


int RDLogLine::fadeupGain() const
{
  return log_fadeup_gain;
}


void RDLogLine::setFadeupGain(int gain)
{
  log_fadeup_gain=gain;
}


int RDLogLine::fadedownPoint(RDLogLine::PointerSource ptr) const
{
  if(ptr==RDLogLine::AutoPointer) {
    if(log_fadedown_point[RDLogLine::LogPointer]>=0) {
      return log_fadedown_point[RDLogLine::LogPointer];
    }
    if(log_fadedown_point[RDLogLine::CartPointer]>=0) {
      return log_fadedown_point[RDLogLine::CartPointer];
    }
    if(log_start_point[RDLogLine::LogPointer]>=0) {
      return log_end_point[RDLogLine::LogPointer];
    }
    return log_end_point[RDLogLine::CartPointer];
  }
  return log_fadedown_point[ptr];
}


void RDLogLine::setFadedownPoint(int point,RDLogLine::PointerSource ptr)
{
  log_fadedown_point[ptr]=point;
}


int RDLogLine::fadedownGain() const
{
  return log_fadedown_gain;
}


void RDLogLine::setFadedownGain(int gain)
{
  log_fadedown_gain=gain;
}


int RDLogLine::duckUpGain() const
{
  return log_duck_up_gain;
}


void RDLogLine::setDuckUpGain(int gain)
{
  log_duck_up_gain=gain;
}

int RDLogLine::duckDownGain() const
{
  return log_duck_down_gain;
}


void RDLogLine::setDuckDownGain(int gain)
{
  log_duck_down_gain=gain;
}


int RDLogLine::talkStartPoint() const
{
  return log_talk_start;
}


void RDLogLine::setTalkStartPoint(int point)
{
  log_talk_start=point;
}


int RDLogLine::talkEndPoint() const
{
  return log_talk_end;
}


void RDLogLine::setTalkEndPoint(int point)
{
  log_talk_end=point;
}


bool RDLogLine::hookMode() const
{
  return log_hook_mode;
}


void RDLogLine::setHookMode(bool state)
{
  log_hook_mode=state;
}


int RDLogLine::hookStartPoint() const
{
  return log_hook_start;
}


void RDLogLine::setHookStartPoint(int point)
{
  log_hook_start=point;
}


int RDLogLine::hookEndPoint() const
{
  return log_hook_end;
}


void RDLogLine::setHookEndPoint(int point)
{
  log_hook_end=point;
}


RDCart::Type RDLogLine::cartType() const
{
  return log_cart_type;
}


void RDLogLine::setCartType(RDCart::Type type)
{
  log_cart_type=type;
}


bool RDLogLine::asyncronous() const
{
  return log_asyncronous;
}


void RDLogLine::setAsyncronous(bool state)
{
  log_asyncronous=state;
}


QString RDLogLine::groupName() const
{
  return log_group_name;
}


void RDLogLine::setGroupName(const QString &name)
{
  log_group_name=name;
}


QColor RDLogLine::groupColor() const
{
  return log_group_color;
}


void RDLogLine::setGroupColor(const QColor &color)
{
  log_group_color=color;
}


QString RDLogLine::title() const
{
  return log_title;
}


void RDLogLine::setTitle(const QString &title)
{
  log_title=title;
}


QString RDLogLine::artist() const
{
  return log_artist;
}


QString RDLogLine::publisher() const
{
  return log_publisher;
}


void RDLogLine::setPublisher(const QString &pub)
{
  log_publisher=pub;
}


QString RDLogLine::composer() const
{
  return log_composer;
}


void RDLogLine::setComposer(const QString &composer)
{
  log_composer=composer;
}


void RDLogLine::setArtist(const QString &artist)
{
  log_artist=artist;
}


QString RDLogLine::album() const
{
  return log_album;
}


void RDLogLine::setAlbum(const QString &album)
{
  log_album=album;
}


QDate RDLogLine::year() const
{
  return log_year;
}


void RDLogLine::setYear(QDate year)
{
  log_year=year;
}


QString RDLogLine::isrc() const
{
  return log_isrc;
}


void RDLogLine::setIsrc(const QString &string)
{
  log_isrc=string;
}


QString RDLogLine::isci() const
{
  return log_isci;
}


void RDLogLine::setIsci(const QString &string)
{
  log_isci=string;
}


QString RDLogLine::label() const
{
  return log_label;
}


void RDLogLine::setLabel(const QString &label)
{
  log_label=label;
}


QString RDLogLine::conductor() const
{
  return log_conductor;
}


void RDLogLine::setConductor(const QString &cond)
{
  log_conductor=cond;
}


QString RDLogLine::songId() const
{
  return log_song_id;
}


void RDLogLine::setSongId(const QString &id)
{
  log_song_id=id;
}


QString RDLogLine::client() const
{
  return log_client;
}


void RDLogLine::setClient(const QString &client)
{
  log_client=client;
}


QString RDLogLine::agency() const
{
  return log_agency;
}


void RDLogLine::setAgency(const QString &agency)
{
  log_agency=agency;
}


QString RDLogLine::outcue() const
{
  return log_outcue;
}


void RDLogLine::setOutcue(const QString &outcue)
{
  log_outcue=outcue;
}


QString RDLogLine::description() const
{
  return log_description;
}


void RDLogLine::setDescription(const QString &desc)
{
  log_description=desc;
}


QString RDLogLine::userDefined() const
{
  return log_user_defined;
}


void RDLogLine::setUserDefined(const QString &string)
{
  log_user_defined=string;
}


QString RDLogLine::cartNotes() const
{
  return log_cart_notes;
}


void RDLogLine::setCartNotes(const QString &str)
{
  log_cart_notes=str;
}


RDCart::UsageCode RDLogLine::usageCode() const
{
  return log_usage_code;
}


void RDLogLine::setUsageCode(RDCart::UsageCode code)
{
  log_usage_code=code;
}


unsigned RDLogLine::forcedLength() const
{
  return log_forced_length;
}


void RDLogLine::setForcedLength(unsigned len)
{
  log_forced_length=len;
}


unsigned RDLogLine::averageSegueLength() const
{
  return log_average_segue_length;
}


void RDLogLine::setAverageSegueLength(unsigned len)
{
  log_average_segue_length=len;
}


unsigned RDLogLine::cutQuantity() const
{
  return log_cut_quantity;
}


void RDLogLine::setCutQuantity(unsigned quan)
{
  log_cut_quantity=quan;
}


unsigned RDLogLine::lastCutPlayed() const
{
  return log_last_cut_played;
}


void RDLogLine::setLastCutPlayed(unsigned cut)
{
  log_last_cut_played=cut;
}


RDCart::PlayOrder RDLogLine::playOrder() const
{
  return log_play_order;
}


void RDLogLine::setPlayOrder(RDCart::PlayOrder order)
{
  log_play_order=order;
}


bool RDLogLine::enforceLength() const
{
  return log_enforce_length;
}


void RDLogLine::setEnforceLength(bool state)
{
  log_enforce_length=state;
}


bool RDLogLine::preservePitch() const
{
  return log_preserve_pitch;
}


void RDLogLine::setPreservePitch(bool state)
{
  log_preserve_pitch=state;
}


QDateTime RDLogLine::startDatetime() const
{
  return log_start_datetime;
}


void RDLogLine::setStartDatetime(const QDateTime &datetime)
{
  log_start_datetime=datetime;
}


QDateTime RDLogLine::endDatetime() const
{
  return log_end_datetime;
}


void RDLogLine::setEndDatetime(const QDateTime &datetime)
{
  log_end_datetime=datetime;
}


RDListViewItem *RDLogLine::listViewItem() const
{
  return log_listview;
}


void RDLogLine::setListViewItem(RDListViewItem *listview)
{
  log_listview=listview;
}


RDLogLine::Type RDLogLine::type() const
{
  return log_type;
}


void RDLogLine::setType(RDLogLine::Type type)
{
  log_type=type;
}


QString RDLogLine::markerComment() const
{
  return log_marker_comment;
}


void RDLogLine::setMarkerComment(const QString &str)
{
  log_marker_comment=str;
}


QString RDLogLine::markerLabel() const
{
  return log_marker_label;
}


void RDLogLine::setMarkerLabel(const QString &str)
{
  log_marker_label=str;
}


int RDLogLine::deck() const
{
  return log_deck;
}


void RDLogLine::setDeck(int deck)
{
  log_deck=deck;
}


QObject *RDLogLine::playDeck()
{
  return log_play_deck;
}


void RDLogLine::setPlayDeck(QObject *deck)
{
  log_play_deck=deck;
}


QString RDLogLine::portName() const
{
  return log_port_name;
}


void RDLogLine::setPortName(const QString &name)
{
  log_port_name=name;
}


QTime RDLogLine::playTime() const
{
  return log_play_time;
}


void RDLogLine::setPlayTime(QTime time)
{
  log_play_time=time;
}


QTime RDLogLine::extStartTime() const
{
  return log_ext_start_time;
}


void RDLogLine::setExtStartTime(QTime time)
{
  log_ext_start_time=time;
}


int RDLogLine::extLength() const
{
  return log_ext_length;
}


void RDLogLine::setExtLength(int length)
{
  log_ext_length=length;
}


QString RDLogLine::extCartName() const
{
  return log_ext_cart_name;
}


void RDLogLine::setExtCartName(const QString &name)
{
  log_ext_cart_name=name;
}


QString RDLogLine::extData() const
{
  return log_ext_data;
}


void RDLogLine::setExtData(const QString &data)
{
  log_ext_data=data;
}


QString RDLogLine::extEventId() const
{
  return log_ext_event_id;
}


void RDLogLine::setExtEventId(const QString &id)
{
  log_ext_event_id=id;
}


QString RDLogLine::extAnncType() const
{
  return log_ext_annc_type;
}


void RDLogLine::setExtAnncType(const QString &type)
{
  log_ext_annc_type=type;
}


int RDLogLine::cutNumber() const
{
  return log_cut_number;
}


void RDLogLine::setCutNumber(int cutnum)
{
  log_cut_number=cutnum;
}


QString RDLogLine::cutName() const
{
  return log_cut_name;
}


void RDLogLine::setCutName(const QString &cutname)
{
  log_cut_name=cutname;
}


int RDLogLine::pauseCard() const
{
  return log_pause_card;
}


void RDLogLine::setPauseCard(int card)
{
  log_pause_card=card;
}


int RDLogLine::pausePort() const
{
  return log_pause_port;
}


void RDLogLine::setPausePort(int port)
{
  log_pause_port=port;
}


bool RDLogLine::timescalingActive() const
{
  return log_timescaling_active;
}


void RDLogLine::setTimescalingActive(bool state)
{
  log_timescaling_active=state;
}


int RDLogLine::effectiveLength() const
{
  if(log_cut_number<0) {
    return log_forced_length;
  }
  return log_effective_length;
}


int RDLogLine::talkLength() const
{
  return log_talk_length;
}


int RDLogLine::segueLength(RDLogLine::TransType next_trans)
{
  switch(type()) {
      case RDLogLine::Cart:
	switch(next_trans) {
	    case RDLogLine::Stop:
	    case RDLogLine::Play:
	      return log_effective_length;
	      
	    case RDLogLine::Segue:
	      if(segueStartPoint(RDLogLine::AutoPointer)<0) {
		return log_effective_length;
	      }
	      return segueStartPoint(RDLogLine::AutoPointer)-
		startPoint(RDLogLine::AutoPointer);

	    default:
	      break;
	}
	break;

      case RDLogLine::Macro:
	return log_effective_length;

      case RDLogLine::Marker:
	return 0;

      default:
	break;
  }
  return 0;
}


int RDLogLine::segueTail(RDLogLine::TransType next_trans)
{
  switch(type()) {
      case RDLogLine::Cart:
	switch(next_trans) {
	    case RDLogLine::Stop:
	    case RDLogLine::Play:
	      return 0;
	      
	    case RDLogLine::Segue:
	      return segueEndPoint(RDLogLine::AutoPointer)-
		segueStartPoint(RDLogLine::AutoPointer);

	    default:
	      break;
	}
	break;

      default:
	return 0;
  }
  return 0;
}


int RDLogLine::forcedStop() const
{
  return log_forced_stop;
}


bool RDLogLine::hasCustomTransition() const
{
  return log_has_custom_transition;
}


void RDLogLine::setHasCustomTransition(bool state)
{
  log_has_custom_transition=state;
}


unsigned RDLogLine::playPosition() const
{
  return log_play_position;
}


void RDLogLine::setPlayPosition(unsigned pos)
{
  log_play_position=pos;
}


bool RDLogLine::playPositionChanged() const
{
  return log_play_position_changed;
}


void RDLogLine::setPlayPositionChanged(bool state)
{
  log_play_position_changed=state;
}


bool RDLogLine::nowNextEnabled() const
{
  return log_now_next_enabled;
}


void RDLogLine::setNowNextEnabled(bool state)
{
  log_now_next_enabled=state;
}


bool RDLogLine::useEventLength() const
{
  return log_use_event_length;
}


void RDLogLine::setUseEventLength(bool state)
{
  log_use_event_length=state;
}


int RDLogLine::eventLength() const
{
  return log_event_length;
}


void RDLogLine::setEventLength(int msec)
{
  log_event_length=msec;
}


QString RDLogLine::linkEventName() const
{
  return log_link_event_name;
}


void RDLogLine::setLinkEventName(const QString &name)
{
  log_link_event_name=name;
}


QTime RDLogLine::linkStartTime() const
{
  return log_link_start_time;
}


void RDLogLine::setLinkStartTime(const QTime &time)
{
  log_link_start_time=time;
}


int RDLogLine::linkLength() const
{
  return log_link_length;
}


void RDLogLine::setLinkLength(int msecs)
{
  log_link_length=msecs;
}


int RDLogLine::linkStartSlop() const
{
  return log_link_start_slop;
}


void RDLogLine::setLinkStartSlop(int msecs)
{
  log_link_start_slop=msecs;
}


int RDLogLine::linkEndSlop() const
{
  return log_link_end_slop;
}


void RDLogLine::setLinkEndSlop(int msecs)
{
  log_link_end_slop=msecs;
}


int RDLogLine::linkId() const
{
  return log_link_id;
}


void RDLogLine::setLinkId(int id)
{
  log_link_id=id;
}


bool RDLogLine::linkEmbedded() const
{
  return log_link_embedded;
}


void RDLogLine::setLinkEmbedded(bool state)
{
  log_link_embedded=state;
}


RDLogLine::StartSource RDLogLine::startSource() const
{
  return log_start_source;
}


void RDLogLine::setStartSource(RDLogLine::StartSource src)
{
  log_start_source=src;
}


QString RDLogLine::resolveWildcards(QString pattern)
{
  pattern.replace("%n",QString().sprintf("%06u",cartNumber()));
  pattern.replace("%h",QString().sprintf("%d",effectiveLength()));
  pattern.replace("%g",groupName());
  pattern.replace("%t",title());
  pattern.replace("%a",artist());
  pattern.replace("%l",album());
  pattern.replace("%y",year().toString("yyyy"));
  pattern.replace("%b",label());
  pattern.replace("%r",conductor());
  pattern.replace("%s",songId());
  pattern.replace("%c",client());
  pattern.replace("%e",agency());
  pattern.replace("%m",composer());
  pattern.replace("%p",publisher());
  pattern.replace("%u",userDefined());

  return pattern;
}


RDLogLine::State RDLogLine::setEvent(int mach,RDLogLine::TransType next_type,
				     bool timescale,int len)
{
  RDCart *cart;
  RDMacroEvent *rml_event;
  QString sql;
  RDSqlQuery *q;
  double time_ratio=1.0;

  switch(log_type) {
      case RDLogLine::Cart:
	cart=new RDCart(log_cart_number);
	if(!cart->exists()) {
	  delete cart;
#ifndef WIN32
	  syslog(LOG_USER|LOG_WARNING,"RDLogLine::setEvent(): no such cart, CART=%06u",log_cart_number);
#endif  // WIN32
	  log_state=RDLogLine::NoCart;
	  return RDLogLine::NoCart;
	}
	cart->selectCut(&log_cut_name);
	if(log_cut_name.isEmpty()) {
	  delete cart;
#ifndef WIN32
	  // syslog(LOG_USER|LOG_WARNING,"RDLogLine::setEvent(): RDCut::selectCut() failed, CART=%06u",log_cart_number);
#endif  // WIN32
	  log_state=RDLogLine::NoCut;
	  return RDLogLine::NoCut;
	}
	log_cut_number=log_cut_name.right(3).toInt();
	sql=QString().sprintf("select LENGTH,START_POINT,END_POINT,\
                               SEGUE_START_POINT,SEGUE_END_POINT,\
			       SEGUE_GAIN,\
                               TALK_START_POINT,TALK_END_POINT,\
                               HOOK_START_POINT,HOOK_END_POINT,\
                               OUTCUE,ISRC,ISCI,DESCRIPTION from CUTS\
                               where CUT_NAME=\"%s\"",
			      (const char *)log_cut_name);
	q=new RDSqlQuery(sql);
	if(!q->first()) {
	  delete q;
	  delete cart;
#ifndef WIN32
	  syslog(LOG_USER|LOG_WARNING,"RDLogLine::setEvent(): no cut record found, SQL=%s",(const char *)sql);
#endif  // WIN32
	  log_state=RDLogLine::NoCut;
	  return RDLogLine::NoCut;
	}
	if(q->value(0).toInt()==0) {
	  delete q;
	  delete cart;
#ifndef WIN32
	  syslog(LOG_USER|LOG_WARNING,"RDLogLine::setEvent(): zero length cut audio, SQL=%s",(const char *)sql);
#endif  // WIN32
	  log_state=RDLogLine::NoCut;
	  return RDLogLine::NoCut;
	}
	if(timescale) {
	  if(len>0) {
	    log_effective_length=len;
	    log_forced_length=len;
	  }
	  else {
	    if(log_hook_mode&&
	       (q->value(8).toInt()>=0)&&(q->value(9).toInt()>=0)) {
	      log_effective_length=q->value(9).toInt()-q->value(8).toInt();
	      log_forced_length=log_effective_length;
	      time_ratio=1.0;
	      timescale=false;
	    }
	    else {
	      log_effective_length=cart->forcedLength();
	      time_ratio=(double)log_forced_length/
		(q->value(2).toDouble()-q->value(1).toDouble());
	      if(((1.0/time_ratio)<RD_TIMESCALE_MIN)||
		 ((1.0/time_ratio)>RD_TIMESCALE_MAX)) {
		timescale=false;
	      }
	    }
	  }
	}
	if(timescale) {
	  log_start_point[0]=(int)(q->value(1).toDouble()*time_ratio);
	  log_end_point[0]=(int)(q->value(2).toDouble()*time_ratio);
	  if(q->value(3).toInt()>=0) {
	    log_segue_start_point[0]=(int)(q->value(3).toDouble()*time_ratio);
	    log_segue_end_point[0]=(int)(q->value(4).toDouble()*time_ratio);
	  }
	  else {
	    log_segue_start_point[0]=-1;
	    log_segue_end_point[0]=-1;
	  }
	  if(log_talk_start>=0) {
	    log_talk_start=(int)((double)log_talk_start*time_ratio);
	    log_talk_end=(int)(q->value(7).toDouble()*time_ratio);
	  }
	  else {
	    log_talk_start=-1;
	    log_talk_end=-1;
	  }
	  log_talk_length=log_talk_end-log_talk_start;
	}
	else {
	  if(log_hook_mode&&
	     (q->value(8).toInt()>=0)&&(q->value(9).toInt()>=0)) {
	    log_start_point[0]=q->value(8).toInt();
	    log_end_point[0]=q->value(9).toInt();
	    log_segue_start_point[0]=-1;
	    log_segue_end_point[0]=-1;
	    log_talk_start=-1;
	    log_talk_end=-1;
	  }
	  else {
	    log_start_point[0]=q->value(1).toInt();
	    log_end_point[0]=q->value(2).toInt();
	    if(log_start_point[RDLogLine::LogPointer]>=0 ||
	       log_end_point[RDLogLine::LogPointer]>=0) {
	      log_effective_length=log_end_point[RDLogLine::LogPointer]-
		log_start_point[RDLogLine::LogPointer];
	    }
	    else {
	      log_effective_length=q->value(0).toUInt();
	    }
	    log_segue_start_point[0]=q->value(3).toInt();
	    log_segue_end_point[0]=q->value(4).toInt();
	    log_talk_start=q->value(6).toInt();
	    log_talk_end=q->value(7).toInt();
	  }
	  log_hook_start=q->value(8).toInt();
	  log_hook_end=q->value(9).toInt();
          if(log_talk_end>log_end_point[RDLogLine::LogPointer] && 
             log_end_point[RDLogLine::LogPointer]>=0) {
            log_talk_end=log_end_point[RDLogLine::LogPointer];
          }
          if(log_talk_end<log_start_point[RDLogLine::LogPointer]) {
            log_talk_end=0;
            log_talk_start=0;
          }
          else {
            if(log_talk_start<log_start_point[RDLogLine::LogPointer]) {
              log_talk_start=0;
              log_talk_end-=log_start_point[RDLogLine::LogPointer];
            }
            if(log_talk_start>log_end_point[RDLogLine::LogPointer] &&
               log_end_point[RDLogLine::LogPointer]>=0) {
              log_talk_start=0;
              log_talk_end=0;
            }
          }
	  log_talk_length=log_talk_end-log_talk_start;
	}
	if(segueStartPoint(RDLogLine::AutoPointer)<0) {
	  log_average_segue_length=cart->averageSegueLength();
	}
	else {
	  log_average_segue_length=segueStartPoint(RDLogLine::AutoPointer)-
	    startPoint(RDLogLine::AutoPointer);
	}
	log_outcue=q->value(10).toString();
	log_isrc=q->value(11).toString();
	log_isci=q->value(12).toString();
	log_description=q->value(13).toString();
	log_segue_gain_cut=q->value(5).toInt();
	delete q;
	delete cart;
	break;

      case RDLogLine::Macro:
	cart=new RDCart(log_cart_number);
	log_effective_length=cart->forcedLength();
	log_average_segue_length=log_effective_length;
	log_forced_stop=false;
	rml_event=new RDMacroEvent();
	rml_event->load(cart->number());
	for(int i=0;i<rml_event->size();i++) {
	  if(rml_event->command(i)->command()==RDMacro::LL) {
	    if(rml_event->command(i)->arg(0).toInt()==mach) {
	      log_forced_stop=true;
	    }
	  }
	}
	log_start_point[0]=-1;
	log_end_point[0]=-1;
	log_segue_start_point[0]=-1;
	log_segue_end_point[0]=-1;
	log_talk_length=0;
	log_talk_start=-1;
	log_talk_end=-1;
	log_segue_gain_cut=0;
	delete rml_event;
	delete cart;
	break;

      case RDLogLine::Marker:
      case RDLogLine::Track:
	log_cut_number=0;
	log_cut_name="";
	log_effective_length=0;
	log_average_segue_length=0;
	log_forced_stop=false;
	log_start_point[0]=-1;
	log_end_point[0]=-1;
	log_segue_start_point[0]=-1;
	log_segue_end_point[0]=-1;
	log_talk_length=0;
	log_talk_start=-1;
	log_talk_end=-1;
	log_segue_gain_cut=0;
	break;

      default:
	break;
  }
  return RDLogLine::Ok;
}


void RDLogLine::loadCart(int cartnum,RDLogLine::TransType next_type,int mach,
			 bool timescale,RDLogLine::TransType type,int len)
{
  QString sql=QString().sprintf("select CART.TYPE,CART.GROUP_NAME,CART.TITLE,\
                                 CART.ARTIST,CART.ALBUM,CART.YEAR,CART.ISRC,\
                                 CART.LABEL,CART.CLIENT,CART.AGENCY,\
                                 CART.USER_DEFINED,CART.CONDUCTOR,CART.SONG_ID,\
                                 CART.FORCED_LENGTH,\
                                 CART.CUT_QUANTITY,CART.LAST_CUT_PLAYED,\
                                 CART.PLAY_ORDER,CART.START_DATETIME,\
                                 CART.END_DATETIME,CART.ENFORCE_LENGTH,\
                                 CART.PRESERVE_PITCH,GROUPS.ENABLE_NOW_NEXT,\
                                 CART.ASYNCRONOUS,CART.PUBLISHER,\
                                 CART.COMPOSER,CART.USAGE_CODE,\
                                 CART.AVERAGE_SEGUE_LENGTH,CART.NOTES,\
                                 GROUPS.COLOR \
                                 from CART left join GROUPS on\
                                 CART.GROUP_NAME=GROUPS.NAME\
                                 where (CART.NUMBER=%d)",
				cartnum);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    log_state=RDLogLine::NoCart;
    return;
  }
  log_cart_number=cartnum;
  log_cart_type=(RDCart::Type)q->value(0).toInt();
  switch((RDCart::Type)q->value(0).toInt()) {
      case RDCart::Audio:
	log_type=RDLogLine::Cart;
	break;

      case RDCart::Macro:
	log_type=RDLogLine::Macro;
	break;

      default:
	break;
  }
  log_group_name=q->value(1).toString();
  log_title=q->value(2).toString();
  log_artist=q->value(3).toString();
  log_album=q->value(4).toString();
  log_year=q->value(5).toDate();
  log_isrc=q->value(6).toString();
  log_label=q->value(7).toString();
  log_client=q->value(8).toString();
  log_agency=q->value(9).toString();
  log_user_defined=q->value(10).toString();
  log_conductor=q->value(11).toString();
  log_song_id=q->value(12).toString();
  log_cut_quantity=q->value(14).toUInt();
  log_last_cut_played=q->value(15).toUInt();
  log_play_order=(RDCart::PlayOrder)q->value(16).toInt();
  log_start_datetime=q->value(17).toDateTime();
  log_end_datetime=q->value(18).toDateTime();
  log_preserve_pitch=RDBool(q->value(20).toString());
  if(len<0) {
    log_forced_length=q->value(13).toUInt();
    log_enforce_length=RDBool(q->value(19).toString());
  }
  else {
    log_forced_length=len;
    log_enforce_length=true;
  }
  log_now_next_enabled=RDBool(q->value(21).toString());
  log_asyncronous=RDBool(q->value(22).toString());
  log_publisher=q->value(23).toString();
  log_composer=q->value(24).toString();
  log_usage_code=(RDCart::UsageCode)q->value(25).toInt();
  log_average_segue_length=q->value(26).toInt();
  log_cart_notes=q->value(27).toString();
  log_group_color=QColor(q->value(28).toString());
  log_play_source=RDLogLine::UnknownSource;
  if(type!=RDLogLine::NoTrans) {
    log_trans_type=type;
  }
  delete q;
  log_state=setEvent(mach,next_type,timescale);
  log_timescaling_active=log_enforce_length&&timescale;
}


void RDLogLine::refreshPointers()
{
  if(log_cut_name.isEmpty()) {
    return;
  }
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select START_POINT,END_POINT,\
                         SEGUE_START_POINT,SEGUE_END_POINT,\
                         TALK_START_POINT,TALK_END_POINT,\
                         FADEUP_POINT,FADEDOWN_POINT,\
                         HOOK_START_POINT,HOOK_END_POINT from CUTS\
                         where CUT_NAME=\"%s\"",
			(const char *)log_cut_name);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    log_start_point[RDLogLine::CartPointer]=q->value(0).toInt();
    log_end_point[RDLogLine::CartPointer]=q->value(1).toInt();
    log_segue_start_point[RDLogLine::CartPointer]=q->value(2).toInt();
    log_segue_end_point[RDLogLine::CartPointer]=q->value(3).toInt();
    log_talk_start=q->value(4).toInt();
    log_talk_end=q->value(5).toInt();
    log_talk_length=log_talk_end-log_talk_start;
    log_fadeup_point[RDLogLine::CartPointer]=q->value(6).toInt();
    log_fadedown_point[RDLogLine::CartPointer]=q->value(7).toInt();
    log_hook_start=q->value(8).toInt();
    log_hook_end=q->value(9).toInt();
  }
  delete q;
}


QString RDLogLine::xml(int line) const
{
  QString ret;
#ifndef WIN32
  ret+="  <logLine>\n";
  ret+="    "+RDXmlField("line",line);
  ret+="    "+RDXmlField("id",id());
  ret+="    "+RDXmlField("type",RDLogLine::typeText(type()));
  ret+="    "+RDXmlField("cartType",RDCart::typeText(cartType()));
  ret+="    "+RDXmlField("cartNumber",cartNumber());
  ret+="    "+RDXmlField("cutNumber",cutNumber());
  ret+="    "+RDXmlField("groupName",groupName());
  ret+="    "+RDXmlField("groupColor",groupColor().name());
  ret+="    "+RDXmlField("title",title());
  ret+="    "+RDXmlField("artist",artist());
  ret+="    "+RDXmlField("publisher",publisher());
  ret+="    "+RDXmlField("composer",composer());
  ret+="    "+RDXmlField("album",album());
  ret+="    "+RDXmlField("label",label());
  if(year().isValid()) {
    ret+="    "+RDXmlField("year",year().year());
  }
  else {
    ret+="    "+RDXmlField("year");
  }
  ret+="    "+RDXmlField("client",client());
  ret+="    "+RDXmlField("agency",agency());
  ret+="    "+RDXmlField("userDefined",userDefined());
  ret+="    "+RDXmlField("usageCode",usageCode());
  ret+="    "+RDXmlField("enforceLength",enforceLength());
  ret+="    "+RDXmlField("forcedLength",RDGetTimeLength(forcedLength(),true));
  ret+="    "+RDXmlField("evergreen",evergreen());
  ret+="    "+RDXmlField("source",RDLogLine::sourceText(source()));
  ret+="    "+RDXmlField("timeType",RDLogLine::timeTypeText(timeType()));
  if(startTime(RDLogLine::Logged).isValid()&&
     (!startTime(RDLogLine::Logged).isNull())) {
    ret+="    "+RDXmlField("startTime",startTime(RDLogLine::Logged).
			   toString("hh:mm:ss.zzz"));
  }
  else {
    ret+="    "+RDXmlField("startTime");
  }
  ret+="    "+RDXmlField("transitionType",RDLogLine::transText(transType()));
  ret+="    "+RDXmlField("cutQuantity",cutQuantity());
  ret+="    "+RDXmlField("lastCutPlayed",lastCutPlayed());
  ret+="    "+RDXmlField("markerComment",markerComment());
  ret+="    "+RDXmlField("markerLabel",markerLabel());

  ret+="    "+RDXmlField("originUser",originUser());
  ret+="    "+RDXmlField("originDateTime",originDateTime());
  ret+="    "+RDXmlField("startPoint",startPoint(RDLogLine::CartPointer),
			 "src=\"cart\"");
  ret+="    "+RDXmlField("startPoint",startPoint(RDLogLine::LogPointer),
			 "src=\"log\"");
  ret+="    "+RDXmlField("endPoint",endPoint(RDLogLine::CartPointer),
			 "src=\"cart\"");
  ret+="    "+RDXmlField("endPoint",endPoint(RDLogLine::LogPointer),
			 "src=\"log\"");
  ret+="    "+RDXmlField("segueStartPoint",
			 segueStartPoint(RDLogLine::CartPointer),
			 "src=\"cart\"");
  ret+="    "+RDXmlField("segueStartPoint",
			 segueStartPoint(RDLogLine::LogPointer),"src=\"log\"");
  ret+="    "+RDXmlField("segueEndPoint",
			 segueEndPoint(RDLogLine::CartPointer),
			 "src=\"cart\"");
  ret+="    "+RDXmlField("segueEndPoint",
			 segueEndPoint(RDLogLine::LogPointer),"src=\"log\"");
  ret+="    "+RDXmlField("segueGain",segueGain());
  ret+="    "+RDXmlField("fadeupPoint",
			 fadeupPoint(RDLogLine::CartPointer),"src=\"cart\"");
  ret+="    "+RDXmlField("fadeupPoint",
			 fadeupPoint(RDLogLine::LogPointer),"src=\"log\"");
  ret+="    "+RDXmlField("fadeupGain",fadeupGain());
  ret+="    "+RDXmlField("fadedownPoint",
			 fadedownPoint(RDLogLine::CartPointer),"src=\"cart\"");
  ret+="    "+RDXmlField("fadedownPoint",
			 fadedownPoint(RDLogLine::LogPointer),"src=\"log\"");
  ret+="    "+RDXmlField("duckUpGain",duckUpGain());
  ret+="    "+RDXmlField("duckDownGain",duckDownGain());
  ret+="    "+RDXmlField("talkStartPoint",talkStartPoint());
  ret+="    "+RDXmlField("talkEndPoint",talkEndPoint());
  ret+="    "+RDXmlField("hookMode",hookMode());
  ret+="    "+RDXmlField("hookStartPoint",hookStartPoint());
  ret+="    "+RDXmlField("hookEndPoint",hookEndPoint());

  ret+="  </logLine>\n";
#endif  // WIN32
  return ret;
}


QString RDLogLine::resolveWildcards(unsigned cartnum,const QString &pattern)
{
  RDLogLine logline;
  logline.loadCart(cartnum,RDLogLine::Play,0,false);
  return logline.resolveWildcards(pattern);
}


QString RDLogLine::startSourceText(RDLogLine::StartSource src)
{
  switch(src) {
      case RDLogLine::StartUnknown:
	return QObject::tr("Unknown");

      case RDLogLine::StartManual:
	return QObject::tr("Manual");

      case RDLogLine::StartPlay:
	return QObject::tr("Play");

      case RDLogLine::StartSegue:
	return QObject::tr("Segue");

      case RDLogLine::StartTime:
	return QObject::tr("Time");

      case RDLogLine::StartPanel:
	return QObject::tr("Panel");

      case RDLogLine::StartMacro:
	return QObject::tr("Macro");

      case RDLogLine::StartChannel:
	return QObject::tr("Channel");
  }
  return QObject::tr("Unknown");
}


QString RDLogLine::transText(RDLogLine::TransType trans)
{
  switch(trans) {
      case RDLogLine::Play:
	return QObject::tr("PLAY");

      case RDLogLine::Segue:
	return QObject::tr("SEGUE");

      case RDLogLine::Stop:
	return QObject::tr("STOP");

      case RDLogLine::NoTrans:
	return QObject::tr("UNKNOWN");
  }
  return QObject::tr("UNKNOWN");
}


QString RDLogLine::typeText(RDLogLine::Type type)
{
  switch(type) {
      case RDLogLine::Cart:
	return QObject::tr("Audio");

      case RDLogLine::Marker:
	return QObject::tr("Marker");

      case RDLogLine::Macro:
	return QObject::tr("Macro");

      case RDLogLine::OpenBracket:
	return QObject::tr("Open Bracket");

      case RDLogLine::CloseBracket:
	return QObject::tr("Close Bracket");

      case RDLogLine::Chain:
	return QObject::tr("ChainTo");

      case RDLogLine::Track:
	return QObject::tr("Track");

      case RDLogLine::MusicLink:
      case RDLogLine::TrafficLink:
	return QObject::tr("Link");

      case RDLogLine::UnknownType:
	return QObject::tr("Unknown");
  }
  return QObject::tr("Unknown");
}


QString RDLogLine::timeTypeText(RDLogLine::TimeType type)
{
  QString ret=QObject::tr("Unknown");

  switch(type) {
  case RDLogLine::Relative:
    ret=QObject::tr("Relative");
    break;

  case RDLogLine::Hard:
    ret=QObject::tr("Hard");
    break;
  }

  return ret;
}


QString RDLogLine::sourceText(RDLogLine::Source src)
{
  switch(src) {
      case RDLogLine::Manual:
	return QObject::tr("Manual");

      case RDLogLine::Traffic:
	return QObject::tr("Traffic");

      case RDLogLine::Music:
	return QObject::tr("Music");

      case RDLogLine::Template:
	return QObject::tr("RDLogManager");

      case RDLogLine::Tracker:
	return QObject::tr("Tracker");
  }
  return QObject::tr("Unknown");
}
