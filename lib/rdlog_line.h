// rdlog_line.h
//
// A container class for a Rivendell Log Line.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlog_line.h,v 1.85.8.10.2.1 2014/05/20 01:45:16 cvs Exp $
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

#ifndef RDLOG_LINE_H
#define RDLOG_LINE_H

#include <qdatetime.h>
#include <qobject.h>
#include <qcolor.h>

#include <rdcart.h>
#include <rdlistviewitem.h>


class RDLogLine
{
 public:
  enum StartTimeType {Imported=0,Logged=1,Predicted=2,Actual=3,Initial=4};
  enum TimeType {Relative=0,Hard=1};
  enum TransType {Play=0,Segue=1,Stop=2,NoTrans=255};
  enum TransEdge {AllTrans=0,LeadingTrans=1,TrailingTrans=2};
  enum Source {Manual=0,Traffic=1,Music=2,Template=3,Tracker=4};
  enum Status {Scheduled=1,Playing=2,Auditioning=3,Finished=4,
	       Finishing=5,Paused=6};
  enum State {Ok=0,NoCart=1,NoCut=2};
  enum Type {Cart=0,Marker=1,Macro=2,OpenBracket=3,CloseBracket=4,Chain=5,
	     Track=6,MusicLink=7,TrafficLink=8,UnknownType=9};
  enum StartSource {StartUnknown=0,StartManual=1,StartPlay=2,StartSegue=3,
		    StartTime=4,StartPanel=5,StartMacro=6,StartChannel=7};
  enum PlaySource {UnknownSource=0,MainLog=1,AuxLog1=2,AuxLog2=3,SoundPanel=4,
		   CartSlot=5};
  enum PointerSource {CartPointer=0,LogPointer=1,AutoPointer=2};
  RDLogLine();
  RDLogLine(unsigned cartnum);
  void clear();
  void clearExternalData();
  void clearTrackData(RDLogLine::TransEdge edge);
  int id() const;
  void setId(int id);
  RDLogLine::Status status() const;
  void setStatus(RDLogLine::Status stat);
  RDLogLine::State state() const;
  void setState(RDLogLine::State state);
  RDCart::Validity validity() const;
  RDCart::Validity validity(const QDateTime &datetime) const;
  void setValidity(RDCart::Validity valid);
  unsigned pass() const;
  void incrementPass();
  void clearPass();
  bool zombified() const;
  void setZombified(bool state);
  bool evergreen() const;
  void setEvergreen(bool state);
  RDLogLine::Source source() const;
  void setSource(RDLogLine::Source src);
  unsigned cartNumber() const;
  void setCartNumber(unsigned cart);
  QTime startTime(RDLogLine::StartTimeType type) const;
  void setStartTime(RDLogLine::StartTimeType type,QTime time);
  int graceTime() const;
  void setGraceTime(int time);
  RDLogLine::TimeType timeType() const;
  void setTimeType(RDLogLine::TimeType type);
  QString originUser() const;
  void setOriginUser(const QString &username);
  QDateTime originDateTime() const;
  void setOriginDateTime(const QDateTime &datetime);
  RDLogLine::TransType transType() const;
  void setTransType(RDLogLine::TransType type);
  int startPoint(PointerSource ptr=RDLogLine::AutoPointer) const;
  void setStartPoint(int point,PointerSource ptr);
  int endPoint(PointerSource ptr=RDLogLine::AutoPointer) const;
  void setEndPoint(int point,PointerSource ptr);
  int segueStartPoint(PointerSource ptr=RDLogLine::AutoPointer) const;
  void setSegueStartPoint(int point,PointerSource ptr);
  int segueEndPoint(PointerSource ptr=RDLogLine::AutoPointer) const;
  void setSegueEndPoint(int point,PointerSource ptr);
  int segueGain() const;
  void setSegueGain(int gain);
  int fadeupPoint(PointerSource ptr=RDLogLine::AutoPointer) const;
  void setFadeupPoint(int point,PointerSource ptr);
  int fadeupGain() const;
  void setFadeupGain(int gain);
  int fadedownPoint(PointerSource ptr=RDLogLine::AutoPointer) const;
  void setFadedownPoint(int point,PointerSource ptr);
  int fadedownGain() const;
  void setFadedownGain(int gain);
  int duckUpGain() const;
  void setDuckUpGain(int gain);
  int duckDownGain() const;
  void setDuckDownGain(int gain);
  int talkStartPoint() const;
  void setTalkStartPoint(int point);
  int talkEndPoint() const;
  void setTalkEndPoint(int point);
  bool hookMode() const;
  void setHookMode(bool state);
  int hookStartPoint() const;
  void setHookStartPoint(int point);
  int hookEndPoint() const;
  void setHookEndPoint(int point);
  RDCart::Type cartType() const;
  void setCartType(RDCart::Type type);
  bool asyncronous() const;
  void setAsyncronous(bool state);
  QString groupName() const;
  void setGroupName(const QString &name);
  QColor groupColor() const;
  void setGroupColor(const QColor &color);
  QString title() const;
  void setTitle(const QString &title);
  QString artist() const;
  void setArtist(const QString &artist);
  QString publisher() const;
  void setPublisher(const QString &pub);
  QString composer() const;
  void setComposer(const QString &composer);
  QString album() const;
  void setAlbum(const QString &album);
  QDate year() const;
  void setYear(QDate year);
  QString isrc() const;
  void setIsrc(const QString &string);
  QString isci() const;
  void setIsci(const QString &string);
  QString label() const;
  void setLabel(const QString &label);
  QString conductor() const;
  void setConductor(const QString &cond);
  QString songId() const;
  void setSongId(const QString &id);
  QString client() const;
  void setClient(const QString &client);
  QString agency() const;
  void setAgency(const QString &agency);
  QString outcue() const;
  void setOutcue(const QString &outcue);
  QString description() const;
  void setDescription(const QString &desc);
  QString userDefined() const;
  void setUserDefined(const QString &string);
  QString cartNotes() const;
  void setCartNotes(const QString &str);
  RDCart::UsageCode usageCode() const;
  void setUsageCode(RDCart::UsageCode code);
  unsigned forcedLength() const;
  void setForcedLength(unsigned len);
  unsigned averageSegueLength() const;
  void setAverageSegueLength(unsigned len);
  unsigned cutQuantity() const;
  void setCutQuantity(unsigned quan);
  unsigned lastCutPlayed() const;
  void setLastCutPlayed(unsigned cut);
  RDCart::PlayOrder playOrder() const;
  void setPlayOrder(RDCart::PlayOrder order);
  bool enforceLength() const;
  void setEnforceLength(bool state);
  bool preservePitch() const;
  void setPreservePitch(bool state);
  QDateTime startDatetime() const;
  void setStartDatetime(const QDateTime &datetime);
  QDateTime endDatetime() const;
  void setEndDatetime(const QDateTime &datetime);
  RDListViewItem *listViewItem() const;
  void setListViewItem(RDListViewItem *);
  RDLogLine::Type type() const;
  void setType(RDLogLine::Type type);
  QString markerComment() const;
  void setMarkerComment(const QString &str);
  QString markerLabel() const;
  void setMarkerLabel(const QString &str);
  int deck() const;
  void setDeck(int deck);
  QObject *playDeck();
  void setPlayDeck(QObject *deck);
  QString portName() const;
  void setPortName(const QString &name);
  QTime playTime() const;
  void setPlayTime(QTime time);
  QTime extStartTime() const;
  void setExtStartTime(QTime time);
  int extLength() const;
  void setExtLength(int length);
  QString extCartName() const;
  void setExtCartName(const QString &name);
  QString extData() const;
  void setExtData(const QString &data);
  QString extEventId() const;
  void setExtEventId(const QString &id);
  QString extAnncType() const;
  void setExtAnncType(const QString &type);
  int cutNumber() const;
  void setCutNumber(int cutnum);
  QString cutName() const;
  void setCutName(const QString &cutname);
  int pauseCard() const;
  void setPauseCard(int card);
  int pausePort() const;
  void setPausePort(int port);
  bool timescalingActive() const;
  void setTimescalingActive(bool state);
  int effectiveLength() const;
  int talkLength() const;
  int segueLength(RDLogLine::TransType next_trans);
  int segueTail(RDLogLine::TransType next_trans);
  int forcedStop() const;
  bool hasCustomTransition() const;
  void setHasCustomTransition(bool state);
  unsigned playPosition() const;
  void setPlayPosition(unsigned pos);
  bool playPositionChanged() const;
  void setPlayPositionChanged(bool state);
  bool nowNextEnabled() const;
  void setNowNextEnabled(bool state);
  bool useEventLength() const;
  void setUseEventLength(bool state);
  int eventLength() const;
  void setEventLength(int msec);
  QString linkEventName() const;
  void setLinkEventName(const QString &name);
  QTime linkStartTime() const;
  void setLinkStartTime(const QTime &time);
  int linkLength() const;
  void setLinkLength(int msecs);
  int linkStartSlop() const;
  void setLinkStartSlop(int msecs);
  int linkEndSlop() const;
  void setLinkEndSlop(int msecs);
  int linkId() const;
  void setLinkId(int id);
  bool linkEmbedded() const;
  void setLinkEmbedded(bool state);
  RDLogLine::StartSource startSource() const;
  void setStartSource(RDLogLine::StartSource src);
  QString resolveWildcards(QString pattern);
  RDLogLine::State setEvent(int mach,RDLogLine::TransType next_type,
			    bool timescale,int len=-1);
  void loadCart(int cartnum,RDLogLine::TransType next_type,int mach,
		bool timescale,RDLogLine::TransType type=RDLogLine::NoTrans,
		int len=-1);
  void refreshPointers();
  QString xml(int line) const;
  static QString resolveWildcards(unsigned cartnum,const QString &pattern);
  static QString startSourceText(RDLogLine::StartSource src);
  static QString transText(RDLogLine::TransType trans);
  static QString typeText(RDLogLine::Type type);
  static QString timeTypeText(RDLogLine::TimeType type);
  static QString sourceText(RDLogLine::Source src);

