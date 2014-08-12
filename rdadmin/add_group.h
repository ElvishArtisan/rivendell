// add_group.h
//
// Add a Rivendell Group
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: add_group.h,v 1.8 2010/07/29 19:32:34 cvs Exp $
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

#ifndef ADD_GROUP_H
#define ADD_GROUP_H

#include <qdialog.h>
#include <qcheckbox.h>
#include <qlineedit.h>


class AddGroup : public QDialog
{
  Q_OBJECT
  public:
   AddGroup(QString *group,QWidget *parent=0,const char *name=0);
   ~AddGroup();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void okData();
   void cancelData();

  private:
   QLineEdit *group_name_edit;
   QCheckBox *group_users_box;
   QCheckBox *group_svcs_box;
   QString *group_group;
};


#endif

