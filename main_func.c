#include "myshell.h"

void execute_command(char *command, char *arguments[]) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0) { // Child process
        if (execve(command, arguments, NULL) == -1) {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("shell");
    } else { // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

int main() {
    char *input = NULL;
    size_t input_size = 0;
    char *arguments[MAX_ARGUMENTS];

    // Check if input is coming from a pipe
    if (!isatty(STDIN_FILENO)) {
        if (getline(&input, &input_size, stdin) == -1) {
            perror("getline");
            free(input);
            exit(EXIT_FAILURE);
        }

        // Remove the trailing newline character
        input[strcspn(input, "\n")] = '\0';

        // Tokenize the input
        char *token = strtok(input, " ");
        int i = 0;

        while (token != NULL && i < MAX_ARGUMENTS - 1) {
            arguments[i] = token;
            i++;
            token = strtok(NULL, " ");
        }

        arguments[i] = NULL; // Set the last element to NULL for execve

        if (i > 0) {
            execute_command(arguments[0], arguments);
        }

        free(input);
        exit(EXIT_SUCCESS);
    }

    // Interactive shell
    while (1) {
        write(STDOUT_FILENO, "shell> ", 7);
        if (getline(&input, &input_size, stdin) == -1) {
            // Handle EOF (Ctrl + D) gracefully
            write(STDOUT_FILENO, "\n", 1);
            free(input);
            break;
        }

        // Remove the trailing newline character
        input[strcspn(input, "\n")] = '\0';

        // Tokenize the input
        char *token = strtok(input, " ");
        int i = 0;

        while (token != NULL && i < MAX_ARGUMENTS - 1) {
            arguments[i] = token;
            i++;
            token = strtok(NULL, " ");
        }

        arguments[i] = NULL; // Set the last element to NULL for execve

        if (i > 0) {
            if (strcmp(arguments[0], "exit") == 0) {
                // Exit the shell
                free(input);
                exit(EXIT_SUCCESS);
            }

            execute_command(arguments[0], arguments);
        }
    }

    free(input);
    return 0;
}
