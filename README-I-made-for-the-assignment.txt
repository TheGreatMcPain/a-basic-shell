Author: James McClain
Program Name: shell
Compile: gcc -o shell -O3 shell.c
Run: ./shell

Built in commands:

"cd":                           Goto the user's home directory.
"cd <path name>":               Change working directory.
"cd ~/<path name>":             Goto a directory that's inside of home.
"cd Path\ With\ Spaces":        If the path name has spaces put a "\"
                                before each space.

"showpid":                      Show the last 5 child process ids.

"lc":                           Rerun the last command.
"lc list":                      List the last 5 commands.
"lc <number from list>":        Rerun one of the last 5 commands.

"exit":                         Exits the shell

"color":                        Shows the currently selected colors.
"color list":                   Lists available colors.
"color prompt <color name>":    Changes the color of the prompt.
"color dir <color name>":       Changes the color of the directory
                                part of the prompt.

"I also added an easter egg command. It doesn't actually do
what you think it does." HINT: It starts with fork.

Challenges that I ran into:

## FROM HALFWAY POINT (02/05/2020) ##

    One of the first things I ran into had to do with
gcc complaining about ignoring that return value of fgets().
Which on it's own seems pretty harmless, but I wanted to silence
that warning, so the first thing I tried came from
here: (https://stackoverflow.com/questions/11888594/ignoring-return-values-in-c)
which looked like this (void)(fgets(cmd, MAX_LINE, stdin)+1).
I later realized how ugly that solution was, so I decided to put
fgets() in an 'if' statement.

    When I was trying to figure out how to remove the newline character
from my fgets() string I came across strcspn(), and used it like this
'cmd[strcspn(cmd, "\n")] = 0;' which came from
here: (https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input)
I later figured out that I could just add '\n' to my strtok() arguments like
this 'token = strtok(cmd, " \n")' which will take care of newlines, and
spaces.

    One thing I found is how "slow" clearing an array with memset()
is which added a bit of delay between 'printf("prompt$ ")' statements if
memset() is on the top of the while loop. To get around this delay
I added a function that checks if a string only contains whitespace called
'isEmpty()'. The code for 'isEmpty()' comes from 'filterStr()'
inside of Project 0, but is modified to only check for spaces using 'isspace()'.
Using 'isEmpty()' I check if the string from 'fgets()' is 'empty', and if it is I just
skip the rest of the code and go to the top of the loop. I also moved memset()
below my isEmpty() statement.

## NEW STUFF ##

    In my notes/comments I mentioned that I decided to have fork/execvp be in
a separate function that would return the child process pid.  I realized the
child process wasn't able to "talk" to the parent process easily, but after reading
this: https://stackoverflow.com/questions/13710003/execvp-fork-how-to-catch-unsuccessful-executions
I learned that I could create a pipe that would allow the child to tell the
parent that 'execvp' failed. To do this I found some example code inside of
the manpage for 'pipe' that gave me a good enough guide to get the pipe laid out.

    When I was working on my 'cd' command I wanted '~' to translate to the
user's home directory. In getting that to work I learned how to use 'strcat'
which basically appends strings together. I also added the ability to 'cd'
into directories with spaces in their name like this (cd Path\ Name).

    Like many other people the first thing I tried to do when making my 'lc'
command was to make a deep copy of my argv array which proved too complicated.
Luckily I was given a more easier solution which was to basically copy my
'cmd' string, and reuse it when 'lc' is typed.  Knowing I just needed a string
I was able to implement some history function to my shell.

    Making sure my shell didn't segfault, or do weird things, was challenging.
Most of it had to do with making sure the code couldn't go out of
bounds when looking through arrays. One notable problem I had was related to
how 'fgets' handles 'stdin'.  When more than 80 characters are send to 'fgets'
it will run multiple times until 'fgets' runs out of characters. Reading
this: https://stackoverflow.com/questions/30388101/how-to-remove-extra-characters-input-from-fgets-in-c
I learned that I could check if my 'cmd' string didn't end in a newline, and
if their isn't a newline I have a while loop that uses 'fgetc' to discard the
rest of 'stdin'.

    I learned how to color strings from
here.: https://stackoverflow.com/questions/3219393/stdlib-and-colored-output-in-c
I thought it would be cool to allow the user to change the color on the fly,
so in doing so I used made a struct that contained the name of the color, and
it's corresponding ANSI code. To refresh my memory on structures I used this
tutorial.: https://www.tutorialspoint.com/cprogramming/c_structures.htm

    To make the prompt more like most default shell prompts I learned how to
get the current user's name, and how to get the machine's hostname
from here: https://stackoverflow.com/questions/504810/how-do-i-find-the-current-machines-full-hostname-in-c-hostname-and-domain-info
and here.: https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux

Other Notes:

## FROM HALFWAY POINT (02/05/2020) ##

    I decided to think ahead a bit, and moved my execvp()/fork() code over
to a separate function in the hopes that it will make life easier when I
implement 'showpid' later on. The arguments from that function,
which is: 'char *const args[]', came from execvp()'s manpage.

## NEW STUFF ##

    When looking how to find the integer limit in order to prevent a segfault
within my 'lastCmd' function (Which is not needed anymore since I learned
how to use 'sscanf' to restrict how many digits are written to an integer.) I
found 'limits.h' which contains macros for limits on many things like
integers. One of this macros is PATH_MAX which is for anything related to
directory strings, so I used that in any string that was going to take in a full
path name.
