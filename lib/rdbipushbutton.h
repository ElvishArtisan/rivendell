//   rdbipushbutton.h
//
//   QPushButton with a two-part legend
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#ifndef RDBIPUSHBUTTON_H
#define RDBIPUSHBUTTON_H

#include <QPushButton>

#include <rdfontengine.h>

class RDBiPushButton : public QPushButton, public RDFontEngine
{
  Q_OBJECT
 public:
  RDBiPushButton(QWidget *parent,RDConfig *c=NULL);
  RDBiPushButton(const QString &top_text,const QString &bottom_text,
		 QWidget *parent,RDConfig *c=NULL);
  QString topText() const;
  void setTopText(const QString &str);
  QString bottomText() const;
  void setBottomText(const QString &str);

 protected:
  void changeEvent(QEvent *e);
  void paintEvent(QPaintEvent *e);

 private:
  QString d_top_text;
  QString d_bottom_text;
};


#endif  // RDBIPUSHBUTTON_H
