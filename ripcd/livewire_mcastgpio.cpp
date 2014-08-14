// livewire_mcastgpio.cpp
//
// A Rivendell multicast GPIO driver for LiveWire networks.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: livewire_mcastgpio.cpp,v 1.1.2.1 2013/11/17 03:40:27 cvs Exp $
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

#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <rddb.h>
#include <rdescape_string.h>

#include <globals.h>
#include <livewire_mcastgpio.h>

LiveWireMcastGpio::LiveWireMcastGpio(RDMatrix *matrix,QObject *parent,const char *name)
  : Switcher(matrix,parent,name)
{
  livewire_gpio_notify=NULL;

  long sockopt;
  struct sockaddr_in sa;
  QString sql;
  RDSqlQuery *q;

  //
  // Get Matrix Parameters
  //
  livewire_stationname=rdstation->name();
  livewire_matrix=matrix->matrix();
  livewire_gpios=matrix->gpis();
  livewire_interface_addr=matrix->ipAddress(RDMatrix::Primary);

  //
  // Initialize Serial Numbers
  //
  time_t t;
  t=time(&t);
  srand(t);
  livewire_gpio_send_serial=rand();

  //
  // Timers
  //
  livewire_gpi_timer_mapper=new QSignalMapper(this);
  connect(livewire_gpi_timer_mapper,SIGNAL(mapped(int)),
	  this,SLOT(gpiTimeoutData(int)));
  for(unsigned i=0;i<livewire_gpios;i++) {
    livewire_gpi_timers.push_back(new QTimer(this));
    livewire_gpi_timer_mapper->setMapping(livewire_gpi_timers.back(),i);
    connect(livewire_gpi_timers.back(),SIGNAL(timeout()),
	    livewire_gpi_timer_mapper,SLOT(map()));
  }
  livewire_gpo_timer_mapper=new QSignalMapper(this);
  connect(livewire_gpo_timer_mapper,SIGNAL(mapped(int)),
	  this,SLOT(gpoTimeoutData(int)));
  for(unsigned i=0;i<livewire_gpios;i++) {
    livewire_gpo_timers.push_back(new QTimer(this));
    livewire_gpo_timer_mapper->setMapping(livewire_gpo_timers.back(),i);
    connect(livewire_gpo_timers.back(),SIGNAL(timeout()),
	    livewire_gpo_timer_mapper,SLOT(map()));
    livewire_gpo_states.push_back(false);
  }

  //
  // GPIO Write Socket
  //
  if((livewire_gpio_write_socket=socket(PF_INET,SOCK_DGRAM,0))<0) {
    syslog(LOG_ERR,"unable to create GPIO write socket [%s]",strerror(errno));
    return;
  }
  sockopt=O_NONBLOCK;
  fcntl(livewire_gpio_write_socket,F_SETFL,sockopt);
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(htons(RD_LIVEWIRE_GPIO_SEND_PORT));
  sa.sin_addr.s_addr=
    htonl(matrix->ipAddress(RDMatrix::Primary).ip4Addr());
  if(bind(livewire_gpio_write_socket,(struct sockaddr *)&sa,sizeof(sa))<0) {
    syslog(LOG_ERR,"unable to bind GPIO write socket [%s]",strerror(errno));
    return;
  }

  //
  // GPIO Read Socket
  //
  if((livewire_gpio_read_socket=socket(PF_INET,SOCK_DGRAM,0))<0) {
    syslog(LOG_ERR,"unable to create GPIO read socket [%s]",strerror(errno));
    return;
  }
  sockopt=O_NONBLOCK;
  fcntl(livewire_gpio_read_socket,F_SETFL,sockopt);
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(RD_LIVEWIRE_GPIO_RECV_PORT);
  sa.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(livewire_gpio_read_socket,(struct sockaddr *)&sa,sizeof(sa))<0) {
    syslog(LOG_ERR,"unable to bind GPIO socket [%s]",strerror(errno));
    return;
  }
  livewire_gpio_notify=
    new QSocketNotifier(livewire_gpio_read_socket,QSocketNotifier::Read,this);
  connect(livewire_gpio_notify,SIGNAL(activated(int)),
	  this,SLOT(gpioActivatedData(int)));
  subscribe(QHostAddress(RD_LIVEWIRE_GPIO_MCAST_ADDR));

  //
  // Source Table
  //
  sql=QString("select SLOT,SOURCE_NUMBER,IP_ADDRESS from LIVEWIRE_GPIO_SLOTS ")+
    "where (STATION_NAME=\""+RDEscapeString(livewire_stationname)+"\")&&"+
    QString().sprintf("(MATRIX=%d) ",livewire_matrix)+
    "order by SLOT";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    livewire_source_numbers[q->value(0).toInt()]=q->value(1).toInt();
    livewire_surface_addresses[q->value(0).toInt()]=
      QHostAddress(q->value(2).toString());
  }
  delete q;
}


