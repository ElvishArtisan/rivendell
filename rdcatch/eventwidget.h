// eventwidget.h
//
// Widget for setting basic event parameters in rdcatch(1)
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

#ifndef EVENTWIDGET_H
#define EVENTWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

#include <rdtimeedit.h>
#include <rdwidget.h>

class EventWidget : public RDWidget
{
  Q_OBJECT
 public:
  enum EventType {RecordEvent=0,PlayEvent=1,OtherEvent=2};
  EventWidget(EventType type,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  bool isActive() const;
  QString stationName() const;
  int deckNumber() const;
  QTime startTime();
  void toRecording(unsigned record_id) const;
  void fromRecording(unsigned record_id);

 signals:
  void stateChanged(bool state);
  void locationChanged(const QString &hostname);
  void locationChanged(const QString &hostname,int decknum);
  void startTimeChanged(const QTime &time);

 private slots:
  void locationActivatedData(const QString &str);
    
 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QCheckBox *d_state_check;
  QLabel *d_state_label;
  QLabel *d_location_label;
  QComboBox *d_location_box;
  QLabel *d_time_label;
  RDTimeEdit *d_time_edit;
  EventType d_event_type;
  QString d_current_station_name;
  int d_current_deck_number;
};


#endif  // DOWSELECTOR_H

