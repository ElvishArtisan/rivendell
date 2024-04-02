// rdeventfilter.h
//
// Filter one or more window system events
//
//   (C) Copyright 2024 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDEVENTFILTER_H
#define RDEVENTFILTER_H

#include <QList>
#include <QObject>

class RDEventFilter : public QObject
{
 public:
  RDEventFilter(QObject *parent);
  QList<QEvent::Type> filterList() const;
  void addFilter(QEvent::Type type);
  void removeFilter(QEvent::Type type);

 protected:
  bool eventFilter(QObject *obj,QEvent *e) override;
  QList<QEvent::Type> d_filter_types;
};

#endif  // RDEVENTFILTER_H
