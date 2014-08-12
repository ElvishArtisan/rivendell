// edit_perms.h
//
// Edit RDLogManager Service Associations
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_perms.h,v 1.6.8.1 2012/04/23 17:22:47 cvs Exp $
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

#ifndef EDIT_PERMS_H
#define EDIT_PERMS_H

#include <qdialog.h>
#include <qsqldatabase.h>

#include <rdlistselector.h>

#include <rdsvc.h>


class EditPerms : public QDialog
{
  Q_OBJECT
 public:
  enum ObjectType {ObjectEvent=1,ObjectClock=2};
  EditPerms(QString object_name,ObjectType type,
	    QWidget *parent=0,const char *name=0);
  ~EditPerms();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  RDListSelector *svc_object_sel;
  ObjectType sel_type;
  QString sel_name;
  QString object_type;
};


#endif

