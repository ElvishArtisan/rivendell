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

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rddialog.h>
#include <rdgroup.h>

class RenameGroup : public RDDialog
{
  Q_OBJECT
  public:
   enum Result {Renamed=0,Merged=1,Cancelled=2};
   RenameGroup(QString group,QWidget *parent=0);
   ~RenameGroup();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  public slots:
   int exec(QString *newname);

  private slots:
   void newNameChangedData(const QString &str);
   void okData();
   void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  void Rename(const QString &old_name,const QString &new_name,
	      bool merge) const;
  void RenameField(const QString &table,const QString &field,
		   const QString &old_name,const QString &new_name) const;
  QLabel *group_name_label;
  QLineEdit *group_name_edit;
  QLabel *group_newname_label;
  QLineEdit *group_newname_edit;
  QPushButton *group_ok_button;
  QPushButton *group_cancel_button;
  QString group_name;
  QString *group_new_name;
};


#endif  // RENAME_GROUP_H

