// rdmacro.h
//
// A container class for a Rivendell Macro Language Command
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmacro.h,v 1.37.4.2.2.1 2014/05/22 19:37:44 cvs Exp $
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

#ifndef RDMACRO_H
#define RDMACRO_H

#include <qstring.h>
#include <qhostaddress.h>
#include <qvariant.h>

#include <rd.h>

#ifdef WIN32
#include <rdwin32.h>
#endif  // WIN32


class RDMacro
{
 public:
  enum Command {AL=0x414C,BO=0x424F,CC=0x4343,CL=0x434C,DB=0x4442,DL=0x444C,
		DP=0x4450,DS=0x4453,DX=0x4458,EX=0x4558,FS=0x4653,GE=0x4745,
		GI=0x4749,GO=0x474F,JC=0x4A43,JD=0x4A44,LB=0x4C42,LC=0x4C43,
		LL=0x4C4C,LO=0x4C4F,MB=0x4D42,MD=0x4D44,MN=0x4D4E,MT=0x4D54,
		NN=0x4E4E,PB=0x5042,PC=0x5043,PD=0x5044,PE=0x5045,PL=0x504C,
		PM=0x504D,PN=0x504E,PP=0x5050,PS=0x5053,PT=0x5054,PU=0x5055,
		PW=0x5057,PX=0x5058,RL=0x524C,RN=0x524E,RS=0x5253,RR=0x5252,
		SA=0x5341,SC=0x5343,SD=0x5344,SG=0x5347,SI=0x5349,SL=0x534C,
		SN=0x534e,SO=0x534F,SP=0x5350,SR=0x5352,ST=0x5354,SX=0x5358,
		SY=0x5359,SZ=0x535A,TA=0x5441,UO=0x554F};
  enum Role {Invalid=0,Cmd=1,Reply=2};
  RDMacro();
  RDMacro::Role role() const;
  void setRole(RDMacro::Role role);
  RDMacro::Command command() const;
  void setCommand(RDMacro::Command cmd);
  QHostAddress address() const;
  void setAddress(QHostAddress addr);
  Q_UINT16 port() const;
  void setPort(Q_UINT16 port);
  bool echoRequested() const;
  void setEchoRequested(bool state);
  QVariant arg(int n) const;
  void setArg(int n,QVariant arg);
  int argQuantity() const;
  void setArgQuantity(int n);
  void acknowledge(bool state);
  QString rollupArgs(int n);
  bool parseString(const char *str,int n);
  int generateString(char *str,int n) const;
  unsigned length() const;
  void clear();

 private:
  RDMacro::Role rml_role;
  RDMacro::Command rml_cmd;
  QHostAddress rml_addr;
  Q_UINT16 rml_port;
  bool rml_echo_requested;
  QVariant rml_arg[RD_RML_MAX_ARGS];
  int rml_arg_quantity;
};


#endif  // RDMACRO_H