LiveWireMcastGpio::~LiveWireMcastGpio()
{
  if(livewire_gpio_notify!=NULL) {
    close(livewire_gpio_read_socket);
    delete livewire_gpio_notify;
    close(livewire_gpio_write_socket);    
  }
  for(unsigned i=0;i<livewire_gpi_timers.size();i++) {
    delete livewire_gpi_timers[i];
  }
  for(unsigned i=0;i<livewire_gpo_timers.size();i++) {
    delete livewire_gpo_timers[i];
  }
}


RDMatrix::Type LiveWireMcastGpio::type()
{
  return RDMatrix::LiveWireMcastGpio;
}


unsigned LiveWireMcastGpio::gpiQuantity()
{
  return livewire_gpios;
}


unsigned LiveWireMcastGpio::gpoQuantity()
{
  return livewire_gpios;
}


bool LiveWireMcastGpio::primaryTtyActive()
{
  return false;
}


bool LiveWireMcastGpio::secondaryTtyActive()
{
  return false;
}


void LiveWireMcastGpio::processCommand(RDMacro *cmd)
{
  int slot;
  int line;

  switch(cmd->command()) {
      case RDMacro::GO:
	if((cmd->argQuantity()!=5)||
	   (cmd->arg(2).toInt()<1)||
	   (cmd->arg(2).toInt()>(int)livewire_gpios)||
	   ((cmd->arg(1).toString().lower()!="i")&&
	    (cmd->arg(1).toString().lower()!="o"))) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}

	if(cmd->arg(1).toString().lower()=="i") {
	  emit gpiChanged(livewire_matrix,cmd->arg(2).toInt()-1,
			  cmd->arg(3).toInt());
	  if(cmd->arg(4).toInt()>0) {
	    livewire_gpi_timers[cmd->arg(2).toInt()-1]->
	      start(cmd->arg(4).toInt(),true);
	  }
	}
	if(cmd->arg(1).toString().lower()=="o") {
	  slot=(cmd->arg(2).toInt()-1)/5;
	  line=(cmd->arg(2).toInt()-1)%5;
	  if(livewire_source_numbers[slot]<=0) {
	    cmd->acknowledge(false);
	    emit rmlEcho(cmd);
	    return;
	  }
	  if(cmd->arg(4).toInt()>0) {
	    livewire_gpo_timers[cmd->arg(2).toInt()-1]->
	      start(cmd->arg(4).toInt(),true);
	  }
	  ProcessGpo(livewire_source_numbers[slot],line,cmd->arg(3).toInt());
	  livewire_gpo_states[cmd->arg(2).toInt()-1]=cmd->arg(3).toInt();
	}
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}


void LiveWireMcastGpio::gpioActivatedData(int sock)
{
  int n;
  char data[1500];
  struct sockaddr_in sa;
  socklen_t sa_len=sizeof(sa);
  uint32_t serial;

  while((n=recvfrom(livewire_gpio_read_socket,data,1500,MSG_DONTWAIT,
		    (struct sockaddr *)(&sa),&sa_len))>0) {
    serial=((0xFF&data[4])<<24)+((0xFF&data[5])<<16)+((0xFF&data[6])<<8)+
      (0xFF&data[7]);
    if(livewire_gpio_recv_serials[sa.sin_addr.s_addr]!=(serial-1)) {
      livewire_gpio_recv_serials[sa.sin_addr.s_addr]=serial;
      ProcessGpi(QHostAddress(ntohl(sa.sin_addr.s_addr)),
		 ((0xFF&data[23])<<8)+(0xFF&data[24]),0x08-(0xff&data[25]),
		 (data[27]&0x40)!=0,(data[27]&0x0A)!=0);
    }
  }
}


void LiveWireMcastGpio::gpiTimeoutData(int gpi)
{
  emit gpiChanged(livewire_matrix,gpi,false);
}


void LiveWireMcastGpio::gpoTimeoutData(int gpo)
{
  int slot=gpo/5;
  int line=gpo%5;

  if(livewire_source_numbers[slot]>0) {
    ProcessGpo(livewire_source_numbers[slot],line,!livewire_gpo_states[gpo]);
    livewire_gpo_states[gpo]=!livewire_gpo_states[gpo];
  }
}


void LiveWireMcastGpio::ProcessGpi(const QHostAddress &src_addr,int chan,
			      unsigned line,bool state,bool pulse)
{
  for(std::map<int,int>::const_iterator it=livewire_source_numbers.begin();
      it!=livewire_source_numbers.end();it++) {
    if((it->second==chan)&&
       ((livewire_surface_addresses[it->first].isNull())||
	(livewire_surface_addresses[it->first]==src_addr))) {
      emit gpiChanged(livewire_matrix,5*it->first+line,state);
      if(pulse) {
	livewire_gpi_timers[5*it->first+line]->
	  start(RD_LIVEWIRE_GPIO_PULSE_WIDTH,true);
      }
    }
  }
}


