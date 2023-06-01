//   rdtranslate.h
//
//   Load translation objects for Rivendell 
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDTRANSLATE_H
#define RDTRANSLATE_H

#include <QTranslator>

class RDTranslator : public QObject
{
  Q_OBJECT;
 public:
  RDTranslator(const QString &cmdname,bool use_translations,QObject *parent=0);

 private:
  bool LoadTranslation(const QString &filename,const QString &dirname);
  QString d_command_name;
};

extern RDTranslator *rdt;

#endif  // RDTRANSLATE_H
