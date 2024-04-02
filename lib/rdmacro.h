// rdmacro.h
//
// A container class for a Rivendell Macro Language Command
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdint.h>

#include <vector>

#include <QHostAddress>
#include <QString>
#include <QStringList>
#include <QVariant>

#include <rd.h>

class RDMacro
{
 public:
  enum Command {AG=0x4147,AL=0x414C,BO=0x424F,CC=0x4343,CE=0x4345,CL=0x434C,
		CP=0x4350,DL=0x444C,DP=0x4450,DS=0x4453,DX=0x4458,EX=0x4558,
		FS=0x4653,GE=0x4745,GI=0x4749,GO=0x474F,JC=0x4A43,JD=0x4A44,
		JZ=0x4A5A,LB=0x4C42,LC=0x4C43,LL=0x4C4C,LM=0x4C4D,LO=0x4C4F,
		MB=0x4D42,MD=0x4D44,MN=0x4D4E,MT=0x4D54,NN=0x4E4E,PB=0x5042,
		PC=0x5043,PD=0x5044,PE=0x5045,PL=0x504C,PM=0x504D,PN=0x504E,
		PP=0x5050,PS=0x5053,PT=0x5054,PU=0x5055,PW=0x5057,PX=0x5058,
		RL=0x524C,RN=0x524E,RS=0x5253,RR=0x5252,SA=0x5341,SC=0x5343,
		SD=0x5344,SG=0x5347,SI=0x5349,SL=0x534C,SN=0x534e,SO=0x534F,
		SP=0x5350,SR=0x5352,ST=0x5354,SX=0x5358,SY=0x5359,SZ=0x535A,
		TA=0x5441,UO=0x554F};
  enum Role {Invalid=0,Cmd=1,Reply=2};
  RDMacro();
  RDMacro::Role role() const;
  void setRole(RDMacro::Role role);
  RDMacro::Command command() const;
  void setCommand(RDMacro::Command cmd);
  void setCommand(const QString &str);
  QHostAddress address() const;
  void setAddress(QHostAddress addr);
  uint16_t port() const;
  void setPort(uint16_t port);
  bool echoRequested() const;
  void setEchoRequested(bool state);
  QString arg(int n) const;
  int addArg(const QString &arg);
  int addArg(int arg);
  int addArg(unsigned arg);
  void setArg(int n,const QString &arg);
  void setArg(int n,int arg);
  void setArg(int n,unsigned arg);
  int argQuantity() const;
  void acknowledge(bool state);
  QString rollupArgs(int n);
  QString toString() const;
  unsigned length() const;
  bool isNull() const;
  void clear();
  static RDMacro fromString(const QString &str,Role role=RDMacro::Cmd);

 private:
  RDMacro::Role rml_role;
  RDMacro::Command rml_cmd;
  QHostAddress rml_addr;
  uint16_t rml_port;
  bool rml_echo_requested;
  QStringList rml_args;
};


#endif  // RDMACRO_H