void LiveWireMcastGpio::ProcessGpo(int chan,unsigned line,bool state)
{
  //
  // Destination Address
  //
  struct sockaddr_in sa;
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(RD_LIVEWIRE_GPIO_SEND_PORT);
  sa.sin_addr.s_addr=
    htonl(QHostAddress(RD_LIVEWIRE_GPIO_MCAST_ADDR).toIPv4Address());

  /*
   * FIXME: Sending this to the mcast addr causes ALL instances
   *        of the source to switch.  How does one specify a particular
   *        surface?  Sending to the surface addr does not work!
   *

  for(std::map<int,int>::const_iterator it=livewire_source_numbers.begin();
      it!=livewire_source_numbers.end();it++) {
    if((it->second==chan)&&(!livewire_surface_addresses[it->first].isNull())) {
      sa.sin_addr.s_addr=
	htonl(livewire_surface_addresses[it->first].toIPv4Address());
    }
  }
  syslog(LOG_NOTICE,"using %s",
	 (const char *)QHostAddress(ntohl(sa.sin_addr.s_addr)).toString());
  */

  uint8_t data[28]={0x03,0x00,0x02,0x07,0xC6,0x04,0x55,0x1E,
		    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		    'I','N','D','I',0x00,0x01,0x00,0x00,
		    0x00,0x00,0x07,0x00};
  data[4]=0xFF&(livewire_gpio_send_serial>>24);
  data[5]=0xFF&(livewire_gpio_send_serial>>16);
  data[6]=0xFF&(livewire_gpio_send_serial>>8);
  data[7]=0xFF&livewire_gpio_send_serial;
  data[23]=0xFF&(chan>>8);
  data[24]=0xFF&chan;
  data[25]=0xFF&(0x0D-line);
  data[27]=0xFF&state;
  sendto(livewire_gpio_write_socket,data,28,MSG_DONTWAIT,
	 (struct sockaddr *)(&sa),sizeof(sa));
  livewire_gpio_send_serial++;
  data[4]=0xFF&(livewire_gpio_send_serial>>24);
  data[5]=0xFF&(livewire_gpio_send_serial>>16);
  data[6]=0xFF&(livewire_gpio_send_serial>>8);
  data[7]=0xFF&livewire_gpio_send_serial;
  sendto(livewire_gpio_write_socket,data,28,MSG_DONTWAIT,
	 (struct sockaddr *)(&sa),sizeof(sa));
  livewire_gpio_send_serial++;
}


QString LiveWireMcastGpio::AddressString(uint32_t addr) const
{
  return QString().sprintf("%d.%d.%d.%d",
			   0xFF&addr,
			   0xFF&(addr>>8),
			   0xFF&(addr>>16),
			   0xFF&(addr>>24));
}


void LiveWireMcastGpio::subscribe(const QHostAddress &addr) const
{
  subscribe(htonl(addr.toIPv4Address()));
}


void LiveWireMcastGpio::subscribe(const uint32_t addr) const
{
  struct ip_mreqn mreq;

  memset(&mreq,0,sizeof(mreq));
  mreq.imr_multiaddr.s_addr=addr;
  mreq.imr_address.s_addr=htonl(livewire_interface_addr.toIPv4Address());
  mreq.imr_ifindex=0;
  if(setsockopt(livewire_gpio_read_socket,SOL_IP,IP_ADD_MEMBERSHIP,&mreq,
		sizeof(mreq))!=0) {
    if(errno!=EADDRINUSE) {
      syslog(LOG_WARNING,"unable to subscribe to %s on %s [%s]",
	     (const char *)AddressString(addr),
	     (const char *)livewire_interface_addr.toString(),
	     strerror(errno));
    }
  }
}


void LiveWireMcastGpio::unsubscribe(const QHostAddress &addr) const
{
  unsubscribe(htonl(addr.toIPv4Address()));
}


void LiveWireMcastGpio::unsubscribe(const uint32_t addr) const
{
  struct ip_mreqn mreq;

  memset(&mreq,0,sizeof(mreq));
  mreq.imr_multiaddr.s_addr=htonl(addr);
  mreq.imr_address.s_addr=htonl(livewire_interface_addr.toIPv4Address());
  mreq.imr_ifindex=0;
  if(setsockopt(livewire_gpio_read_socket,SOL_IP,IP_DROP_MEMBERSHIP,&mreq,
		sizeof(mreq))!=0) {
    if(errno!=ENODEV) {
      syslog(LOG_WARNING,"unable to unsubscribe from %s on %s [%s]",
	     (const char *)AddressString(addr),
	     (const char *)livewire_interface_addr.toString(),
	     strerror(errno));
    }
  }
}
