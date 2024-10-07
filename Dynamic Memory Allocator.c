#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
UserId=Moinul
Password=Moinul909
*/

// Define login credentials
#define USER_ID "Moinul"
#define PASSWORD "Moinul909"

// Node structure to manage memory blocks
typedef struct Block {
    size_t size;          // Size of the memory block
    int free;             // 1 if the block is free, 0 if it is occupied
    struct Block *next;   // Pointer to the next block in the linked list
} Block;

// Head pointer for the linked list of memory blocks
Block *head = NULL;
Block *tail = NULL;

// Define the total size of the memory pool
#define MEMORY_POOL_SIZE 1024 * 1024  // 1 MB
char memoryPool[MEMORY_POOL_SIZE];    // Memory pool for the custom allocator

// Function to log in with credentials
int login() {
    char inputUserId[50];
    char inputPassword[50];

    printf("Enter User ID: ");
    scanf("%s", inputUserId);

    printf("Enter Password: ");
    scanf("%s", inputPassword);

    // Check if the entered credentials match the predefined credentials
    if (strcmp(inputUserId, USER_ID) == 0 && strcmp(inputPassword, PASSWORD) == 0) {
        printf("Login successful!\n\n");
        return 1;  // Return 1 for successful login
    } else {
        printf("Invalid User ID or Password.\n");
        return 0;  // Return 0 for failed login
    }
}

// Initialize the memory pool with a single free block
void initializeMemoryPool() {
    head = (Block*)memoryPool;  // Head points to the beginning of the memory pool
    head->size = MEMORY_POOL_SIZE - sizeof(Block);
    head->free = 1;  // The block is initially free
    head->next = NULL;
    tail = head;     // Tail points to the same initial block
}

// Split a block into two parts if there is enough remaining space after allocation
void splitBlock(Block *fittingBlock, size_t size) {
    Block *newBlock = (Block*)((char*)fittingBlock + sizeof(Block) + size);
    newBlock->size = fittingBlock->size - size - sizeof(Block);
    newBlock->free = 1;
    newBlock->next = fittingBlock->next;

    fittingBlock->size = size;
    fittingBlock->free = 0;
    fittingBlock->next = newBlock;

    if (fittingBlock == tail) {
        tail = newBlock;
    }
}

// Allocate memory using custom allocator (similar to malloc)
void* my_malloc(size_t size) {
    Block *curr = head;

    // Find a free block that is large enough
    while (curr != NULL && !(curr->free && curr->size >= size)) {
        curr = curr->next;
    }

    if (curr == NULL) {
        printf("Error: Memory allocation failed. Not enough space.\n");
        return NULL;  // No suitable block found
    }

    // If there is enough space left after allocation, split the block
    if (curr->size > size + sizeof(Block)) {
        splitBlock(curr, size);
    } else {
        curr->free = 0;  // Mark the block as occupied
    }

    return (void*)((char*)curr + sizeof(Block));  // Return pointer to the allocated memory space
}

// Merge adjacent free blocks to avoid fragmentation
void mergeBlocks() {
    Block *curr = head;

    while (curr != NULL && curr->next != NULL) {
        if (curr->free && curr->next->free) {  // Merge adjacent free blocks
            curr->size += curr->next->size + sizeof(Block);
            curr->next = curr->next->next;

            if (curr->next == NULL) {
                tail = curr;
            }
        } else {
            curr = curr->next;
        }
    }
}

// Free allocated memory (similar to free)
void my_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    Block *blockPtr = (Block*)((char*)ptr - sizeof(Block));
    blockPtr->free = 1;  // Mark the block as free
    mergeBlocks();       // Merge adjacent free blocks to prevent fragmentation
}

// Display memory block status for debugging purposes
void displayMemoryBlocks() {
    Block *curr = head;
    printf("\nMemory Blocks Status:\n");
    printf("Address\t\tSize\tFree/Occupied\n");
    while (curr != NULL) {
        printf("%p\t%zu\t%s\n", (void*)curr, curr->size, curr->free ? "Free" : "Occupied");
        curr = curr->next;
    }
    printf("\n");
}

// Main function
int main() {
    // Login mechanism
    if (login()) {
        printf("Access granted. You can use the custom dynamic memory allocator.\n\n");

        initializeMemoryPool();  // Initialize the memory pool

        // Example allocations and deallocations using custom allocator
        printf("Allocating 200 bytes...\n");
        void *p1 = my_malloc(200);
        displayMemoryBlocks();

        printf("Allocating 500 bytes...\n");
        void *p2 = my_malloc(500);
        displayMemoryBlocks();

        printf("Freeing the first allocated block...\n");
        my_free(p1);
        displayMemoryBlocks();

        printf("Allocating 100 bytes...\n");
        void *p3 = my_malloc(100);
        displayMemoryBlocks();

        printf("Freeing the second and third allocated blocks...\n");
        my_free(p2);
        my_free(p3);
        displayMemoryBlocks();
    } else {
        printf("Access denied. Exiting program.\n");
    }

    return 0;
}

