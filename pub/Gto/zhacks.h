
#ifndef __ZHACKS_H__
#define __ZHACKS_H__

#include <stdio.h>
#include <zlib.h>

//
//  Copyright (c) 2009, Tweak Software
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
//     * Neither the name of the Tweak Software nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
//
//  THIS SOFTWARE IS PROVIDED BY Tweak Software ''AS IS'' AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL Tweak Software BE LIABLE FOR
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
//  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//

#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */


//
// Seek directly to a flush point in a gz file.  Also resets
// the decompressor.  No checking is done to make sure there
// is actually a flush point at the given offset.
//
z_off_t gzseek_raw(gzFile file, z_off_t raw_offset);


//
// We need to access some fields from a zlib gz_stream through an opaque
// pointer, so we roll our own.  In this instance, "GTO" is properly
// pronounced "ghetto".
//
typedef struct gz_stream {
    z_stream gto_stream;
    int      gto_z_err, gto_z_eof;
    FILE     *gto_z_file;
    Byte     *gto_inbuf, *__foo2;
    uLong    gto_crc;
    char     *__foo3, *__foo4;
    int       __foo0;
    char     gto_mode;
    z_off_t  gto_start, gto_in, gto_out;
    int      gto_back, gto_last;
} gz_stream;

#endif    // End #ifdef __ZHACKS_H__
