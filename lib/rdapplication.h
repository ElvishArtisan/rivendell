// rdapplication.h
//
// Base GUI Application Class
//
//   (C) Copyright 2021-2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDAPPLICATION_H
#define RDAPPLICATION_H

#include <rdcoreapplication.h>
#include <rdiconengine.h>

class RDApplication : public RDCoreApplication
{
  Q_OBJECT;
 public:
  RDApplication(const QString &module_name,const QString &cmdname,
		const QString &usage,bool use_translations,QObject *parent);
  ~RDApplication();
  RDIconEngine *iconEngine() const;
  static QString locale();

 private:
  void Raise(const QString win_id);
  bool app_singleton;
  RDIconEngine *app_icon_engine;
};

extern RDApplication *rda;

#endif  // RDAPPLICATION_H
