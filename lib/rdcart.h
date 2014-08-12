// rdcart.h
//
// Abstract a Rivendell Cart
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcart.h,v 1.39.6.5.2.5 2014/05/30 00:26:28 cvs Exp $
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

#include <qdatetime.h>
#include <qstringlist.h>

#include <rdconfig.h>
#include <rdwavedata.h>

#include <rdcut.h>
#include <rddb.h>
#include <rduser.h>
#include <rdstation.h>

#ifndef RDCART_H
#define RDCART_H

#define MAX_SERVICES 16

class RDCart
{
 public:
  enum Type {All=0,Audio=1,Macro=2};
  enum PlayOrder {Sequence=0,Random=1};
  enum UsageCode {UsageFeature=0,UsageOpen=1,UsageClose=2,UsageTheme=3,
		  UsageBackground=4,UsagePromo=5,UsageLast=6};
  enum Validity {NeverValid=0,ConditionallyValid=1,AlwaysValid=2,
		 EvergreenValid=3,FutureValid=4};
  RDCart(unsigned number);
  ~RDCart();
  bool exists() const;
  bool selectCut(QString *cut) const;
  bool selectCut(QString *cut,const QTime &time) const;
  RDCart::Type type() const;
  void setType(RDCart::Type type);
  unsigned number() const;
  QString groupName() const;
  void setGroupName(const QString &name);
  QString title() const;
  void setTitle(const QString &title);
  QString artist() const;
  void setArtist(const QString &name);
  QString album() const;
  void setAlbum(const QString &album);
  int year() const;
  void setYear(int year=-1);
  QString schedCodes() const;
  void setSchedCodes(const QString &sched_codes) const;
  QStringList schedCodesList() const;
  void setSchedCodesList(const QStringList &codes);
  void addSchedCode(const QString &code);
  void updateSchedCodes(const QString &add_codes,
			const QString &remove_codes) const;
  QString conductor() const;
  void setConductor(const QString &cond);
  QString label() const;
  void setLabel(const QString &label);
  QString client() const;
  void setClient(const QString &client);
  QString agency() const;
  void setAgency(const QString &agency);
  QString publisher() const;
  void setPublisher(const QString &publisher);
  QString composer() const;
  void setComposer(const QString &composer);
  QString userDefined() const;
  void setUserDefined(const QString &string);
  QString songId() const;
  void setSongId(const QString &id);
  unsigned beatsPerMinute() const;
  void setBeatsPerMinute(unsigned bpm);
  RDCart::UsageCode usageCode() const;
  void setUsageCode(RDCart::UsageCode code);
  QString notes() const;
  void setNotes(const QString &notes);
  unsigned forcedLength() const;
  void setForcedLength(unsigned length);
  unsigned lengthDeviation() const;
  void setLengthDeviation(unsigned length) const;
  unsigned calculateAverageLength(unsigned *max_dev=0) const;
  unsigned averageLength() const;
  void setAverageLength(unsigned length) const;
  unsigned averageSegueLength() const;
  void setAverageSegueLength(unsigned length) const;
  unsigned averageHookLength() const;
  void setAverageHookLength(unsigned length) const;
  unsigned cutQuantity() const;
  void setCutQuantity(unsigned quan) const;
  unsigned lastCutPlayed() const;
  void setLastCutPlayed(unsigned cut) const;
  RDCart::PlayOrder playOrder() const;
  void setPlayOrder(RDCart::PlayOrder order) const;
  RDCart::Validity validity() const;
  void setValidity(RDCart::Validity state);
  QDateTime startDateTime() const;
  void setStartDateTime(const QDateTime &time);
  void setStartDateTime();
  QDateTime endDateTime() const;
  void setEndDateTime(const QDateTime &time);
  void setEndDateTime();
  bool enforceLength() const;
  void setEnforceLength(bool state);
  bool preservePitch() const;
  void setPreservePitch(bool state) const;
  bool asyncronous() const;
  void setAsyncronous(bool state) const;
  QString owner() const;
  void setOwner(const QString &owner) const;
  bool useEventLength() const;
  void setUseEventLength(bool state) const;
  void setPending(const QString &station_name);
  void clearPending() const;
  QString macros() const;
  void setMacros(const QString &cmds) const;
  bool validateLengths(int len) const;
  void getMetadata(RDWaveData *data) const;
  void setMetadata(const RDWaveData *data);
  QString xml(bool include_cuts) const;
  void updateLength();
  void updateLength(bool enforce_length,unsigned length);
  void resetRotation() const;
  void writeTimestamp();
  int addCut(unsigned format,unsigned bitrate,unsigned chans,
	     const QString &isci="",QString desc="");
  bool removeAllCuts(RDStation *station,RDUser *user,RDConfig *config);
  bool removeCut(RDStation *station,RDUser *user,const QString &cutname,
		 RDConfig *config);
  bool removeCutAudio(RDStation *station,RDUser *user,
		      const QString &cutname,RDConfig *config);
  bool create(const QString &groupname,RDCart::Type type);
  bool remove(RDStation *station,RDUser *user,RDConfig *config) const;
  static bool exists(unsigned cartnum);
  static QString playOrderText(RDCart::PlayOrder order);
  static QString usageText(RDCart::UsageCode usage);
  static QString typeText(RDCart::Type type);
  static bool removeCart(unsigned cart_num,RDStation *station,RDUser *user,
			 RDConfig *config);
  static bool removeCutAudio(RDStation *station,RDUser *user,unsigned cart_num,
			     const QString &cutname,RDConfig *config);
  static void removePending(RDStation *station,RDUser *user,RDConfig *config);
  
 private:
  QString GetNextCut(RDSqlQuery *q) const;
  int GetNextFreeCut() const;
  RDCut::Validity ValidateCut(RDSqlQuery *q,bool enforce_length,
			      unsigned length,bool *time_ok) const;
  QString VerifyTitle(const QString &title) const;

  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,unsigned value) const;
  void SetRow(const QString &param,const QDateTime &value) const;
  void SetRow(const QString &param,const QDate &value) const;
  void SetRow(const QString &param) const;
  unsigned cart_number;
  bool metadata_changed;
};


#endif  // RDCART_H
