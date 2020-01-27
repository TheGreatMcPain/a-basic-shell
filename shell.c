#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 80
#define ARGV_SIZE 10

int main() {
  pid_t pid;
  char cmd[MAX_LINE];
  char *token = NULL;
  char *myArgv[ARGV_SIZE];
  int argNum;

  while (1) {
    // Set argNum to 0, and clear myArgv.
    argNum = 0;
    memset(myArgv, 0, sizeof(myArgv));

    /* Get Use input */
    printf("prompt$ ");
    fgets(cmd, MAX_LINE, stdin);

    /* Exit? */
    if (strncmp(cmd, "exit", 4) == 0) {
      printf("Exiting!\n");
      break;
    }

    // Remove '\n' from string.
    cmd[strcspn(cmd, "\n")] = 0;

    /* Do something with input */
    token = strtok(cmd, " ");

    // Assign each tokens to myArgv array.
    while (token != NULL) {
      myArgv[argNum] = token;
      token = strtok(NULL, " ");
      argNum++;
    }

    // Fork process and exec myArgv.
    if ((pid = fork()) == 0) {
      execvp(myArgv[0], myArgv);
    } else {
      waitpid(-1, NULL, 0);
    }
  }

  return (0);
}
