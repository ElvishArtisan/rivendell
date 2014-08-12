// rdlistsvcs.h
//
// Service Picker dialog
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlistsvcs.h,v 1.1.2.2 2012/11/16 18:10:40 cvs Exp $
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

#ifndef RDLISTSVCS_H
#define RDLISTSVCS_H

#include <qdialog.h>
#include <qlistbox.h>
#include <qpushbutton.h>

class RDListSvcs : public QDialog
{
  Q_OBJECT
 public:
  RDListSvcs(const QString &caption,QWidget *parent=0);
  ~RDListSvcs();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(QString *svcname);

 private slots:
  void doubleClickedData(QListBoxItem *item);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QListBox *edit_svc_list;
  QString *edit_svcname;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  QString edit_caption;
};


#endif  // RDLISTSVCS_H
