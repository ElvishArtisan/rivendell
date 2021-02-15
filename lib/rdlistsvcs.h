// rdlistsvcs.h
//
// Service Picker dialog
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QPushButton>

#include <rddialog.h>
#include <rdservicelistmodel.h>
#include <rdtableview.h>

class RDListSvcs : public RDDialog
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
  void doubleClickedData(const QModelIndex &index);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDTableView *edit_svc_view;
  RDServiceListModel *edit_svc_model;
  QString *edit_svcname;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  QString edit_caption;
};


#endif  // RDLISTSVCS_H
