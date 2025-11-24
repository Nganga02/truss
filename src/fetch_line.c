#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "fetch_line.h"

//This is a function to trim white spaces in a line
static char *trim_line(char *s){
    //Moving the cursor to the start of the line where there are no leading space
    while(isspace((unsigned char) *s))
    {
        s++;
    }
    char *t = s;

    //Moving the t pointer to a null character or the start of a comment
    while(*t != '\0'&& *t != '#')
    {
        t++;
    }
    if(*t == '#')
    {
        *t = '\0';
    }
    if(t != s)
    {
        t--;
        while(isspace((unsigned char)*t))
        {
            *t = '\0';
            t--;
        }
    }
    return s;
}




//This is a function to fetch the line and return the string
char *fetch_line(char *buf, int buflen, FILE* stream, int *lineno)
{
    char *s;
    if (fgets(buf, buflen, stream) == NULL)
        return NULL;
    ++*lineno;
    if (buf[strlen(buf) - 1]!= '\n')
    {
        fprintf(stderr, "*** reading error: input line %d too long for %s buf[%d]\n",*lineno, __func__, buflen);
        exit(EXIT_FAILURE);
    }
    s = trim_line(buf);
    if (*s != '\0')
        return s;
    else
        return fetch_line(buf, buflen, stream, lineno); // recursive
}
