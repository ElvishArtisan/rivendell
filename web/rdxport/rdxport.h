// rdxport.h
//
// Rivendell web service portal
//
//   (C) Copyright 2010,2014 Fred Gleason <fredg@paravelsystems.com>
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


#ifndef RDXPORT_H
#define RDXPORT_H

#include <qobject.h>

#include <rdconfig.h>
#include <rduser.h>
#include <rdsystem.h>
#include <rdcart.h>
#include <rdcut.h>

class Xport : public QObject
{
 public:
  Xport(QObject *parent=0,const char *name=0);

 private:
  bool Authenticate();
  void Export();
  void Import();
  void DeleteAudio();
  void SetCartInfo(RDCart& cart, const QString& group_name);
  void AddCart();
  void ListCarts();
  void ListCart();
  void EditCart();
  void RemoveCart();
  void SetCutInfo(RDCut& cut);
  void AddCut();
  void ListCuts();
  void ListCut();
  void EditCut();
  void RemoveCut();
  void CopyCut();
  void ListGroups();
  void ListGroup();
  void ExportPeaks();
  void TrimAudio();
  void CopyAudio();
  void AudioInfo();
  void AudioStore();
  void ListLogs();
  void ListLog();
  void ListServices();
  void Exit(int code);
  void XmlExit(const QString &str,int code,
	       RDAudioConvert::ErrorCode err=RDAudioConvert::ErrorOk);
  RDFormPost *xport_post;
  RDUser *xport_user;
  RDConfig xport_config;
  RDSystem xport_system;
};


#endif  // RDXPORT_H
