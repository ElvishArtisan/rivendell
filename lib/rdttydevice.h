//   rdttydevice.h
//
//   A Qt driver for serial ports.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdttydevice.h,v 1.5 2011/05/04 18:09:06 cvs Exp $
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

#ifndef RDTTYDEVICE_H
#define RDTTYDEVICE_H

#define WIN32_BUFFER_SIZE 130

#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif  // WIN32
#include <qiodevice.h>
#include <qstring.h>


/**
 * @short A QIODevice-compatible class for serial ports.
 * @author Fred Gleason <fredg@paravelsystems.com>
 *
 * This class implements an QIODevice-compatible class for dealing 
 * with serial ports.
 **/

class RDTTYDevice : public QIODevice
{
  public:
   enum Parity {None=0,Even=1,Odd=2};

  /**
   * Create an RDTTYDevice object
   **/
  RDTTYDevice();

  /**
   * Destroy an RDTTYDevice object
   **/
  ~RDTTYDevice();

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool open(int mode);

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  void close();

#ifndef WIN32
  int socket();
#endif  // WIN32

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  void flush();

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  Q_LONG readBlock(char *data,Q_ULONG maxlen);

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  Q_LONG writeBlock(const char *data,Q_ULONG len);

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  int getch();

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  int putch(int ch);

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  int ungetch(int ch);


  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  QIODevice::Offset size() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  int flags() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  int mode() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  int state() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isDirectAccess() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isSequentialAccess() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isCombinedAccess() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isBuffered() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isRaw() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isSynchronous() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isAsynchronous() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isTranslated() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isReadable() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isWritable() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isReadWrite() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isInactive() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  bool isOpen() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  int status() const;

  /**
   * This method is reimplemented from the QIODevice class, the API is
   * unaffected.
   **/
  void resetStatus();

  /**
   * Set the name of the tty device to use.
   * @param name Name of tty device to use.
   **/
  void setName(QString name);

  /**
   * Returns the speed of the tty device, in bits per second.
   **/
  int speed() const;

  /**
   * Set the speed of the tty device.
   * @param speed Speed of the tty device, in bits per second.
   **/
  void setSpeed(int speed);

  /**
   * Returns the word length of the tty device.
   **/
  int wordLength() const;

  /**
   * Set the word length of the tty device.
   * @param length Word length in bits.  Legal values are 5, 6, 7 or 8.
   **/
  void setWordLength(int length);

  /**
   * Returns the parity coding of the tty device.
   **/
  RDTTYDevice::Parity parity() const;

  /**
   * Set the parity coding of the tty device.
   * @param parity Parity coding.
   **/
  void setParity(Parity);

 private:
  void Init();
  Parity tty_parity;
  QString tty_name;
  bool tty_open;
  int tty_flags;
  int tty_mode;
  int tty_status;
#ifdef WIN32
  HANDLE tty_fd;
  int tty_speed;
  int tty_length;
#else
  int tty_fd;
  speed_t tty_speed;
  tcflag_t tty_length;
#endif  // WIN32
};


#endif  // RDTTYDEVICE_H
