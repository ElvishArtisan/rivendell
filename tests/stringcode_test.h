// stringcode_test.h
//
// Test the Rivendell string encoder routines.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: stringcode_test.h,v 1.1.2.1 2013/10/16 21:14:38 cvs Exp $
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

#ifndef STRINGCODE_TEST_H
#define STRINGCODE_TEST_H

#include <qobject.h>

#define STRINGCODE_TEST_USAGE "[options]\n\nTest the Rivendell string encoding routines in RDWeb\n\nOptions are:\n--xml-encode\n     Encode using RDXmlEscape()\n\n--xml-decode\n     Decode using RDXmlUnescape()\n\n--encode-url\n     Encode using RDUrlEscape()\n\n--url-decode\n     Decode using RDUrlUnescape()\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);
};


#endif  // STRINGCODE_TEST_H
