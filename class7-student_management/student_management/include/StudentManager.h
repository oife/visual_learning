#ifndef STUDENT_MANAGER
#define STUDENT_MANAGER

#include "Student.h"
#include <vector>
using namespace std;

class StudentManager{
public:
    vector<Student> students;

    void addInformation(int xuehao,string name,int age,int chengji);
    void show();
    void find(int xuehao);
    void average();
};

#endif