// rdtty.h
//
// Abstract a Rivendell TTY
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdtty.h,v 1.8 2010/07/29 19:32:34 cvs Exp $
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

#include <qsqldatabase.h>

#include <rdttydevice.h>

#ifndef RDTTY_H
#define RDTTY_H

class RDTty
{
  public:
   enum Termination {NoTerm=0,CrTerm=1,LfTerm=2,CrLfTerm=3};
   RDTty(const QString &station,unsigned port_id,bool create=false);
   QString station() const;
   int portId() const;
   bool active();
   void setActive(bool state);
   QString port();
   void setPort(QString port);
   int baudRate();
   void setBaudRate(int rate);
   int dataBits();
   void setDataBits(int bits);
   int stopBits();
   void setStopBits(int bits);
   RDTTYDevice::Parity parity();
   void setParity(RDTTYDevice::Parity);
   RDTty::Termination termination();
   void setTermination(RDTty::Termination term);

  private:
   bool GetBoolValue(const QString &field);
   QString GetStringValue(const QString &field);
   int GetIntValue(const QString &field);
   void SetRow(const QString &param,bool value);
   void SetRow(const QString &param,const QString &value);
   void SetRow(const QString &param,int value);
   QString tty_station;
   int tty_id;
};


#endif 
