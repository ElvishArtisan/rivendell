//   rdlabel.cpp
//
//   Multiline button labelling that is smart about spaces and the like 
//
//   (C) Copyright 2002,2016 Fred Gleason <fredg@paravelsystems.com>
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
//

#include <stdlib.h>
#include <stdio.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qpalette.h>
#include <qsize.h>
#include <qrect.h>
#include <qpixmap.h>

#include <rdlabel.h>


RDLabel::RDLabel(QWidget *parent,Qt::WFlags f)
  : QLabel(parent,"",f)
{
  label_wrap=false;
}


RDLabel::RDLabel(const QString &text,QWidget *parent=0,Qt::WFlags f)
  : QLabel(text,parent,"",f)
{
  label_wrap=false;
  label_text=text;
  QLabel::setText(WrapText());
}


RDLabel::RDLabel(QWidget *buddy,const QString &text,QWidget *parent,Qt::WFlags f)
  : QLabel(buddy,text,parent,"",f)
{
  label_wrap=false;
  label_text=text;
  QLabel::setText(WrapText());
}


QString RDLabel::text() const
{
  return label_text;
}


void RDLabel::setFont(const QFont &font)
{
  label_font=font;
  QLabel::setFont(font);
  QLabel::setText(WrapText());
}


bool RDLabel::wordWrapEnabled() const
{
  return label_wrap;
}


void RDLabel::setWordWrapEnabled(bool state)
{
  label_wrap=state;
  QLabel::setText(WrapText());
}


void RDLabel::setText(const QString &string)
{
  label_text=string;
  QLabel::setText(WrapText());
}


QString RDLabel::WrapText()
{
  return label_text;
  /*
  QFontMetrics fm(label_font);
  QString str;
  QString residue=label_text;
  bool space_found=false;
  int l;

  if(label_wrap&&!label_text.isEmpty()) {
    while(!residue.isEmpty()) {
      space_found=false;
      for(int i=residue.length();i>=0;i--) {
	if((i==((int)residue.length()))||(residue.at(i).isSpace())) {
	  if(fm.boundingRect(residue.left(i)).width()<=width()) {
	    space_found=true;
	    if(!str.isEmpty()) {
	      str+="\n";
	    }
	    str+=residue.left(i);
	    if(i==(int)residue.length()) {
	      return str;
	    }
	    residue=residue.right(residue.length()-i-1);
	  }
	}
      }
      if(!space_found) {
	l=residue.length();
	for(int i=l;i>=0;i--) {
	  if(fm.boundingRect(residue.left(i)).width()<=width()) {
	    if(!str.isEmpty()) {
	      str+="\n";
	    }
	    str+=residue.left(i);
	    if(i==(int)residue.length()) {
	      return str;
	    }
	    residue=residue.right(residue.length()-i-1);
	  }
	}
      }
    }
  }
  return label_text;
  */
}
