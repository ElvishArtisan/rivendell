// rdlogedit_conf.h
//
// Abstract RDHotkeys Configuration
//

#ifndef RDHOTKEYS_H
#define RDHOTKEYS_H

#include <qsqldatabase.h>


class RDHotkeys
{
 public:
  RDHotkeys(const QString &station,const QString &module);
  QString station() const;
  int inputCard() const;
  QString GetRowLabel(const QString &station,const QString &value,const QString &module) const;

 private:
  QString station_hotkeys;
  QString module_name;
};


#endif 
