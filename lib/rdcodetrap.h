//   rdcodetrap.h
//
//   A class for trapping arbitrary character sequences.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdcodetrap.h,v 1.4 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDCODETRAP_H
#define RDCODETRAP_H


#include <vector>

#include <qobject.h>

using namespace std;


struct RTrapEvent {
  int id;
  char *code;
  int length;
  int istate;
};


class RDCodeTrap : public QObject
{
 Q_OBJECT
 public:
  RDCodeTrap(QObject *parent=0,const char *name=0);
  ~RDCodeTrap();
  void addTrap(int id,const char *code,int length);
  void removeTrap(int id);
  void removeTrap(const char *code,int length);
  void removeTrap(int id,const char *code,int length);
  void scan(const char *buf,int length);
  void clear();

 signals:
  void trapped(int id);

 private:
  vector<struct RTrapEvent> trap_events;
};


#endif  // RDCODETRAP_H
