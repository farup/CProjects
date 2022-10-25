#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <unistd.h>

#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define PATH_MAX 100
#define INPUT_MAX 100

typedef struct node
/* Struct of elements (nodes) in linked list. */
{
    pid_t pid;
    char commandline[INPUT_MAX];
    struct Node *next;
} Node;

Node *create_new_node(pid_t pid, char *commandline)
/* Creates new node with pid and copy of cmd line.*/
{
    Node *new_node = malloc(sizeof(Node));
    new_node->pid = pid;
    strcpy(new_node->commandline, commandline);
    return new_node;
}

void add_node(Node **head, Node *new_node)
/* Add new node to linked list.
 If head is null, set new_node to head.*/
{
    if (head == NULL)
    {
        *head = new_node;
    }
    else
    {
        new_node->next = *head;
        *head = new_node;
    }
}

void print_linked_list(Node *head)
{
    Node *temp = head;
    while (temp->next != NULL)
    {
        printf("PID %d: %s\n", temp->pid, temp->commandline);
        temp = temp->next;
    }
}

int print_working_dir()
/**Prints working directory path*/
{
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        fflush(stdin);
        printf("%s: ", cwd);
        return 0;
    }
    else
    {
        perror("getcwd() error");
        return 1;
    }
}

int get_user_input(char *new_input)
/* Function for taking user input */
{
    int ret = scanf("%[^\n]24s", new_input); // Reads input, with whitespaces
    getchar();
    fflush(stdin);
    // int ret = fgets(new_input, sizeof(new_input), stdin);
    return ret;
}

void split_string(char *string_input, char **split_array, char *copy_string, FILE **file_input, FILE **file_output)
/* Splits input string by space or tab.
Checks if splits are I/O redirections (<, >).
    If so opens next split as a file descriptor in file_input.
Checks if end of commandline is &
    if so stops appending new splits to split_array
Returns pointer to array with pointer of to char */

{
    const char delimiter[] = "\t \n";

    int counter = 0;
    int end_arg = 0;

    char *split = strtok(string_input, delimiter);

    while (split != NULL)
    {
        if (strcmp(split, "<") == 0)
        {
            end_arg = 1;
            char *test = strtok(NULL, delimiter);
            if (!(*file_input = fopen(test, "r")))
            {
                exit(0);
            }
        }
        else if (strcmp(split, ">") == 0)
        {
            end_arg = 1;
            char *test = strtok(NULL, delimiter);
            if (!(*file_output = fopen(test, "w")))
            {
                exit(0);
            }
        }
        else if (strcmp(split, "&") == 0)
            end_arg = 1;
        if (!end_arg)
            split_array[counter] = split;

        counter++;
        split = strtok(NULL, delimiter);
    }
}

int background_task(char *copy_string)
/** Help function for background check */

{
    char temp;
    temp = copy_string[strlen(copy_string) - 1];

    if (strcmp(&temp, "&") == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int execute_command(char **split_array, char *copy_string, Node **head, FILE *file_input, FILE *file_output)
/* Forks and execute command. Checks if command line is background task (&)
If background task. Create new node and append to linked list. */
{
    pid_t pid = fork(); // Creating a child process. fork() returns 0 for child.
    int exit_status;

    if (pid < 0)
    {
        printf("Error creating process");
        return 2;
    }
    if (pid == 0)
    /*CHILD*/
    {
        if (file_input)
        {
            dup2(fileno(file_input), STDIN_FILENO); // Redirect standard input (0) to file_input
            fclose(file_input);
        }
        if (file_output)
        {
            dup2(fileno(file_output), STDOUT_FILENO);
            fclose(file_output);
        }
        execvp(split_array[0], split_array); // Executes command (slplit_array[0])
        exit(0);
    }

    else if (pid > 0)
    {
        /*PARENT */
        /* waits for the termination of the started process
        Checks if task is background task, if so does not wait before new prompts */

        if (background_task(copy_string))
        {
            printf("background from parent\n");
            waitpid(pid, &exit_status, WNOHANG);
            Node *new_node = create_new_node(pid, copy_string);
            add_node(head, new_node);
            // print_linked_list(*head);
            printf("background from  parent after \n");
        }

        else
        {
            if (waitpid(pid, &exit_status, 0) == -1) // Waits for child process with pid number "pid", before finish
            {
                printf("Error calling waitpid");
                exit(0);
            }
        }
        printf("Exit status [%s %s] = %d \n", split_array[0], split_array[1], exit_status);
    }
    return 0;
}

int main()
{

    char *copy_string[INPUT_MAX];
    char *buf[INPUT_MAX];
    char *temp;
    char **split_array = malloc(INPUT_MAX * sizeof(char)); // Pointer to array with pointers to char.  size of max input

    Node *head = NULL;
    Node *tail = NULL;
    FILE *file_input;
    FILE *file_output;

    while (1)
    {
        file_input = NULL;
        file_output = NULL;
        memset(split_array, 0, INPUT_MAX); // Clearing array

        print_working_dir();
        if (get_user_input(buf) == EOF)
        {
            printf("\n");
            exit(0);
        }

        strcpy(copy_string, buf);

        split_string(buf, split_array, copy_string, &file_input, &file_output);

        if (strcmp(split_array[0], "cd") == 0) // if user input is cd, the change dir
        {
            printf("Path  = %s\n", split_array[1]);
            chdir(split_array[1]);
            print_working_dir();
        }

        if (strcmp(split_array[0], "jobs") == 0) // if user input is job, the change dir
        {
            print_linked_list(head);
        }

        else
        {
            execute_command(split_array, copy_string, &head, file_input, file_output);
        }
    }
    return 0;
}