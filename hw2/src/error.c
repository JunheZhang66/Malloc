/*
 * Error handling routines
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
int errors;
int warnings;
int dbflag = 1;

void fatal(char *fmt, ...)
//char *fmt, *a1, *a2, *a3, *a4, *a5, *a6;
{
        va_list arguments;
        va_start(arguments, fmt);
        fprintf(stderr, "\nFatal error: ");
        vfprintf(stderr, fmt, arguments);
        fprintf(stderr, "\n");
        va_end(arguments);
        exit(1);
}

void error(char *fmt, ...)
//char *fmt, *a1, *a2, *a3, *a4, *a5, *a6;
{
        va_list arguments;
        va_start(arguments, fmt);
        fprintf(stderr, "\nError: ");
        vfprintf(stderr, fmt, arguments);
        fprintf(stderr, "\n");
        va_end(arguments);
        errors++;
}

void warning(char *fmt, ...)
//char *fmt, *a1, *a2, *a3, *a4, *a5, *a6;
{
        va_list arguments;
        va_start(arguments, fmt);
        fprintf(stderr, "\nWarning: ");
        vfprintf(stderr, fmt, arguments);
        fprintf(stderr, "\n");
        va_end(arguments);
        warnings++;
}

void debug(char *fmt, ...)
//char *fmt, *a1, *a2, *a3, *a4, *a5, *a6;
{
        va_list arguments;
        va_start(arguments, fmt);
        if(!dbflag) return;
        fprintf(stderr, "\nDebug: ");
        vfprintf(stderr, fmt, arguments);
        //fprintf(stderr, fmt, a1, a2, a3, a4, a5, a6);
        fprintf(stderr, "\n");
        va_end(arguments);
}
