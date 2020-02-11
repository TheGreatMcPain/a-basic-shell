/***************************************************************
 * Name of Program: shell
 * Aurhor: James McClain
 * Description: Just a basic shell program that allows the user
 *              to run other terminal programs.
 ***************************************************************/

// use stdbool.h to basically convert 'true' to 1,
// and 'false' to 0.
#include <ctype.h>
#include <limits.h>
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
#define MAX_HIS 5
#define MAX_SHOWPID 5

// Prototypes
int isEmpty(char const *str);
int isNumber(char const *str);
void strToArgv(char *argv[], char str[]);
int lastCmd(char history[][MAX_LINE], char *str);
void changeDir(char *const args[]);
pid_t execArgv(char *const args[]);

int main() {
  pid_t pid;
  bool success = true;
  int lc;
  char ch;
  char cmd[MAX_LINE];
  char cmdCopy[MAX_LINE];
  char *myArgv[ARGV_SIZE];
  char history[MAX_HIS][MAX_LINE];
  int pids[MAX_SHOWPID];
  int i;

  memset(pids, 0, sizeof(pids));
  memset(history, 0, sizeof(history));

  while (1) {
    /* Get User input */
    printf("prompt$ ");

    // Error checking for 'fgets'.
    // This also makes sure gcc doesn't compliain about fgets' return value.
    if (fgets(cmd, MAX_LINE, stdin) != NULL) {
      // If the last char of cmd is not a newline (which should be their).
      // Print an error message.
      if (cmd[strlen(cmd) - 1] != '\n') {
        printf("Input must be less than %d characters\n", MAX_LINE);
        while ((ch = fgetc(stdin)) != '\n' && ch != EOF) {
          // Sending the rest of stdin to the void. (I think)
        }
        continue;
      }
      // If no input is provided. (or is just whitespace)
      // go to the beginning of the while loop.
      if (isEmpty(cmd)) {
        continue;
      }
      // Get return value of lastCmd.
      lc = lastCmd(history, cmd);
      // if 'lc list' was ran just restart the loop.
      if (lc == 2) {
        continue;
      }
    } else {
      // Probably don't need this.
      printf("Failed to get input.\n");
    }

    // Make a backup of cmd which will later
    // be saved into the history array.
    strcpy(cmdCopy, cmd);

    // clear myArgv.
    memset(myArgv, 0, sizeof(myArgv));

    /* Convert input to array of strings */
    strToArgv(myArgv, cmd);

    /* Build-in commands */
    if (strncmp(myArgv[0], "exit", 4) == 0) {
      printf("Exiting!\n");
      exit(0);
    } else if (strncmp(myArgv[0], "cd", 2) == 0) {
      changeDir(myArgv);
    } else if (strncmp(myArgv[0], "showpid", 7) == 0) {
      // skim through the pid array, and if it's
      // not 0 print it.
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
      // If execArgv returns 0 change 'success' is false.
      if (pid == 0) {
        success = false;
      }
    }

    // If 'lc', or 'lc #', was ran don't add the copy of cmd
    // to history.
    if (lc != 1) {
      // If success is true add copy of cmd to history.
      if (success == true) {
        for (i = 1; i < MAX_HIS; i++) {
          strcpy(history[i - 1], history[i]);
        }
        strcpy(history[MAX_HIS - 1], cmdCopy);
      }
    }
  }

  return 0;
}

void strToArgv(char *argv[], char str[]) {
  char tok[3] = " \n";
  char *token = NULL;
  int i = 0;

  token = strtok(str, tok);

  while (token != NULL) {
    argv[i] = token;
    token = strtok(NULL, tok);
    i++;
  }
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

// Fuction that is similar to isEmpty, but this
// time it checks if a string is actually an integer.
int isNumber(char const *str) {
  while (*str != '\0') {
    if (!isdigit(*str)) {
      return false;
    }
    str++;
  }
  return true;
}

// This function provides the 'lc' builtin command.
//
// This function will return 2 if it want to return to
// the top of the main while loop.
//
// This function will return 1 if it whats to run a command,
// but skip updating the history array.
int lastCmd(char history[][MAX_LINE], char *str) {
  char *args[2];
  char newCmd[MAX_LINE];
  int i;
  int num;
  int counter;
  int selection[MAX_HIS + 1];

  // Make copy if str, so that we don't mess up
  // the original str with strtok.
  strcpy(newCmd, str);
  // Make sure arrays is actually empty.
  memset(selection, 0, sizeof(selection));
  memset(args, 0, sizeof(args));
  // convert the copy of str to args array.
  strToArgv(args, newCmd);

  if (strncmp(args[0], "lc", 2) == 0) {
    // List the contents of history.
    if (args[1] == NULL) {
      // Don't do anything if nothing is in history.
      if (strncmp(history[MAX_HIS - 1], "\0", 1) == 0) {
        printf("Nothing has been added to history.\n");
        return 2;
      }
      strcpy(str, history[MAX_HIS - 1]);
      return 1;
    } else if (strncmp(args[1], "list", 5) == 0) {
      counter = 1;
      for (i = 0; i < MAX_HIS; i++) {
        if (strncmp(history[i], "\0", 1) != 0) {
          // Label each history string with a number.
          printf("%d: %s", counter, history[i]);
          counter++;
        }
      }
      return 2;
    } else if (isNumber(args[1])) {
      if (strlen(args[1]) > 2) {
        printf("Number must be 2 digits or less.\n");
        return 2;
      }
      sscanf(args[1], "%2d", &num);
      counter = 1;
      // prepare 'selection' so that we can put a number into
      // 'selection', and it will return the "index" of what we want
      // from 'history'.
      for (i = 0; i < MAX_HIS; i++) {
        if (strncmp(history[i], "\0", 1) != 0) {
          selection[counter] = i;
          counter++;
        }
      }
      // Don't attempt to access something from 'history' that
      // doesn't exist.
      if (num >= MAX_HIS + 1) {
        printf("\"%d\" is an invalid option.\n", num);
        return 2;
      } else if (selection[num] == 0) {
        printf("\"%d\" is an invalid option.\n", num);
        return 2;
      }
      strcpy(str, history[selection[num]]);
      return 1;
    } else if (args[1] != NULL) {
      printf("\"%s\" is not a valid option.\n", args[1]);
      return 2;
    }
  }
  return 0;
}

// Function for 'cd'
void changeDir(char *const args[]) {
  char *home = getenv("HOME");
  char newDir[MAX_LINE] = "\0";
  unsigned long i = 0;

  // if 'cd' is not supplied with any arguments
  // just change directory to HOME.
  if (args[1] == NULL) {
    for (i = 0; i < strlen(home); i++) {
      newDir[i] = home[i];
    }
    // if the first character in the directory
    // is a '~' replace '~' with the HOME directory.
  } else if (strncmp(args[1], "~", 1) == 0) {
    strcat(newDir, home);
    while (args[1][i + 1] != '\0') {
      newDir[strlen(home) + i] = args[1][i + 1];
      i++;
    }
    // If the argument is just a directory with no '~' at
    // the beginning just make the argument 'newDir'.
  } else {
    for (i = 0; args[1][i] != '\0'; i++) {
      newDir[i] = args[1][i];
    }
  }
  // If we succeed in changing directories change the PWD
  // variable to the new directory.
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
