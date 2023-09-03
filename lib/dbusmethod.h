#ifndef DBUSMETHOD_H
#define DBUSMETHOD_H

#include <QString>
#include <functional>


class DbusMethodInterface{
public:
    ~DbusMethodInterface() = default;
};


/**
Name: DbusMethod
Description: Add Method to DbusAPI
Example: DbusMethod<int(),int> = method return int with no paremter
*/
template<typename METHOD_TYPE,typename ReturnType,class... Ts>
class DbusMethod : public DbusMethodInterface {

    using target = std::function<METHOD_TYPE>;
    public:
        DbusMethod() {
        }
        void changePointer(target p) {
            pointer=p;
        }
        ReturnType call(const Ts... args) {
            return pointer(args...);
        }
    private:
        QString name;
        target pointer;

};


#endif // DBUSMETHOD_H
