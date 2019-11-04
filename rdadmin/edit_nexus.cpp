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

#include <QtGui>
#include <QHostAddress>

#include "rddb.h"
#include "rdstation.h"
#include "rdapplication.h"
#include "edit_nexus.h"

EditNexus::EditNexus(QWidget *parent)
  : QDialog(parent)
{
  setModal(true);

  setWindowTitle("RDAdmin - "+tr("Nexus Settings"));

  //
  // Get Nexus Server Settings
  //
  nexus=new RDNexus();
  nexus->nexusServer(nexus_server);
  connect(nexus,SIGNAL(ping(bool)),this,SLOT(pingData(bool)));

  //
  // Tabs
  //
  tabs=new QTabWidget();

  settings_tab=new SettingsTab(nexus);
  tabs->addTab(settings_tab,tr("Settings"));

  stations_tab=new StationsTab(nexus);
  tabs->addTab(stations_tab,tr("Stations"));

  fields_tab=new FieldsTab(nexus);
  tabs->addTab(fields_tab,tr("Field Map"));

  connect(tabs,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));

  //
  //  Ok and Cancel Buttons
  //
  QDialogButtonBox *buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  connect(buttons,SIGNAL(accepted()),this,SLOT(okData()));
  connect(buttons,SIGNAL(rejected()),this,SLOT(cancelData()));

  //
  // Create a vertical layout
  //
  QVBoxLayout *layout=new QVBoxLayout();
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->addWidget(tabs);
  layout->addWidget(buttons);
  setLayout(layout);

  //
  // Disable Stations and Fields tabs until connected
  //
  tabs->setTabEnabled(1,false);
  tabs->setTabEnabled(2,false);
}


EditNexus::~EditNexus()
{
  delete nexus;
}


#if 0
QSize EditNexus::sizeHint() const
{
  return QSize(500,150);
} 


QSizePolicy EditNexus::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}
#endif


void EditNexus::pingData(bool resp)
{
  if(resp) {
    stations_tab->LoadObjects();
    fields_tab->LoadObjects();
    tabs->setTabEnabled(1,true);
    tabs->setTabEnabled(2,true);
  }
  else {
    tabs->setTabEnabled(1,false);
    tabs->setTabEnabled(2,false);
  }
}


void EditNexus::tabChanged(int index)
{
#if 0
  //
  // Use local settings
  //
  if(index!=0&&!settings_tab->saveData()) {
    tabs->setCurrentIndex(0);
    return;
  }

  //
  // Fields Tab
  //
  if(index==1) {
  }
#endif
}

void EditNexus::okData()
{
  if(!settings_tab->saveData()) {
    return;
  }
  if (!stations_tab->saveData()) {
    return;
  }
  if (!fields_tab->saveData()) {
    return;
  }

  done(0);
}


void EditNexus::cancelData()
{
  done(-1);
}

//
// Settings Tab
//
SettingsTab::SettingsTab(RDNexus *nexus_object,QWidget *parent)
  : QWidget(parent)
{
  nexus=nexus_object;

  RDNexusServer server;
  nexus->nexusServer(server);

  enable_box=new QCheckBox();
  rd_host_combobox=new QComboBox();
  nexus_address_line=new QLineEdit();
  connect(nexus_address_line,SIGNAL(textEdited(const QString &)),this,SLOT(addressEdited(const QString &)));

  nexus_port_line=new QLineEdit();
  connect(nexus_port_line,SIGNAL(textEdited(const QString &)),this,SLOT(portEdited(const QString &)));

  connect_button=new QPushButton(tr("Connect"));
  connect(connect_button,SIGNAL(clicked()),this,SLOT(connectClicked()));

  QFormLayout *layout=new QFormLayout();

  layout->addRow("Enable MusicMaster Nexus Integration:",enable_box);
  layout->addRow("Rivendell Host System:",rd_host_combobox);
  layout->addRow("MusicMaster Nexus Server IP Address:",nexus_address_line);
  layout->addRow("MusicMaster Nexus Port:",nexus_port_line);
  layout->addRow("Test Nexus Connectivity:",connect_button);

  setLayout(layout);

  //
  // Initialize objects
  //
  enable_box->setChecked(server.enabled);
  nexus_address_line->setText(server.address);
  if(server.port) {
    nexus_port_line->setText(QString::number(server.port));
  }

  //
  // Host System
  //
  QStringList hosts=rda->station()->list();
  rd_host_combobox->insertItems(0,hosts);
  rd_host_combobox->setCurrentIndex(rd_host_combobox->findText(server.host));

  //
  // If a server hasn't been specified disable connect button
  //
  if(!AddressValid()) {
    connect_button->setEnabled(false);
  }
}


