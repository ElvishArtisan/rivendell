// rdcart_search_text.h
//
// Generates a standardized SQL 'WHERE' clause for filtering Rivendell carts.
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCART_SEARCH_TEXT_H
#define RDCART_SEARCH_TEXT_H

#include <qstring.h>
#include <qstringlist.h>

#include <rdstation.h>


QString RDCartSearchText(QString filter,const QString &group,
			 const QString &schedcode,bool incl_cuts);
QString RDCartSearchText(QString filter,const QString &group,
			 const QStringList &schedcodes,bool incl_cuts);
QString RDAllCartSearchText(const QString &filter,const QString &schedcode,
			    const QString &user,bool incl_cuts);
QString RDAllCartSearchText(const QString &filter,const QStringList &schedcodes,
			    const QString &user,bool incl_cuts);


#endif  // RDCART_SEARCH_TEXT_H
