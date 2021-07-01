/* vim: set filetype=c: */

#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdbool.h>
#include <stdio.h>

typedef struct Result Result;
struct Result {
    bool has_err;
    char* err_str;
};

extern Result Result_err(char* err_str);
extern Result Result_noerr();
/* err_prefix: string to print before the error, if on occurred; prints nothing if it has value NULL
 * no_err: string to print if no error occured; prints nothing if it has value NULL
 * err_file: where to output the string, if an error has occurred
 * no_err_file: where to output, if no error has occurred */
extern void Result_print_err_or(const Result obj, const char* err_prefix, const char* no_err, FILE* err_file, FILE* no_err_file);

#endif /* _ERROR_H_ */