bool SettingsTab::AddressValid()
{
  int port=nexus_port_line->text().toInt();
  QHostAddress address(nexus_address_line->text());

  return(address.protocol()==QAbstractSocket::IPv4Protocol&&port>=1024&&port<=65535);
}


void SettingsTab::addressEdited(const QString &text)
{
  connect_button->setEnabled(AddressValid());
}


void SettingsTab::portEdited(const QString &text)
{
  connect_button->setEnabled(AddressValid());
}

void SettingsTab::connectClicked()
{
  RDNexusServer server;

  server.address=nexus_address_line->text();
  server.port=nexus_port_line->text().toInt();

  //
  // Set Timeout to 2 Seconds
  //
  nexus->setServer(server.address,server.port,2);

  if(!nexus->ping()) {
    QMessageBox::critical(this,tr("Connection Failed"),
      tr(QString("Could not connect to MusicMaster Nexus\n\
      Server at %1:%2\n%3")
      .arg(server.address)
      .arg(server.port)
      .arg(nexus->responseError())));
  }
  else {
    QMessageBox::information(this,tr("Connection Success"),
      tr(QString("Connected to MusicMaster Nexus\n\
      Server at %1:%2")
      .arg(server.address)
      .arg(server.port)));
  }
}

SettingsTab::~SettingsTab()
{
}

bool SettingsTab::saveData()
{
  if(!AddressValid()) {
    QMessageBox::warning(this, tr("Invalid IP Address and/or Port"),
      tr("You must specifiy a valid MusicMaster\nNexus Server IP Address and Port."));
    return false;
  }

  RDNexusServer server;

  server.enabled=enable_box->isChecked();
  server.address=nexus_address_line->text();
  server.port=nexus_port_line->text().toInt();
  server.host=rd_host_combobox->currentText();

  return nexus->setNexusServer(server);
}

//
// Stations Tab
//
StationsTab::StationsTab(RDNexus *nexus_object,QWidget *parent)
  : QWidget(parent)
{
  nexus=nexus_object;

  //
  // ComboBox of Rivendell Services
  //
  services_combobox=new QComboBox();
  connect(services_combobox,SIGNAL(activated(int)),this,SLOT(serviceActivated(int)));

  //
  // ComboBox of Nexus Stations
  //
  stations_combobox=new QComboBox();
  connect(stations_combobox,SIGNAL(activated(int)),this,SLOT(stationActivated(int)));

  //
  // ComboBox of Rivendell Groups
  //
  group_combobox=new QComboBox();
  connect(group_combobox,SIGNAL(activated(int)),this,SLOT(groupActivated(int)));

  //
  // ComboBox of Rivendell Schedule Codes
  //
  sched_code_combobox=new QComboBox();
  connect(sched_code_combobox,SIGNAL(activated(int)),this,SLOT(schedCodeActivated(int)));

  //
  // ComboBox of Nexus Categories
  //
  category_combobox=new QComboBox();
  connect(category_combobox,SIGNAL(activated(int)),this,SLOT(categoryActivated(int)));

  //
  // Auto Merge Checkbox
  //
  automerge_box=new QCheckBox();
  connect(automerge_box,SIGNAL(stateChanged(int)),this,SLOT(autoMergeSlot(int)));

  QFormLayout *layout=new QFormLayout();

  layout->addRow("Rivendell Service:",services_combobox);
  layout->addRow("MusicMaster Nexus Station:",stations_combobox);
  layout->addRow("Default Rivendell Group:",group_combobox);
  layout->addRow("Default Rivendell Scheduler Code:",sched_code_combobox);
  layout->addRow("Default MusicMaster Category:",category_combobox);
  layout->addRow("Automatically Merge Logs:",automerge_box);

  setLayout(layout);
}

