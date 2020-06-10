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
  * @file       hbutils_types.h
  * @author     JP Norair
  * @version    R100
  * @date       1 June 2020
  * @brief      Types common across HBUtils
  * @ingroup    hbutils
  *
  * @note       Relies on cmdtab.  Cmdtab will be absorbed eventually by hbutils.
  ******************************************************************************
  */


#ifndef _hbutils_types_h_
#define _hbutils_types_h_

#include <cmdtab.h>


typedef enum {
    HBFMT_Default  = 0,
    HBFMT_Json     = 1,
    HBFMT_Bintex   = 2,
    HBFMT_Hex      = 3,
    HBFMT_MAX
} HBFMT_Type;


typedef int (*hb_printer_t)(char*);


typedef int (*hbcmd_t)( void* handle, 
                        uint8_t* dst, size_t* dst_bytes, size_t dstmax, 
                        uint8_t* src, int* src_bytes );


#endif

