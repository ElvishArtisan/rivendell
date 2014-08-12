// rdlist_groups.h
//
// A widget to select a Rivendell Group.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlist_groups.h,v 1.5 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDLIST_GROUPS_H
#define RDLIST_GROUPS_H

#include <qdialog.h>
#include <qlistview.h>
#include <qpushbutton.h>


class RDListGroups : public QDialog
{
 Q_OBJECT
 public:
 RDListGroups(QString *groupname,const QString &username,
	      QWidget *parent=0,const char *name=0);
 ~RDListGroups();
 QSize sizeHint() const;
 QSizePolicy sizePolicy() const;

 private slots:
  void doubleClickedData(QListViewItem *item,const QPoint &pt,int col);
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  void BuildGroupList(const QString &username);
  QString *group_name;
  QListView *group_group_list;
};


#endif  //  RDLIST_GROUPS_H

