// add_recording.h
//
// Add a Rivendell RDCatch Event
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

#ifndef ADD_RECORDING_H
#define ADD_RECORDING_H

#include <rddialog.h>

class AddRecording : public RDDialog
{
  Q_OBJECT
 public:
  AddRecording(QString *filter,QWidget *parent=0);
  ~AddRecording();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(RDRecording::Type *type,int rec_id);

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
  RDRecording::Type *add_type;
  QString *add_filter;
};


#endif

