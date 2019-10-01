// rdcueeditdialog.h
//
// A Dialog Box for using an RDCueEdit widget.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCUEEDITDIALOG_H
#define RDCUEEDITDIALOG_H

#include <rdcueedit.h>
#include <rddialog.h>
#include <rdlog_line.h>

class RDCueEditDialog : public RDDialog
{
 Q_OBJECT
 public:
  RDCueEditDialog(RDCae *cae,int play_card,int play_port,const QString &caption,
		  QWidget *parent=0);
  ~RDCueEditDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(RDLogLine *logline);

 private slots:
  void okData();
  void cancelData();

 private:
  RDLogLine *edit_logline;
  RDCueEdit *cue_edit;
};


#endif  // RDCUEEDITDIALOG_H
