
#ifndef DBUSMODULE_H
#define DBUSMODULE_H



#define DBUS_SERVICE_NAME org.rivendell.dbus_api;


#include "dbuslink.h"
#include "dbuslink_adaptor.h"
#include "dbusfield.h"
#include "dbusmethod.h"

#include "dbusmoduleglobal.h"

#include <QStringBuilder>
#include <QString>
#include <iterator>
#include <map>
#include <QtDBus/QtDBus>


class DbusLinkPartVirtual : public DbusLink
{
    public:
        DbusLinkPartVirtual(QObject *obj): DbusLink{obj}{

        }
};


/**
Name: DbusModule
Description: Core Module for add method and field with a path
Example: DbusModule *module = new DbusModule("/RdAirPlay");
*/
class DbusModule : public DbusModuleGlobal
{
    public:
        DbusModule(QString path){
            path_a= path;
        }
        void link(DbusLink *db){
            new DbusLinkAdaptor(db);

            dblink = db;
            QDBusConnection::sessionBus().registerObject(path_a,dblink);

            QDBusConnection::sessionBus().registerService("org.rivendell.dbus_api");
        }
        void addPath(QString path){
            QDBusConnection::sessionBus().registerObject(path,dblink);
        }
        void addDBusField(DbusField* field){
            fields.insert(std::pair<QString,DbusField*>(field->getName(),field));
        }
        void addDBusMethod(QString name,DbusMethodInterface* field) {
            methods.insert(std::pair<QString,DbusMethodInterface*>(name,field));
        }
        DbusField* getField(QString name){
            return fields.at(name);
        }
        DbusMethodInterface* getMethod(QString name){
            return methods.at(name);
        }
        bool hasField(QString name){
            return fields.find(name) != fields.end();
        }
        bool hasMethod(QString name){
            return methods.find(name) != methods.end();
        }

        QString listField(){


          typedef std::map<QString,DbusField*>::iterator it_type;
          QString text = "";
          for(it_type iterator = fields.begin(); iterator != fields.end(); iterator++) {

            text=text % ","% iterator->first;
            // iterator->first = key
            // iterator->second = value
            // Repeat if you also want to iterate through the second map.
          }
          return text;
        }
        QString listMethod(){
          typedef std::map<QString,DbusMethodInterface*>::iterator it_type;
          QString text = "";
          for(it_type iterator = methods.begin(); iterator != methods.end(); iterator++) {

            text=text % ","% iterator->first;
            // iterator->first = key
            // iterator->second = value
            // Repeat if you also want to iterate through the second map.
          }
          return text;
        }
    private:
        QString path_a;
        DbusLink *dblink;
        std::map<QString,DbusField*> fields;
        std::map<QString,DbusMethodInterface*> methods;

};

/**
Name: DbusLinkPart
Description: Class is used for link method and field with dbus systems
Example:

DbusModule *module = new DbusModule("/RdAirPlay");

// DbusField and DbusMethod called

QObject obj;
DbusLinkPart *part = new DbusLinkPart(obj);
part->setModule(module);
module->link(part);

*/
class DbusLinkPart : public DbusLinkPartVirtual
{
    public:
        DbusLinkPart(QObject *obj): DbusLinkPartVirtual{obj}{};
    public slots:
        void setModule(DbusModule *module){core = module;}


        QString listField() override{
          return core->listField();
        }
        QString listMethod() override{
          return core->listMethod();
        }

        bool hasField(QString name) override{
            return core->hasField(name);
        }
        bool hasMethod(QString name) override{
            return core->hasMethod(name);
        }

        int getFieldInt(QString name) override{
            if (core->hasField(name)==false){return -1;}
            return core->getField(name)->getInt();
        };
        double getFieldDouble(QString name) override{
            if (core->hasField(name)==false){return 2.0;}
            return core->getField(name)->getDouble();
        }
        QString getFieldString(QString name) override{
            if (core->hasField(name)==false){return QString("nodef");}
            return core->getField(name)->getString();
        }
        QTime getFieldTime(QString name) override{
            if (core->hasField(name)==false){return QTime();}
            return core->getField(name)->getTime();
        }
        QDate getFieldDate(QString name) override{
             if (core->hasField(name)==false){return QDate();}
            return core->getField(name)->getDate();
        };
        bool getFieldBoolean(QString name)override{
            if (core->hasField(name)==false){return false;}
            return core->getField(name)->getBoolean();
        };


        QString getFieldLabel(QString name) override{
          if (core->hasField(name)==false){return "nodef";}
          return core->getField(name)->getLabel();
          return "";
        };




        // METHODS

        void callMethod_Int(QString name,int a)override{
           if (core->hasMethod(name)==false){return;}
           DbusMethod<void(int),void,int> *method = reinterpret_cast<DbusMethod<void(int),void,int>*>(core->getMethod(name));
           method->call(a);
        };
        void callMethod_Boolean(QString name,bool a)override{
           if (core->hasMethod(name)==false){return;}
           DbusMethod<void(bool),void,bool> *method = reinterpret_cast<DbusMethod<void(bool),void,bool>*>(core->getMethod(name));
           method->call(a);
        };
        void callMethod_Double(QString name,double a)override{
           if (core->hasMethod(name)==false){return;}
           DbusMethod<void(double),void,double> *method = reinterpret_cast<DbusMethod<void(double),void,double>*>(core->getMethod(name));
           method->call(a);
        };
        void callMethod_String(QString name,QString a)override{
           if (core->hasMethod(name)==false){return;}
           DbusMethod<void(QString),void,QString> *method = reinterpret_cast<DbusMethod<void(QString),void,QString>*>(core->getMethod(name));
           method->call(a);
        };
        void callMethod_Date(QString name,QDate a)override{
           if (core->hasMethod(name)==false){return;}
           DbusMethod<void(QDate),void,QDate> *method = reinterpret_cast<DbusMethod<void(QDate),void,QDate>*>(core->getMethod(name));
           method->call(a);
        };
        void callMethod_Time(QString name,QTime a)override{
           if (core->hasMethod(name)==false){return;}
           DbusMethod<void(QTime),void,QTime> *method = reinterpret_cast<DbusMethod<void(QTime),void,QTime>*>(core->getMethod(name));
           method->call(a);
        };

