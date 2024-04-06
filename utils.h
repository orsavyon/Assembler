#ifndef UTILS_H
#define UTILS_H

char *strdup(char *);

void fileCopy(FILE *, FILE *);

void printFile(FILE *);

void skipAndCopy(FILE *, FILE *);

void skipWhiteLines(FILE *fp);

void handleError(const char *errorMessage, int lineNumber, char *line);

#endif
