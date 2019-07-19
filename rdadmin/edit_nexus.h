// edit_nexus.cpp
//
// Edit Nexus Configuration for Host
//
//   Patrick Linstruth <patrick@deltecent.com>
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_NEXUS_H
#define EDIT_NEXUS_H

#include <QDialog>
#include <QWidget>
#include <QTabWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

#include "rdstation.h"
#include "rdnexus.h"

class SettingsTab : public QWidget
{
  Q_OBJECT

  public:
    SettingsTab(RDNexus *nexus,QWidget *parent=0);
    ~SettingsTab();
    bool saveData();

  private slots:
    void addressEdited(const QString &address);
    void portEdited(const QString &address);
    void connectClicked();

  private:
    RDNexus *nexus;
    QLabel *enable_label;
    QCheckBox *enable_box;
    QLabel *nexus_address_label;
    QLineEdit *nexus_address_line;
    QLabel *nexus_port_label;
    QLineEdit *nexus_port_line;
    QComboBox *rd_host_combobox;
    QPushButton *connect_button;
    bool AddressValid();
};

class StationsTab : public QWidget
{
  Q_OBJECT

  public:
    StationsTab(RDNexus *nexus_object,QWidget *parent=0);
    ~StationsTab();
    void LoadObjects();
    bool saveData();

  private slots:
    void serviceActivated(int index);
    void stationActivated(int index);
    void groupActivated(int index);
    void schedCodeActivated(int index);
    void categoryActivated(int index);
    void autoMergeSlot(int state);

  private:
    RDNexus *nexus;
    RDNexusStationMapList stationmaplist;
    RDNexusStationList stationlist;
    QComboBox *stations_combobox;
    QComboBox *services_combobox;
    QComboBox *group_combobox;
    QComboBox *sched_code_combobox;
    QComboBox *category_combobox;
    QCheckBox *automerge_box;
    void setMapping(QString service);
    void initStationMap();
    int stationMapIndex(QString service);
};


class FieldsTab : public QWidget
{
  Q_OBJECT

  public:
    FieldsTab(RDNexus *nexus_object,QWidget *parent=0);
    ~FieldsTab();
    void LoadObjects();
    bool loadData(RDNexus *nexus,RDNexusStationMap &station);
    bool saveData();

  private slots:
    void stationActivated(int index);
    void fieldChanged(const QString &field);

  private:
    RDNexus *nexus;
    RDNexusStationList stationlist;
    RDNexusFieldList fieldlist;
    QMap<QString,QComboBox *> comboboxmaps;
    QList<RDNexusFieldList> stationfieldlist;
    QList<RDNexusFieldMapList> stationfieldmaplist;
    QComboBox *stations_combobox;
    QLabel *stations_label;
    void SetMapping(int index);
    bool HasRdName(RDNexusFieldMapList &fieldmaplist, const QString &rd_name);
};

class EditNexus : public QDialog
{
  Q_OBJECT
  public:
   EditNexus(QWidget *parent=0);
   ~EditNexus();
#if 0
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;
#endif
    QTabWidget *tabs;
    SettingsTab *settings_tab;
    StationsTab *stations_tab;
    FieldsTab *fields_tab;

  private slots:
   void pingData(bool resp);
   void tabChanged(int index);
   void okData();
   void cancelData();

 private:
    RDNexus *nexus;
    RDNexusServer nexus_server;
    QFont font;
    QFont normal_font;
};


#endif  // EDIT_NEXUS_H
