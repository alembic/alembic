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

#ifndef _GtoContainer_Writer_h_
#define _GtoContainer_Writer_h_

#include <Gto/Writer.h>
#include <string>
#include <vector>

namespace GtoContainer {

//-*****************************************************************************
// Forward Declarations
class Component;
class Property;
class PropertyContainer;
class ObjectVector;

//-*****************************************************************************
class Writer
{
public:
    typedef Gto::Writer::FileType FileType;

    Writer( const char *stamp = NULL );
    virtual ~Writer();

    // File types are Gto::BinaryGTO,
    //                Gto::CompressedGTO,
    //                Gto::TextGTO
    bool                    write( const char *filename, 
                                   const ObjectVector &ov, 
                                   FileType type = Gto::Writer::CompressedGTO );

private:
    void                    writeComponent( bool header, const Component * );
    void                    writeProperty( bool header, const Property * );

private:
    std::string             m_stamp;
    Gto::Writer             m_writer;

    std::vector<std::string> m_orderedStrings;
};

} // End namespace GtoContainer

#endif

