// rdcsv.cpp
//
// Routines for generating CSV files
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdcsv.h"


QString RDCsvField(const QString &val,bool last)
{
  QString ret=val;
  if(val.contains(",")||val.contains("\"")) {
    ret="\""+ret.replace("\"","\"\"")+"\"";
  }
  if(last) {
    ret+="\r\n";
  }
  else {
    ret+=",";
  }

  return ret;
}


QString RDCsvField(int val,bool last)
{
  return RDCsvField(QString::asprintf("%d",val),last);
}


QString RDCsvField(unsigned val,bool last)
{
  return RDCsvField(QString::asprintf("%u",val),last);
}
