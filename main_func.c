#include "myshell.h"

/**
 * execute_command - function to handle command execution
 * @command: command to be handled
 * @arguments: an array of arguments
 */
void execute_command(char *command, char *arguments[])
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0)
	{ /* Child process */
		if (execve(command, arguments, NULL) == -1)
			perror("shell");
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
		perror("shell");
	else
	{ /* Parent process */
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}
/**
 * gets_input - handles user input
 * @input: user's input
 * @input_size: size of input
 */
void gets_input(char *input, size_t input_size)
{
	if (getline(&input, &input_size, stdin) == -1)
	{
		if (feof(stdin)) /* Handle EOF (Ctrl + D) gracefully */
		{
			write(STDOUT_FILENO, "\n", 1);
			free(input);
			exit(EXIT_SUCCESS);
		}
		else
		{
			perror("fgets");
			exit(EXIT_FAILURE);
		}
		/* Remove the trailing new line character */
		input[strcspn(input, "\n")] = '\0';
	}
}
/**
 * _strtok - tokenizes the input string
 * @input: pointer to the string
 * @delim: pointer to the delimiter
 *
 * Return: NULL
 */
char *_strtok(char *input, const char *delim)
{
	char *token = strtok(input, delim);
	char *arguments[MAX_ARGUMENTS];
	int i = 0;

	while (token != NULL && i < MAX_ARGUMENTS - 1)
	{
		arguments[i] = token;
		i++;
		token = strtok(NULL, delim);
		/* Set the last element to NULL for execve */
		arguments[i] = NULL;
	}
	return (arguments[i]);
}
/**
 * main - entry point
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: an integer
 */
int main(int argc, char *argv[])
{
	int i = 0;
	size_t len = strlen(argv[0]), input_size = 0;
	char *input = NULL, *delim = " ";
	char *prompt = (char *)malloc(len + 2);
	char *arguments[MAX_ARGUMENTS];
	(void) argc;

	if (!isatty(STDIN_FILENO))
	{
		if (getline(&input, &input_size, stdin) == -1)
		{
			perror("getline");
			free(input);
			exit(EXIT_FAILURE);
		}
		input[strcspn(input, "\n")] = '\0';
		_strtok(input, delim);
		if (i > 0)
			execute_command(arguments[0], arguments);
		free(input);
		exit(EXIT_SUCCESS);
	}
	strcpy(prompt, argv[0]);
	strcat(prompt, "$ ");
	while (1) /* interactive shell */
	{
		write(STDOUT_FILENO, prompt, 7);
		gets_input(input, input_size);
		_strtok(input, delim); /* Tokenize the input */
		if (i > 0)
		{
			if (strcmp(arguments[0], "exit") == 0)
			{ /* Exit the shell */
				free(input);
				exit(EXIT_SUCCESS);
			}
			execute_command(arguments[0], arguments);
		}
	}
	return (0);
}
