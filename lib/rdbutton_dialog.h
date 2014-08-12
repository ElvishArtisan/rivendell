
// rdbutton_dialog.h
//
// Button Editor for the SoundPanel in RDAirPlay.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdbutton_dialog.h,v 1.14.8.1 2012/11/26 20:19:36 cvs Exp $
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

#ifndef RDBUTTON_DIALOG_H
#define RDBUTTON_DIALOG_H

#include <qdialog.h>
#include <qcombobox.h>
#include <qlineedit.h>

#include <rdpanel_button.h>
#include <rdcart_dialog.h>

class RDButtonDialog : public QDialog
{
  Q_OBJECT
 public:
  RDButtonDialog(QString station_name,const QString &label_template,
		 RDCartDialog *cart_dialog,const QString &svcname,
		 QWidget *parent=0,const char *name=0);
/*
  RDButtonDialog(QString station_name,const QString &label_template,
		 RDCae *cae,RDRipc *ripc,RDStation *station,
		 int audition_card,int audition_port,
		 const QString &editor_cmd,
		 QWidget *parent=0,const char *name=0);
*/
  ~RDButtonDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(RDPanelButton *button,bool hookmode,const QString &username,
	   const QString &passwd);

 private slots:
  void setCartData();
  void clearCartData();
  void setColorData();
  void okData();
  void cancelData();

 private:
  void DisplayCart(int cartnum);
  QLineEdit *edit_label_edit;
  QLineEdit *edit_cart_edit;
  RDPanelButton *edit_button;
  QString edit_label_template;
  QString edit_filter;
  QString edit_group;
  QString edit_user_name;
  QString edit_user_password;
  QPushButton *edit_color_button;
  int edit_cart;
  QColor edit_color;
  QString edit_station_name;
  bool edit_hookmode;
  RDCartDialog *edit_cart_dialog;
  QString edit_svcname;
};


#endif

