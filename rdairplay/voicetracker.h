// voicetracker.h
//
// Voice tracker panel for RDAirPlay
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef VOICETRACKER_H
#define VOICETRACKER_H

#include <QLabel>
#include <QPushButton>

#include <rdlist_logs.h>
#include <rdtrackerwidget.h>
#include <rdwidget.h>

class VoiceTracker : public RDWidget
{
  Q_OBJECT
 public:
  VoiceTracker(QWidget *parent=0);
  QSize sizeHint() const;

 private slots:
  void loadData();
  void unloadData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);

 private:
  RDListLogs *d_listlogs_dialog;
  RDTrackerWidget *d_tracker_widget;
  QPushButton *d_load_button;
  QString d_import_path;
};

#endif  // VOICETRACKER_H
