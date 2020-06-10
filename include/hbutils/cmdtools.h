/* Copyright 2020, JP Norair
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
  * @file       hbutils_args.h
  * @author     JP Norair
  * @version    R100
  * @date       1 June 2020
  * @brief      Utilities for command argument parsing
  * @ingroup    hbutils
  *
  ******************************************************************************
  */

#ifndef _hbutils_args_h_
#define _hbutils_args_h_

#include <stdint.h>
#include <stdlib.h>

#include "hbutils/types.h"



///@todo this binary search will be converted to a hash table at 
///      some point in the near future.
#define CMD_NAME_MAX    24 

typedef struct {
    uint32_t       index;
    int32_t        argbytes;
} hbcmd_bundle_t;

typedef struct {
    const char     name[CMD_NAME_MAX]; 
    hbcmd_t*       action; 
    hbcmd_bundle_t bundle;
} hbcmd_item_t;




uint8_t* hbutils_markstring(uint8_t** psrc, int* search_limit, int string_limit);

int hbutils_parseargv(char*** pargv, const char* cmdname, char* dst, char* src, size_t src_limit);

void hbutils_freeargv(char** argv);

const hbcmd_item_t* hbutils_cmdsearch(const hbcmd_item_t* cmdtab, const char *cmdname, size_t cmd_count);




#endif
