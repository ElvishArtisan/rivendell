// add_svc.h
//
// Add a Rivendell Service
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef ADD_SVC_H
#define ADD_SVC_H

#include <QComboBox>

#include <rddialog.h>
#include <rdservicelistmodel.h>
#include <rdsvc.h>

class AddSvc : public RDDialog
{
 Q_OBJECT
 public:
 AddSvc(QString *svcname,QWidget *parent=0);
 ~AddSvc();
 QSize sizeHint() const;
 QSizePolicy sizePolicy() const;
 
 private slots:
  void okData();
  void cancelData();

 private:
  QLineEdit *svc_name_edit;
  QComboBox *svc_exemplar_box;
  RDServiceListModel *svc_services_model;
  QString *svc_name;
};


#endif  // ADD_SVC_H
