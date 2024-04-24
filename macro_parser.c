#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "data.h"

#include "macro_parser.h"

Macro *macroTable[MACRO_TABLE_SIZE];
int hasMcr;

/* macro parser: first macro parsing of file */
void macroParser(FILE *fp, char *fileName)
{
  char line[MAX_LINE];
  FILE *outFile;
  Macro *mc;
  int writeLine;
  char *word, *tempLine;
  outFile = fopen(strcat(fileName, ".am"), "w+");
  initMacroTable();
  hasMcr = 0;
  if (outFile == NULL)
  {
    fprintf(stderr, "Failed to open output file in macroParser");
    exit(1);
  }

  fprintf(stdout, "\n --- in macroParser --- \n\n");

  while (fgets(line, MAX_LINE, fp) != NULL)
  {
    writeLine = 1;
    tempLine = strdup(line);
    word = strtok(tempLine, " \t\n");

    while (word != NULL)
    {
      if ((mc = lookup(word)) != NULL)
      {
        fputs(mc->content, outFile);
      }
      else if (strcmp(word, "mcr") == 0)
      {
        word = strtok(NULL, " \t\n");

        if (lookup(word) != NULL)
        {
          fprintf(stderr, "Duplicate macro name\n");
          exit(1);
        }
        else if (isReservedWord(word))
        {
          fprintf(stderr, "Reserved word cannot be used as macro name\n");
          exit(1);
        }

        else
        {
          hasMcr = 1;
          insertMacroToTable(fp, word);
          hasMcr = 0;
        }
      }
      else
      {
        if (writeLine)
        {
          fputs(line, outFile);
          writeLine = 0;
        }
      }
      word = strtok(NULL, " \t\n");
    }
  }

  printMacroTable(macroTable);
  fclose(outFile);
}

/* insert macro: insert macro to file */
void insertMacroToTable(FILE *fp, char *macroName)
{
  char line[MAX_LINE];
  char *content = NULL;
  size_t contentLength = 0;
  int endMcrFound = 0;

  while (!endMcrFound && fgets(line, MAX_LINE, fp) != NULL)
  {

    char *word = strtok(line, " \t");
    while (word != NULL)
    {
      if (strncmp(word, "endmcr", 6) != 0)
      {
        size_t wordLength = strlen(word);
        size_t newContentLength = contentLength + wordLength + 1;

        char *newContent = realloc(content, newContentLength + 1);
        if (newContent == NULL)
        {
          fprintf(stderr, "Memory allocation error\n");
          free(content);
          return;
        }

        content = newContent;
        strcpy(content + contentLength, word);

        contentLength = newContentLength;

        if (!endMcrFound)
        {
          content[contentLength - 1] = ' ';
          content[contentLength] = '\0';
        }

        word = strtok(NULL, " \t");
      }
      else
      {
        endMcrFound = 1;
        break;
      }
    }
  }
  if (content != NULL && contentLength > 0)
  {
    if (content[contentLength - 1] == ' ')
    {
      content[contentLength - 1] = '\0';
    }
    addMacro(macroName, content);
    free(content);
  }
}

/* hash function for macro names */
unsigned int hashMacroName(char *name)
{
  unsigned int hashVal = 0;
  for (; *name != '\0'; name++)
  {
    hashVal = *name + 31 * hashVal;
  }
  return hashVal % MACRO_TABLE_SIZE;
}

/* initialize macro table */
void initMacroTable()
{
  int i;
  for (i = 0; i < MACRO_TABLE_SIZE; i++)
  {
    macroTable[i] = NULL;
  }
}

/* lookup: macro table lookup */
struct Macro *lookup(char *name)
{
  struct Macro *mc;
  for (mc = macroTable[hashMacroName(name)]; mc != NULL; mc = mc->next)
  {
    if (strcmp(name, mc->name) == 0)
    {
      return mc; /* found */
    }
  }
  return NULL; /* not found */
}

/* add macro to table */
void addMacro(char *name, char *content)
{
  unsigned int hashVal;
  struct Macro *mc;

  if ((mc = lookup(name)) == NULL)
  { /* not found */
    mc = (struct Macro *)malloc(sizeof(*mc));
    if (mc == NULL || (mc->name = strdup(name)) == NULL)
    {
      fprintf(stderr, "Memory allocation error\n");
      return; /* Handle memory allocation failure */
    }
    mc->content = strdup(content);
    hashVal = hashMacroName(name);
    mc->next = macroTable[hashVal];
    macroTable[hashVal] = mc;
  }
  else
  {
    fprintf(stderr, "%s already exists in the macro table\n", name);
  }
}

/* print macro table */

void printMacroTable(Macro *macroTable[])
{
  int i;
  Macro *mc;
  printf("Macro Table Content:\n");
  for (i = 0; i < MACRO_TABLE_SIZE; i++)
  {
    mc = macroTable[i];
    if (mc != NULL)
    {
      printf("Bucket %d:\n", i);
      while (mc != NULL)
      {
        printf("  Name: %s\n", mc->name);
        printf("  Content:\n%s\n", mc->content);
        mc = mc->next;
      }
    }
  }
}
