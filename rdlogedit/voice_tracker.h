// voice_tracker.h
//
// Rivendell Voice Tracker Dialog
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

#ifndef VOICE_TRACKER_H
#define VOICE_TRACKER_H

#include <QPushButton>

#include <rddialog.h>
#include <rdtrackerwidget.h>

class VoiceTracker : public RDDialog
{
  Q_OBJECT
 public:
  VoiceTracker(QString *import_path,QWidget *parent=0);
  ~VoiceTracker();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(const QString &logname);

 private slots:
  void closeData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);
  
 private:
  RDTrackerWidget *d_tracker_widget;
  QPushButton *d_close_button;
  QString *d_import_path;
};


#endif  // VOICE_TRACKER_H
