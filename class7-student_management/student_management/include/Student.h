#ifndef STUDENT
#define STUDENT

#include <string>
using namespace std;

class Student{
public:
    int xuehao;
    string name;
    int age;
    int chengji;
    Student(){
        xuehao=0;
        name="";
        age=0;
        chengji=0;
    }
    Student(int,string,int,int);
};

#endif