// add_station.h
//
// Add a Rivendell Workstation
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: add_station.h,v 1.11 2010/07/29 19:32:34 cvs Exp $
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

#ifndef ADD_STATION_H
#define ADD_STATION_H

#include <qdialog.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qsqldatabase.h>
#include <qlineedit.h>

#include <rdstation.h>


class AddStation : public QDialog
{
  Q_OBJECT
  public:
   AddStation(QString *stationname,QWidget *parent=0,const char *name=0);
   ~AddStation();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void okData();
   void cancelData();

  private:
   void CloneEncoderValues(const QString &paramname,int src_id,int dest_id);
   QLineEdit *add_name_edit;
   QComboBox *add_exemplar_box;
   QString *add_name;
};


#endif

