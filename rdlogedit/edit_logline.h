// edit_logline.h
//
// Edit a Rivendell cart event
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_LOGLINE_H
#define EDIT_LOGLINE_H

#include <rdcart_dialog.h>
#include <rdlog_event.h>
#include <rdgroup_list.h>

#include "edit_event.h"

class EditLogLine : public EditEvent
{
  Q_OBJECT
 public:
  EditLogLine(RDLogLine *,QString *filter,QString *group,QString *schedcode,
	      QString svcname,RDGroupList *grplist,RDLogEvent *log,int lineno,
	      QWidget *parent=0);
  ~EditLogLine();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void selectCartData();

 protected:
  bool saveData();
  
 private:
  void FillCart(int cartnum);
  QCheckBox *edit_overlap_box;
  QLabel *edit_overlap_label;
  QLineEdit *edit_cart_edit;
  QLineEdit *edit_title_edit;
  QLineEdit *edit_artist_edit;
  RDCartDialog *edit_cart_dialog;
  QString *edit_filter;
  QString *edit_group;
  QString *edit_schedcode;
  QString edit_service;
  RDGroupList *edit_group_list;
  RDLogEvent *edit_log_event;
  int edit_line;
};


#endif  // EDIT_LOGLINE_H
