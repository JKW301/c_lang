/**
 * 2 - GetNth()
 * p. 10
 */

#include <stdio.h>
#include <stdlib.h>

struct node{
    int data;
    struct node* next;
};

struct node* NewNode(int data){
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode -> data = data;
    newNode -> next = NULL;
    return newNode;
}

struct node* BuildOneTwoThree(){
    struct node* head = NewNode(1);
    head -> next = NewNode(2);
    head -> next -> next = NewNode(3);
    return head;
}

int GetNth(struct node* head, int index) {
    struct node* current = head;
    int count = 0;
    while (current != NULL) {
        if (count == index) {
            return current->data;
        }
        current = current->next;
        count++;
    }
    // If the index is out of bounds, return -1 (or handle as needed)
    return -1;
}

void GetNthTest(){
    struct node* myList = BuildOneTwoThree();
    int lastNode = GetNth(myList, 2);
    printf("The last node is %d\n", lastNode);
}

void printList(struct node* head){
    struct node* current = head;
    while(current !=NULL){
        printf("%d", current->data);
        current = current->next;
    }
    printf("\n");
}

//  function DeleteList() that takes a list, deallocates all of its memory and sets itshead pointer to NULL (the empty list)
void DeleteList(struct node** headRef){
    struct node* current = *headRef;
    struct node* next;
    while(current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    *headRef = NULL;
    if(myList == NULL){
        printf("List is empty\n");
    }
}

int main (int argc, char* argv[]){
    struct node* myList = BuildOneTwoThree();
    printf("List: ");
    printList(myList);
    GetNthTest();

    printf("Deletion of the list\n");
    DeleteList(&myList);
    
    
    return 0;
}
