/* vim: set filetype=c: */

#include "error.h"

#include <assert.h>

Result Result_err(char* err_str) {
    Result res;
    res.has_err = true;
    res.err_str = err_str;
    return res;
}

Result Result_noerr() {
    Result res;
    res.has_err = false;
    res.err_str = NULL;
    return res;
}

extern void Result_print_err_or(const Result obj, const char* err_prefix, const char* no_err, FILE* err_file, FILE* no_err_file) {
    if(obj.has_err) {
        if(err_prefix != NULL)
            fprintf(err_file, "%s", err_prefix);
        assert(obj.err_str != NULL);
        fprintf(err_file, "%s\n", obj.err_str);
    }
    else if(no_err != NULL)
        fprintf(no_err_file, "%s\n", no_err);
}
