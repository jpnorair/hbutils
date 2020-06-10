/* Copyright 2017, JP Norair
*
* Licensed under the OpenTag License, Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.indigresso.com/wiki/doku.php?id=opentag:license_1_0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
/**
  * @file       cmdtools.c
  * @author     JP Norair
  * @version    R100
  * @date       1 Nov 2017
  * @brief      Command Search and Parsing tools
  * @ingroup    hbutils
  *
  ******************************************************************************
  */

#include "hbutils/cmdtools.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


///@todo convert this to hash table in the near future

// comapres two strings by alphabet,
// returns 0 - if equal, -1 - first one bigger, 1 - 2nd one bigger.
static int local_strcmp(const char *s1, const char *s2);

// comapres first x characters of two strings by alphabet,
// returns 0 - if equal, -1 - first one bigger, 1 - 2nd one bigger.
static int local_strcmpc(const char *s1, const char *s2, int x);




uint8_t* hbutils_markstring(uint8_t** psrc, int* search_limit, int string_limit) {
    size_t      code_len;
    size_t      code_max;
    uint8_t*    cursor;
    uint8_t*    front;
    
    /// 1. Set search limit on the string to mark within the source string
    code_max    = (*search_limit < string_limit) ? *search_limit : string_limit; 
    front       = *psrc;
    
    /// 2. Go past whitespace in the front of the source string if there is any.
    ///    This updates the position of the source string itself, so the caller
    ///    must save the position of the source string if it wishes to go back.
    while (isspace(**psrc)) { 
        (*psrc)++; 
    }
    
    /// 3. Put a Null Terminator where whitespace is found after the marked
    ///    string.
    for (code_len=0, cursor=*psrc; (code_len < code_max); code_len++, cursor++) {
        if (isspace(*cursor)) {
            *cursor = 0;
            cursor++;
            break;
        }
    }
    
    /// 4. Go past any whitespace after the cursor position, and update cursor.
    while (isspace(*cursor)) { 
        cursor++; 
    }
    
    /// 5. reduce the message limit counter given the bytes we've gone past.
    *search_limit -= (cursor - front);
    
    return cursor;
}



int hbutils_parseargv(char*** pargv, const char* cmdname, char* dst, char* src, size_t src_limit) {
    char* cursor;
    int argc = 0;
    int remdata;
    int paren, brack, quote;
    bool ws_lead;
    
    if (dst != src) {
        strncpy(dst, src, src_limit);
    }
    cursor = dst;

    /// Zero out whitespace that's not protected
    /// Protections:
    /// - Quotes "..."
    /// - Brackets [...]
    /// - Parentheses (...)
    ws_lead = true;
    remdata = (int)src_limit;
    paren   = 0;
    brack   = 0;
    quote   = 0;
    while ((*cursor != 0) && (remdata != 0)) {
        switch (*cursor) {
            case '\"':  quote = (quote == 0);   break;     
            case '[':   brack++;                break;
            case '(':   paren++;                break;
            case ']':   brack -= (brack > 0);   break;
            case ')':   paren -= (paren > 0);   break;
            
            default:
                if ((quote==0) && (brack==0) && (paren==0)) {
                    if (isspace(*cursor)) {
                        *cursor = 0;
                    }
                }
                break;
        }
        if ((ws_lead == true) && (*cursor != 0)) {
            argc++;
        }
        ws_lead = (bool)(*cursor == 0);
        cursor++;
        remdata--;
    }
    
    /// If 'cmdname' is NULL: the command name is included in the source string
    /// If 'cmdname' is not NULL: the command name is supplied in 'cmdname'
    if (cmdname != NULL) {
        argc++;
    }
    
    /// If argc is not 0:
    /// - allocate argv
    /// - populate argv array with pointers to first chars after zeros.
    if (argc != 0) {
        *pargv = calloc(sizeof(char*), argc);
        if (*pargv == NULL) {
            argc = -2;
        }
        else {
            int i;
            if (cmdname != NULL) {
                (*pargv)[0] = (char*)cmdname;
                i = 1;
            }
            else {
                i = 0;
            }
        
            // Bypass leading whitespace (now zeros)
            // Fill argv[i]
            // Bypass trailing non-whitespace
            cursor = dst;
            for (; i<argc; i++) {
                for (; *cursor==0; cursor++);
                (*pargv)[i] = cursor;
                for (; *cursor!=0; cursor++);
            }
        }
    }

    return argc;
}

void hbutils_freeargv(char** argv) {
    if (argv != NULL) {
        free(argv);
    }
}


const hbcmd_item_t* hbutils_cmdsearch(const hbcmd_item_t* cmdtab, const char *cmdname, size_t cmd_count) {
/// Verify that cmdname is not a zero-length string, then search for it in the
/// list of available commands
    if (*cmdname != 0) {
    
        // This is a binary search across the static array
        int l           = 0;
        int r           = (int)cmd_count - 1;
        int cci;
        int csc;
    
        while (r >= l) {
            cci = (l + r) >> 1;
            csc = local_strcmp((char*)cmdtab[cci].name, cmdname);
            
            switch (csc) {
                case -1: r = cci - 1; break;
                case  1: l = cci + 1; break;
                default: return &cmdtab[cci];
            }
        }
        // End of binary search implementation
        
    }
    
	return NULL;
}



static int local_strcmp(const char *s1, const char *s2) {
	for (; (*s1 == *s2) && (*s1 != 0); s1++, s2++);	
	return (*s1 < *s2) - (*s1 > *s2);
}


static int local_strcmpc(const char *s1, const char *s2, int x) {
    for (int i = 0; (*s1 == *s2) && (*s1 != 0) && (i < x - 1); s1++, s2++, i++) ;
    return (*s1 < *s2) - (*s1 > *s2);
}