 private:
  int log_id;
  RDLogLine::Status log_status;
  RDLogLine::State log_state;
  RDLogLine::Source log_source;
  RDCart::Validity log_validity;
  unsigned log_pass;
  unsigned log_cart_number;
  QTime log_start_time[5];
  RDLogLine::TimeType log_time_type;
  QString log_origin_user;
  QDateTime log_origin_datetime;
  RDLogLine::TransType log_trans_type;
  int log_start_point[2];
  int log_end_point[2];
  int log_segue_start_point[2];
  int log_segue_end_point[2];
  int log_segue_gain;
  int log_segue_gain_cut;
  int log_fadeup_point[2];
  int log_fadeup_gain;
  int log_fadedown_point[2];
  int log_fadedown_gain;
  int log_duck_up_gain;
  int log_duck_down_gain;
  bool log_hook_mode;
  int log_hook_start;
  int log_hook_end;
  RDCart::Type log_cart_type;
  QString log_group_name;
  QColor log_group_color;
  QString log_title;
  QString log_artist;
  QString log_album;
  QString log_publisher;
  QString log_composer;
  QString log_isrc;
  QString log_isci;
  QDate log_year;
  QString log_label;
  QString log_conductor;
  QString log_song_id;
  QString log_client;
  QString log_agency;
  QString log_outcue;
  QString log_description;
  QString log_user_defined;
  QString log_cart_notes;
  RDCart::UsageCode log_usage_code;
  unsigned log_forced_length;
  unsigned log_cut_quantity;
  unsigned log_last_cut_played;
  RDCart::PlayOrder log_play_order;
  bool log_enforce_length;
  bool log_preserve_pitch;
  QDateTime log_start_datetime;
  QDateTime log_end_datetime;
  int log_deck;
  QTime log_play_time;
  int log_cut_number;
  int log_effective_length;
  int log_talk_start;
  int log_talk_end;
  int log_talk_length;
  unsigned log_play_position;
  RDLogLine::Type log_type;
  QString log_marker_comment;
  QString log_marker_label;
  QTime log_marker_post_time;
  RDListViewItem *log_listview;
  QString log_port_name;
  int log_grace_time;
  bool log_forced_stop;
  QObject *log_play_deck;
  bool log_play_position_changed;
  bool log_evergreen;
  QTime log_ext_start_time;
  int log_ext_length;
  int log_average_segue_length;
  QString log_ext_cart_name;
  QString log_ext_data;
  QString log_ext_event_id;
  QString log_ext_annc_type;
  int log_pause_card;
  int log_pause_port;
  bool log_now_next_enabled;
  PlaySource log_play_source;
  StartSource log_start_source;
  bool log_zombified;
  bool log_timescaling_active;
  bool log_asyncronous;
  QString log_cut_name;
  bool log_has_custom_transition;
  bool log_use_event_length;
  int log_event_length;
  QString log_link_event_name;
  QTime log_link_start_time;
  int log_link_length;
  int log_link_start_slop;
  int log_link_end_slop;
  int log_link_id;
  bool log_link_embedded;
};


#endif
