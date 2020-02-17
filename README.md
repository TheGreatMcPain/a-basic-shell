# A Simple Shell

## A shell I made for a college project.

### It can run programs, but doesn't have most of the fancy stuff like bash has (yet).

## How to build

$ make

## How to run

$ ./shell

## Built in commands

| 'cd' | Goto home directory. |
|------|----------------------|
| 'cd <path>' | Change current directory. |
| 'cd ~/<path>' | Goto a directory that's inside of home. |
| 'cd Path\ With\ Spaces' | If the path name has spaces put a '\' before each space. |
| 'showpid' | Show the last 5 child process ids. |
| 'lc' | Rerun the last command. |
| 'lc list' | List the last 5 commands. |
| 'lc <number from list>' | Rerun one of the last 5 commands. |
| 'exit' | Exits the shell |
| 'color' | Shows the currently selected colors. |
| 'color list' | Lists available colors. |
| 'color prompt <color name>' | Changes the color of the prompt. |
| 'color dir <color name>' | Changes the color of the directory part of the prompt. |

## Known limitations


