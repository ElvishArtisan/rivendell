// help_audios.h
//
// Display help for audio ports (edit_audios.*)
//
// (C) Copyright 2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: help_audios.h,v 1.5 2010/07/29 19:32:34 cvs Exp $
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
//

#ifndef HELP_AUDIOS_H
#define HELP_AUDIOS_H

#include <qdialog.h>
#include <qtextedit.h>


class HelpAudioPorts : public QDialog
{
 Q_OBJECT
 public:
  HelpAudioPorts(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void closeData();

 private:
  QTextEdit *help_edit;
};


#endif
