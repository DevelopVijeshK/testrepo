
#include <string>
#include <iostream>

#pragma once
class LearnClass {

	private:
                int age;
                std::string name;
	public:
                        LearnClass      ();
                int addmodifyage (int pAge);
                void printdetails ();
};

LearnClass::LearnClass ()
{
            age  = 20;
            name = "Raju";
 }

int
     LearnClass::addmodifyage  (int pAge)
    {
        age = pAge + 2;
        return age;
    } 

void
    LearnClass::printdetails  ()
    {
    //std::printf ("name: %s, age: % d \n", name, age.c_str());
        std::cout << "name:" << name << "\nage:" << age<<"\n==========================\n";

    }