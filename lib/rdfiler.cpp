// rdfiler.cpp
//
// Delegate for opening/opening/deleting files
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <unistd.h>
#include <sys/socket.h>

#include <QStringList>

#include "rdfiler.h"

RDFiler::RDFiler(uid_t uid,gid_t gid,const QString &root_dir)
{
  d_uid=uid;
  d_gid=gid;
  d_root_directory=root_dir;
  d_service_pid=-1;
  d_unix_socket=-1;
}


RDFiler::~RDFiler()
{
  if(d_unix_socket>=0) {
    close(d_unix_socket);
  }
}


bool RDFiler::start()
{
  int socks[2];

  if(socketpair(AF_UNIX,SOCK_DGRAM,0,socks)<0) {
    return false;
  }
  d_service_pid=fork();
  if(d_service_pid<0) {
    return false;
  }
  if(d_service_pid==0) {
    //
    // We are the child
    //
    close(socks[0]);
    d_unix_socket=socks[1];
    RDApplication::ExitCode exit_code=ServiceLoop();
    close(d_unix_socket);
    exit(exit_code);
  }

  //
  // We are the parent
  //
  close(socks[1]);
  d_unix_socket=socks[0];

  return true;
}


int RDFiler::open(const QString &pathname,int flags,mode_t mode) const
{
  int fd=-1;
  ssize_t n;
  char resp[1501];
  QByteArray cmd=(QString("OPEN ")+
		  pathname+" "+
		  QString::asprintf("%u ",flags)+
		  QString::asprintf("%u",mode)).toUtf8();
  if(send(d_unix_socket,cmd,cmd.size(),0)<0) {
    return -1;
  }
  printf("HERE A\n");
  struct msghdr msg;
  struct iovec iov[1];
  union {
    struct cmsghdr cm;
    char control[CMSG_SPACE(sizeof(int))];
  } control_un;
  struct cmsghdr *cmptr=NULL;

  msg.msg_control=control_un.control;
  msg.msg_controllen=sizeof(control_un.control);
  msg.msg_name=NULL;
  msg.msg_namelen=0;

  iov[0].iov_base=resp;
  iov[0].iov_len=1500;
  msg.msg_iov=iov;
  msg.msg_iovlen=1;

  printf("HERE B\n");
  if((n=recvmsg(d_unix_socket,&msg,0))<0) {
  printf("HERE C\n");
    return -1;
  }
  printf("n: %d\n",n);
  printf("HERE D\n");

  if(((cmptr=CMSG_FIRSTHDR(&msg))!=NULL)&&
     cmptr->cmsg_len==CMSG_LEN(sizeof(int))) {
    if((cmptr->cmsg_level==SOL_SOCKET)&&(cmptr->cmsg_type==SCM_RIGHTS)) {
      fd=*((int *) CMSG_DATA(cmptr));
    }
  }


  /*
  if((n=recv(d_unix_socket,resp,1500,0))<0) {
    return -1;
  }
  resp[n]=0;
  if(QString(resp)=="OK") {
  }
  */

  return fd;
}


bool RDFiler::unlink(const QString &pathname) const
{
  ssize_t n;
  char resp[1501];
  QByteArray msg=(QString("UNLINK ")+pathname).toUtf8();
  if(send(d_unix_socket,msg,msg.size(),0)<0) {
    return false;
  }
  if((n=recv(d_unix_socket,resp,1500,0))<0) {
    return false;
  }
  resp[n]=0;

  return QString(resp)=="OK";
}


RDApplication::ExitCode RDFiler::ServiceLoop()
{
  char msg[1501];
  char resp[256];
  ssize_t n=0;
  bool ok;

  //
  // Set filesystem context
  //
  if(chdir(d_root_directory.toUtf8())<0) {
    close(d_unix_socket);
    return RDApplication::ExitSyscallError;
  }
  if(chroot(d_root_directory.toUtf8())<0) {
    close(d_unix_socket);
    return RDApplication::ExitSyscallError;
  }

  //
  // Drop root permissions
  //
  if(setgid(d_gid)<0) {
    close(d_unix_socket);
    return RDApplication::ExitSyscallError;
  }
  if(setuid(d_uid)<0) {
    close(d_unix_socket);
    return RDApplication::ExitSyscallError;
  }

  //
  // Main Loop
  //
  while((n=read(d_unix_socket,msg,1500))>0) {
    strncpy(resp,"ERROR",255);
    ok=false;
    msg[n]=0;
    QStringList f0=QString(msg).split(" ",QString::SkipEmptyParts);

    if(f0.at(0)=="OPEN") {
      printf("HERE1\n");
      int flags=0;
      mode_t mode=0;
      int fd=-1;
      printf("HERE2\n");
      if(f0.size()==4) {
      printf("HERE3\n");
	flags=f0.at(2).toInt(&ok);
	if(ok) {
      printf("HERE4\n");
	  mode=f0.at(3).toInt(&ok);
	  if(ok) {
      printf("HERE5\n");
            if((fd=open(f0.at(1).toUtf8(),flags,mode))>=0) {
      printf("HERE6\n");
              strncpy(resp,"OK",255);
	    }
      printf("HERE7\n");
	  }
	}
      }
      printf("\nsend_fd: %d\n",fd);
      /*
      if(send(d_unix_socket,resp,resp.size(),0)<0) {
	break;
      }
      */
      //
      // Transfer file descriptor
      //
      struct msghdr msg;
      struct iovec iov[1];
      union {
	struct cmsghdr cm;
	char control[CMSG_SPACE(sizeof(int))];
      } control_un;
      struct cmsghdr *cmptr=NULL;
      msg.msg_control=control_un.control;
      msg.msg_controllen=sizeof(control_un.control);

      cmptr=CMSG_FIRSTHDR(&msg);
      cmptr->cmsg_len=CMSG_LEN(sizeof(int));
      cmptr->cmsg_level=SOL_SOCKET;
      cmptr->cmsg_type=SCM_RIGHTS;
      *((int *) CMSG_DATA(cmptr))=fd;

      msg.msg_name=NULL;
      msg.msg_namelen=0;

      iov[0].iov_base=resp;
      iov[0].iov_len=strlen(resp);
      msg.msg_iov=iov;
      msg.msg_iovlen=1;

      if(sendmsg(d_unix_socket,&msg,0)<0) {
	break;
      }
    }

    if(f0.at(0)=="UNLINK") {
      if(f0.size()==2) {
	if((::unlink(f0.at(1).toUtf8())==0)||(errno=ENOENT)) {
	  strncpy(resp,"OK",255);
	}
      }
      if(send(d_unix_socket,resp,strlen(resp),0)<0) {
	break;
      }
    }
  }

  close(d_unix_socket);
  return RDApplication::ExitOk;
}
