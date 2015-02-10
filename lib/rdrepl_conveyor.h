// rdrepl_conveyor.h
//
// Manage replicator conveyor queues.
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDREPL_CONVEYOR_H
#define RDREPL_CONVEYOR_H

#define RD_REPL_DIR "/var/cache/rivendell"

class RDReplConveyor
{
 public:
  enum Direction {Inbound=0,Outbound=1};
  RDReplConveyor(const QString &repl_name);
  bool push(Direction dir,const QString &filename) const;
  bool nextPackage(int *id,Direction dir,const QString &outfile) const;
  void pop(int id) const;
  static QString fileName(int id);

 private:
  QString conv_repl_name;
};


#endif  // RDREPL_CONVEYOR_H
