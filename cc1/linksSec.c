/*
1 - Count() 
p. 10
*/


#include <stdio.h>
#include <stdlib.h>


struct node{
    int data;
    struct node* next;
};


// Function to create a new node
struct node* NewNode(int data) {
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to build the initial list {1, 2, 3}
struct node* BuildOneTwoThree() {
    struct node* head = NewNode(1);
    head->next = NewNode(2);
    head->next->next = NewNode(3);
    return head;
}

int Count(struct node* head, int searchFor){
    int count = 0;
    struct node* current = head;
    while(current != NULL){
        if(current->data == searchFor){
            count++;
        }
        current = current->next;
    }
    return count;
};

void countTest(){
    struct node* myList = BuildOneTwoThree();
    int count = Count(myList, 2);
};

/*
 Given a list and an int, return the number of times that int occurs
 in the list.
*/


void printList(struct node* head) {
    struct node* current = head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

int main (int argc, char *argv[]) {
    struct node* myList = BuildOneTwoThree();
    printf("List: ");
    printList(myList);

    int count = Count(myList, 2);
    printf("Count of 2: %d\n", count);

    return 0;
}