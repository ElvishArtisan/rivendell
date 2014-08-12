//   rdttydevice_win32.cpp
//
//   The Win32 version of a Qt driver for serial ports.
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdttydevice_win32.cpp,v 1.3 2010/07/29 19:32:34 cvs Exp $
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

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <qiodevice.h>

#include <rdttydevice.h>



RDTTYDevice::RDTTYDevice() : QIODevice()
{
  Init();
}

RDTTYDevice::~RDTTYDevice()
{
  if(tty_open) {
    close();
  }
}

bool RDTTYDevice::open(int mode)
{
  DWORD flags=0;
  WCHAR name[255];
  DCB dcb;
  COMMTIMEOUTS timeouts;

  tty_mode=mode;
  if((mode&IO_ReadWrite)==IO_ReadWrite) {
    flags|=GENERIC_WRITE|GENERIC_READ;
  }
  else {
    if(((mode&IO_WriteOnly)!=0)) {
      flags|=GENERIC_WRITE;
    }
    if(((mode&IO_ReadOnly)!=0)) {
      flags|=GENERIC_READ;
    }
  }
  wcscpy(name,(TCHAR*)qt_winTchar(tty_name,true));
  tty_fd=CreateFile(name,flags,0,NULL,OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL,NULL);
  if(tty_fd==INVALID_HANDLE_VALUE) {
    tty_status=IO_OpenError;
    return false;
  }
  tty_open=true;
  tty_status=IO_Ok;

  SetupComm(tty_fd,WIN32_BUFFER_SIZE,WIN32_BUFFER_SIZE);
  switch(tty_parity) {
  case RDTTYDevice::None:
    BuildCommDCB((TCHAR*)qt_winTchar(QString().
				    sprintf("%d,N,%d,1",
				    tty_speed,tty_length),
				    true),&dcb);
    break;

  case RDTTYDevice::Even:
    BuildCommDCB((TCHAR*)qt_winTchar(QString().
				    sprintf("%d,E,%d,1",
				    tty_speed,tty_length),
				    true),&dcb);
    break;

  case RDTTYDevice::Odd:
    BuildCommDCB((TCHAR*)qt_winTchar(QString().
				    sprintf("%d,O,%d,1",
				    tty_speed,tty_length),
				    true),&dcb);
    break;
  }
  SetCommState(tty_fd,&dcb);
  timeouts.ReadIntervalTimeout=MAXDWORD;
  timeouts.ReadTotalTimeoutMultiplier=0;
  timeouts.ReadTotalTimeoutConstant=0;
  timeouts.WriteTotalTimeoutMultiplier=0;
  timeouts.WriteTotalTimeoutConstant=0;
  SetCommTimeouts(tty_fd,&timeouts);

  return true;
}


void RDTTYDevice::close()
{
  if(tty_open) {
    CloseHandle(tty_fd);
  }
  tty_open=false;
}


void RDTTYDevice::flush()
{
}


Q_LONG RDTTYDevice::readBlock(char *data,Q_ULONG maxlen)
{
  Q_ULONG n;

  if(!ReadFile(tty_fd,data,maxlen,&n,NULL)) {
    if(GetLastError()!=ERROR_TIMEOUT) {
      tty_status=IO_ReadError;
      return -1;
    }
  }
  tty_status=IO_Ok;
  return n;
}


Q_LONG RDTTYDevice::writeBlock(const char *data,Q_ULONG len)
{
  Q_ULONG n;

  if(!WriteFile(tty_fd,data,len,&n,NULL)) {
    tty_status=IO_WriteError;
    return n;
  }
  tty_status=IO_Ok;
  return n;
}


int RDTTYDevice::getch()
{
  char c;
  int n;

  if((n=readBlock(&c,1))<0) {
    tty_status=IO_ReadError;
    return -1;
  }
  return (int)c;
}


int RDTTYDevice::putch(int ch)
{
  char c;
  int n;

  c=(char)ch;
  if((n=writeBlock(&c,1))<0) {
    tty_status=IO_WriteError;
    return -1;
  }
  return ch;
}


int RDTTYDevice::ungetch(int ch)
{
  tty_status=IO_WriteError;
  return -1;
}


QIODevice::Offset RDTTYDevice::size() const
{
  return 0;
}


int RDTTYDevice::flags() const
{
  return tty_mode|state();
}


int RDTTYDevice::mode() const
{
  return tty_mode;
}


int RDTTYDevice::state() const
{
  if(tty_open) {
    return IO_Open;
  }
  return 0;
}


bool RDTTYDevice::isDirectAccess() const
{
  return false;
}


bool RDTTYDevice::isSequentialAccess() const
{
  return true;
}


bool RDTTYDevice::isCombinedAccess() const
{
  return false;
}


bool RDTTYDevice::isBuffered() const
{
  return false;
}


bool RDTTYDevice::isRaw() const
{
  return true;
}


bool RDTTYDevice::isSynchronous() const
{
  return true;
}


bool RDTTYDevice::isAsynchronous() const
{
  return false;
}


bool RDTTYDevice::isTranslated() const
{
  return false;
}


bool RDTTYDevice::isReadable() const
{
  if(((tty_mode&IO_ReadOnly)!=0)||((tty_mode&IO_ReadWrite)!=0)) {
    return true;
  }
  return false;
}


bool RDTTYDevice::isWritable() const
{
  if(((tty_mode&IO_WriteOnly)!=0)||((tty_mode&IO_ReadWrite)!=0)) {
    return true;
  }
  return false;
}


bool RDTTYDevice::isReadWrite() const
{
  if((tty_mode&IO_ReadWrite)!=0) {
    return true;
  }
  return false;

}


bool RDTTYDevice::isInactive() const
{
  if(!tty_open) {
    return true;
  }
  return false;
}


bool RDTTYDevice::isOpen() const
{
  if(tty_open) {
    return true;
  }
  return false;
}


int RDTTYDevice::status() const
{
  return tty_status;
}


void RDTTYDevice::resetStatus()
{
  tty_status=IO_Ok;
}


void RDTTYDevice::setName(QString name)
{
  tty_name=name;
}


int RDTTYDevice::speed() const
{
  return tty_speed;
}


void RDTTYDevice::setSpeed(int speed)
{
  tty_speed=speed;
}


int RDTTYDevice::wordLength() const
{
  return tty_length;
}


void RDTTYDevice::setWordLength(int length)
{
  tty_length=length;
}


RDTTYDevice::Parity RDTTYDevice::parity() const
{
  return tty_parity;
}


void RDTTYDevice::setParity(Parity parity)
{
  tty_parity=parity;
}


void RDTTYDevice::Init()
{
  tty_speed=9600;
  tty_length=8;
  tty_parity=RDTTYDevice::None;
  tty_open=false;
}
