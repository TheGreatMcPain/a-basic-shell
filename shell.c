#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_LINE 80
#define ARGV_SIZE 10

int main() {
  char cmd[MAX_LINE];
  char *token = NULL;
  char *myArgv[ARGV_SIZE];
  int argNum;

  while (1) {
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
      printf("%s\n", token);
      token = strtok(NULL, " ");
      argNum++;
    }
  }

  return (0);
}
