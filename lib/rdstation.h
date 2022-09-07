// rdstation.h
//
// Abstract a Rivendell Workstation
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSTATION_H
#define RDSTATION_H

#include <QHostAddress>

#include <rdconfig.h>

class RDStation
{
 public:
  enum AudioDriver {None=0,Hpi=1,Jack=2,Alsa=3};
  enum Capability {HaveOggenc=0,HaveOgg123=1,HaveFlac=2,
		   HaveLame=3,HaveMpg321=4,HaveTwoLame=5,HaveMp4Decode=6};
  enum FilterMode {FilterSynchronous=0,FilterAsynchronous=1};
  RDStation(const QString &name,bool create=false);
  ~RDStation();
  QString name() const;
  bool exists() const;
  QString shortName() const;
  void setShortName(const QString &str) const;
  QString description() const;
  void setDescription(QString path) const;
  QString userName() const;
  void setUserName(QString name) const;
  QString defaultName() const;
  void setDefaultName(QString name) const;
  QHostAddress address() const;
  void setAddress(QHostAddress addr) const;
  QHostAddress httpAddress(RDConfig *config) const;
  QString httpStation() const;
  void setHttpStation(const QString &str);
  QHostAddress caeAddress(RDConfig *config) const;
  QString caeStation() const;
  void setCaeStation(const QString &str);
  QString webServiceUrl(RDConfig *config) const;
  int timeOffset();
  void setTimeOffset(int msecs);
  unsigned heartbeatCart() const;
  void setHeartbeatCart(unsigned cartnum) const;
  unsigned heartbeatInterval() const;
  void setHeartbeatInterval(unsigned interval) const;
  unsigned startupCart() const;
  void setStartupCart(unsigned cartnum) const;
  QString reportEditorPath() const;
  void setReportEditorPath(const QString &cmd);
  QString browserPath() const;
  void setBrowserPath(const QString &cmd) const;
  QString sshIdentityFile() const;
  void setSshIdentityFile(const QString &str) const;
  RDStation::FilterMode filterMode() const;
  void setFilterMode(RDStation::FilterMode mode) const;
  bool startJack() const;
  void setStartJack(bool state) const;
  QString jackServerName() const;
  void setJackServerName(const QString &str) const;
  QString jackCommandLine() const;
  void setJackCommandLine(const QString &str) const;
  int jackPorts() const;
  void setJackPorts(int ports) const;
  int cueCard() const;
  void setCueCard(int card);
  int cuePort() const;
  void setCuePort(int port);
  unsigned cueStartCart() const;
  void setCueStartCart(unsigned cartnum) const;
  unsigned cueStopCart() const;
  void setCueStopCart(unsigned cartnum) const;
  int cartSlotColumns() const;
  void setCartSlotColumns(int cols);
  int cartSlotRows() const;
  void setCartSlotRows(int rows);
  bool enableDragdrop() const;
  void setEnableDragdrop(bool state);
  bool enforcePanelSetup() const;
  void setEnforcePanelSetup(bool state);
  bool systemMaint() const;
  void setSystemMaint(bool state) const;
  bool scanned() const;
  void setScanned(bool state) const;
  bool haveCapability(Capability cap) const;
  void setHaveCapability(Capability cap,bool state) const;
  int cards() const;
  RDStation::AudioDriver cardDriver(int cardnum) const;
  void setCardDriver(int cardnum,AudioDriver driver) const;
  QString driverVersion(AudioDriver driver) const;
  void setDriverVersion(AudioDriver driver,QString ver) const;
  QString cardName(int cardnum) const;
  void setCardName(int cardnum,QString name) const;
  int cardInputs(int cardnum) const;
  void setCardInputs(int cardnum,int inputs) const;
  int cardOutputs(int cardnum) const;
  void setCardOutputs(int cardnum,int outputs) const;
  static bool create(const QString &name,QString *err_msg,
		     const QString &exemplar="",
		     const QHostAddress &hostaddr=QHostAddress());
  static void remove(const QString &name);
  static QString audioDriverText(AudioDriver dvr);

 private:
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,unsigned value) const;
  void SetRow(const QString &param,bool value) const;
  QString station_name;
  int time_offset;
  bool time_offset_valid;
};


#endif 