StationsTab::~StationsTab()
{
}


void StationsTab::LoadObjects()
{
  QString sql;
  RDSqlQuery *q;

  //
  // Load Rivendell Services ComboBox
  //
  services_combobox->clear();
  q=new RDSqlQuery("select NAME from SERVICES");
  while(q->next()) {
    services_combobox->addItem(q->value(0).toString());
  }
  delete q;

  //
  // Load Nexus Stations ComboBox
  //
  stations_combobox->clear();
  nexus->getStations(stationlist);
  stations_combobox->addItem("--None--","");
  for(int i=0;i<stationlist.size();i++) {
    stations_combobox->addItem(stationlist.at(i).id,stationlist.at(i).id);
  }

  //
  // Load Rivendell Schedule Code ComboBox
  //
  sched_code_combobox->clear();
  sched_code_combobox->addItem("--None--","");
  q=new RDSqlQuery("select CODE from SCHED_CODES");
  while(q->next()) {
    sched_code_combobox->addItem(q->value(0).toString(),q->value(0).toString());
  }
  delete q;

  initStationMap();

  setMapping(services_combobox->itemText(0));
}


void StationsTab::serviceActivated(int index)
{
  setMapping(services_combobox->itemText(index));
}

void StationsTab::stationActivated(int index)
{
  stationmaplist[services_combobox->currentIndex()].station=stations_combobox->itemData(index).toString();
}

void StationsTab::groupActivated(int index)
{
}

void StationsTab::schedCodeActivated(int index)
{
  stationmaplist[services_combobox->currentIndex()].schedcode=sched_code_combobox->itemData(index).toString();
}

void StationsTab::categoryActivated(int index)
{
}

void StationsTab::autoMergeSlot(int state)
{
  stationmaplist[services_combobox->currentIndex()].automerge=state;
}

//
// Initialize Nexus Station Map
//
void StationsTab::initStationMap()
{
  RDNexusStationMapList newmaplist;
  RDNexusStationMap stationmap;
  int i;

  nexus->stationMapList(stationmaplist);

  for(int s=0;s<services_combobox->count();s++) {
    if((i=stationMapIndex(services_combobox->itemText(s)))!=-1) {
      newmaplist.insert(s,stationmaplist[i]);
    }
    else {
      stationmap.service=services_combobox->itemText(s);
      stationmap.station="";
      stationmap.automerge=false;
      newmaplist.insert(s,stationmap);
    }
  }

  stationmaplist=newmaplist;
}

//
// Returns stationmaplist index for matching service
// -1 if no match
//
int StationsTab::stationMapIndex(QString service)
{
  for(int i=0;i<stationmaplist.size();i++) {
    if(stationmaplist.at(i).service==service) {
      return i;
    }
  }

  return(-1);
}

//
// Set objects for the selected Rivendell service
//
void StationsTab::setMapping(QString service)
{
  stations_combobox->setCurrentIndex(0);
  group_combobox->setCurrentIndex(0);
  sched_code_combobox->setCurrentIndex(0);
  category_combobox->setCurrentIndex(0);
  automerge_box->setChecked(false);

  for(int i=0;i<stationmaplist.size();i++) {
    if(stationmaplist.at(i).service==service) {
      int index=stations_combobox->findText(stationmaplist.at(i).station);
      if(index>0) {
        stations_combobox->setCurrentIndex(index);
        group_combobox->setCurrentIndex(group_combobox->findData(stationmaplist.at(i).group));
        sched_code_combobox->setCurrentIndex(sched_code_combobox->findData(stationmaplist.at(i).schedcode));
        category_combobox->setCurrentIndex(category_combobox->findData(stationmaplist.at(i).category));
        automerge_box->setChecked(stationmaplist.at(i).automerge);
      }
    }
  }
}

bool StationsTab::saveData()
{
  for(int station=0;station<stationmaplist.size();station++) {
    nexus->saveStationMap(stationmaplist.at(station));
  }
  return true;
}


