#include <stdio.h>
#include <stdlib.h> 

// Define the node structure
struct node {
    int data;
    struct node* next;
};

// Change the passed in head pointer to be NULL
// Uses a reference pointer to access the caller's memory
void ChangeToNull(struct node** headRef) {
    *headRef = NULL; // use '*' to access the value of interest
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
    
    ChangeToNull(&head1); // use '&' to compute and pass a pointer to
    ChangeToNull(&head2); // the value of interest
    
    printf("After ChangeToNull:\n");
    printf("head1: %p\n", (void*)head1);
    printf("head2: %p\n", (void*)head2);
    
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
