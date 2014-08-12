// rdpeaksexport.h
//
// Export peak data using the RdXport Web Service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpeaksexport.h,v 1.1.6.1 2013/11/13 23:36:33 cvs Exp $
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

#ifndef RDPEAKSEXPORT_H
#define RDPEAKSEXPORT_H

#include <qobject.h>

#include <rdconfig.h>
#include <rdstation.h>
#include <rdsettings.h>

class RDPeaksExport
{
 public:
  enum ErrorCode {ErrorOk=0,ErrorNoSource=2,
		  ErrorInternal=5,ErrorUrlInvalid=7,
		  ErrorService=8,ErrorInvalidUser=9,ErrorAborted=10};
  RDPeaksExport(RDStation *station,RDConfig *config,QObject *parent=0,
		const char *name=0);
  ~RDPeaksExport();
  void setCartNumber(unsigned cartnum);
  void setCutNumber(unsigned cutnum);
  RDPeaksExport::ErrorCode runExport(const QString &username,
				     const QString &password);
  unsigned energySize();
  unsigned short energy(unsigned frame);
  int readEnergy(unsigned short buf[],int count);
  static QString errorText(RDPeaksExport::ErrorCode err);

 private:
  RDStation *conv_station;
  RDConfig *conv_config;
  unsigned conv_cart_number;
  unsigned conv_cut_number;
  unsigned short *conv_energy_data;
  unsigned conv_write_ptr;
  friend size_t RDPeaksExportWrite(void *ptr, size_t size, size_t nmemb, 
				   void *userdata);
};


#endif  // RDPEAKSEXPORT_H
