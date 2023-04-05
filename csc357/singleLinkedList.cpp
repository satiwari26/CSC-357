#include <iostream>
#include <stdio.h>
using namespace std;

//creating a single linked list nodes

struct node{
    int value;  //value stored in the node
    node * link; //link to connet the next node
};

void endInsertion(node * head, node * temp);
void begInsertion(node **head, node * temp);


void newNode(int value, node ** head){
    node *temp; // to store the head of the node and to store the temp addr of the new node

    bool firstNode = false; //bool to check if it is first node

    temp = new node; //creating new node on heap

    if(*head == NULL){   //head will point to the first node
        *head = temp;   //deref the head to give access to ptr head which now points to temp
        firstNode = true;
    }
    else{
        firstNode = false;
    }

    temp->value = value; //storing the 400 value on the new node
    temp->link = NULL; //the link points to null

    if(firstNode == false){         //if first node do not do the end insertion.
    endInsertion(*head,temp);
   // begInsertion(head,temp);
    }
}

void begInsertion(node **head, node * temp){
    temp->link = *head; //points to the begning of the node
    *head = temp;   //head points to the new temp node
}

void endInsertion(node * head, node * temp){

    node * temp1 = head; //temp1 starts from the head node

    //traversing through the nodes to get to the end node
    while(temp1->link != NULL){ //checking if there is next node
        temp1 = temp1->link;    //if there is set the pointer value to the next node
    }
    //once at the last node set the link of the last node to the new node
    temp1->link = temp;

    //new node added at the end of the list
}

void printVal(node * head){
    node * point = head;
    while(point !=NULL){
        cout<<point->value<<" ";
        point = point->link;
        
    }
    cout<<endl;
}

int main(){

    node * head = NULL;

    newNode(200,&head); //passing the addr of ptr head

    newNode(300,&head);
    newNode(400,&head);

    newNode(500,&head);
    newNode(600,&head);
    newNode(700,&head);

    printVal(head);


    // node * head, *temp; // to store the head of the node and to store the temp addr of the new node
    // head = NULL;

    // temp = new node; //creating new node on heap

    // temp->value = 400; //storing the 400 value on the new node
    // temp->link = NULL; //the link points to null

    //endInsertion(head,temp); // calling the end Insertion function 

    return 0;
}