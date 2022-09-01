//   rddummylookup.h
//
//   RDDiscLookup instance class for dummy lookup
//
//   (C) Copyright 2003-2022 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#ifndef RDDUMMYLOOKUP_H
#define RDDUMMYLOOKUP_H

#include <rddisclookup.h>

class RDDummyLookup : public RDDiscLookup
{
  Q_OBJECT
 public:
  RDDummyLookup(const QString &caption,FILE *profile_msgs,QWidget *parent=0);

 protected:
  void lookupRecord();
};

#endif  // RDDUMMYLOOKUP_H
