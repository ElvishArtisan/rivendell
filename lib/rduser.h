// rduser.h
//
// Abstract a Rivendell User
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDUSER_H
#define RDUSER_H

#include <QDateTime>
#include <QHostAddress>

class RDUser
{
 public:
  enum Type {TypeAll=0,TypeAdminConfig=1,TypeAdminRss=2,TypeLocalUser=3,
	     TypeExternalUser=4,TypeAdmin=5,TypeUser=6,TypeLast=7};
  RDUser(const QString &name);
  RDUser();
  QString name() const;
  void setName(const QString &name);
  bool exists() const;
  Type type() const;
  bool authenticated(bool webuser) const;
  bool checkPassword(const QString &password,bool webuser);
  QString password() const;
  void setPassword(const QString &password);
  bool enableWeb() const;
  void setEnableWeb(bool state) const;
  bool localAuthentication() const;
  void setLocalAuthentication(bool state) const;
  QString pamService() const;
  void setPamService(const QString &str) const;
  QString fullName() const;
  void setFullName(const QString &name) const;
  QString emailAddress() const;
  void setEmailAddress(const QString &str) const;
  QString emailContact() const;
  QString description() const;
  void setDescription(const QString &desc) const;
  QString phone() const;
  void setPhone(const QString &phone) const;
  int webapiAuthTimeout() const;
  void setWebapiAuthTimeout(int sec) const;
  bool adminConfig() const;
  void setAdminConfig(bool priv) const;
  bool adminRss() const;
  void setAdminRss(bool priv) const;
  bool createCarts() const;
  void setCreateCarts(bool priv) const;
  bool deleteCarts() const;
  void setDeleteCarts(bool priv) const;
  bool modifyCarts() const;
  void setModifyCarts(bool priv) const;
  bool editAudio() const;
  void setEditAudio(bool priv) const;
  bool webgetLogin() const;
  void setWebgetLogin(bool priv) const;
  bool createLog() const;
  void setCreateLog(bool priv) const;
  bool deleteLog() const;
  void setDeleteLog(bool priv) const;
  bool deleteRec() const;
  void setDeleteRec(bool priv) const;
  bool playoutLog() const;
  void setPlayoutLog(bool priv) const;
  bool arrangeLog() const;
  void setArrangeLog(bool priv) const;
  bool addtoLog() const;
  void setAddtoLog(bool priv) const;
  bool removefromLog() const;
  void setRemovefromLog(bool priv) const;
  bool configPanels() const;
  void setConfigPanels(bool priv) const;
  bool voicetrackLog() const;
  void setVoicetrackLog(bool priv) const;
  bool modifyTemplate() const;
  void setModifyTemplate(bool priv) const;
  bool editCatches() const;
  void setEditCatches(bool priv) const;
  bool addPodcast() const;
  void setAddPodcast(bool priv) const;
  bool editPodcast() const;
  void setEditPodcast(bool priv) const;
  bool deletePodcast() const;
  void setDeletePodcast(bool priv) const;
  bool groupAuthorized(const QString &group_name);
  QStringList groups() const;
  bool cartAuthorized(unsigned cartnum) const;
  bool feedAuthorized(const QString &keyname);
  QString serviceCheckDefault(QString serv) const;
  QStringList services() const;
  bool createTicket(QString *ticket,QDateTime *expire_dt,
		    const QHostAddress &client_addr,
		    QDateTime start_dt=QDateTime()) const;
  static bool ticketIsValid(const QString &ticket,
			    const QHostAddress &client_addr,
			    QString *username=NULL,QDateTime *expire_dt=NULL);
  static bool emailIsValid(const QString &addr);
  static QString emailContact(const QString &addr,const QString &fullname);
  static QString typeText(Type type);

 private:
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,bool value) const;
  void SetRowNull(const QString &param) const;
  QString user_name;
  QString user_password;
};


#endif  // RDUSER_H
