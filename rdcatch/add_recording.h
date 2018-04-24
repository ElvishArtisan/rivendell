// add_recording.h
//
// Add a Rivendell RDCatch Event
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

#ifndef ADD_RECORDING_H
#define ADD_RECORDING_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <q3datetimeedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <QKeyEvent>

#include <rdrecording.h>
#include <rddeck.h>

class AddRecording : public QDialog
{
  Q_OBJECT
 public:
  AddRecording(int id,QString *filter,QWidget *parent=0);
  ~AddRecording();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 protected:
  void closeEvent(QCloseEvent *e);

 private slots:
  void recordingData();
  void playoutData();
  void downloadData();
  void uploadData();
  void macroData();
  void switchData();
  void cancelData();

 protected:
  void keyPressEvent(QKeyEvent *e);

 private: 
  int add_id;
  QString *add_filter;
};


#endif

