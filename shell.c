/***************************************************************
 * Name of Program: shell
 * Aurhor: James McClain
 * Description: Just a basic shell program that allows the user
 *              to run other terminal programs.
 ***************************************************************/

// use stdbool.h to basically convert 'true' to 1,
// and 'false' to 0.
#include <ctype.h>
#include <stdbool.h>
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
int isEmpty(char const *str);
pid_t execArgv(char *const args[]);

int main() {
  pid_t pid;
  char cmd[MAX_LINE];
  char *token = NULL;
  char *myArgv[ARGV_SIZE];
  char tok[3] = " \n";
  int argNum;

  while (1) {
    /* Get User input */
    printf("prompt$ ");

    // Error checking for 'fgets'.
    // This also makes sure gcc doesn't compliain about fgets' return value.
    if (fgets(cmd, MAX_LINE, stdin) != NULL) {
      // If no input is provided. (or is just whitespace)
      // go to the beginning of the while loop.
      if (isEmpty(cmd)) {
        continue;
      }
    } else {
      printf("Failed to get input.\n");
    }

    // Set argNum to 0, and clear myArgv.
    argNum = 0;
    memset(myArgv, 0, sizeof(myArgv));

    /* Convert input to array of strings */
    // Split string at each 'space', and 'newline'.
    token = strtok(cmd, tok);

    // Assign each token to myArgv array.
    while (token != NULL) {
      myArgv[argNum] = token;
      token = strtok(NULL, tok);
      argNum++;
    }

    /* Build-in commands */
    if (strncmp(myArgv[0], "exit", 4) == 0) {
      printf("Exiting!\n");
      exit(0);
    }

    pid = execArgv(myArgv);
  }

  return 0;
}

// Return true if input string is only whitespace.
// otherwise return false.
int isEmpty(char const *str) {
  while (*str != '\n') {
    if (!isspace(*str)) {
      return false;
    }
    str++;
  }
  return true;
}

// Moved fork and execvp into it's own function
// that returns the pid from execvp.
// Hopefully this will make 'showpid' easier to
// implement.
pid_t execArgv(char *const args[]) {
  pid_t pid;
  if ((pid = fork()) == 0) {
    execvp(args[0], args);
    printf("Error: Command could not be executed\n");
    exit(0);
  } else {
    waitpid(-1, NULL, 0);
  }
  return pid;
}
