// edit_feed_perms.h
//
// Edit Rivendell RSS Feed Permissions
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_feed_perms.h,v 1.3 2010/07/29 19:32:34 cvs Exp $
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

#ifndef EDIT_FEED_PERMS_H
#define EDIT_FEED_PERMS_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <rdlistselector.h>

#include <rduser.h>


class EditFeedPerms : public QDialog
{
  Q_OBJECT
 public:
  EditFeedPerms(RDUser *user,QWidget *parent=0,const char *name=0);
  ~EditFeedPerms();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void okData();
  void cancelData();

 private:
  RDListSelector *feed_host_sel;
  RDUser *feed_user;
};


#endif

