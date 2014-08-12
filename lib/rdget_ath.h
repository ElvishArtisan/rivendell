// get_ath.h
//
// Get an Agreggate Tuning Hours (ATH) Figure.
//
//   (C) Copyright 2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdget_ath.h,v 1.4 2010/07/29 19:32:33 cvs Exp $
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

#ifndef GET_ATH_H
#define GET_ATH_H

#include <qdialog.h>
#include <qlineedit.h>


class RDGetAth : public QDialog
{
  Q_OBJECT
  public:
   RDGetAth(double *ath,QWidget *parent=0,const char *name=0);
   ~RDGetAth();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void okData();
   void cancelData();

  private:
   QLineEdit *ath_ath_edit;
   double *ath_ath;
};


#endif

