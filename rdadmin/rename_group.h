// rename_group.h
//
// Rename a Rivendell Group
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

#ifndef RENAME_GROUP_H
#define RENAME_GROUP_H

#include <qlineedit.h>

#include <rddialog.h>
#include <rdgroup.h>

class RenameGroup : public RDDialog
{
  Q_OBJECT
  public:
   RenameGroup(QString group,QWidget *parent=0);
   ~RenameGroup();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  public slots:
   int exec(QString *newname);

  private slots:
   void okData();
   void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  QLineEdit *group_name_edit;
  QLineEdit *group_newname_edit;
  QString group_name;
  QString *group_new_name;
};


#endif  // RENAME_GROUP_H

