// rdmarkerreadout.h
//
// Read-only display for cut marker digital information
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

#ifndef RDMARKERREADOUT_H
#define RDMARKERREADOUT_H

#include <QLabel>
#include <QList>
#include <QPalette>

#include <rdmarkerview.h>
#include <rdpushbutton.h>

class RDMarkerReadout : public RDPushButton
{
  Q_OBJECT;
 public:
  RDMarkerReadout(RDMarkerHandle::PointerRole role,QWidget *parent=0);
  ~RDMarkerReadout();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  void setValue(RDMarkerHandle::PointerRole role,int value);
  void setSelectedMarkers(RDMarkerHandle::PointerRole start_role,
			  RDMarkerHandle::PointerRole end_role);
  void setEnabled(bool state);
  void setDisabled(bool state);

 protected:
  void resizeEvent(QResizeEvent *);

 private:
  QLabel *d_label;
  QList <QLabel *> d_edits;
  QList<RDMarkerHandle::PointerRole> d_roles;
  RDMarkerHandle::PointerRole d_selected_marker;
  int d_pointers[RDMarkerHandle::LastRole];
  QPalette d_default_palette;
  QPalette d_selected_palette;
};


#endif  // RDMARKERREADOUT_H
