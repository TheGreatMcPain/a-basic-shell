#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 80
#define ARGV_SIZE 10

// Prototypes
int isEmpty(const char *str);

int main() {
  pid_t pid;
  char cmd[MAX_LINE];
  char *token = NULL;
  char *myArgv[ARGV_SIZE];
  int argNum;

  while (1) {
    /* Get User input */
    printf("prompt$ ");
    // send the return value to the void.
    // Tells compiler to ignore fgets' return value.
    (void)(fgets(cmd, MAX_LINE, stdin) + 1);

    // If no input is provided.
    // go to the beginning of the while loop.
    if (isEmpty(cmd)) {
      continue;
    }

    // Set argNum to 0, and clear myArgv.
    argNum = 0;
    memset(myArgv, 0, sizeof(myArgv));

    /* Convert input to array of strings */
    token = strtok(cmd, " \n");

    // Assign each token to myArgv array.
    while (token != NULL) {
      myArgv[argNum] = token;
      token = strtok(NULL, " \n");
      argNum++;
    }

    /* Build-in commands */
    if (strncmp(myArgv[0], "exit", 4) == 0) {
      printf("Exiting!\n");
      exit(0);
    }

    // Fork process and exec myArgv.
    if ((pid = fork()) == 0) {
      execvp(myArgv[0], myArgv);
      printf("Error: Command could not be executed\n");
      exit(0);
    } else {
      waitpid(-1, NULL, 0);
    }
  }

  return (0);
}

// Return 1 if input string is only whitespace.
// otherwise return 0.
int isEmpty(const char *str) {
  while (*str != '\n') {
    if (!isspace(*str)) {
      return 0;
    }
    str++;
  }
  return 1;
}
