// rdmainwindow.h
//
// Top-level window for Rivendell GUI modules.
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

#ifndef RDMAINWINDOW_H
#define RDMAINWINDOW_H

#include <stdio.h>

#include <QDir>
#include <QString>

#include <rdconfig.h>
#include <rdprofile.h>
#include <rdwidget.h>

class RDMainWindow : public RDWidget
{
  Q_OBJECT;
 public:
  RDMainWindow(const QString &cmdname,Qt::WindowFlags f=0);
  RDMainWindow(const QString &cmdname,RDConfig *c,Qt::WindowFlags f=0);
  ~RDMainWindow();
  bool loadSettings(bool apply_geometry);
  bool saveSettings() const;

 protected:
  QString commandName() const;
  virtual void loadLocalSettings(RDProfile *p);
  virtual void saveLocalSettings(FILE *f) const;
  
 private:
  QString d_command_name;
  QDir *d_conf_dir;
  QString d_conf_path;
};


#endif  // RDWIDGET_H
