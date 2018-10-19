//   rdttydevice.h
//
//   A Qt driver for serial ports.
//
//   (C) Copyright 2010-2018 Fred Gleason <fredg@paravelsystems.com>
//       All Rights Reserved
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

#ifndef RDTTYDEVICE_H
#define RDTTYDEVICE_H

#include <termios.h>
#include <unistd.h>

#include <queue>

#include <qiodevice.h>
#include <qsocketnotifier.h>
#include <qtimer.h>

class RDTTYDevice : public QIODevice
{
  Q_OBJECT;
  public:
  enum Parity {None=0,Even=1,Odd=2};
  enum FlowControl {FlowNone=0,FlowRtsCts=1,FlowXonXoff=2};
  RDTTYDevice(QObject *parent=0);
  ~RDTTYDevice();
  QString name() const;
  void setName(const QString &str);
  bool open(QIODevice::OpenMode mode);
  void close();
  qint64 read(char *data,qint64 maxlen);
  QByteArray read(qint64 maxlen);
  QByteArray readAll();
  qint64 readBlock(char *data,qint64 maxlen);
  qint64 write(const char *data,qint64 len);
  qint64 write(const QByteArray &array);
  bool getChar(char *ch);
  bool putChar(char ch);
  qint64 size() const;
  qint64 bytesAvailable() const;
  qint64 bytesToWrite() const;
  bool isSequential() const;
  bool isReadable() const;
  bool isWritable() const;
  bool isOpen() const;
  QString deviceName() const;
  void setDeviceName(QString name);
  int speed() const;
  void setSpeed(int speed);
  int wordLength() const;
  void setWordLength(int length);
  RDTTYDevice::Parity parity() const;
  void setParity(Parity);
  RDTTYDevice::FlowControl flowControl() const;
  void setFlowControl(FlowControl ctrl);
  int fileDescriptor() const;

 signals:
  void readChannelFinished();

 protected:
  qint64 readData(char *data,qint64 maxlen);
  qint64 writeData(const char *data,qint64 len);

 private slots:
  void readTtyData(int sock);
  void writeTtyData();

 private:
  Parity tty_parity;
  FlowControl tty_flow_control;
  QString tty_name;
  bool tty_open;
  int tty_flags;
  QIODevice::OpenMode tty_mode;
  int tty_fd;
  speed_t tty_speed;
  tcflag_t tty_length;
  QSocketNotifier *tty_notifier;
  std::queue<char> tty_write_queue;
  QTimer *tty_write_timer;
};


#endif  // RDTTYDEVICE_H
