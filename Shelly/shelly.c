/*
 * shelly interface program

 KUSIS ID: 60673 PARTNER NAME: Baran Berkay Hökelek
 KUSIS ID: 54309 PARTNER NAME: Tümay Özdemir

*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/fcntl.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define HASHSIZE 101

int parseCommand(char inputBuffer[], char *args[], int *background);

struct nlist
{                     /* table entry: */
  struct nlist *next; /* next entry in chain */
  char *name;         /* defined name */
  char *defn;         /* replacement text */
};

char *strdupl(char *);

/* hash table implementation */

static struct nlist *hashtab[HASHSIZE]; /* pointer table */

/* hash: form hash value for string s */
unsigned hash(char *s)
{
  unsigned hashval;
  for (hashval = 0; *s != '\0'; s++)
    hashval = *s + 31 * hashval;
  return hashval % HASHSIZE;
}

/* lookup: look for s in hashtab */
struct nlist *lookup(char *s)
{
  struct nlist *np;
  for (np = hashtab[hash(s)]; np != NULL; np = np->next)
    if (strcmp(s, np->name) == 0)
      return np; /* found */
  return NULL;   /* not found */
}

/* install: put (name, defn) in hashtab */
struct nlist *install(char *name, char *defn)
{
  struct nlist *np;
  unsigned hashval;
  if ((np = lookup(name)) == NULL)
  { /* not found */
    np = (struct nlist *)malloc(sizeof(*np));
    if (np == NULL || (np->name = strdupl(name)) == NULL)
      return NULL;
    hashval = hash(name);
    np->next = hashtab[hashval];
    hashtab[hashval] = np;
  }
  else                      /* already there */
    free((void *)np->defn); /*free previous defn */
  if ((np->defn = strdupl(defn)) == NULL)
    return NULL;
  return np;
}

char *strdupl(char *c) /* make a duplicate of s */
{
  char *p;
  p = (char *)malloc(strlen(c) + 1); /* +1 for ’\0’ */
  if (p != NULL)
    strcpy(p, c);
  return p;
}

int undef(char *name)
{
  struct nlist *np1, *np2;

  if ((np1 = lookup(name)) == NULL) /*  name not found  */
    return 1;

  for (np1 = np2 = hashtab[hash(name)]; np1 != NULL;
       np2 = np1, np1 = np1->next)
  {
    if (strcmp(name, np1->name) == 0)
    { /*  name found  */

      /*  Remove node from list  */

      if (np1 == np2)
        hashtab[hash(name)] = np1->next;
      else
        np2->next = np1->next;

      /*  Free memory  */

      free(np1->name);
      free(np1->defn);
      free(np1);

      return 0;
    }
  }

  return 1; /*  name not found  */
}

/* end of hash table. NOTE: Taken from K&R */

