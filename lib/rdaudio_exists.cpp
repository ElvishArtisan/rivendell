// rdaudio_exists.cpp
//
// Test for the existence of audio in a Rivendell Audio Cut.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdaudio_exists.cpp,v 1.10 2010/09/13 23:14:25 cvs Exp $
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

#include <qstring.h>
#include <qfile.h>
#include <rd.h>
#include <rdcut.h>
#include <rdaudio_exists.h>

bool RDAudioExists(QString cutname)
{
  RDCut *cut=new RDCut(cutname);
  bool ret=cut->exists()&&(cut->length()>0);
  delete cut;
  return ret;
}
