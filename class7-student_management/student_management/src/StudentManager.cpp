#include "StudentManager.h"
#include <string>
using namespace std;

void StudentManager::addInformation(int xuehao,string name,int age,int chengji){
    Student x;
    x.xuehao = xuehao;
    x.age = age;
    x.name = name;
    x.chengji = chengji;
    students.push_back(x);
}
void StudentManager::show(){
    for(const auto& x:students){
        printf("the student's information:\nxuehao:%d\nname:%s\nage:%d\nchengji:%d\n\n",x.xuehao,x.name.c_str(),x.age,x.chengji);
    }
}
void StudentManager::find(int xuehao){
    for(const auto &i : students){
        if(xuehao == i.xuehao){
            printf("the student's information:\nxuehao:%d\nname:%s\nage:%d\nchengji:%d\n\n",i.xuehao,i.name.c_str(),i.age,i.chengji);
            return;
        }
    }
    printf("sorry,please check you had input the right xuehao");
    return;
}
void StudentManager::average(){
    int ssum=0;
    int num=0;
    for(const auto &i : students){
        ssum+=i.chengji;
        num++;
    }
    printf("%.2f",ssum*1.0/num);
}