// vguest.h
//
// A Rivendell switcher driver for the Logitek vGuest Protocol
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: vguest.h,v 1.17.8.1 2013/11/06 01:32:31 cvs Exp $
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

#ifndef VGUEST_H
#define VGUEST_H

#include <vector>

#include <qhostaddress.h>
#include <qtimer.h>

#include <rdsocket.h>
#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>
#include <rdoneshot.h>

#include <switcher.h>

#define VGUEST_RECONNECT_MIN_INTERVAL 5000
#define VGUEST_RECONNECT_MAX_INTERVAL 15000.0
#define VGUEST_MAX_COMMAND_LENGTH 255
#define VGUEST_ID_BYTE 0x0C;
#define VGUEST_DEFAULT_SURFACE_NUMBER 1
#define VGUEST_DEFAULT_PORT 10212
#define VGUEST_MAX_TEXT_LENGTH 60
#define VGUEST_PING_INTERVAL 15000

class VGuest : public Switcher
{
 Q_OBJECT
 public:
  VGuest(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~VGuest();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void ipConnect(int id);
  void connectedData(int id);
  void connectionClosedData(int id);
  void readyReadData(int id);
  void errorData(int err,int id);
  void gpioOneshotData(void *data);
  void pingData(int id);
  void pingResponseData(int id);

 private:
  void SendCommand(char *str,int len);
  void DispatchCommand(char *cmd,int len,int id);
  void MetadataCommand(char *cmd,int len,int id);
  int GetRelay(int enginenum,int devicenum,int surfacenum,int relaynum);
  int GetHoldoff();
  QString RenderCommand(char *cmd,int len);
  QString PadString(QString str,unsigned len);
  void ExecuteMacroCart(unsigned cartnum);
  RDTTYDevice *vguest_device[2];
  RDSocket *vguest_socket[2];
  int vguest_matrix;
  QHostAddress vguest_ipaddress[2];
  int vguest_ipport[2];
  int vguest_inputs;
  QString vguest_username[2];
  QString vguest_password[2];
  unsigned vguest_start_cart[2];
  unsigned vguest_stop_cart[2];
  int vguest_outputs;
  int vguest_gpis;
  int vguest_gpos;
  int vguest_istate[2];
  int vguest_cmd_length[2];
  int vguest_cmd_ptr[2];
  char vguest_cmd_buffer[2][VGUEST_MAX_COMMAND_LENGTH];
  QSignalMapper *vguest_ping_mapper;
  QTimer *vguest_ping_timer[2];
  QSignalMapper *vguest_ping_response_mapper;
  QTimer *vguest_ping_response_timer[2];
  QTimer *vguest_reconnect_timer[2];
  bool vguest_error_notified[2];
  RDMatrix::PortType vguest_porttype[2];
  std::vector<int>vguest_input_engine_nums;
  std::vector<int>vguest_input_device_nums;
  std::vector<int>vguest_output_engine_nums;
  std::vector<int>vguest_output_device_nums;
  std::vector<int>vguest_relays_engine_nums;
  std::vector<int>vguest_relays_device_nums;
  std::vector<int>vguest_relays_surface_nums;
  std::vector<int>vguest_relays_relay_nums;
  std::vector<int>vguest_displays_engine_nums;
  std::vector<int>vguest_displays_device_nums;
  std::vector<int>vguest_displays_surface_nums;
  RDOneShot *vguest_gpio_oneshot;
};


#endif  // VGUEST_H
