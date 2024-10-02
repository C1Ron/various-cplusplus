#ifndef MYCLASS_HPP
#include <iostream>
#include <string>
#include <array>

class MyClass
{
public:
    MyClass();
    MyClass(int iVar);
    MyClass(double dVar);
    MyClass(std::string sVar);
    MyClass(int iVar, double dVar, std::string sVar);
    
    ~MyClass();

    void getValues();
private:
    int iVar;
    double dVar;
    std::string sVar;
};
#endif // MYCLASS_HPP