      void callMethod(QString name)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(),void> *method = reinterpret_cast<DbusMethod<void(),void>*>(core->getMethod(name));
         method->call();
      };

      void callMethod_Int_Int(QString name,int a,int b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(int,int),void,int,int> *method = reinterpret_cast<DbusMethod<void(int,int),void,int,int>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Int_Double(QString name,int a,double b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(int,double),void,int,double> *method = reinterpret_cast<DbusMethod<void(int,double),void,int,double>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Int_Boolean(QString name,int a,bool b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(int,bool),void,int,bool> *method = reinterpret_cast<DbusMethod<void(int,bool),void,int,bool>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Int_Date(QString name,int a,QDate b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(int,QDate),void,int,QDate> *method = reinterpret_cast<DbusMethod<void(int,QDate),void,int,QDate>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Int_String(QString name,int a,QString b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(int,QString),void,int,QString> *method = reinterpret_cast<DbusMethod<void(int,QString),void,int,QString>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Int_Time(QString name,int a,QTime b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(int,QTime),void,int,QTime> *method = reinterpret_cast<DbusMethod<void(int,QTime),void,int,QTime>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Double_Int(QString name,double a,int b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(double,int),void,double,int> *method = reinterpret_cast<DbusMethod<void(double,int),void,double,int>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Double_Double(QString name,double a,double b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(double,double),void,double,double> *method = reinterpret_cast<DbusMethod<void(double,double),void,double,double>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Double_Boolean(QString name,double a,bool b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(double,bool),void,double,bool> *method = reinterpret_cast<DbusMethod<void(double,bool),void,double,bool>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Double_Date(QString name,double a,QDate b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(double,QDate),void,double,QDate> *method = reinterpret_cast<DbusMethod<void(double,QDate),void,double,QDate>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Double_String(QString name,double a,QString b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(double,QString),void,double,QString> *method = reinterpret_cast<DbusMethod<void(double,QString),void,double,QString>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Double_Time(QString name,double a,QTime b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(double,QTime),void,double,QTime> *method = reinterpret_cast<DbusMethod<void(double,QTime),void,double,QTime>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Boolean_Int(QString name,bool a,int b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(bool,int),void,bool,int> *method = reinterpret_cast<DbusMethod<void(bool,int),void,bool,int>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Boolean_Double(QString name,bool a,double b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(bool,double),void,bool,double> *method = reinterpret_cast<DbusMethod<void(bool,double),void,bool,double>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Boolean_Boolean(QString name,bool a,bool b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(bool,bool),void,bool,bool> *method = reinterpret_cast<DbusMethod<void(bool,bool),void,bool,bool>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Boolean_Date(QString name,bool a,QDate b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(bool,QDate),void,bool,QDate> *method = reinterpret_cast<DbusMethod<void(bool,QDate),void,bool,QDate>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Boolean_String(QString name,bool a,QString b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(bool,QString),void,bool,QString> *method = reinterpret_cast<DbusMethod<void(bool,QString),void,bool,QString>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Boolean_Time(QString name,bool a,QTime b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(bool,QTime),void,bool,QTime> *method = reinterpret_cast<DbusMethod<void(bool,QTime),void,bool,QTime>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Date_Int(QString name,QDate a,int b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QDate,int),void,QDate,int> *method = reinterpret_cast<DbusMethod<void(QDate,int),void,QDate,int>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Date_Double(QString name,QDate a,double b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QDate,double),void,QDate,double> *method = reinterpret_cast<DbusMethod<void(QDate,double),void,QDate,double>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Date_Boolean(QString name,QDate a,bool b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QDate,bool),void,QDate,bool> *method = reinterpret_cast<DbusMethod<void(QDate,bool),void,QDate,bool>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Date_Date(QString name,QDate a,QDate b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QDate,QDate),void,QDate,QDate> *method = reinterpret_cast<DbusMethod<void(QDate,QDate),void,QDate,QDate>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Date_String(QString name,QDate a,QString b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QDate,QString),void,QDate,QString> *method = reinterpret_cast<DbusMethod<void(QDate,QString),void,QDate,QString>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Date_Time(QString name,QDate a,QTime b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QDate,QTime),void,QDate,QTime> *method = reinterpret_cast<DbusMethod<void(QDate,QTime),void,QDate,QTime>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_String_Int(QString name,QString a,int b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QString,int),void,QString,int> *method = reinterpret_cast<DbusMethod<void(QString,int),void,QString,int>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_String_Double(QString name,QString a,double b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QString,double),void,QString,double> *method = reinterpret_cast<DbusMethod<void(QString,double),void,QString,double>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_String_Boolean(QString name,QString a,bool b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QString,bool),void,QString,bool> *method = reinterpret_cast<DbusMethod<void(QString,bool),void,QString,bool>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_String_Date(QString name,QString a,QDate b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QString,QDate),void,QString,QDate> *method = reinterpret_cast<DbusMethod<void(QString,QDate),void,QString,QDate>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_String_String(QString name,QString a,QString b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QString,QString),void,QString,QString> *method = reinterpret_cast<DbusMethod<void(QString,QString),void,QString,QString>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_String_Time(QString name,QString a,QTime b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QString,QTime),void,QString,QTime> *method = reinterpret_cast<DbusMethod<void(QString,QTime),void,QString,QTime>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Time_Int(QString name,QTime a,int b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QTime,int),void,QTime,int> *method = reinterpret_cast<DbusMethod<void(QTime,int),void,QTime,int>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Time_Double(QString name,QTime a,double b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QTime,double),void,QTime,double> *method = reinterpret_cast<DbusMethod<void(QTime,double),void,QTime,double>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Time_Boolean(QString name,QTime a,bool b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QTime,bool),void,QTime,bool> *method = reinterpret_cast<DbusMethod<void(QTime,bool),void,QTime,bool>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Time_Date(QString name,QTime a,QDate b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QTime,QDate),void,QTime,QDate> *method = reinterpret_cast<DbusMethod<void(QTime,QDate),void,QTime,QDate>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Time_String(QString name,QTime a,QString b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QTime,QString),void,QTime,QString> *method = reinterpret_cast<DbusMethod<void(QTime,QString),void,QTime,QString>*>(core->getMethod(name));
         method->call(a,b);
      };

      void callMethod_Time_Time(QString name,QTime a,QTime b)override{
         if (core->hasMethod(name)==false){return;}
         DbusMethod<void(QTime,QTime),void,QTime,QTime> *method = reinterpret_cast<DbusMethod<void(QTime,QTime),void,QTime,QTime>*>(core->getMethod(name));
         method->call(a,b);
      };

      int callMethodInt(QString name)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(),int> *method = reinterpret_cast<DbusMethod<int(),int>*>(core->getMethod(name));
         return method->call();
      };

      int callMethodInt_Int_Int(QString name,int a,int b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(int,int),int,int,int> *method = reinterpret_cast<DbusMethod<int(int,int),int,int,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Int_Double(QString name,int a,double b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(int,double),int,int,double> *method = reinterpret_cast<DbusMethod<int(int,double),int,int,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Int_Boolean(QString name,int a,bool b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(int,bool),int,int,bool> *method = reinterpret_cast<DbusMethod<int(int,bool),int,int,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Int_Date(QString name,int a,QDate b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(int,QDate),int,int,QDate> *method = reinterpret_cast<DbusMethod<int(int,QDate),int,int,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Int_String(QString name,int a,QString b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(int,QString),int,int,QString> *method = reinterpret_cast<DbusMethod<int(int,QString),int,int,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Int_Time(QString name,int a,QTime b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(int,QTime),int,int,QTime> *method = reinterpret_cast<DbusMethod<int(int,QTime),int,int,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Int(QString name,int a)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(int),int,int> *method = reinterpret_cast<DbusMethod<int(int),int,int>*>(core->getMethod(name));
         return method->call(a);
      };

      int callMethodInt_Double_Int(QString name,double a,int b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(double,int),int,double,int> *method = reinterpret_cast<DbusMethod<int(double,int),int,double,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Double_Double(QString name,double a,double b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(double,double),int,double,double> *method = reinterpret_cast<DbusMethod<int(double,double),int,double,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Double_Boolean(QString name,double a,bool b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(double,bool),int,double,bool> *method = reinterpret_cast<DbusMethod<int(double,bool),int,double,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Double_Date(QString name,double a,QDate b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(double,QDate),int,double,QDate> *method = reinterpret_cast<DbusMethod<int(double,QDate),int,double,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Double_String(QString name,double a,QString b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(double,QString),int,double,QString> *method = reinterpret_cast<DbusMethod<int(double,QString),int,double,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Double_Time(QString name,double a,QTime b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(double,QTime),int,double,QTime> *method = reinterpret_cast<DbusMethod<int(double,QTime),int,double,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Double(QString name,double a)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(double),int,double> *method = reinterpret_cast<DbusMethod<int(double),int,double>*>(core->getMethod(name));
         return method->call(a);
      };

      int callMethodInt_Boolean_Int(QString name,bool a,int b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(bool,int),int,bool,int> *method = reinterpret_cast<DbusMethod<int(bool,int),int,bool,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Boolean_Double(QString name,bool a,double b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(bool,double),int,bool,double> *method = reinterpret_cast<DbusMethod<int(bool,double),int,bool,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Boolean_Boolean(QString name,bool a,bool b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(bool,bool),int,bool,bool> *method = reinterpret_cast<DbusMethod<int(bool,bool),int,bool,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Boolean_Date(QString name,bool a,QDate b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(bool,QDate),int,bool,QDate> *method = reinterpret_cast<DbusMethod<int(bool,QDate),int,bool,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Boolean_String(QString name,bool a,QString b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(bool,QString),int,bool,QString> *method = reinterpret_cast<DbusMethod<int(bool,QString),int,bool,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Boolean_Time(QString name,bool a,QTime b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(bool,QTime),int,bool,QTime> *method = reinterpret_cast<DbusMethod<int(bool,QTime),int,bool,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Boolean(QString name,bool a)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(bool),int,bool> *method = reinterpret_cast<DbusMethod<int(bool),int,bool>*>(core->getMethod(name));
         return method->call(a);
      };

      int callMethodInt_Date_Int(QString name,QDate a,int b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QDate,int),int,QDate,int> *method = reinterpret_cast<DbusMethod<int(QDate,int),int,QDate,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Date_Double(QString name,QDate a,double b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QDate,double),int,QDate,double> *method = reinterpret_cast<DbusMethod<int(QDate,double),int,QDate,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Date_Boolean(QString name,QDate a,bool b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QDate,bool),int,QDate,bool> *method = reinterpret_cast<DbusMethod<int(QDate,bool),int,QDate,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Date_Date(QString name,QDate a,QDate b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QDate,QDate),int,QDate,QDate> *method = reinterpret_cast<DbusMethod<int(QDate,QDate),int,QDate,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Date_String(QString name,QDate a,QString b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QDate,QString),int,QDate,QString> *method = reinterpret_cast<DbusMethod<int(QDate,QString),int,QDate,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Date_Time(QString name,QDate a,QTime b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QDate,QTime),int,QDate,QTime> *method = reinterpret_cast<DbusMethod<int(QDate,QTime),int,QDate,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Date(QString name,QDate a)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QDate),int,QDate> *method = reinterpret_cast<DbusMethod<int(QDate),int,QDate>*>(core->getMethod(name));
         return method->call(a);
      };

      int callMethodInt_String_Int(QString name,QString a,int b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QString,int),int,QString,int> *method = reinterpret_cast<DbusMethod<int(QString,int),int,QString,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_String_Double(QString name,QString a,double b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QString,double),int,QString,double> *method = reinterpret_cast<DbusMethod<int(QString,double),int,QString,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_String_Boolean(QString name,QString a,bool b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QString,bool),int,QString,bool> *method = reinterpret_cast<DbusMethod<int(QString,bool),int,QString,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_String_Date(QString name,QString a,QDate b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QString,QDate),int,QString,QDate> *method = reinterpret_cast<DbusMethod<int(QString,QDate),int,QString,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_String_String(QString name,QString a,QString b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QString,QString),int,QString,QString> *method = reinterpret_cast<DbusMethod<int(QString,QString),int,QString,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_String_Time(QString name,QString a,QTime b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QString,QTime),int,QString,QTime> *method = reinterpret_cast<DbusMethod<int(QString,QTime),int,QString,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_String(QString name,QString a)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QString),int,QString> *method = reinterpret_cast<DbusMethod<int(QString),int,QString>*>(core->getMethod(name));
         return method->call(a);
      };

      int callMethodInt_Time_Int(QString name,QTime a,int b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QTime,int),int,QTime,int> *method = reinterpret_cast<DbusMethod<int(QTime,int),int,QTime,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Time_Double(QString name,QTime a,double b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QTime,double),int,QTime,double> *method = reinterpret_cast<DbusMethod<int(QTime,double),int,QTime,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Time_Boolean(QString name,QTime a,bool b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QTime,bool),int,QTime,bool> *method = reinterpret_cast<DbusMethod<int(QTime,bool),int,QTime,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Time_Date(QString name,QTime a,QDate b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QTime,QDate),int,QTime,QDate> *method = reinterpret_cast<DbusMethod<int(QTime,QDate),int,QTime,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Time_String(QString name,QTime a,QString b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QTime,QString),int,QTime,QString> *method = reinterpret_cast<DbusMethod<int(QTime,QString),int,QTime,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Time_Time(QString name,QTime a,QTime b)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QTime,QTime),int,QTime,QTime> *method = reinterpret_cast<DbusMethod<int(QTime,QTime),int,QTime,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      int callMethodInt_Time(QString name,QTime a)override{
         if (core->hasMethod(name)==false){return -1;}
         DbusMethod<int(QTime),int,QTime> *method = reinterpret_cast<DbusMethod<int(QTime),int,QTime>*>(core->getMethod(name));
         return method->call(a);
      };

      double callMethodDouble(QString name)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(),double> *method = reinterpret_cast<DbusMethod<double(),double>*>(core->getMethod(name));
         return method->call();
      };

      double callMethodDouble_Int_Int(QString name,int a,int b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(int,int),double,int,int> *method = reinterpret_cast<DbusMethod<double(int,int),double,int,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Int_Double(QString name,int a,double b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(int,double),double,int,double> *method = reinterpret_cast<DbusMethod<double(int,double),double,int,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Int_Boolean(QString name,int a,bool b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(int,bool),double,int,bool> *method = reinterpret_cast<DbusMethod<double(int,bool),double,int,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Int_Date(QString name,int a,QDate b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(int,QDate),double,int,QDate> *method = reinterpret_cast<DbusMethod<double(int,QDate),double,int,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Int_String(QString name,int a,QString b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(int,QString),double,int,QString> *method = reinterpret_cast<DbusMethod<double(int,QString),double,int,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Int_Time(QString name,int a,QTime b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(int,QTime),double,int,QTime> *method = reinterpret_cast<DbusMethod<double(int,QTime),double,int,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Int(QString name,int a)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(int),double,int> *method = reinterpret_cast<DbusMethod<double(int),double,int>*>(core->getMethod(name));
         return method->call(a);
      };

      double callMethodDouble_Double_Int(QString name,double a,int b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(double,int),double,double,int> *method = reinterpret_cast<DbusMethod<double(double,int),double,double,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Double_Double(QString name,double a,double b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(double,double),double,double,double> *method = reinterpret_cast<DbusMethod<double(double,double),double,double,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Double_Boolean(QString name,double a,bool b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(double,bool),double,double,bool> *method = reinterpret_cast<DbusMethod<double(double,bool),double,double,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Double_Date(QString name,double a,QDate b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(double,QDate),double,double,QDate> *method = reinterpret_cast<DbusMethod<double(double,QDate),double,double,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Double_String(QString name,double a,QString b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(double,QString),double,double,QString> *method = reinterpret_cast<DbusMethod<double(double,QString),double,double,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Double_Time(QString name,double a,QTime b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(double,QTime),double,double,QTime> *method = reinterpret_cast<DbusMethod<double(double,QTime),double,double,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Double(QString name,double a)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(double),double,double> *method = reinterpret_cast<DbusMethod<double(double),double,double>*>(core->getMethod(name));
         return method->call(a);
      };

      double callMethodDouble_Boolean_Int(QString name,bool a,int b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(bool,int),double,bool,int> *method = reinterpret_cast<DbusMethod<double(bool,int),double,bool,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Boolean_Double(QString name,bool a,double b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(bool,double),double,bool,double> *method = reinterpret_cast<DbusMethod<double(bool,double),double,bool,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Boolean_Boolean(QString name,bool a,bool b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(bool,bool),double,bool,bool> *method = reinterpret_cast<DbusMethod<double(bool,bool),double,bool,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Boolean_Date(QString name,bool a,QDate b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(bool,QDate),double,bool,QDate> *method = reinterpret_cast<DbusMethod<double(bool,QDate),double,bool,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Boolean_String(QString name,bool a,QString b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(bool,QString),double,bool,QString> *method = reinterpret_cast<DbusMethod<double(bool,QString),double,bool,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Boolean_Time(QString name,bool a,QTime b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(bool,QTime),double,bool,QTime> *method = reinterpret_cast<DbusMethod<double(bool,QTime),double,bool,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Boolean(QString name,bool a)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(bool),double,bool> *method = reinterpret_cast<DbusMethod<double(bool),double,bool>*>(core->getMethod(name));
         return method->call(a);
      };

      double callMethodDouble_Date_Int(QString name,QDate a,int b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QDate,int),double,QDate,int> *method = reinterpret_cast<DbusMethod<double(QDate,int),double,QDate,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Date_Double(QString name,QDate a,double b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QDate,double),double,QDate,double> *method = reinterpret_cast<DbusMethod<double(QDate,double),double,QDate,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Date_Boolean(QString name,QDate a,bool b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QDate,bool),double,QDate,bool> *method = reinterpret_cast<DbusMethod<double(QDate,bool),double,QDate,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Date_Date(QString name,QDate a,QDate b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QDate,QDate),double,QDate,QDate> *method = reinterpret_cast<DbusMethod<double(QDate,QDate),double,QDate,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Date_String(QString name,QDate a,QString b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QDate,QString),double,QDate,QString> *method = reinterpret_cast<DbusMethod<double(QDate,QString),double,QDate,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Date_Time(QString name,QDate a,QTime b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QDate,QTime),double,QDate,QTime> *method = reinterpret_cast<DbusMethod<double(QDate,QTime),double,QDate,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Date(QString name,QDate a)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QDate),double,QDate> *method = reinterpret_cast<DbusMethod<double(QDate),double,QDate>*>(core->getMethod(name));
         return method->call(a);
      };

      double callMethodDouble_String_Int(QString name,QString a,int b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QString,int),double,QString,int> *method = reinterpret_cast<DbusMethod<double(QString,int),double,QString,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_String_Double(QString name,QString a,double b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QString,double),double,QString,double> *method = reinterpret_cast<DbusMethod<double(QString,double),double,QString,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_String_Boolean(QString name,QString a,bool b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QString,bool),double,QString,bool> *method = reinterpret_cast<DbusMethod<double(QString,bool),double,QString,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_String_Date(QString name,QString a,QDate b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QString,QDate),double,QString,QDate> *method = reinterpret_cast<DbusMethod<double(QString,QDate),double,QString,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_String_String(QString name,QString a,QString b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QString,QString),double,QString,QString> *method = reinterpret_cast<DbusMethod<double(QString,QString),double,QString,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_String_Time(QString name,QString a,QTime b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QString,QTime),double,QString,QTime> *method = reinterpret_cast<DbusMethod<double(QString,QTime),double,QString,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_String(QString name,QString a)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QString),double,QString> *method = reinterpret_cast<DbusMethod<double(QString),double,QString>*>(core->getMethod(name));
         return method->call(a);
      };

      double callMethodDouble_Time_Int(QString name,QTime a,int b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QTime,int),double,QTime,int> *method = reinterpret_cast<DbusMethod<double(QTime,int),double,QTime,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Time_Double(QString name,QTime a,double b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QTime,double),double,QTime,double> *method = reinterpret_cast<DbusMethod<double(QTime,double),double,QTime,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Time_Boolean(QString name,QTime a,bool b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QTime,bool),double,QTime,bool> *method = reinterpret_cast<DbusMethod<double(QTime,bool),double,QTime,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Time_Date(QString name,QTime a,QDate b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QTime,QDate),double,QTime,QDate> *method = reinterpret_cast<DbusMethod<double(QTime,QDate),double,QTime,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Time_String(QString name,QTime a,QString b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QTime,QString),double,QTime,QString> *method = reinterpret_cast<DbusMethod<double(QTime,QString),double,QTime,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Time_Time(QString name,QTime a,QTime b)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QTime,QTime),double,QTime,QTime> *method = reinterpret_cast<DbusMethod<double(QTime,QTime),double,QTime,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      double callMethodDouble_Time(QString name,QTime a)override{
         if (core->hasMethod(name)==false){ return 2.0;}
         DbusMethod<double(QTime),double,QTime> *method = reinterpret_cast<DbusMethod<double(QTime),double,QTime>*>(core->getMethod(name));
         return method->call(a);
      };

      bool callMethodBoolean(QString name)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(),bool> *method = reinterpret_cast<DbusMethod<bool(),bool>*>(core->getMethod(name));
         return method->call();
      };

      bool callMethodBoolean_Int_Int(QString name,int a,int b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(int,int),bool,int,int> *method = reinterpret_cast<DbusMethod<bool(int,int),bool,int,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Int_Double(QString name,int a,double b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(int,double),bool,int,double> *method = reinterpret_cast<DbusMethod<bool(int,double),bool,int,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Int_Boolean(QString name,int a,bool b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(int,bool),bool,int,bool> *method = reinterpret_cast<DbusMethod<bool(int,bool),bool,int,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Int_Date(QString name,int a,QDate b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(int,QDate),bool,int,QDate> *method = reinterpret_cast<DbusMethod<bool(int,QDate),bool,int,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Int_String(QString name,int a,QString b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(int,QString),bool,int,QString> *method = reinterpret_cast<DbusMethod<bool(int,QString),bool,int,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Int_Time(QString name,int a,QTime b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(int,QTime),bool,int,QTime> *method = reinterpret_cast<DbusMethod<bool(int,QTime),bool,int,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Int(QString name,int a)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(int),bool,int> *method = reinterpret_cast<DbusMethod<bool(int),bool,int>*>(core->getMethod(name));
         return method->call(a);
      };

      bool callMethodBoolean_Double_Int(QString name,double a,int b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(double,int),bool,double,int> *method = reinterpret_cast<DbusMethod<bool(double,int),bool,double,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Double_Double(QString name,double a,double b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(double,double),bool,double,double> *method = reinterpret_cast<DbusMethod<bool(double,double),bool,double,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Double_Boolean(QString name,double a,bool b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(double,bool),bool,double,bool> *method = reinterpret_cast<DbusMethod<bool(double,bool),bool,double,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Double_Date(QString name,double a,QDate b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(double,QDate),bool,double,QDate> *method = reinterpret_cast<DbusMethod<bool(double,QDate),bool,double,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Double_String(QString name,double a,QString b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(double,QString),bool,double,QString> *method = reinterpret_cast<DbusMethod<bool(double,QString),bool,double,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Double_Time(QString name,double a,QTime b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(double,QTime),bool,double,QTime> *method = reinterpret_cast<DbusMethod<bool(double,QTime),bool,double,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Double(QString name,double a)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(double),bool,double> *method = reinterpret_cast<DbusMethod<bool(double),bool,double>*>(core->getMethod(name));
         return method->call(a);
      };

      bool callMethodBoolean_Boolean_Int(QString name,bool a,int b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(bool,int),bool,bool,int> *method = reinterpret_cast<DbusMethod<bool(bool,int),bool,bool,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Boolean_Double(QString name,bool a,double b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(bool,double),bool,bool,double> *method = reinterpret_cast<DbusMethod<bool(bool,double),bool,bool,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Boolean_Boolean(QString name,bool a,bool b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(bool,bool),bool,bool,bool> *method = reinterpret_cast<DbusMethod<bool(bool,bool),bool,bool,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Boolean_Date(QString name,bool a,QDate b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(bool,QDate),bool,bool,QDate> *method = reinterpret_cast<DbusMethod<bool(bool,QDate),bool,bool,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Boolean_String(QString name,bool a,QString b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(bool,QString),bool,bool,QString> *method = reinterpret_cast<DbusMethod<bool(bool,QString),bool,bool,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Boolean_Time(QString name,bool a,QTime b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(bool,QTime),bool,bool,QTime> *method = reinterpret_cast<DbusMethod<bool(bool,QTime),bool,bool,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Boolean(QString name,bool a)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(bool),bool,bool> *method = reinterpret_cast<DbusMethod<bool(bool),bool,bool>*>(core->getMethod(name));
         return method->call(a);
      };

      bool callMethodBoolean_Date_Int(QString name,QDate a,int b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QDate,int),bool,QDate,int> *method = reinterpret_cast<DbusMethod<bool(QDate,int),bool,QDate,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Date_Double(QString name,QDate a,double b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QDate,double),bool,QDate,double> *method = reinterpret_cast<DbusMethod<bool(QDate,double),bool,QDate,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Date_Boolean(QString name,QDate a,bool b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QDate,bool),bool,QDate,bool> *method = reinterpret_cast<DbusMethod<bool(QDate,bool),bool,QDate,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Date_Date(QString name,QDate a,QDate b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QDate,QDate),bool,QDate,QDate> *method = reinterpret_cast<DbusMethod<bool(QDate,QDate),bool,QDate,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Date_String(QString name,QDate a,QString b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QDate,QString),bool,QDate,QString> *method = reinterpret_cast<DbusMethod<bool(QDate,QString),bool,QDate,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Date_Time(QString name,QDate a,QTime b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QDate,QTime),bool,QDate,QTime> *method = reinterpret_cast<DbusMethod<bool(QDate,QTime),bool,QDate,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Date(QString name,QDate a)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QDate),bool,QDate> *method = reinterpret_cast<DbusMethod<bool(QDate),bool,QDate>*>(core->getMethod(name));
         return method->call(a);
      };

      bool callMethodBoolean_String_Int(QString name,QString a,int b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QString,int),bool,QString,int> *method = reinterpret_cast<DbusMethod<bool(QString,int),bool,QString,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_String_Double(QString name,QString a,double b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QString,double),bool,QString,double> *method = reinterpret_cast<DbusMethod<bool(QString,double),bool,QString,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_String_Boolean(QString name,QString a,bool b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QString,bool),bool,QString,bool> *method = reinterpret_cast<DbusMethod<bool(QString,bool),bool,QString,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_String_Date(QString name,QString a,QDate b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QString,QDate),bool,QString,QDate> *method = reinterpret_cast<DbusMethod<bool(QString,QDate),bool,QString,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_String_String(QString name,QString a,QString b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QString,QString),bool,QString,QString> *method = reinterpret_cast<DbusMethod<bool(QString,QString),bool,QString,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_String_Time(QString name,QString a,QTime b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QString,QTime),bool,QString,QTime> *method = reinterpret_cast<DbusMethod<bool(QString,QTime),bool,QString,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_String(QString name,QString a)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QString),bool,QString> *method = reinterpret_cast<DbusMethod<bool(QString),bool,QString>*>(core->getMethod(name));
         return method->call(a);
      };

      bool callMethodBoolean_Time_Int(QString name,QTime a,int b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QTime,int),bool,QTime,int> *method = reinterpret_cast<DbusMethod<bool(QTime,int),bool,QTime,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Time_Double(QString name,QTime a,double b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QTime,double),bool,QTime,double> *method = reinterpret_cast<DbusMethod<bool(QTime,double),bool,QTime,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Time_Boolean(QString name,QTime a,bool b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QTime,bool),bool,QTime,bool> *method = reinterpret_cast<DbusMethod<bool(QTime,bool),bool,QTime,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Time_Date(QString name,QTime a,QDate b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QTime,QDate),bool,QTime,QDate> *method = reinterpret_cast<DbusMethod<bool(QTime,QDate),bool,QTime,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Time_String(QString name,QTime a,QString b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QTime,QString),bool,QTime,QString> *method = reinterpret_cast<DbusMethod<bool(QTime,QString),bool,QTime,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Time_Time(QString name,QTime a,QTime b)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QTime,QTime),bool,QTime,QTime> *method = reinterpret_cast<DbusMethod<bool(QTime,QTime),bool,QTime,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      bool callMethodBoolean_Time(QString name,QTime a)override{
         if (core->hasMethod(name)==false){return false;}
         DbusMethod<bool(QTime),bool,QTime> *method = reinterpret_cast<DbusMethod<bool(QTime),bool,QTime>*>(core->getMethod(name));
         return method->call(a);
      };

      QDate callMethodDate(QString name)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(),QDate> *method = reinterpret_cast<DbusMethod<QDate(),QDate>*>(core->getMethod(name));
         return method->call();
      };

      QDate callMethodDate_Int_Int(QString name,int a,int b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(int,int),QDate,int,int> *method = reinterpret_cast<DbusMethod<QDate(int,int),QDate,int,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Int_Double(QString name,int a,double b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(int,double),QDate,int,double> *method = reinterpret_cast<DbusMethod<QDate(int,double),QDate,int,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Int_Boolean(QString name,int a,bool b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(int,bool),QDate,int,bool> *method = reinterpret_cast<DbusMethod<QDate(int,bool),QDate,int,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Int_Date(QString name,int a,QDate b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(int,QDate),QDate,int,QDate> *method = reinterpret_cast<DbusMethod<QDate(int,QDate),QDate,int,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Int_String(QString name,int a,QString b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(int,QString),QDate,int,QString> *method = reinterpret_cast<DbusMethod<QDate(int,QString),QDate,int,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Int_Time(QString name,int a,QTime b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(int,QTime),QDate,int,QTime> *method = reinterpret_cast<DbusMethod<QDate(int,QTime),QDate,int,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Int(QString name,int a)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(int),QDate,int> *method = reinterpret_cast<DbusMethod<QDate(int),QDate,int>*>(core->getMethod(name));
         return method->call(a);
      };

      QDate callMethodDate_Double_Int(QString name,double a,int b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(double,int),QDate,double,int> *method = reinterpret_cast<DbusMethod<QDate(double,int),QDate,double,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Double_Double(QString name,double a,double b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(double,double),QDate,double,double> *method = reinterpret_cast<DbusMethod<QDate(double,double),QDate,double,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Double_Boolean(QString name,double a,bool b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(double,bool),QDate,double,bool> *method = reinterpret_cast<DbusMethod<QDate(double,bool),QDate,double,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Double_Date(QString name,double a,QDate b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(double,QDate),QDate,double,QDate> *method = reinterpret_cast<DbusMethod<QDate(double,QDate),QDate,double,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Double_String(QString name,double a,QString b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(double,QString),QDate,double,QString> *method = reinterpret_cast<DbusMethod<QDate(double,QString),QDate,double,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Double_Time(QString name,double a,QTime b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(double,QTime),QDate,double,QTime> *method = reinterpret_cast<DbusMethod<QDate(double,QTime),QDate,double,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Double(QString name,double a)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(double),QDate,double> *method = reinterpret_cast<DbusMethod<QDate(double),QDate,double>*>(core->getMethod(name));
         return method->call(a);
      };

      QDate callMethodDate_Boolean_Int(QString name,bool a,int b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(bool,int),QDate,bool,int> *method = reinterpret_cast<DbusMethod<QDate(bool,int),QDate,bool,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Boolean_Double(QString name,bool a,double b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(bool,double),QDate,bool,double> *method = reinterpret_cast<DbusMethod<QDate(bool,double),QDate,bool,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Boolean_Boolean(QString name,bool a,bool b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(bool,bool),QDate,bool,bool> *method = reinterpret_cast<DbusMethod<QDate(bool,bool),QDate,bool,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Boolean_Date(QString name,bool a,QDate b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(bool,QDate),QDate,bool,QDate> *method = reinterpret_cast<DbusMethod<QDate(bool,QDate),QDate,bool,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Boolean_String(QString name,bool a,QString b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(bool,QString),QDate,bool,QString> *method = reinterpret_cast<DbusMethod<QDate(bool,QString),QDate,bool,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Boolean_Time(QString name,bool a,QTime b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(bool,QTime),QDate,bool,QTime> *method = reinterpret_cast<DbusMethod<QDate(bool,QTime),QDate,bool,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Boolean(QString name,bool a)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(bool),QDate,bool> *method = reinterpret_cast<DbusMethod<QDate(bool),QDate,bool>*>(core->getMethod(name));
         return method->call(a);
      };

      QDate callMethodDate_Date_Int(QString name,QDate a,int b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QDate,int),QDate,QDate,int> *method = reinterpret_cast<DbusMethod<QDate(QDate,int),QDate,QDate,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Date_Double(QString name,QDate a,double b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QDate,double),QDate,QDate,double> *method = reinterpret_cast<DbusMethod<QDate(QDate,double),QDate,QDate,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Date_Boolean(QString name,QDate a,bool b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QDate,bool),QDate,QDate,bool> *method = reinterpret_cast<DbusMethod<QDate(QDate,bool),QDate,QDate,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Date_Date(QString name,QDate a,QDate b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QDate,QDate),QDate,QDate,QDate> *method = reinterpret_cast<DbusMethod<QDate(QDate,QDate),QDate,QDate,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Date_String(QString name,QDate a,QString b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QDate,QString),QDate,QDate,QString> *method = reinterpret_cast<DbusMethod<QDate(QDate,QString),QDate,QDate,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Date_Time(QString name,QDate a,QTime b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QDate,QTime),QDate,QDate,QTime> *method = reinterpret_cast<DbusMethod<QDate(QDate,QTime),QDate,QDate,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Date(QString name,QDate a)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QDate),QDate,QDate> *method = reinterpret_cast<DbusMethod<QDate(QDate),QDate,QDate>*>(core->getMethod(name));
         return method->call(a);
      };

      QDate callMethodDate_String_Int(QString name,QString a,int b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QString,int),QDate,QString,int> *method = reinterpret_cast<DbusMethod<QDate(QString,int),QDate,QString,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_String_Double(QString name,QString a,double b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QString,double),QDate,QString,double> *method = reinterpret_cast<DbusMethod<QDate(QString,double),QDate,QString,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_String_Boolean(QString name,QString a,bool b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QString,bool),QDate,QString,bool> *method = reinterpret_cast<DbusMethod<QDate(QString,bool),QDate,QString,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_String_Date(QString name,QString a,QDate b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QString,QDate),QDate,QString,QDate> *method = reinterpret_cast<DbusMethod<QDate(QString,QDate),QDate,QString,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_String_String(QString name,QString a,QString b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QString,QString),QDate,QString,QString> *method = reinterpret_cast<DbusMethod<QDate(QString,QString),QDate,QString,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_String_Time(QString name,QString a,QTime b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QString,QTime),QDate,QString,QTime> *method = reinterpret_cast<DbusMethod<QDate(QString,QTime),QDate,QString,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_String(QString name,QString a)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QString),QDate,QString> *method = reinterpret_cast<DbusMethod<QDate(QString),QDate,QString>*>(core->getMethod(name));
         return method->call(a);
      };

      QDate callMethodDate_Time_Int(QString name,QTime a,int b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QTime,int),QDate,QTime,int> *method = reinterpret_cast<DbusMethod<QDate(QTime,int),QDate,QTime,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Time_Double(QString name,QTime a,double b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QTime,double),QDate,QTime,double> *method = reinterpret_cast<DbusMethod<QDate(QTime,double),QDate,QTime,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Time_Boolean(QString name,QTime a,bool b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QTime,bool),QDate,QTime,bool> *method = reinterpret_cast<DbusMethod<QDate(QTime,bool),QDate,QTime,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Time_Date(QString name,QTime a,QDate b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QTime,QDate),QDate,QTime,QDate> *method = reinterpret_cast<DbusMethod<QDate(QTime,QDate),QDate,QTime,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Time_String(QString name,QTime a,QString b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QTime,QString),QDate,QTime,QString> *method = reinterpret_cast<DbusMethod<QDate(QTime,QString),QDate,QTime,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Time_Time(QString name,QTime a,QTime b)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QTime,QTime),QDate,QTime,QTime> *method = reinterpret_cast<DbusMethod<QDate(QTime,QTime),QDate,QTime,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QDate callMethodDate_Time(QString name,QTime a)override{
         if (core->hasMethod(name)==false){return QDate();}
         DbusMethod<QDate(QTime),QDate,QTime> *method = reinterpret_cast<DbusMethod<QDate(QTime),QDate,QTime>*>(core->getMethod(name));
         return method->call(a);
      };

      QString callMethodString(QString name)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(),QString> *method = reinterpret_cast<DbusMethod<QString(),QString>*>(core->getMethod(name));
         return method->call();
      };

      QString callMethodString_Int_Int(QString name,int a,int b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(int,int),QString,int,int> *method = reinterpret_cast<DbusMethod<QString(int,int),QString,int,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Int_Double(QString name,int a,double b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(int,double),QString,int,double> *method = reinterpret_cast<DbusMethod<QString(int,double),QString,int,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Int_Boolean(QString name,int a,bool b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(int,bool),QString,int,bool> *method = reinterpret_cast<DbusMethod<QString(int,bool),QString,int,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Int_Date(QString name,int a,QDate b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(int,QDate),QString,int,QDate> *method = reinterpret_cast<DbusMethod<QString(int,QDate),QString,int,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Int_String(QString name,int a,QString b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(int,QString),QString,int,QString> *method = reinterpret_cast<DbusMethod<QString(int,QString),QString,int,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Int_Time(QString name,int a,QTime b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(int,QTime),QString,int,QTime> *method = reinterpret_cast<DbusMethod<QString(int,QTime),QString,int,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Int(QString name,int a)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(int),QString,int> *method = reinterpret_cast<DbusMethod<QString(int),QString,int>*>(core->getMethod(name));
         return method->call(a);
      };

      QString callMethodString_Double_Int(QString name,double a,int b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(double,int),QString,double,int> *method = reinterpret_cast<DbusMethod<QString(double,int),QString,double,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Double_Double(QString name,double a,double b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(double,double),QString,double,double> *method = reinterpret_cast<DbusMethod<QString(double,double),QString,double,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Double_Boolean(QString name,double a,bool b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(double,bool),QString,double,bool> *method = reinterpret_cast<DbusMethod<QString(double,bool),QString,double,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Double_Date(QString name,double a,QDate b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(double,QDate),QString,double,QDate> *method = reinterpret_cast<DbusMethod<QString(double,QDate),QString,double,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Double_String(QString name,double a,QString b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(double,QString),QString,double,QString> *method = reinterpret_cast<DbusMethod<QString(double,QString),QString,double,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Double_Time(QString name,double a,QTime b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(double,QTime),QString,double,QTime> *method = reinterpret_cast<DbusMethod<QString(double,QTime),QString,double,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Double(QString name,double a)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(double),QString,double> *method = reinterpret_cast<DbusMethod<QString(double),QString,double>*>(core->getMethod(name));
         return method->call(a);
      };

      QString callMethodString_Boolean_Int(QString name,bool a,int b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(bool,int),QString,bool,int> *method = reinterpret_cast<DbusMethod<QString(bool,int),QString,bool,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Boolean_Double(QString name,bool a,double b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(bool,double),QString,bool,double> *method = reinterpret_cast<DbusMethod<QString(bool,double),QString,bool,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Boolean_Boolean(QString name,bool a,bool b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(bool,bool),QString,bool,bool> *method = reinterpret_cast<DbusMethod<QString(bool,bool),QString,bool,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Boolean_Date(QString name,bool a,QDate b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(bool,QDate),QString,bool,QDate> *method = reinterpret_cast<DbusMethod<QString(bool,QDate),QString,bool,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Boolean_String(QString name,bool a,QString b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(bool,QString),QString,bool,QString> *method = reinterpret_cast<DbusMethod<QString(bool,QString),QString,bool,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Boolean_Time(QString name,bool a,QTime b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(bool,QTime),QString,bool,QTime> *method = reinterpret_cast<DbusMethod<QString(bool,QTime),QString,bool,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Boolean(QString name,bool a)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(bool),QString,bool> *method = reinterpret_cast<DbusMethod<QString(bool),QString,bool>*>(core->getMethod(name));
         return method->call(a);
      };

      QString callMethodString_Date_Int(QString name,QDate a,int b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QDate,int),QString,QDate,int> *method = reinterpret_cast<DbusMethod<QString(QDate,int),QString,QDate,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Date_Double(QString name,QDate a,double b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QDate,double),QString,QDate,double> *method = reinterpret_cast<DbusMethod<QString(QDate,double),QString,QDate,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Date_Boolean(QString name,QDate a,bool b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QDate,bool),QString,QDate,bool> *method = reinterpret_cast<DbusMethod<QString(QDate,bool),QString,QDate,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Date_Date(QString name,QDate a,QDate b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QDate,QDate),QString,QDate,QDate> *method = reinterpret_cast<DbusMethod<QString(QDate,QDate),QString,QDate,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Date_String(QString name,QDate a,QString b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QDate,QString),QString,QDate,QString> *method = reinterpret_cast<DbusMethod<QString(QDate,QString),QString,QDate,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Date_Time(QString name,QDate a,QTime b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QDate,QTime),QString,QDate,QTime> *method = reinterpret_cast<DbusMethod<QString(QDate,QTime),QString,QDate,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Date(QString name,QDate a)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QDate),QString,QDate> *method = reinterpret_cast<DbusMethod<QString(QDate),QString,QDate>*>(core->getMethod(name));
         return method->call(a);
      };

      QString callMethodString_String_Int(QString name,QString a,int b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QString,int),QString,QString,int> *method = reinterpret_cast<DbusMethod<QString(QString,int),QString,QString,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_String_Double(QString name,QString a,double b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QString,double),QString,QString,double> *method = reinterpret_cast<DbusMethod<QString(QString,double),QString,QString,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_String_Boolean(QString name,QString a,bool b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QString,bool),QString,QString,bool> *method = reinterpret_cast<DbusMethod<QString(QString,bool),QString,QString,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_String_Date(QString name,QString a,QDate b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QString,QDate),QString,QString,QDate> *method = reinterpret_cast<DbusMethod<QString(QString,QDate),QString,QString,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_String_String(QString name,QString a,QString b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QString,QString),QString,QString,QString> *method = reinterpret_cast<DbusMethod<QString(QString,QString),QString,QString,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_String_Time(QString name,QString a,QTime b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QString,QTime),QString,QString,QTime> *method = reinterpret_cast<DbusMethod<QString(QString,QTime),QString,QString,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_String(QString name,QString a)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QString),QString,QString> *method = reinterpret_cast<DbusMethod<QString(QString),QString,QString>*>(core->getMethod(name));
         return method->call(a);
      };

      QString callMethodString_Time_Int(QString name,QTime a,int b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QTime,int),QString,QTime,int> *method = reinterpret_cast<DbusMethod<QString(QTime,int),QString,QTime,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Time_Double(QString name,QTime a,double b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QTime,double),QString,QTime,double> *method = reinterpret_cast<DbusMethod<QString(QTime,double),QString,QTime,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Time_Boolean(QString name,QTime a,bool b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QTime,bool),QString,QTime,bool> *method = reinterpret_cast<DbusMethod<QString(QTime,bool),QString,QTime,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Time_Date(QString name,QTime a,QDate b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QTime,QDate),QString,QTime,QDate> *method = reinterpret_cast<DbusMethod<QString(QTime,QDate),QString,QTime,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Time_String(QString name,QTime a,QString b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QTime,QString),QString,QTime,QString> *method = reinterpret_cast<DbusMethod<QString(QTime,QString),QString,QTime,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Time_Time(QString name,QTime a,QTime b)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QTime,QTime),QString,QTime,QTime> *method = reinterpret_cast<DbusMethod<QString(QTime,QTime),QString,QTime,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QString callMethodString_Time(QString name,QTime a)override{
         if (core->hasMethod(name)==false){return QString("def");}
         DbusMethod<QString(QTime),QString,QTime> *method = reinterpret_cast<DbusMethod<QString(QTime),QString,QTime>*>(core->getMethod(name));
         return method->call(a);
      };

      QTime callMethodTime(QString name)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(),QTime> *method = reinterpret_cast<DbusMethod<QTime(),QTime>*>(core->getMethod(name));
         return method->call();
      };

      QTime callMethodTime_Int_Int(QString name,int a,int b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(int,int),QTime,int,int> *method = reinterpret_cast<DbusMethod<QTime(int,int),QTime,int,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Int_Double(QString name,int a,double b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(int,double),QTime,int,double> *method = reinterpret_cast<DbusMethod<QTime(int,double),QTime,int,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Int_Boolean(QString name,int a,bool b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(int,bool),QTime,int,bool> *method = reinterpret_cast<DbusMethod<QTime(int,bool),QTime,int,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Int_Date(QString name,int a,QDate b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(int,QDate),QTime,int,QDate> *method = reinterpret_cast<DbusMethod<QTime(int,QDate),QTime,int,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Int_String(QString name,int a,QString b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(int,QString),QTime,int,QString> *method = reinterpret_cast<DbusMethod<QTime(int,QString),QTime,int,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Int_Time(QString name,int a,QTime b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(int,QTime),QTime,int,QTime> *method = reinterpret_cast<DbusMethod<QTime(int,QTime),QTime,int,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Int(QString name,int a)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(int),QTime,int> *method = reinterpret_cast<DbusMethod<QTime(int),QTime,int>*>(core->getMethod(name));
         return method->call(a);
      };

      QTime callMethodTime_Double_Int(QString name,double a,int b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(double,int),QTime,double,int> *method = reinterpret_cast<DbusMethod<QTime(double,int),QTime,double,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Double_Double(QString name,double a,double b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(double,double),QTime,double,double> *method = reinterpret_cast<DbusMethod<QTime(double,double),QTime,double,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Double_Boolean(QString name,double a,bool b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(double,bool),QTime,double,bool> *method = reinterpret_cast<DbusMethod<QTime(double,bool),QTime,double,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Double_Date(QString name,double a,QDate b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(double,QDate),QTime,double,QDate> *method = reinterpret_cast<DbusMethod<QTime(double,QDate),QTime,double,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Double_String(QString name,double a,QString b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(double,QString),QTime,double,QString> *method = reinterpret_cast<DbusMethod<QTime(double,QString),QTime,double,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Double_Time(QString name,double a,QTime b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(double,QTime),QTime,double,QTime> *method = reinterpret_cast<DbusMethod<QTime(double,QTime),QTime,double,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Double(QString name,double a)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(double),QTime,double> *method = reinterpret_cast<DbusMethod<QTime(double),QTime,double>*>(core->getMethod(name));
         return method->call(a);
      };

      QTime callMethodTime_Boolean_Int(QString name,bool a,int b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(bool,int),QTime,bool,int> *method = reinterpret_cast<DbusMethod<QTime(bool,int),QTime,bool,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Boolean_Double(QString name,bool a,double b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(bool,double),QTime,bool,double> *method = reinterpret_cast<DbusMethod<QTime(bool,double),QTime,bool,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Boolean_Boolean(QString name,bool a,bool b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(bool,bool),QTime,bool,bool> *method = reinterpret_cast<DbusMethod<QTime(bool,bool),QTime,bool,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Boolean_Date(QString name,bool a,QDate b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(bool,QDate),QTime,bool,QDate> *method = reinterpret_cast<DbusMethod<QTime(bool,QDate),QTime,bool,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Boolean_String(QString name,bool a,QString b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(bool,QString),QTime,bool,QString> *method = reinterpret_cast<DbusMethod<QTime(bool,QString),QTime,bool,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Boolean_Time(QString name,bool a,QTime b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(bool,QTime),QTime,bool,QTime> *method = reinterpret_cast<DbusMethod<QTime(bool,QTime),QTime,bool,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Boolean(QString name,bool a)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(bool),QTime,bool> *method = reinterpret_cast<DbusMethod<QTime(bool),QTime,bool>*>(core->getMethod(name));
         return method->call(a);
      };

      QTime callMethodTime_Date_Int(QString name,QDate a,int b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QDate,int),QTime,QDate,int> *method = reinterpret_cast<DbusMethod<QTime(QDate,int),QTime,QDate,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Date_Double(QString name,QDate a,double b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QDate,double),QTime,QDate,double> *method = reinterpret_cast<DbusMethod<QTime(QDate,double),QTime,QDate,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Date_Boolean(QString name,QDate a,bool b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QDate,bool),QTime,QDate,bool> *method = reinterpret_cast<DbusMethod<QTime(QDate,bool),QTime,QDate,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Date_Date(QString name,QDate a,QDate b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QDate,QDate),QTime,QDate,QDate> *method = reinterpret_cast<DbusMethod<QTime(QDate,QDate),QTime,QDate,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Date_String(QString name,QDate a,QString b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QDate,QString),QTime,QDate,QString> *method = reinterpret_cast<DbusMethod<QTime(QDate,QString),QTime,QDate,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Date_Time(QString name,QDate a,QTime b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QDate,QTime),QTime,QDate,QTime> *method = reinterpret_cast<DbusMethod<QTime(QDate,QTime),QTime,QDate,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Date(QString name,QDate a)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QDate),QTime,QDate> *method = reinterpret_cast<DbusMethod<QTime(QDate),QTime,QDate>*>(core->getMethod(name));
         return method->call(a);
      };

      QTime callMethodTime_String_Int(QString name,QString a,int b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QString,int),QTime,QString,int> *method = reinterpret_cast<DbusMethod<QTime(QString,int),QTime,QString,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_String_Double(QString name,QString a,double b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QString,double),QTime,QString,double> *method = reinterpret_cast<DbusMethod<QTime(QString,double),QTime,QString,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_String_Boolean(QString name,QString a,bool b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QString,bool),QTime,QString,bool> *method = reinterpret_cast<DbusMethod<QTime(QString,bool),QTime,QString,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_String_Date(QString name,QString a,QDate b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QString,QDate),QTime,QString,QDate> *method = reinterpret_cast<DbusMethod<QTime(QString,QDate),QTime,QString,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_String_String(QString name,QString a,QString b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QString,QString),QTime,QString,QString> *method = reinterpret_cast<DbusMethod<QTime(QString,QString),QTime,QString,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_String_Time(QString name,QString a,QTime b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QString,QTime),QTime,QString,QTime> *method = reinterpret_cast<DbusMethod<QTime(QString,QTime),QTime,QString,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_String(QString name,QString a)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QString),QTime,QString> *method = reinterpret_cast<DbusMethod<QTime(QString),QTime,QString>*>(core->getMethod(name));
         return method->call(a);
      };

      QTime callMethodTime_Time_Int(QString name,QTime a,int b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QTime,int),QTime,QTime,int> *method = reinterpret_cast<DbusMethod<QTime(QTime,int),QTime,QTime,int>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Time_Double(QString name,QTime a,double b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QTime,double),QTime,QTime,double> *method = reinterpret_cast<DbusMethod<QTime(QTime,double),QTime,QTime,double>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Time_Boolean(QString name,QTime a,bool b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QTime,bool),QTime,QTime,bool> *method = reinterpret_cast<DbusMethod<QTime(QTime,bool),QTime,QTime,bool>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Time_Date(QString name,QTime a,QDate b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QTime,QDate),QTime,QTime,QDate> *method = reinterpret_cast<DbusMethod<QTime(QTime,QDate),QTime,QTime,QDate>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Time_String(QString name,QTime a,QString b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QTime,QString),QTime,QTime,QString> *method = reinterpret_cast<DbusMethod<QTime(QTime,QString),QTime,QTime,QString>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Time_Time(QString name,QTime a,QTime b)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QTime,QTime),QTime,QTime,QTime> *method = reinterpret_cast<DbusMethod<QTime(QTime,QTime),QTime,QTime,QTime>*>(core->getMethod(name));
         return method->call(a,b);
      };

      QTime callMethodTime_Time(QString name,QTime a)override{
         if (core->hasMethod(name)==false){return QTime();}
         DbusMethod<QTime(QTime),QTime,QTime> *method = reinterpret_cast<DbusMethod<QTime(QTime),QTime,QTime>*>(core->getMethod(name));
         return method->call(a);
      };






    private:
        DbusModule *core;
};


#define DBUSMODULE_H_OK

#endif // DBUSMODULE_H
