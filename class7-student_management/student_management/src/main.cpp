#include "Student.h"
#include "StudentManager.h"

#include <iostream>
using namespace std;

void menu(){
    printf("=============WELCOME TO THE STUDENT MANAGER SYSTEM======================\n");
    printf("input 1 to add information\n");
    printf("input 2 to show all the informations\n");
    printf("input 3 to find a student's information through his xuehao\n");
    printf("input 4 to compute all the students' average grades");
    printf("input 5 to exit");
}

int main(){
    StudentManager manager;
    int choice;

    do{
        menu();
        cin>>choice;
        switch(choice){
            case 1:{
                int xuehao;
                string name;
                int age;
                int chengji;
                cin>>xuehao>>name>>age>>chengji;
                manager.addInformation(xuehao,name,age,chengji);
                break;
            }
            case 2:{
                manager.show();
                break;
            }
            case 3:{
                int xuehao;
                cin>>xuehao;
                manager.find(xuehao);
                break;
            }
            case 4:{
                manager.average();
                break;
            }
            case 5:{
                break;
            }
            default:{
                printf("please check your number and input again\n");
            }
        }
    }while (choice != 5);

    return 0;
    
}