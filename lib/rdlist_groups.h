// rdlist_groups.h
//
// A widget to select a Rivendell Group.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QPushButton>

#include <rddialog.h>
#include <rdgrouplistmodel.h>
#include <rdtableview.h>

class RDListGroups : public RDDialog
{
 Q_OBJECT
 public:
 RDListGroups(QString *groupname,const QString &caption,QWidget *parent=0);
 QSize sizeHint() const;
 QSizePolicy sizePolicy() const;

 private slots:
  void doubleClickedData(const QModelIndex &index);
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  void BuildGroupList(const QString &username);
  QString *group_name;
  RDTableView *group_group_view;
  RDGroupListModel *group_group_model;
};


#endif  //  RDLIST_GROUPS_H

