// rdcastmanager.h
//
// Web-Based RSS Podcast Manager for Rivendell.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcastmanager.h,v 1.5 2010/07/29 19:32:40 cvs Exp $
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


#ifndef RDCASTMANAGER_H
#define RDCASTMANAGER_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qhostaddress.h>

#include <rdformpost.h>

//
// CGI Values
//
#define RDCASTMANAGER_POST_SIZE 4096
#define RDCASTMANAGER_MAX_FILTER_LENGTH 256
#define RDCASTMANAGER_COMMAND_LOGIN 0
#define RDCASTMANAGER_COMMAND_LOGOUT 1
#define RDCASTMANAGER_COMMAND_LIST_FEEDS 2
#define RDCASTMANAGER_COMMAND_LIST_CASTS 3
#define RDCASTMANAGER_COMMAND_EDIT_CAST 4
#define RDCASTMANAGER_COMMAND_COMMIT_CAST 5
#define RDCASTMANAGER_COMMAND_CONFIRM_DELETE_CAST 6
#define RDCASTMANAGER_COMMAND_DELETE_CAST 7
#define RDCASTMANAGER_COMMAND_SUBSCRIPTION_PICK_DATES 8
#define RDCASTMANAGER_COMMAND_SUBSCRIPTION_REPORT 9
#define RDCASTMANAGER_COMMAND_EPISODE_PICK_DATES 10
#define RDCASTMANAGER_COMMAND_EPISODE_REPORT 11
#define RDCASTMANAGER_COMMAND_PLAY_CAST 12
#define RDCASTMANAGER_COMMAND_POST_EPISODE 13

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  int AuthenticatePost();
  void ServeLogin();
  void ServeLogout();
  void ServeListFeeds();
  void ServeListCasts();
  void ServeEditCast(int cast_id=-1);
  void ServePlay();
  void CommitCast();
  void ConfirmDeleteCast();
  void DeleteCast();
  void ServeSubscriptionReport();
  void ServeEpisodeReport();
  void PostEpisode();
  void ServeDatePicker(int cmd);
  void TitleSection(const QString &title,int cmd,int colspan) const;
  void GetUserPerms();
  void GetContext();
  void SetContext(int cmd) const;
  void Exit(int code);
  RDFormPost *cast_post;
  QString cast_login_name;
  long int cast_session_id;
  QHostAddress cast_client_addr;
  QString cast_key_name;
  int cast_feed_id;
  int cast_cast_id;
  bool cast_add_priv;
  bool cast_edit_priv;
  bool cast_delete_priv;
  QDate cast_start_date;
  QDate cast_end_date;
  RDConfig *cast_config;
};


#endif  // RDCASTMANAGER_H