int main(void)
{
  char inputBuffer[MAX_LINE];   /* buffer to hold the command entered */
  int background;               /* equals 1 if a command is followed by '&' */
  char *args[MAX_LINE / 2 + 1]; /* command line (of 80) has max of 40 arguments */
  pid_t child;                  /* process id of the child process */
  int status;                   /* result from execv system call*/
  int shouldrun = 1;

  int i, upper;

  // For part2
  // counter for script
  int scount = 0;
  char *array[50];

  FILE *bookmarkPtr = fopen("myshell-bookmarks.txt", "w+");

  /* get contents from bookmarkPtr & append them to hashtab. if file is empty, do nothing. */
  char buffer[1000];
  const char s[2] = "-";
  char *token;
  long size;
  if (bookmarkPtr != NULL)
  {
    fseek(bookmarkPtr, 0, SEEK_END);
    size = ftell(bookmarkPtr);

    if (size != 0)
    {
      fseek(bookmarkPtr, 0, SEEK_SET);         /* set file pointer to beginning of the file */
      fread(buffer, 1, size + 1, bookmarkPtr); /* read the entire file into buffer */
      token = strtok(buffer, s);
      /* What did i just do?
         I planned to save the key-value pairs to file in the format: key1-value1-key2-value2-...
         I will tokenize those strings, and then pair them in 2's & install them into hashtab. */
      while (token != NULL)
      {
        char *key = token;
        token = strtok(NULL, s);
        install(key, token);
        token = strtok(NULL, s);
      }
    }
  }

  struct nlist *np;
  while (shouldrun)
  { /* Program terminates normally inside setup */
    background = 0;

    shouldrun = parseCommand(inputBuffer, args, &background); /* get next command */

    if (strncmp(inputBuffer, "exit", 4) == 0)
      shouldrun = 0; /* Exiting from myshell*/

    if ((np = lookup(args[0])) != NULL)
    {
      shouldrun = parseCommand(np->defn, args, &background);
    }

    if (shouldrun)
    {
      /*
        After reading user input, the steps are
        (1) Fork a child process using fork()
        (2) the child process will invoke execv()
        (3) if command included &, parent will invoke wait()
      */

      // Part2-2
      if (strncmp(args[0], "script", 6) == 0)
      {
        strcpy(array, args[1]);
      }
      if (strncmp(args[0], "exit", 6) == 0)
        scount = 0;

      if ((strncmp(args[0], "script", 6) == 0) || scount == 1)
      {

        child = fork();
        if (child == 0)
        {

          int fp2;
          // opening the output file to write
          fp2 = open(array, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
          // writing the output file
          dup2(fp2, 1);

          if (scount == 1)
            execvp(args[0], args);
          scount = 1;

          // close(fp2);
        }

        if (child > 0)
        {
          if (background == 1)
            wait(NULL);

          if (strncmp(args[0], "script", 6) == 0)
            scount = 1;
        }
      }
      else
      {
        if (strncmp(args[0], "bookmark", 8) == 0)
        {
          /* bookmark stuff */
          install(args[1], args[2]); /* update hashtab */
          printf("Bookmark Created\n");
          fprintf(bookmarkPtr, "%s-%s-", args[1], args[2]); /* update bookmarkPtr */
        }
        else
        {

          child = fork();
          if (child == 0)
          {
            part2function(args);
          }

          if (child > 0)
          {
            if (background == 1)
              wait(NULL);
          }
        }
      }
    }
  }
  return 0;
}

// Part1
// Function doing part2: io redirection if command includes > or >>
void part2function(char *args[])
{

  int i = 0;
  int counter = 0;
  int check = 0;
  while (args[i] != NULL)
  {
    if ((strncmp(args[i], ">", 1) == 0) || (strncmp(args[i], ">>", 2) == 0))
    {
      check = 1;
      break;
    }
    else
      counter++;
    i++;
  }
  // printf("%d", counter); //counter=1

  if (check == 1)
  {
    // File redirection
    int fp;
    if (strncmp(args[counter], ">", 1) == 0)
      fp = open(args[++counter], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    else
      fp = open(args[++counter], O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    dup2(fp, 1);

    char *temp[counter];
    for (int i = 0; i < counter - 1; i++)
    {
      temp[i] = args[i];
      // For checking correctness
      printf("%s", temp[i]);
    }
    temp[counter - 1] = NULL;

    execvp(temp[0], temp);

    close(fp);
  }
  // Part1
  else
    execvp(args[0], args);
}

/**
 * The parseCommand function below will not return any value, but it will just: read
 * in the next command line; separate it into distinct arguments (using blanks as
 * delimiters), and set the args array entries to point to the beginning of what
 * will become null-terminated, C-style strings.
 */

int parseCommand(char inputBuffer[], char *args[], int *background)
{
  int length,         /* # of characters in the command line */
      i,              /* loop index for accessing inputBuffer array */
      start,          /* index where beginning of next command parameter is */
      ct,             /* index of where to place the next parameter into args[] */
      command_number; /* index of requested command number */

  ct = 0;

  /* read what the user enters on the command line */
  do
  {
    printf("shelly>");
    fflush(stdout);
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
  } while (inputBuffer[0] == '\n'); /* swallow newline characters */

  /**
   *  0 is the system predefined file descriptor for stdin (standard input),
   *  which is the user's screen in this case. inputBuffer by itself is the
   *  same as &inputBuffer[0], i.e. the starting address of where to store
   *  the command that is read, and length holds the number of characters
   *  read in. inputBuffer is not a null terminated C-string.
   */
  start = -1;
  if (length == 0)
    exit(0); /* ^d was entered, end of user command stream */

  /**
   * the <control><d> signal interrupted the read system call
   * if the process is in the read() system call, read returns -1
   * However, if this occurs, errno is set to EINTR. We can check this  value
   * and disregard the -1 value
   */

  if ((length < 0) && (errno != EINTR))
  {
    perror("error reading the command");
    exit(-1); /* terminate with error code of -1 */
  }

  /**
   * Parse the contents of inputBuffer
   */

  for (i = 0; i < length; i++)
  {
    /* examine every character in the inputBuffer */

    switch (inputBuffer[i])
    {
    case ' ':
    case '\t': /* argument separators */
      if (start != -1)
      {
        args[ct] = &inputBuffer[start]; /* set up pointer */
        ct++;
      }
      inputBuffer[i] = '\0'; /* add a null char; make a C string */
      start = -1;
      break;

    case '\n': /* should be the final char examined */
      if (start != -1)
      {
        args[ct] = &inputBuffer[start];
        ct++;
      }
      inputBuffer[i] = '\0';
      args[ct] = NULL; /* no more arguments to this command */
      break;

    default: /* some other character */
      if (start == -1)
        start = i;
      if (inputBuffer[i] == '&')
      {
        *background = 1;
        inputBuffer[i - 1] = '\0';
      }
    } /* end of switch */
  }   /* end of for */

  /**
   * If we get &, don't enter it in the args array
   */

  if (*background)
    args[--ct] = NULL;

  args[ct] = NULL; /* just in case the input line was > 80 */

  return 1;

} /* end of parseCommand routine */
