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
#define MAX_SHOWPID 5

// Prototypes
int isEmpty(char const *str);
void changeDir(char *const args[]);
pid_t execArgv(char *const args[]);

int main() {
  pid_t pid;
  char cmd[MAX_LINE];
  char *myArgv[ARGV_SIZE];
  char *token = NULL;
  char tok[3] = " \n";
  int pids[MAX_SHOWPID];
  int argNum = 0;
  int i;

  memset(pids, 0, sizeof(pids));

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
    } else if (strncmp(myArgv[0], "cd", 2) == 0) {
      changeDir(myArgv);
    } else if (strncmp(myArgv[0], "showpid", 7) == 0) {
      for (i = 0; i < MAX_SHOWPID; i++) {
        if (pids[i] != 0) {
          printf("%d\n", pids[i]);
        }
      }
    } else {
      // Run non-builtin, and store pid in pid array.
      pid = execArgv(myArgv);
      // if execArgv returns 0 don't add it to pid array.
      if (pid != 0) {
        for (i = 1; i < MAX_SHOWPID; i++) {
          pids[i - 1] = pids[i];
        }
        pids[MAX_SHOWPID - 1] = pid;
      }
    }
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

// Function for 'cd'
void changeDir(char *const args[]) {
  char *home = getenv("HOME");
  char newDir[MAX_LINE] = "\0";
  unsigned long i = 0;

  if (args[1] == NULL) {
    for (i = 0; i < strlen(home); i++) {
      newDir[i] = home[i];
    }
    // if the first character in the directory
    // is a '~' change it to the HOME directory.
  } else if (strncmp(args[1], "~", 1) == 0) {
    strcat(newDir, home);
    while (args[1][i + 1] != '\0') {
      newDir[strlen(home) + i] = args[1][i + 1];
      i++;
    }
    // Otherwise just put the directory
    // into newDir.
  } else {
    for (i = 0; args[1][i] != '\0'; i++) {
      newDir[i] = args[1][i];
    }
  }
  if (chdir(newDir) == 0) {
    setenv("PWD", newDir, 1);
  } else {
    printf("\"%s\" does not exist.\n", newDir);
  }
}

// Moved fork and execvp into it's own function
// that returns the pid from execvp.
// Hopefully this will make 'showpid' easier to
// implement.
pid_t execArgv(char *const args[]) {
  pid_t pid;
  // Used for return value of execvp.
  int error = 0;
  // Create pipe that will allow child to
  // send the return value of execvp to parent.
  // pipefd[0] is the read end, and
  // pipefd[1] is the write end.
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    printf("Failed to create pipe.\n");
  }

  if ((pid = fork()) == 0) {
    close(pipefd[0]);  // prevent child from writing to read end.
    error = execvp(args[0], args);
    // write execvp's return value to pipe.
    if (write(pipefd[1], &error, sizeof(error)) == -1) {
      printf("Write to pipe failed.\n");
    }
    close(pipefd[1]);  // close pipe when done.
    printf("Error: Command could not be executed\n");
    exit(0);
  } else {
    close(pipefd[1]);  // prevent parent from reading the write end.
    // read the value that the child wrote, and put it in 'error'.
    if (read(pipefd[0], &error, sizeof(error)) == -1) {
      printf("Read from pipe failed.\n");
    }
    close(pipefd[0]);  // close pipe when done.
    waitpid(-1, NULL, 0);
  }

  // If execvp fails return 0
  if (error == -1) {
    return 0;
  } else {
    return pid;
  }
}
