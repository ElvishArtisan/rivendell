// autofill_carts.h
//
// Edit a List of Autofill Carts
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: autofill_carts.h,v 1.6 2010/07/29 19:32:34 cvs Exp $
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

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlistview.h>

#include <rdsvc.h>

class AutofillCarts : public QDialog
{
 Q_OBJECT
 public:
  AutofillCarts(RDSvc *svc,QWidget *parent=0,const char *name=0);
  ~AutofillCarts();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void addData();
  void deleteData();
  void okData();
  void cancelData();

 private:
  void RefreshList();
  RDSvc *svc_svc;
  QListView *svc_cart_list;
  QString svc_cart_filter;
  QString svc_cart_group;
};


#endif

