// rdframe.h
//
// Base class for Rivendell QFrame-based widgets.
//
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDFRAME_H
#define RDFRAME_H

#include <qwidget.h>

#include <rdfontset.h>

class RDFrame : public QFrame, public RDFontSet
{
  Q_OBJECT;
 public:
  RDFrame(QWidget *parent=0,Qt::WindowFlags f=0);
  RDFrame(RDConfig *config,QWidget *parent=0,Qt::WindowFlags f=0);
};


#endif  // RDFRAME_H
