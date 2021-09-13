// rdgroup.h
//
// Abstract a Rivendell Service
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


#include <rdcart.h>

#ifndef RDGROUP_H
#define RDGROUP_H

class RDGroup
{
 public:
  enum ExportType {None=0,Traffic=1,Music=2};
  RDGroup(QString name,bool create=false);
  QString name() const;
  bool exists() const;
  QString description() const;
  void setDescription(const QString &desc) const;
  RDCart::Type defaultCartType() const;
  void setDefaultCartType(RDCart::Type type) const;
  unsigned defaultLowCart() const;
  void setDefaultLowCart(unsigned cartnum) const;
  unsigned defaultHighCart() const;
  void setDefaultHighCart(unsigned cartnum) const;
  int defaultCutLife() const;
  void setDefaultCutLife(int days) const;
  int cutShelflife() const;
  void setCutShelflife(int days) const;
  bool deleteEmptyCarts() const;
  void setDeleteEmptyCarts(bool state) const;
  QString defaultTitle() const;
  void setDefaultTitle(const QString &str);
  QString generateTitle(const QString &pathname);
  bool enforceCartRange() const;
  void setEnforceCartRange(bool state) const;
  bool exportReport(ExportType type) const;
  void setExportReport(ExportType type,bool state) const;
  QColor color() const;
  void setColor(const QColor &color);
  QString notifyEmailAddress() const;
  void setNotifyEmailAddress(const QString &addr) const;
  unsigned nextFreeCart(unsigned startcart=0) const;
  int freeCartQuantity() const;
  bool reserveCarts(QList<unsigned> *cart_nums,const QString &station_name,
		    RDCart::Type type,int quan) const;
  bool cartNumberValid(unsigned cartnum) const;
  QString xml() const;
  static bool create(const QString &name,bool allow_all_users,
		     bool allow_all_services,QString *err_msg);
  static bool remove(const QString &name,QString *err_msg);

 private:
  unsigned GetNextFreeCart(unsigned startcart) const;
  bool ReserveCart(const QString &station_name,RDCart::Type type,
		   unsigned cart_num) const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,unsigned value) const;
  void SetRow(const QString &param,const QString &value) const;
  QString ReportField(ExportType type) const;
  QString group_name;
};


#endif  // RDGROUP_H
