// rmlsend.h
//
// A utility for sending RML Commands
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rmlsend.h,v 1.5 2010/07/29 19:32:40 cvs Exp $
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

#ifndef RMLSEND_H
#define RMLSEND_H

#include <qmainwindow.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qsocketdevice.h>
#include <qlabel.h>
#include <qtimer.h>

#include <rd.h>

//
// Settings
//
#define RMLSEND_USAGE "[--to-host=<hostname>] [--to-port=<port>] [<rml>|--from-file=<file>]\n\nWhere <hostname> is the name or IP address of the host to send the command to\n(default = localhost), <port> is the UDP port to to send the message to\n(default = 5859), <rml> is any valid RML code and <file> is the name of a file\ncontaining valid RML code.  If '-' is specified as <file>, then rmlsend will\nread the list of RML commands to be sent from standard input.\n\nWhen specifying RML code on the command line, it will likely be necessary\nto escape any special characters (such as spaces or bang [!] characters)\nto protect them from the shell.\n\nExamples:\n rmlsend LL\\ TestLog\\!\n    Send the RML command 'LL 1 TestLog!' to the local host.\n\n rmlsend --to-host=host.mydomain.com --to-port=5858 --from-file=test.rml\n    Send the RML commands in 'test.rml' to the system at 'host.mydomain.com' at\n    port 5858.\n"
#define RMLSEND_DEFAULT_ADDR "localhost"
#define RMLSEND_DEFAULT_PORT 5859

class MainWidget : public QMainWindow
{
  Q_OBJECT
 public:
  enum DestMode {Rml=0,RmlNoEcho=1,Manual=2};
  MainWidget(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void sendCommand();
  void readResponse();
  void destChangedData(int id);

 private:
  QPushButton *send,*quit;
  QLineEdit *host,*command,*response;
  QLabel *response_label;
  QLabel *port_edit_label;
  QComboBox *port_box;
  QLineEdit *port_edit;
  QSocketDevice *udp_command,*udp_response;
  QHostAddress host_addr;
  QTimer *timer;
  int countdown;
  QFont main_font;
  QPixmap *rivendell_map;
};


class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0,const char *name=0);
  ~MainObject();

 private:
  void ReadSwitches();
  void ResolveName();
  void InitStream();
  void CloseStream();
  bool GetNextChar(char *c);
  void ProcessCommands();
  int input_fd;
  QString input_file;
  QString dest_hostname;
  QHostAddress *dest_addr;
  unsigned dest_port;
  QString rml_cmd;
  unsigned rml_ptr;
};

#endif  // RMLSEND_H
