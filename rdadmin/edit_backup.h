// edit_backup.h
//
// Edit an automatic backup configuration
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_BACKUP_H
#define EDIT_BACKUP_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>

#include <rdstation.h>

class EditBackup : public QDialog
{
  Q_OBJECT
  public:
   EditBackup(RDStation *station,QWidget *parent=0);
   ~EditBackup();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void lifeChangedData(int days);
   void okData();
   void cancelData();

  private:
   RDStation *edit_station;
   QSpinBox *edit_life_box;
   QLineEdit *edit_path_edit;
   QLabel *edit_path_label;
};


#endif  // EDIT_BACKUP_H
