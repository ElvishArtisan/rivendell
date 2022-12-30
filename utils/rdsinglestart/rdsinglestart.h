// rdsinglestart.h
//
//  Start a program so as to allow only a single instance.
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSINGLESTART_H
#define RDSINGLESTART_H

#include <QObject>

#include <rdcmd_switch.h>

#define RDSINGLESTART_USAGE "<program>\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

 private:
  void Raise(const QString win_id);
  void Start(RDCmdSwitch *cmd);
};


#endif  // RDSINGLESTART_H
