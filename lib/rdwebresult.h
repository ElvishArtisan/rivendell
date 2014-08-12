// rdwebresult.h
//
// Container class for Rivendel Web Service result messages.
//
//   (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdwebresult.h,v 1.1 2011/12/23 23:07:00 cvs Exp $
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

#include <rdaudioconvert.h>

#ifndef RDWEBRESULT_H
#define RDWEBRESULT_H

class RDWebResult
{
 public:
  RDWebResult(const QString &str,int resp_code,
	     RDAudioConvert::ErrorCode conv_code);
  RDWebResult();
  QString text() const;
  void setText(const QString &str);
  int responseCode() const;
  void setResponseCode(int code);
  RDAudioConvert::ErrorCode converterErrorCode() const;
  void setConverterErrorCode(RDAudioConvert::ErrorCode code);
  QString xml() const;
  bool readXml(const QString &xml);
  bool readXmlFromFile(const QString &filename);

 private:
  QString web_text;
  int web_response_code;
  RDAudioConvert::ErrorCode web_converter_code;
};


#endif  // RDWEBRESULT_H
