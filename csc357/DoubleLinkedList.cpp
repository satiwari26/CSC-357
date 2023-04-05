#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

struct listElement{
    char text[1000];    //char array to store the string
    listElement * next, *prev; //pointer of the node to store the next and prev links
};


listElement * head = NULL; //global head pointer to point to the nodes

void newNode(char value[1000]);
void printList();
void deleteList(int nodeNumber);

void newNode(char value[1000]){
    bool firstNode = false;
    listElement * temp; //temp pointer to hold the address of the new node
    temp = new listElement; //creating a new node on the heap
    if(head == NULL){
        head = temp;    //head points to the new node
        firstNode = true;
    }
    else{
        firstNode = false; // to prevent linking of the node to itself;
    }

    strcpy(temp->text,value);   //copy the entire string from array value to array text.
    temp->next = NULL;
    temp->prev = NULL;

    //temp1 is declared to traverse through the list
    listElement *temp1 = head;
    if(firstNode==false){
        while(temp1->next != NULL){   //checking if the next node is present
            temp1 = temp1->next;    //if it is setting the temp1 to the next node
        }
        temp1->next = temp;  //linking the last node to new node
        temp->prev = temp1; //linking the new node prev to last node
    }
}

void printList(){
    listElement * temp = head;  //temp pointer to traverse through the list
    if(temp == NULL){
        cout<<"There are no List elements."<<endl;
    }
    else{
        while(temp != NULL){    //print until temp is NULL
            cout<<temp->text<<endl;  //dereferncing itself and getting the value of the current node its on
            temp = temp->next;  //after printing the current value of the node move temp to the next node.
        }

        cout<<endl; //to exit it to new line.
    }
}

void deleteList(int nodeNumber){
    bool checker = false;   //to check if the corresponding number of node on the list

    if(head == NULL){
        cout<<"There are no List Elements."<<endl;
    }
    else{
        listElement * curr = head;
        //listElement * temp = NULL;
        int count =1;
        while(curr !=NULL){
            if(nodeNumber == count){
                if(nodeNumber==1){
                    head = curr->next;
                    if(curr->next != NULL){
                        curr->next->prev = NULL;
                    }
                    //temp = curr;
                    //curr = curr->next;
                    delete(curr);
                    checker = true;
                    break;
                }
                else{
                    if(curr->next ==NULL){  //if the last node set previous node next to null
                        curr->prev->next = NULL;
                        delete (curr);
                        checker = true;
                        break;
                    }
                    else{
                    curr->prev->next = curr->next;  //connecting the previous(next) to current next
                    curr->next->prev = curr->prev; //connecting the next(previous) to current previous
                    delete (curr);
                    checker = true;
                    break;
                    }
                }
            }
            else{
                curr = curr->next;
                count = count+1;
            }
        }
        if(checker == false){
            cout<<"The corresponding number node doesn't exist on the list"<<endl;
        }

    }
}

int main(){

    // char text[1000];
    // //strcpy(text,"Cruel_hero");
    // newNode(text);

    newNode("Saumitra");
    newNode("Dhruv");
    newNode("JUNior");
    newNode("LOLVV");
    newNode("Saumitra");
    newNode("Dhruv");
    newNode("JUNior");

    printList();

    deleteList(1);

    printList();

    return 0;
}