#include "MyClass.hpp"

MyClass::MyClass() : iVar(3), dVar(3.14), sVar("Default") 
{
    // Empty constructor body
}

MyClass::MyClass(double dVar) : iVar(3), dVar(dVar), sVar("Default")
{
    // Empty constructor body
}

MyClass::MyClass(std::string sVar) : iVar(3), dVar(3.14), sVar(sVar)
{
    // Empty constructor body
}

MyClass::MyClass(int iVar, double dVar, std::string sVar) : iVar(iVar), dVar(dVar), sVar(sVar)
{
    // Empty constructor body
}

MyClass::~MyClass()
{
    std::cout << "Destructor called" << std::endl;
}

void MyClass::getValues()
{
    std::cout << "iVar: " << iVar << std::endl;
    std::cout << "dVar: " << dVar << std::endl;
    std::cout << "sVar: " << sVar << std::endl;
}

