/**
 * 2 - GetNth()
 * p. 10
 */

// b+tree pour gérer les tables mutlples
// sanitizer, valgrind

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
}

void Push(struct node** headRef, int data) {
    struct node* newNode = NewNode(data);
    newNode->next = *headRef;
    *headRef = newNode;
}


/*
 The opposite of Push(). Takes a non-empty list
 and removes the front node, and returns the data
 which was in that node.
*/
int Length(struct node* head) {
    int count = 0;
    struct node* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

int Pop(struct node** headRef) {
    struct node* head = *headRef;
    int result = -1;
    if (head != NULL) {
        result = head->data;
        *headRef = head->next;
        free(head);
    }
    return result;
}

void PopTest() {
    struct node* head = BuildOneTwoThree(); // build {1, 2, 3}
    int a = Pop(&head); // deletes "1" node and returns 1
    int b = Pop(&head); // deletes "2" node and returns 2
    int c = Pop(&head); // deletes "3" node and returns 3
    int len = Length(head); // the list is now empty, so len == 0
    printf("a = %d, b = %d, c = %d, len = %d\n", a, b, c, len);
}

void InsertNth(struct node** headRef, int index, int data){
    if(index==0){
        Push(headRef, data);     
    } else {
        struct node* current = *headRef;
        for(int i = 0; i < index - 1; i++){
            if(current == NULL){
                return;
            }
            current = current->next;
        }
        if(current != NULL){
            struct node* newNode = NewNode(data);
            /**
             * Inserts a new node into the linked list after the current node.
            */
            newNode->next = current->next;
            current->next = newNode;
        }
    }
}

void InsertNthTest() {
    struct node* head = NULL; // start with the empty list
    InsertNth(&head, 0, 1); // build {13)
    InsertNth(&head, 1, 2); // build {13, 42}
    InsertNth(&head, 1, 8); // build {13, 5, 42}
    printf("List: ");
    printList(head);

    DeleteList(&head); // clean up after ourselves
}

int main (int argc, char* argv[]){
    struct node* myList = BuildOneTwoThree();
    printf("List: ");
    printList(myList);
    GetNthTest();

    //inserts
    InsertNthTest();

    
    printf("Deletion of the list\n");
    
    DeleteList(&myList);
        if(myList == NULL){
        printf("List is empty\n");
    }
    
    
    return 0;
}
