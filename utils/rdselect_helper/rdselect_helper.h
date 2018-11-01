// rdselect_help.h
//
// SETUID helper script for rdselect(1)
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSELECT_HELPER_H
#define RDSELECT_HELPER_H

#include <stdio.h>
#include <stdint.h>

#include <qobject.h>
#include <qstringlist.h>

#include "rdconfig.h"

#define RDSELECT_AUTOMOUNT_CONFIG "/etc/auto.rd.audiostore"
#define RDSELECT_AUTOMOUNT_WARNING "# THE CONTENTS OF THIS FILE ARE MANAGED BY RIVENDELL'S RDSELECT SYSTEM!\n# DO NOT HAND EDIT!\n\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private:
  void Startup();
  void Shutdown();
  void ControlAutomounter(const QString &cmd);
  bool ProcessActive(const QStringList &cmds) const;
  bool ModulesActive() const;
  QString helper_config_filename;
  RDConfig *helper_config;
  RDConfig *helper_prev_config;
};


#endif  // RDSELECT_HELPER_H
