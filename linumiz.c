#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

// Define a linked list node to represent a directory stack entry
typedef struct Node {
    char *path;          // Directory path
    struct Node *next;   // Pointer to the next node in the stack
} Node;

Node *stack = NULL;  // Initialize the stack as empty

// Push a directory onto the stack
void push(const char *path) {
    Node *newNode = (Node *)malloc(sizeof(Node));  // Allocate memory for a new node
    if (newNode == NULL) {
        perror("Unable to allocate memory for new node");  // Print error message if allocation fails
        exit(EXIT_FAILURE);
    }
    newNode->path = strdup(path);  // Duplicate the path string
    newNode->next = stack;         // Link the new node to the current top of the stack
    stack = newNode;               // Update the stack to point to the new node
}

// Pop a directory from the stack
char *pop() {
    if (stack == NULL) {
        return NULL;  // Return NULL if the stack is empty
    }
    Node *temp = stack;  // Store the top node
    char *path = temp->path;  // Get the path from the top node
    stack = stack->next;  // Move the stack top to the next node
    free(temp);  // Free the memory of the popped node
    return path;  // Return the path
}

// Get the current working directory
char *get_cwd() {
    char *cwd = (char *)malloc(PATH_MAX);  // Allocate memory for the current working directory path
    if (cwd == NULL) {
        perror("Unable to allocate memory for current working directory");  // Print error message if allocation fails
        exit(EXIT_FAILURE);
    }
    if (getcwd(cwd, PATH_MAX) == NULL) {
        perror("getcwd() error");  // Print error message if getcwd fails
        free(cwd);
        exit(EXIT_FAILURE);
    }
    return cwd;
}

// Change to the specified directory and push the old directory onto the stack
void pushd(const char *dir) {
    char *cwd = get_cwd();  // Get the current working directory
    if (chdir(dir) != 0) {
        perror("chdir() error");  // Print error message if chdir fails
        free(cwd);
        return;
    }
    push(cwd);  // Push the old working directory onto the stack
    free(cwd);  // Free the memory of the old working directory path
    cwd = get_cwd();  // Get the new current working directory
    printf("%s\n", cwd);  // Print the new working directory
    free(cwd);  // Free the memory of the new working directory path
}

// Pop a directory from the stack and change to that directory
void popd() {
    char *path = pop();  // Pop the top directory from the stack
    if (path == NULL) {
        fprintf(stderr, "popd: directory stack empty\n");  // Print error message if the stack is empty
        return;
    }
    if (chdir(path) != 0) {
        perror("chdir() error");  // Print error message if chdir fails
        free(path);
        return;
    }
    printf("%s\n", path);  // Print the current working directory after the change
    free(path);  // Free the memory of the popped directory path
}

// Main function to handle command line arguments
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [directory]\n", argv[0]);  // Print usage message if arguments are insufficient
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "pushd") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s pushd <directory>\n", argv[0]);  // Print usage message for pushd command
            exit(EXIT_FAILURE);
        }
        pushd(argv[2]);  // Call pushd with the specified directory
    } else if (strcmp(argv[1], "popd") == 0) {
        popd();  // Call popd to change to the directory on top of the stack
    } else {
        fprintf(stderr, "Invalid command. Use 'pushd' or 'popd'.\n");  // Print error message for invalid command
        exit(EXIT_FAILURE);
    }

    return 0;
}