//
// Fields Tab
//
FieldsTab::FieldsTab(RDNexus *nexus_object,QWidget *parent)
  : QWidget(parent)
{
  nexus=nexus_object;

  //
  // ComboBox of MusicMaster Stations
  //
  stations_combobox=new QComboBox();
  connect(stations_combobox,SIGNAL(activated(int)),this,SLOT(stationActivated(int)));

  // Initialize ComboBoxes
  comboboxmaps["SONGID"]=new QComboBox;
  comboboxmaps["NUMBER"]=new QComboBox;
  comboboxmaps["ARTIST"]=new QComboBox;
  comboboxmaps["ARTIST_KEYWORDS"]=new QComboBox;
  comboboxmaps["TITLE"]=new QComboBox;
  comboboxmaps["TITLE_KEYWORDS"]=new QComboBox;
  comboboxmaps["YEAR"]=new QComboBox;
  comboboxmaps["NOTES"]=new QComboBox;
  comboboxmaps["MINIMUM_TALK_LENGTH"]=new QComboBox;
  comboboxmaps["AVERAGE_SEGUE_LENGTH"]=new QComboBox;
  comboboxmaps["FORCED_LENGTH"]=new QComboBox;
  comboboxmaps["CATEGORY"]=new QComboBox;

  QGridLayout *layout=new QGridLayout();

  int row=0;

  layout->addWidget(new QLabel("MusicMaster Station:"),row,0,Qt::AlignRight);
  layout->addWidget(stations_combobox,row,1);
  layout->addWidget(new QLabel("MusicMaster Station to Configure Field Map"),row,2);
  layout->setRowMinimumHeight(row++, 60);

  layout->addWidget(new QLabel("MusicMaster Song ID:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["SONGID"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Song ID is Stored"),row++,2);

  layout->addWidget(new QLabel("MusicMaster Category:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["CATEGORY"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Category is Stored"),row++,2);

  layout->addWidget(new QLabel("Rivendell Cart Number:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["NUMBER"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Cart Number is Stored"),row++,2);

  layout->addWidget(new QLabel("Artist:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["ARTIST"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Artist is Stored"),row++,2);

  layout->addWidget(new QLabel("Artist Keywords:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["ARTIST_KEYWORDS"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Artist Keywords is Stored"),row++,2);

  layout->addWidget(new QLabel("Title:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["TITLE"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Title is Stored"),row++,2);

  layout->addWidget(new QLabel("Title Keywords:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["TITLE_KEYWORDS"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Title Keywords is Stored"),row++,2);

  layout->addWidget(new QLabel("Length:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["FORCED_LENGTH"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Run Time is Stored"),row++,2);

  layout->addWidget(new QLabel("Talk Length:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["MINIMUM_TALK_LENGTH"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Intro is Stored"),row++,2);

  layout->addWidget(new QLabel("Segue Length:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["AVERAGE_SEGUE_LENGTH"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Ending is Stored"),row++,2);

  layout->addWidget(new QLabel("Year:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["YEAR"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Year is Stored"),row++,2);

  layout->addWidget(new QLabel("Notes:"),row,0,Qt::AlignRight);
  layout->addWidget(comboboxmaps["NOTES"],row,1);
  layout->addWidget(new QLabel("MusicMaster Field where Notes is Stored"),row++,2);

  setLayout(layout);

  //
  // Set Signal Mapper for ComboBoxes
  //
  QSignalMapper *combomapper = new QSignalMapper(this);

  QStringList keys;
  keys=comboboxmaps.keys();

  for(int i=0;i<keys.size();i++) {
    QString rd_name=keys.at(i);
    QComboBox *combobox=comboboxmaps[rd_name];

    connect(combobox,SIGNAL(activated(int)),combomapper,SLOT(map()));
    combomapper->setMapping(combobox,rd_name);

    //
    // Set model for sorting
    //
    QSortFilterProxyModel *proxy=new QSortFilterProxyModel(combobox);
    proxy->setSourceModel(combobox->model());
    combobox->model()->setParent(proxy);
    combobox->setModel(proxy);

    //
    // Set minimum width
    //
    combobox->setMinimumContentsLength(20);
  }

  connect(combomapper,SIGNAL(mapped(const QString &)),this,SLOT(fieldChanged(const QString &)));
}

FieldsTab::~FieldsTab()
{
}


