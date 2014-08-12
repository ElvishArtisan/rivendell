// filter.h
//
// Set Filter Widget for RDLibrary.
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: filter.h,v 1.5 2010/07/29 19:32:36 cvs Exp $
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

#ifndef FILTER_H
#define FILTER_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qradiobutton.h>



class Filter : public QDialog
{
  Q_OBJECT
  public:
   Filter(QString *filter,QWidget *parent=0,const char *name=0);
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void okData();
   void clearData();
   void cancelData();

  private:
   QString *filter_text;
   QLineEdit *filter_text_edit;
};


#endif

