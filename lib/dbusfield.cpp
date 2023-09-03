#include "dbusfield.h"

DbusField::DbusField(QString nameq)
{
    name=nameq;
}

QString DbusField::getName(){
    return name;
}
int DbusField::getInt()
{
    return *intp;
}
void DbusField::setInt(int* a)
{
   intp=a;
}
QDate DbusField::getDate()
{
    return *datep;
}
void DbusField::setDate(QDate* a)
{
   datep=a;
}
bool DbusField::getBoolean()
{
    return *boolp;
}
void DbusField::setBoolean(bool* a)
{
   boolp=a;
}
double DbusField::getDouble()
{
    return *doublep;
}
void DbusField::setDouble(double* a)
{
    doublep=a;
}
QString DbusField::getString()
{
    return *stringp;
}
void DbusField::setString(QString* a)
{
    stringp=a;
}
QTime DbusField::getTime()
{
    return *timep;
}
void DbusField::setQTime(QTime* a)
{
    timep=a;
}

QString DbusField::getLabel()
{
    return QString(labelp->text());
}
void DbusField::setLabel(QLabel* a)
{
    labelp=a;
}
