// rename_item.h
//
// Rename an RDLogManager Event or Clock
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RENAME_ITEM_H
#define RENAME_ITEM_H

#include <qlineedit.h>
#include <qpushbutton.h>

#include <rddialog.h>

class RenameItem : public RDDialog
{
 Q_OBJECT
 public:
  RenameItem(QString *text,QString table,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 private:
  QLineEdit *edit_name_edit;
  QString *edit_text;
  QString edit_tablename;
};


#endif  // EDIT_NOTE_H

