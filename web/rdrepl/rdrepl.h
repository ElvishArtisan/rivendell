// rdrepl.h
//
// Rivendell replicator portal
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


#ifndef RDREPL_H
#define RDREPL_H

#include <qobject.h>

#include <rdconfig.h>
#include <rduser.h>
#include <rdsystem.h>

class Repl : public QObject
{
 public:
  Repl(QObject *parent=0,const char *name=0);

 private:
  bool Authenticate();
  void Export();
  void Import();
  void Exit(int code);
  void XmlExit(const QString &str,int code,
	       RDAudioConvert::ErrorCode err=RDAudioConvert::ErrorOk);
  RDFormPost *repl_post;
  RDConfig *repl_config;
  RDSystem *repl_system;
};


#endif  // RDREPL_H
