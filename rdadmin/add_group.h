// add_group.h
//
// Add a Rivendell Group
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

#ifndef ADD_GROUP_H
#define ADD_GROUP_H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rddialog.h>

class AddGroup : public RDDialog
{
  Q_OBJECT
  public:
   AddGroup(QString *group,QWidget *parent=0);
   ~AddGroup();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void groupNameChangedData(const QString &str);
   void okData();
   void cancelData();

  protected:
   void resizeEvent(QResizeEvent *e);

  private:
   QLabel *group_name_label;
   QLineEdit *group_name_edit;
   QCheckBox *group_users_box;
   QLabel *group_users_label;
   QCheckBox *group_svcs_box;
   QLabel *group_svcs_label;
   QString *group_group;
   QPushButton *group_ok_button;
   QPushButton *group_cancel_button;
};


#endif  // ADD_GROUP_H

