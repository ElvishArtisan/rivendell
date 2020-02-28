#ifndef DBUSFIELD_H
#define DBUSFIELD_H

#include <QString>
#include <QTime>
#include <QLabel>

/**
Name: DbusField
Description: Add Field to DbusAPI
Example: DbusField field = new DbusField("test");  field.setInt(%test);
*/
class DbusField
{
    public:
        DbusField(QString name);


        QString getName();
        int getInt();
        void setInt(int* a);
        double getDouble();
        void setDouble(double* a);
        QString getString();
        void setString(QString* a);
        QTime getTime();
        void setQTime(QTime* a);

        QDate getDate();
        void setDate(QDate* a);
        bool getBoolean();
        void setBoolean(bool* a);

        QString getLabel();
        void setLabel(QLabel* a);

    private:
        QString name;
        int* intp;
        double* doublep;
        QTime*  timep;
        QString* stringp;
        QLabel* labelp;

        QDate*  datep;
        bool* boolp;

};

#endif // DBUSFIELD_H