void FieldsTab::LoadObjects()
{
  //
  // Load Nexus Station Field Mapping
  //
  stations_combobox->clear();
  nexus->getStations(stationlist);
  for(int i=0;i<stationlist.size();i++) {
    stations_combobox->addItem(stationlist.at(i).id);

    nexus->setStation(stationlist.at(i).id);

    RDNexusFieldList fieldlist;
    nexus->getFieldList(fieldlist);
    stationfieldlist.append(fieldlist);

    RDNexusFieldMapList fieldmaplist;
    nexus->getFieldMapList(fieldmaplist);

    //
    // Make sure all fields are present
    //
    RDNexusFieldMap fieldmap;
    fieldmap.clear();

    QStringList rd_names;
    rd_names << "NUMBER";
    rd_names << "ARTIST";
    rd_names << "ARTIST_KEYWORDS";
    rd_names << "TITLE";
    rd_names << "TITLE_KEYWORDS";
    rd_names << "YEAR";
    rd_names << "NOTES";
    rd_names << "GROUP_NAME";
    rd_names << "FORCED_LENGTH";
    rd_names << "MINIMUM_TALK_LENGTH";
    rd_names << "AVERAGE_SEGUE_LENGTH";
    rd_names << "SONGID";
    rd_names << "CATEGORY";

    for(int r=0;r<rd_names.size();r++) {
      if(!HasRdName(fieldmaplist,rd_names[r])) {
        fieldmap.rd_name=rd_names[r];
        fieldmaplist.append(fieldmap);
      }
    }

    stationfieldmaplist.append(fieldmaplist);
  }

  SetMapping(0);
}


void FieldsTab::stationActivated(int index)
{
  SetMapping(index);
}

void FieldsTab::fieldChanged(const QString &field)
{
  int station=stations_combobox->currentIndex();

  for(int i=0;i<stationfieldmaplist.at(station).size();i++) {
    if(stationfieldmaplist[station][i].rd_name==field) {
      stationfieldmaplist[station][i].nexus_name=comboboxmaps[field]->currentText();
      stationfieldmaplist[station][i].nexus_id=comboboxmaps[field]->itemData(comboboxmaps[field]->currentIndex()).toInt();
    }
  }
}

bool FieldsTab::loadData(RDNexus *nexus,RDNexusStationMap &station)
{
  return true;
}

bool FieldsTab::saveData()
{
  for(int station=0;station<stationfieldmaplist.size();station++) {
    nexus->saveFieldMap(stations_combobox->itemText(station),stationfieldmaplist.at(station));
  }
  return true;
}

//
// Set initial objects for the selected MusicMaster Station
//
void FieldsTab::SetMapping(int station)
{
  RDNexusFieldList fieldlist;
  RDNexusFieldMapList fieldmaplist;
  QStringList keys;

  fieldlist=stationfieldlist.at(station);
  fieldmaplist=stationfieldmaplist.at(station);

  keys=comboboxmaps.keys();

  //
  // Load all MusicMaster Fields
  //
  for(int key=0;key<keys.size();key++) {
    QString rd_name=keys.at(key);
    QComboBox *combobox=comboboxmaps[rd_name];

    combobox->clear();
    combobox->addItem("",0);
    for(int i=0;i<fieldlist.size();i++) {
      combobox->addItem(fieldlist.at(i).name,fieldlist.at(i).id);
    }

    //
    // Sort the fields
    //
    combobox->model()->sort(0);

    //
    // Set Active Field
    //
    for(int i=0;i<stationfieldmaplist.at(station).size();i++) {
      if(stationfieldmaplist[station][i].rd_name==rd_name) {
        int c;

        if((c=combobox->findText(stationfieldmaplist[station][i].nexus_name))!=-1) {
          combobox->setCurrentIndex(c);
        }
        else {
          combobox->setCurrentIndex(0);
        }
      }
    }
  }
}

bool FieldsTab::HasRdName(RDNexusFieldMapList &fieldmaplist, const QString &rd_name)
{
  for(int i=0;i<fieldmaplist.size();i++) {
    if(fieldmaplist.at(i).rd_name==rd_name) {
      return true;
    }
  }

  return false;
}


