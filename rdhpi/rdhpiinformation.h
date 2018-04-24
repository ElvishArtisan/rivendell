// rdhpiinformation.h
//
// A Container Class for AudioScience HPI Adapter Info
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDHPIINFORMATION_H
#define RDHPIINFORMATION_H

#include <stdint.h>

class RDHPIInformation
{
 public:
  RDHPIInformation();
  unsigned serialNumber() const;
  void setSerialNumber(unsigned num);
  unsigned hpiMajorVersion() const;
  void setHpiMajorVersion(unsigned ver);
  unsigned hpiMinorVersion() const;
  void setHpiMinorVersion(unsigned ver);
  unsigned hpiPointVersion() const;
  void setHpiPointVersion(unsigned ver);
  uint32_t hpiVersion() const;
  void setHpiVersion(uint32_t ver);
  unsigned dspMajorVersion() const;
  void setDspMajorVersion(unsigned ver);
  unsigned dspMinorVersion() const;
  void setDspMinorVersion(unsigned ver);
  char pcbVersion() const;
  void setPcbVersion(char ver);
  unsigned assemblyVersion() const;
  void setAssemblyVersion(unsigned ver);
  void clear();

 private:
  unsigned serial_number;
  unsigned dsp_major_version;
  uint32_t hpi_version;
  unsigned dsp_minor_version;
  char pcb_version;
  unsigned assembly_version;
};


#endif  // RDHPIINFORMATION_H

