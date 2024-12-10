#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Task {
    int priority;
    char name[50];
    char description[200];
    int status;
    struct Task* next;
} Task;

void freeTaskList(Task* head) {
    Task* current;
    while (head) {
        current = head;
        head = head->next;
        free(current);
    }
}

Task* createTask(const char* name, const char* description, int priority, int status) {
    Task* newTask = (Task*)malloc(sizeof(Task));
    if (!newTask) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    strcpy(newTask->name, name);
    strcpy(newTask->description, description);
    newTask->priority = priority;
    newTask->status = status;
    newTask->next = NULL;
    return newTask;
}


Task* retrieve(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        return NULL;
    }

    Task* head = NULL;
    Task* tail = NULL;

    char line[300];
    while (fgets(line, sizeof(line), file)) {

        char* name = strtok(line, "#");
        char* description = strtok(NULL, "#");
        char* priorityStr = strtok(NULL, "#");
        char* statusStr = strtok(NULL, "\n");

        if (name && description && priorityStr && statusStr) {
            int priority = atoi(priorityStr);
            int status = atoi(statusStr);
            Task* newTask = createTask(name, description, priority, status);

            if (newTask) {

                if (head == NULL) {
                    head = newTask;
                    tail = newTask;
                } else {
                    tail->next = newTask;
                    tail = newTask;
                }
            }
        }
    }

    fclose(file);
    return head;
}


Task* tri(Task* head) {
    if (!head || !head->next) {
        return head;
    }

    Task* sorted = NULL;
    while (head) {
        Task** maxPtr = &head;
        Task* current = head;
        while (current->next) {
            if (current->next->priority > (*maxPtr)->priority) {
                maxPtr = &current->next;
            }
            current = current->next;
        }

        Task* maxTask = *maxPtr;
        *maxPtr = maxTask->next;

        maxTask->next = sorted;
        sorted = maxTask;
    }

    return sorted;
}


void displayTasks(Task* head) {
    Task* current = head;
    while (current) {
        printf("Name: %s\n", current->name);
        printf("Description: %s\n", current->description);
        printf("Priority: %d\n", current->priority);
        printf("-------------------\n");
        current = current->next;
    }
}


void addTask(Task** notStarted, Task** inProgress, Task** completed) {
    Task* newTask = malloc(sizeof(Task));
    if (!newTask) {
        printf("Memory allocation failed\n");
        return;
    }


    printf("Enter task name: ");
    scanf(" %[^\n]", newTask->name);
    printf("Enter task description: ");
    scanf(" %[^\n]", newTask->description);

    do {
        printf("Enter priority (1 - High, 2 - Medium, 3 - Low): ");
        scanf("%d", &newTask->priority);
    } while (newTask->priority < 1 || newTask->priority > 3);

    do {
        printf("Enter status (0 = not started, 1 = in progress, 2 = completed): ");
        scanf("%d", &newTask->status);
    } while (newTask->status < 0 || newTask->status > 2);

    newTask->next = NULL;


    switch (newTask->status) {
        case 0:
            newTask->next = *notStarted;
            *notStarted = newTask;
            break;
        case 1:
            newTask->next = *inProgress;
            *inProgress = newTask;
            break;
        case 2:
            newTask->next = *completed;
            *completed = newTask;
            break;
    }
}


void deleteTask(Task** head) {
    char name[100];
    printf("Enter the name of the task to delete: ");
    scanf(" %[^\n]", name);

    Task* current = *head;
    Task* prev = NULL;


    while (current != NULL && strcmp(current->name, name) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Task with name '%s' not found.\n", name);
        return;
    }


    if (prev == NULL) {
        *head = current->next;
    } else {
        prev->next = current->next;
    }


    free(current);
    printf("Task '%s' deleted successfully.\n", name);
}


void moveTask(Task** fromList, Task** toList) {
    char name[100];
    printf("Enter the name of the task to move: ");
    scanf(" %[^\n]", name);

    Task* current = *fromList;
    Task* prev = NULL;


    while (current != NULL && strcmp(current->name, name) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Task not found.\n");
        return;
    }


    if (prev == NULL) {
        *fromList = current->next;
    } else {
        prev->next = current->next;
    }


    current->next = *toList;
    *toList = current;

    printf("Task moved successfully!\n");
}



void save(const char* filename, Task* head) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file %s for writing\n", filename);
        return;
    }

    Task* current = head;
    while (current) {
        fprintf(file, "%s#%s#%d#%d\n",
            current->name,
            current->description,
            current->priority,
            current->status);
        current = current->next;
    }

    fclose(file);
}

int main() {
    int running = 1;

    while (running) {
        Task *l1, *l2, *l3;
        l1 = retrieve("todo.txt");
        l2 = retrieve("in_progress.txt");
        l3 = retrieve("done.txt");
        int choice;

        printf("\n--- Task Management System ---\n");
        printf("1 - Display task list\n");
        printf("2 - Add a task\n");
        printf("3 - Delete a task\n");
        printf("4 - Move a task\n");
        printf("5 - Exit\n");
        printf("Your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                l1 = tri(l1);
                l2 = tri(l2);
                l3 = tri(l3);
                printf("To do:\n");
                displayTasks(l1);
                printf("In progress:\n");
                displayTasks(l2);
                printf("Done:\n");
                displayTasks(l3);
                break;
            case 2:
                addTask(&l1, &l2, &l3);
                break;
            case 3:
                printf("Which list to delete from?\n");
                printf("1 - To do\n2 - In progress\n3 - Done\n");
                int list_choice;
                scanf("%d", &list_choice);
                switch (list_choice) {
                    case 1: deleteTask(&l1); break;
                    case 2: deleteTask(&l2); break;
                    case 3: deleteTask(&l3); break;
                    default: printf("Invalid list choice.\n");
                }
                break;
            case 4:
                printf("Move from which list?\n");
                printf("1 - To do to In progress\n");
                printf("2 - In progress to Done\n");
                printf("3 - Done to In progress\n");
                int move_choice;
                scanf("%d", &move_choice);
                switch (move_choice) {
                    case 1: moveTask(&l1, &l2); break;
                    case 2: moveTask(&l2, &l3); break;
                    case 3: moveTask(&l3, &l2); break;
                    default: printf("Invalid move choice.\n");
                }
                break;
            case 5:
                running = 0;
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }


        save("todo.txt", l1);
        save("in_progress.txt", l2);
        save("done.txt", l3);


        freeTaskList(l1);
        freeTaskList(l2);
        freeTaskList(l3);
    }

    return 0;
}
