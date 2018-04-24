// rdmarkeredit.cpp
//
//   An flashing button widget.
//
//   (C) Copyright 2002,2016 Fred Gleason <fredg@paravelsystems.com>
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
//

#include <rdmarker_edit.h>
//Added by qt3to4:
#include <QKeyEvent>

RDMarkerEdit::RDMarkerEdit(QWidget *parent)
  : QLineEdit(parent)
{
}


RDMarkerEdit::RDMarkerEdit(const QString &contents,QWidget *parent)
  : QLineEdit(contents,parent)
{
}


void RDMarkerEdit::keyPressEvent(QKeyEvent *e)
{
  switch(e->key()) {
      case Qt::Key_Escape:
	emit escapePressed();
	break;

      case Qt::Key_Delete:
	e->ignore();
	return;
	break;

      case Qt::Key_Space:
	e->ignore();
	return;
	break;

      case Qt::Key_Home:
	e->ignore();
	return;
	break;

      case Qt::Key_End:
	e->ignore();
	return;
	break;

      default:
	break;
  }
  QLineEdit::keyPressEvent(e);
}
