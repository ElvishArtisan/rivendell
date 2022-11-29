// rdmulticaster.cpp
//
// Multi-interface multicast transciever
//
//   (C) Copyright 2018-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <unistd.h>

#include <QNetworkDatagram>

#include "rdmulticaster.h"

RDMulticaster::RDMulticaster(QObject *parent)
  : QObject(parent)
{
  multi_socket=new QUdpSocket(this);
  connect(multi_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));

  GetInterfaces();
}


bool RDMulticaster::bind(uint16_t port)
{
  return multi_socket->bind(QHostAddress("0.0.0.0"),port);
}


void RDMulticaster::enableLoopback(bool state)
{
  int var=state;

  setsockopt(multi_socket->socketDescriptor(),IPPROTO_IP,IP_MULTICAST_LOOP,
	     &var,sizeof(var));
}


void RDMulticaster::subscribe(const QHostAddress &addr)
{
  struct ip_mreqn mreq;

  for(int i=0;i<multi_iface_addresses.size();i++) {
    memset(&mreq,0,sizeof(mreq));
    mreq.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
    mreq.imr_address.s_addr=htonl(multi_iface_addresses[i].toIPv4Address());
    mreq.imr_ifindex=0;
    if(setsockopt(multi_socket->socketDescriptor(),IPPROTO_IP,IP_ADD_MEMBERSHIP,
		  &mreq,sizeof(mreq))<0) {
      fprintf(stderr,"%s\n",(tr("Unable to subscribe to multicast address")+
			     " \""+addr.toString()+"\" ["+
			     QString(strerror(errno))+"]").
	      toUtf8().constData());
    }
  }
}


void RDMulticaster::unsubscribe(const QHostAddress &addr)
{
  struct ip_mreqn mreq;

  for(int i=0;i<multi_iface_addresses.size();i++) {
    memset(&mreq,0,sizeof(mreq));
    mreq.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
    mreq.imr_address.s_addr=htonl(multi_iface_addresses[i].toIPv4Address());
    mreq.imr_ifindex=0;
    if(setsockopt(multi_socket->socketDescriptor(),IPPROTO_IP,IP_DROP_MEMBERSHIP,
		  &mreq,sizeof(mreq))<0) {
      fprintf(stderr,"%s\n",(tr("Unable to subscribe to multicast address")+
			     " \""+addr.toString()+"\" ["+
			     QString(strerror(errno))+"]").
	      toUtf8().constData());
    }
  }
}


void RDMulticaster::send(const QString &msg,const QHostAddress &m_addr,
			 uint16_t port)
{
  multi_socket->writeDatagram(msg.toUtf8(),m_addr,port);
}


void RDMulticaster::readyReadData()
{
  while(multi_socket->hasPendingDatagrams()) {
    QNetworkDatagram dg=multi_socket->receiveDatagram();
    printf("emitting: %s",dg.data().constData());
    emit received(QString::fromUtf8(dg.data()),dg.senderAddress());
  }
}


void RDMulticaster::GetInterfaces()
{
  int fd;
  struct ifreq ifr;
  int index=0;
  sockaddr_in *sa=NULL;

  if((fd=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    return;
  }

  memset(&ifr,0,sizeof(ifr));
  index=1;
  ifr.ifr_ifindex=index;
  while(ioctl(fd,SIOCGIFNAME,&ifr)==0) {
    if(ioctl(fd,SIOCGIFADDR,&ifr)==0) {
      sa=(struct sockaddr_in *)(&(ifr.ifr_addr));
      if((ntohl(sa->sin_addr.s_addr)&0xFF000000)!=0x7F000000) {
	multi_iface_addresses.push_back(QHostAddress());
	multi_iface_addresses.back().setAddress(ntohl(sa->sin_addr.s_addr));
      }
    }
    ifr.ifr_ifindex=++index;
  }
  ::close(fd);
}
