// rduser.h
//
// Abstract a Rivendell User
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rduser.h,v 1.22.8.1 2013/12/03 23:34:34 cvs Exp $
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

#include <qsqldatabase.h>

#ifndef RDUSER_H
#define RDUSER_H

class RDUser
{
 public:
  RDUser(const QString &name);
  QString name() const;
  bool exists() const;
  bool authenticated(bool webuser) const;
  bool checkPassword(const QString &password,bool webuser);
  QString password() const;
  void setPassword(const QString &password);
  bool enableWeb() const;
  void setEnableWeb(bool state) const;
  QString fullName() const;
  void setFullName(const QString &name) const;
  QString description() const;
  void setDescription(const QString &desc) const;
  QString phone() const;
  void setPhone(const QString &phone) const;
  bool adminConfig() const;
  void setAdminConfig(bool priv) const;
  bool createCarts() const;
  void setCreateCarts(bool priv) const;
  bool deleteCarts() const;
  void setDeleteCarts(bool priv) const;
  bool modifyCarts() const;
  void setModifyCarts(bool priv) const;
  bool editAudio() const;
  void setEditAudio(bool priv) const;
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

  /** Check a default service to ensure it is valid for the current user.
   *
   * @param serv QString with the proposed default service, presumably gotten
   *             from RDAirPlayConf::defaultSvc()
   * @return QString with serv if it was valid, otherwise an empty QString.
   */
  QString serviceCheckDefault(QString serv) const;

  /** Calculate the services associated with a user, based on the user's group
   * membership and the relationship of groups to services.
   *
   * Note: admin users, those who pass adminConfig(), can see all services.
   *
   * @return QStringList with all the services associated with the user.
   */
  QStringList services() const;

 private:
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,bool value) const;
  QString user_name;
  QString user_password;
};


#endif  // RDUSER_H
