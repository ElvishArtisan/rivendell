// import_track.h
//
// Import Audio for a Voice Track
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: import_track.h,v 1.5 2010/07/29 19:32:37 cvs Exp $
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

#ifndef IMPORT_TRACK_H
#define IMPORT_TRACK_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>

#include <rdrecording.h>
#include <rddeck.h>

class ImportTrack : public QDialog
{
  Q_OBJECT
 public:
  ImportTrack(QString *filter,QString *group,
	      QWidget *parent=0,const char *name=0);
  ~ImportTrack();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 protected:
  void closeEvent(QCloseEvent *e);

 private slots:
  void cartData();
  void importData();
  void cancelData();

 protected:
  void keyPressEvent(QKeyEvent *e);

 private: 
  int add_id;
  QString *add_filter;
  QString *add_group;
};


#endif  // IMPORT_TRACK_H
