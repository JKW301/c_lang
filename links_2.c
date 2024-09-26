#include <stdio.h>
#include <stdlib.h> 

// Define the node structure
struct node {
    int data;
    struct node* next;
    // struct node* next means that next is a pointer to a node
};

// Change the passed in head pointer to be NULL
// Uses a reference pointer to access the caller's memory
void ChangeToNull(struct node** headRef) {
    *headRef = NULL; // use '*' to access the value of interest
    // headref is a reference pointer to the caller's memory
}

void ChangeCaller() {
    struct node* head1 = (struct node*)malloc(sizeof(struct node));
    struct node* head2 = (struct node*)malloc(sizeof(struct node));
    
    head1->data = 1;
    head1->next = NULL;
    
    head2->data = 2;
    head2->next = NULL;
    
    printf("Before ChangeToNull:\n");
    printf("head1: %p\n", (void*)head1);
    printf("head2: %p\n", (void*)head2);
    // %p is used to print the pointer value
    
    ChangeToNull(&head1); // use '&' to compute and pass a pointer to
    ChangeToNull(&head2); // the value of interest
    
    printf("After ChangeToNull:\n");
    printf("head1: %p\n", (void*)head1);
    printf("head1: %d\n", head1->data); //return the same value as before
    printf("head2: %p\n", (void*)head2);
    // (void*)head1 is used to cast the pointer to a void pointer
    // without this, the compiler will throw a warning
    
    // Free the allocated memory (though it's NULL now, it's good practice)
    free(head1);
    free(head2);

    printf("After freeing the memory :\nhead1: %p\n", (void*)head1);
    printf("head2: %p\n", (void*)head2);
}

int main() {
    ChangeCaller();
    return 0;
}
