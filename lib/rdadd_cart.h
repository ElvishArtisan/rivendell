// rdadd_cart.h
//
// Add a Rivendell Cart
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdadd_cart.h,v 1.5 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDADD_CART_H
#define RDADD_CART_H

#include <qdialog.h>
#include <qcombobox.h>
#include <qlineedit.h>

#include <rdcart.h>
#include <rduser.h>
#include <rdsystem.h>

class RDAddCart : public QDialog
{
  Q_OBJECT
  public:
   RDAddCart(QString *group,RDCart::Type *type,QString *title,
	     const QString &username,RDSystem *system,
	     QWidget *parent=0,const char *name=0);
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void groupActivatedData(const QString &);
   void okData();
   void cancelData();

 protected:
   void closeEvent(QCloseEvent *e);

  private:
   QLineEdit *cart_number_edit;
   QComboBox *cart_group_box;
   QComboBox *cart_type_box;
   QLineEdit *cart_title_edit;
   QString *cart_group;
   RDCart::Type *cart_type;
   QString *cart_title;
   RDSystem *cart_system;
};


#endif  // RDADD_CART_H
