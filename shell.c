/***************************************************************
 * Name of Program: shell
 * Author: James McClain
 * Description: Just a basic shell program that allows the user
 *              to run other terminal programs.
 ***************************************************************/

// use stdbool.h to basically convert 'true' to 1, and 'false' to 0.
// Using limits.h to provide PATH_MAX
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

// Sizes for arrays.
#define MAX_LINE 80
#define ARGV_SIZE 10
#define MAX_HIS 5
#define MAX_PIDS 5

// ANSI Colors
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"
#define NUM_COLORS 6

// Structures
struct Color {
  char name[10];
  char ansi[10];
};

// Prototypes
void genPrompt(struct Color color1, struct Color color2);
int isEmpty(char const *str);
int isPosInteger(char const *str);
int strToArgv(char *argv[], int size, char str[]);
int lastCmd(char history[][MAX_LINE], char *str);
void changeColor(char *const args[], struct Color colors[],
                 struct Color *colorPtr1, struct Color *colorPtr2);
void changeDir(char *const args[]);
pid_t execArgv(char *const args[]);

int main() {
  pid_t pid;
  bool success = true;
  int lc;
  char cmd[MAX_LINE];
  char cmdCopy[MAX_LINE];
  char *myArgv[ARGV_SIZE];
  char history[MAX_HIS][MAX_LINE];
  struct Color colors[NUM_COLORS];
  struct Color colorPrompt;
  struct Color colorDir;
  int pids[MAX_PIDS];
  int i;

  // There's probably a better way to do this.
  memset(colors, 0, sizeof(colors));
  strcpy(colors[0].ansi, RED);
  strcpy(colors[1].ansi, GREEN);
  strcpy(colors[2].ansi, YELLOW);
  strcpy(colors[3].ansi, BLUE);
  strcpy(colors[4].ansi, MAGENTA);
  strcpy(colors[5].ansi, CYAN);

  strcpy(colors[0].name, "red");
  strcpy(colors[1].name, "green");
  strcpy(colors[2].name, "yellow");
  strcpy(colors[3].name, "blue");
  strcpy(colors[4].name, "magenta");
  strcpy(colors[5].name, "cyan");

  // Set default color.
  // '3' is blue.
  colorPrompt = colors[1];
  colorDir = colors[3];

  memset(pids, 0, sizeof(pids));
  memset(history, 0, sizeof(history));

  while (1) {
    memset(cmd, 0, sizeof(cmd));
    /* Get User input */
    genPrompt(colorPrompt, colorDir);

    // Error checking for 'fgets'.
    // This also makes sure gcc doesn't complain about fgets' return value.
    if (fgets(cmd, MAX_LINE, stdin) != NULL) {
      // If the last char of cmd is not a newline (which should be there).
      // Print an error message.
      if (cmd[strlen(cmd) - 1] != '\n') {
        printf("Input must be less than %d characters\n", MAX_LINE);

        while (fgetc(stdin) != '\n') {
          // Discard until newline.
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
    if (strToArgv(myArgv, ARGV_SIZE, cmd) == 1) {
      printf("Input must be less than %d arguments.\n", ARGV_SIZE);
      continue;
    }

    /* Other Build-in commands */
    if (strncmp(myArgv[0], "exit", 4) == 0) {
      printf("Exiting!\n");
      exit(0);
    } else if (strncmp(myArgv[0], "color", 5) == 0) {
      changeColor(myArgv, colors, &colorPrompt, &colorDir);
    } else if (strncmp(myArgv[0], "cd", 2) == 0) {
      changeDir(myArgv);
    } else if (strncmp(myArgv[0], "showpid", 7) == 0) {
      // skim through the pid array, and if it's
      // not 0 print it.
      for (i = 0; i < MAX_PIDS; i++) {
        if (pids[i] != 0) {
          printf("%d\n", pids[i]);
        }
      }
    } else {
      // Run non-builtin, and store pid in pid array.
      pid = execArgv(myArgv);
      // if execArgv returns 0 don't add it to pid array.
      if (pid != 0) {
        for (i = 1; i < MAX_PIDS; i++) {
          pids[i - 1] = pids[i];
        }
        pids[MAX_PIDS - 1] = pid;
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

// Simply prints the 'prompt'
void genPrompt(struct Color color1, struct Color color2) {
  char *user = getlogin();
  char *home = getenv("HOME");
  char *currentDir = getenv("PWD");
  char newDir[PATH_MAX];
  char hostname[MAX_LINE];

  // Grabs the hosts' hostname.
  gethostname(hostname, MAX_LINE);

  // Replace the home directory with '~'.
  if (strncmp(currentDir, home, strlen(home)) == 0) {
    strcpy(newDir, "~");
    // This is setting the starting pointer of currentDir
    // so that we skip the 'home' directory.
    currentDir = currentDir + strlen(home);
    strcat(newDir, currentDir);
  } else {
    // Otherwise just copy currentDir to newDir.
    strcpy(newDir, currentDir);
  }

  // This should print "user@hostname current/directory >"
  // where "user@hostname", and ">" will have their own color, and
  // "current/directory" will have a different color.
  printf("%s%s@%s %s%s%s > " RESET, color1.ansi, user, hostname, color2.ansi,
         newDir, color1.ansi);
}

int strToArgv(char *argv[], int size, char str[]) {
  char tok[3] = " \n";
  char *token = NULL;
  int i = 0;
  int x;
  int y;

  token = strtok(str, tok);

  while (token != NULL) {
    if (i == size - 1) {
      return 1;
    }
    argv[i] = token;

    token = strtok(NULL, tok);
    i++;
  }

  // If one of the strings from argv have a '\'.
  // Go replace '\' with a space, and append the next
  // string(s). This is to allow 'cd' to work on
  // directories with spaces in their name.
  i = 0;
  while (argv[i] != NULL) {
    y = i;
    while (argv[y][strlen(argv[y]) - 1] == '\\') {
      argv[y][strlen(argv[y]) - 1] = '\0';
      strcat(argv[y], " ");
      strcat(argv[y], argv[y + 1]);
      for (x = y + 1; argv[x] != NULL; x++) {
        argv[x] = argv[x + 1];
      }
    }
    i++;
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

// Function that is similar to isEmpty, but this
// time it checks if a string is actually an integer.
// Due to how this function works it will return false for
// negative integers.
int isPosInteger(char const *str) {
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
  char *args[3];
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
  strToArgv(args, 3, newCmd);

  // This doesn't actually forkbomb I swear!
  if (strncmp(args[0], "forkbomb", 8) == 0) {
    printf("Starting in...\n");
    for (counter = 5; counter > 0; counter--) {
      printf("%d\n", counter);
      sleep(1);
    }
    printf("\nJust Kidding!\n");

    return 2;
  }

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
    } else if (isPosInteger(args[1])) {
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
      if (num >= MAX_HIS + 1 || selection[num] == 0) {
        printf("\"%d\" Doesn't exist in history list.\n\n", num);
        printf("Use 'lc list' to list command history.\n\n");
        return 2;
      }

      strcpy(str, history[selection[num]]);
      return 1;
    } else if (args[1] != NULL) {
      printf("\"%s\" is not a valid option.\n\n", args[1]);
      printf("Use 'lc' to run the previous command.\n\n");
      printf("'lc list' to list command history starting with the latest.\n\n");
      printf("'lc <# from 'lc list'>' to run commands from the history.\n\n");
      return 2;
    }
  }
  return 0;
}

// This function is for the 'color' command.
//
// If no other arguments are provided it will simply print
// the current color.
//
// If 'list' is provided it will print the available colors.
//
// If the name of a color is provided it will change the value of
// colorPtr to that color.
void changeColor(char *const args[], struct Color colors[],
                 struct Color *colorPtr1, struct Color *colorPtr2) {
  struct Color *colorTmpPtr = NULL;
  int i;

  // If no arguments print the current color.
  if (args[1] == NULL) {
    printf("Current prompt color is: %s%s\n" RESET, colorPtr1->ansi,
           colorPtr1->name);
    printf("Current directory color is: %s%s\n" RESET, colorPtr2->ansi,
           colorPtr2->name);
    // If 'list' is supplied list the available colors.
  } else if (strncmp(args[1], "list", 4) == 0) {
    for (i = 0; i < NUM_COLORS; i++) {
      printf("%s%s\n", colors[i].ansi, colors[i].name);
    }
    // if 'prompt' is supplied change the 'colorTmpPtr' pointer
    // to colorPtr1. This way when we change the value of 'colorTmpPtr'
    // we also change the value that colorPtr1 points to.
  } else if (strncmp(args[1], "prompt", 6) == 0) {
    colorTmpPtr = colorPtr1;
    // Same as before, but for colorPtr2.
  } else if (strncmp(args[1], "dir", 3) == 0) {
    colorTmpPtr = colorPtr2;
    // Tell the user how to use this command.
  } else if (strncmp(args[1], "\0", 1) != 0) {
    printf("\"%s\" Is not a valid option.\n\n", args[1]);

    printf("Use 'color' to show the currently selected colors.\n\n");
    printf("'color list' to list available colors.\n\n");
    printf("'color prompt <color>' will change the color of the prompt.\n\n");
    printf("'color dir <color>' will change the color of current directory");
    printf("part of the prompt.\n\n");
  }

  if (colorTmpPtr != NULL) {
    if (args[2] == NULL) {
      printf("You need to input a color.\n");
    } else {
      for (i = 0; i < NUM_COLORS; i++) {
        if (strcmp(colors[i].name, args[2]) == 0) {
          *colorTmpPtr = colors[i];
          break;
        }
      }
      if (i == NUM_COLORS) {
        printf("\"%s\" Is not a color.\n", args[2]);
      }
    }
  }
}

// Function for 'cd'
void changeDir(char *const args[]) {
  char *strPtr = NULL;
  char cwd[PATH_MAX];
  char *home = getenv("HOME");
  char newDir[PATH_MAX] = "\0";

  // if 'cd' is not supplied with any arguments
  // just change directory to HOME.
  if (args[1] == NULL) {
    strcpy(newDir, home);
    // if the first character in the directory
    // is a '~' replace '~' with the HOME directory.
  } else if (strncmp(args[1], "~", 1) == 0) {
    strcpy(newDir, home);
    // Ignore the '~' in args[1].
    strPtr = args[1] + 1;
    strcat(newDir, strPtr);
    // If the argument is just a directory with no '~' at
    // the beginning just make the argument 'newDir'.
  } else {
    strcpy(newDir, args[1]);
  }
  // If we succeed in changing directories change the PWD
  // variable to the new directory.
  if (chdir(newDir) == 0) {
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
      printf("getcwd() failed.");
    } else {
      setenv("PWD", cwd, 1);
    }
  } else {
    printf("\"%s\" does not exist.\n", newDir);
  }
}

// Moved fork and execvp into it's own function
// that returns the pid from execvp.
// Hopefully this will make 'showpid' easier to
// implement.
//
// Update 02-06-2020: So I learned how to use pipes
// so that the child process can pass information back to
// the parent process. In this case I'm sending the return value
// to the parent which will help the parent know if execvp fails.
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
