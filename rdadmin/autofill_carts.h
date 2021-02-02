// autofill_carts.h
//
// Edit a List of Autofill Carts
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

#ifndef AUTOFILL_CARTS_H
#define AUTOFILL_CARTS_H

#include <QPushButton>

#include <rddialog.h>
#include <rdlibrarymodel.h>
#include <rdsvc.h>
#include <rdtableview.h>

class AutofillCarts : public RDDialog
{
 Q_OBJECT
 public:
  AutofillCarts(RDSvc *svc,QWidget *parent=0);
  ~AutofillCarts();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void addData();
  void deleteData();
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDSvc *svc_svc;
  RDTableView *svc_cart_view;
  RDLibraryModel *svc_cart_model;
  QPushButton *svc_add_button;
  QPushButton *svc_remove_button;
  QPushButton *svc_close_button;
  QString svc_cart_filter;
  QString svc_cart_group;
};


#endif  // AUTOFILL_CARTS_H
