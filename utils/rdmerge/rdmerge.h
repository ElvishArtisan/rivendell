// rddbmgr.h
//
// Rivendell database merge utility
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDMERGE_H
#define RDMERGE_H

#include <qobject.h>

#include <rdconfig.h>
#include <rdfeed.h>
#include <rdstation.h>

#define RDMERGE_USAGE "[options]\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

 private:
  void DumpEvents() const;
  void DumpEventLines() const;
  void DumpClocks() const;
  void DumpClockLines() const;
  void DumpServices() const;
  void DumpServiceClocks() const;
};


#endif  // RDMERGE_H
