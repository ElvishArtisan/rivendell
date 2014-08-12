//   rdwavedata.cpp
//
//   A Container Class for Audio Meta Data.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdwavedata.cpp,v 1.4.8.3.2.2 2014/07/15 20:02:23 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <rdwavedata.h>


RDWaveData::RDWaveData()
{
  clear();
}


bool RDWaveData::metadataFound() const
{
  return data_metadata_found;
}


void RDWaveData::setMetadataFound(bool state)
{
  data_metadata_found=state;
}


QString RDWaveData::title() const
{
  return data_title;
}


void RDWaveData::setTitle(const QString &str)
{
  data_title=str;
}


QString RDWaveData::artist() const
{
  return data_artist;
}


void RDWaveData::setArtist(const QString &str)
{
  data_artist=str;
}


QString RDWaveData::album() const
{
  return data_album;
}


void RDWaveData::setAlbum(const QString &str)
{
  data_album=str;
}


QString RDWaveData::conductor() const
{
  return data_conductor;
}


void RDWaveData::setConductor(const QString &str)
{
  data_conductor=str;
}


QString RDWaveData::label() const
{
  return data_label;
}


void RDWaveData::setLabel(const QString &str)
{
  data_label=str;
}


int RDWaveData::releaseYear() const
{
  return data_release_year;
}


void RDWaveData::setReleaseYear(int year)
{
  data_release_year=year;
}


QString RDWaveData::client() const
{
  return data_client;
}


void RDWaveData::setClient(const QString &str)
{
  data_client=str;
}


QString RDWaveData::agency() const
{
  return data_agency;
}


void RDWaveData::setAgency(const QString &str)
{
  data_agency=str;
}


QString RDWaveData::composer() const
{
  return data_composer;
}


void RDWaveData::setComposer(const QString &str)
{
  data_composer=str;
}


QString RDWaveData::publisher() const
{
  return data_publisher;
}


void RDWaveData::setPublisher(const QString &str)
{
  data_publisher=str;
}


int RDWaveData::usageCode() const
{
  return data_usage_code;
}


void RDWaveData::setUsageCode(int code)
{
  data_usage_code=code;
}


QStringList RDWaveData::schedCodes() const
{
  return data_sched_codes;
}


void RDWaveData::setSchedCodes(const QStringList &codes)
{
  data_sched_codes=codes;
}


QString RDWaveData::licensingOrganization() const
{
  return data_licensing_organization;
}


void RDWaveData::setLicensingOrganization(const QString &str)
{
  data_licensing_organization=str;
}


QString RDWaveData::copyrightNotice() const
{
  return data_copyright_notice;
}


void RDWaveData::setCopyrightNotice(const QString &str)
{
  data_copyright_notice=str;
}


QString RDWaveData::cutId() const
{
  return data_cut_id;
}


void RDWaveData::setCutId(const QString &str)
{
  data_cut_id=str;
}


QString RDWaveData::classification() const
{
  return data_classification;
}


void RDWaveData::setClassification(const QString &str)
{
  data_classification=str;
}


QString RDWaveData::category() const
{
  return data_category;
}


void RDWaveData::setCategory(const QString &str)
{
  data_category=str;
}


QString RDWaveData::url() const
{
  return data_url;
}


void RDWaveData::setUrl(const QString &str)
{
  data_url=str;
}


QString RDWaveData::tagText() const
{
  return data_tag_text;
}


void RDWaveData::setTagText(const QString &str)
{
  data_tag_text=str;
}


QString RDWaveData::description() const
{
  return data_description;
}


void RDWaveData::setDescription(const QString &str)
{
  data_description=str;
}


QString RDWaveData::originator() const
{
  return data_originator;
}


void RDWaveData::setOriginator(const QString &str)
{
  data_originator=str;
}


QString RDWaveData::originatorReference() const
{
  return data_originator_reference;
}


void RDWaveData::setOriginatorReference(const QString &str)
{
  data_originator_reference=str;
}


QString RDWaveData::codingHistory() const
{
  return data_coding_history;
}


void RDWaveData::setCodingHistory(const QString &str)
{
  data_coding_history=str;
}


QString RDWaveData::userDefined() const
{
  return data_user_defined;
}


void RDWaveData::setUserDefined(const QString &str)
{
  data_user_defined=str;
}


QString RDWaveData::isrc() const
{
  return data_isrc;
}


void RDWaveData::setIsrc(const QString &str)
{
  data_isrc=str;
}


QString RDWaveData::isci() const
{
  return data_isci;
}


void RDWaveData::setIsci(const QString &str)
{
  data_isci=str;
}


QString RDWaveData::mcn() const
{
  return data_mcn;
}


void RDWaveData::setMcn(const QString &str)
{
  data_mcn=str;
}


QString RDWaveData::outCue() const
{
  return data_out_cue;
}


void RDWaveData::setOutCue(const QString &str)
{
  data_out_cue=str;
}


RDWaveData::EndType RDWaveData::endType() const
{
  return data_end_type;
}


void RDWaveData::setEndType(RDWaveData::EndType type)
{
  data_end_type=type;
}


