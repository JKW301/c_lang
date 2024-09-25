/*
p. 4
*/
#include <stdio.h>
#include <stdlib.h>

// Definition of the node structure
struct node {
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

// Function to push a new node at the front of the list
void Push(struct node** headRef, int data) {
    struct node* newNode = NewNode(data);
    newNode->next = *headRef;
    *headRef = newNode;
}

// Function to compute the length of the list
int Length(struct node* head) {
    int count = 0;
    struct node* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void BasicsCaller() {
    struct node* head;
    int len;
    head = BuildOneTwoThree(); // Start with {1, 2, 3}
    Push(&head, 13); // Push 13 on the front, yielding {13, 1, 2, 3}
    // (The '&' is because head is passed
    // as a reference pointer.)
    Push(&(head->next), 42); // Push 42 into the second position
    // yielding {13, 42, 1, 2, 3}
    // Demonstrates a use of '&' on
    // the .next field of a node.
    // (See technique #2 below.)
    len = Length(head); // Computes that the length is 5

    // Print the list and its length
    struct node* current = head;
    printf("List: ");
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\nLength: %d\n", len);
}

int main() {
    BasicsCaller();
    return 0;
}