int RDWaveData::introStartPos() const
{
  return data_intro_start_pos;
}


void RDWaveData::setIntroStartPos(int msec)
{
  data_intro_start_pos=msec;
}


int RDWaveData::introEndPos() const
{
  return data_intro_end_pos;
}


void RDWaveData::setIntroEndPos(int msec)
{
  data_intro_end_pos=msec;
}


int RDWaveData::segueStartPos() const
{
  return data_segue_start_pos;
}


void RDWaveData::setSegueStartPos(int msec)
{
  data_segue_start_pos=msec;
}


int RDWaveData::segueEndPos() const
{
  return data_segue_end_pos;
}


void RDWaveData::setSegueEndPos(int msec)
{
  data_segue_end_pos=msec;
}


int RDWaveData::startPos() const
{
  return data_start_pos;
}


void RDWaveData::setStartPos(int msec)
{
  data_start_pos=msec;
}


int RDWaveData::endPos() const
{
  return data_end_pos;
}


void RDWaveData::setEndPos(int msec)
{
  data_end_pos=msec;
}


int RDWaveData::hookStartPos() const
{
  return data_hook_start_pos;
}


void RDWaveData::setHookStartPos(int msec)
{
  data_hook_start_pos=msec;
}


int RDWaveData::hookEndPos() const
{
  return data_hook_end_pos;
}


void RDWaveData::setHookEndPos(int msec)
{
  data_hook_end_pos=msec;
}


int RDWaveData::fadeUpPos() const
{
  return data_fade_up_pos;
}


void RDWaveData::setFadeUpPos(int msec)
{
  data_fade_up_pos=msec;
}


int RDWaveData::fadeDownPos() const
{
  return data_fade_down_pos;
}


void RDWaveData::setFadeDownPos(int msec)
{
  data_fade_down_pos=msec;
}


int RDWaveData::beatsPerMinute() const
{
  return data_beats_per_minute;
}


void RDWaveData::setBeatsPerMinute(int bpm)
{
  data_beats_per_minute=bpm;
}


QString RDWaveData::tmciSongId() const
{
  return data_tmci_song_id;
}


void RDWaveData::setTmciSongId(const QString &str)
{
  data_tmci_song_id=str;
}


QDate RDWaveData::startDate() const
{
  return data_start_date;
}


void RDWaveData::setStartDate(const QDate &date)
{
  data_start_date=date;
}


QTime RDWaveData::startTime() const
{
  return data_start_time;
}


void RDWaveData::setStartTime(const QTime &time)
{
  data_start_time=time;
}


QDate RDWaveData::originationDate() const
{
  return data_origination_date;
}


void RDWaveData::setOriginationDate(const QDate &date)
{
  data_origination_date=date;
}


QTime RDWaveData::originationTime() const
{
  return data_origination_time;
}


void RDWaveData::setOriginationTime(const QTime &time)
{
  data_origination_time=time;
}


QDate RDWaveData::endDate() const
{
  return data_end_date;
}


void RDWaveData::setEndDate(const QDate &date)
{
  data_end_date=date;
}


QTime RDWaveData::endTime() const
{
  return data_end_time;
}


void RDWaveData::setEndTime(const QTime &time)
{
  data_end_time=time;
}


QTime RDWaveData::daypartStartTime() const
{
  return data_daypart_start_time;
}


void RDWaveData::setDaypartStartTime(const QTime &time)
{
  data_daypart_start_time=time;
}


QTime RDWaveData::daypartEndTime() const
{
  return data_daypart_end_time;
}


void RDWaveData::setDaypartEndTime(const QTime &time)
{
  data_daypart_end_time=time;
}


void RDWaveData::clear()
{
  data_metadata_found=false;
  data_title="";
  data_artist="";
  data_album="";
  data_conductor="";
  data_label="";
  data_release_year=0;
  data_client="";
  data_agency="";
  data_composer="";
  data_publisher="";
  data_usage_code=0;
  data_sched_codes.clear();
  data_licensing_organization="";
  data_copyright_notice="";
  data_user_defined="";
  data_cut_id="";
  data_classification="";
  data_category="";
  data_url="";
  data_tag_text="";
  data_description="";
  data_originator="";
  data_originator_reference="";
  data_isrc="";
  data_isci="";
  data_mcn="";
  data_out_cue="";
  data_end_type=RDWaveData::UnknownEnd;
  data_intro_start_pos=-1;
  data_intro_end_pos=-1;
  data_segue_start_pos=-1;
  data_segue_end_pos=-1;
  data_start_pos=-1;
  data_end_pos=-1;
  data_hook_start_pos=-1;
  data_hook_end_pos=-1;
  data_fade_up_pos=-1;
  data_fade_down_pos=-1;
  data_beats_per_minute=0;
  data_tmci_song_id="";
  data_origination_date=QDate();
  data_origination_time=QTime();
  data_start_date=QDate();
  data_start_time=QTime();
  data_end_date=QDate();
  data_end_time=QTime();
  data_daypart_start_time=QTime();
  data_daypart_end_time=QTime();
}
