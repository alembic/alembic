//-*****************************************************************************
//
// Copyright (c) 2009-2011,
//  Sony Pictures Imageworks, Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#ifndef AbcClients_WFObjConvert_Reader_h
#define AbcClients_WFObjConvert_Reader_h

#include <AbcClients/WFObjConvert/Export.h>
#include <AbcClients/WFObjConvert/Foundation.h>

namespace AbcClients {
namespace WFObjConvert {

//-*****************************************************************************
// This class definition is taken from Gto/WFObj.
//-*****************************************************************************

//-*****************************************************************************
//! The Reader class is an abstract base class which represents the
//! actions which may occur in the parsing of a Wavefront OBJ file.
//! As the file or stream is parsed, each of these actions will be called
//! as their corresponding actions are encountered in the file. The
//! default behavior is to ignore everything.
//! Because the files being read are ascii, they can actually represent
//! arbitrary precision. Therefore, the Reader is implemented in double
//! precision for maximum accuracy.
class ABC_WFOBJ_CONVERT_EXPORT Reader
{
public:
    typedef std::vector<index_t> IndexVec;

    typedef std::vector<std::string> StringVec;

    //! In current ilm-base, there is no Vec4.
    //! By default, we will treat 4-dimensional
    //! points as homogenous and dehomogenize them, subsequently
    //! calling the V3d version of 'v'

    //! Default constructor does nothing
    //! ...
    Reader() {}

    //! Virtual destructor as support for derived classes.
    //! ...
    virtual ~Reader() {}

    //! This function is called by the parser
    //! when parsing begins.
    //! The argument passed is a name for the stream - in the
    //! case of a file, it will be the file name; otherwise it
    //! will be an identifier of some kind.
    virtual void parsingBegin( const std::string &iStreamName ) {}

    //! This function is called by the parser
    //! if an error has occurred, causing parsing to stop.
    //! An error description and a line number are given.
    virtual void parsingError( const std::string &iStreamName,
                               const std::string &iErrorDesc,
                               size_t iErrorLine ) {}

    //! This function is called by the parser
    //! when parsing ends. The name passed in will match
    //! the name passed to the begin function.
    virtual void parsingEnd( const std::string &iStreamName,
                             size_t iNumLinesParsed ) {}


    //-*************************************************************************
    //! POINT DECLARATION
    //! Points may be of type:
    //! "v", which are 3 or 4-dimensional points,
    //! "vt", which are 1, 2, or 3 dimensional texture coordinates,
    //! "vn", which are 3-dimensional normals,
    //! "vp", which are 'parameter coordinates'. Unclear what these are for.
    //! These functions will each also be passed the integer index that
    //! they correspond to, for synchronization and checking.
    //! OBJ files begin with the first vertex at index '1', allowing for
    //! vertex "0" to be used as a special location.
    //-*************************************************************************

    //! Set a three-dimensional vertex.
    //! ...
    virtual void v( index_t iIndex, const V3d &iVal ) {}

    //! Set a 4-dimensional vertex, interpreted as homogenous.
    //! The default behavior is to dehomogenize and call the 3-dim version.
    //! It will not divide by zero - it will leave the point alone in that
    //! case.
    virtual void v( index_t iIndex, const V3d &iVal, double iW );

    //! Set a 1-dimensional texture vertex.
    //! ...
    virtual void vt( index_t iIndex, double iVal ) {}

    //! Set a 2-dimensional texture vertex.
    //! ...
    virtual void vt( index_t iIndex, const V2d &iVal ) {}

    //! Set a 3-dimensional texture vertex.
    //! ...
    virtual void vt( index_t iIndex, const V3d &iVal ) {}

    //! Set a 3-dimensional normal.
    //! Using V3d instead of N3f/N3T
    virtual void vn( index_t iIndex, const V3d &iVal ) {}

    //! Set a 1-dimensional parameter vertex.
    //! ...
    virtual void vp( index_t iIndex, double iVal ) {}

    //! Set a 2-dimensional parameter vertex.
    //! ...
    virtual void vp( index_t iIndex, const V2d &iVal ) {}

    //-*************************************************************************
    //! ELEMENT DECLARATION
    //! In this Parser/Reader, we have three types of elements.
    //! They are "faces", "lines", and "points".
    //! points can actually have multiple points.
    //! An element consists of vectors of Vertex, Normal, and Texture
    //! indices. If these indices were unspecified, the vectors will be empty.
    //! The vertex indices will always be non-empty
    //-*************************************************************************

    //! Declare a face.
    //! ...
    virtual void f( const IndexVec &iVertexIndices,
                    const IndexVec &iTextureIndices,
                    const IndexVec &iNormalIndices ) {}

    //! Declare a polyline.
    //! ...
    virtual void l( const IndexVec &iVertexIndices,
                    const IndexVec &iTextureIndices,
                    const IndexVec &iNormalIndices ) {}

    //! Declare a point-set.
    //! ...
    virtual void p( const IndexVec &iVertexIndices,
                    const IndexVec &iTextureIndices,
                    const IndexVec &iNormalIndices ) {}

    //-*************************************************************************
    //! GROUPS AND OBJECT DECLARATION
    //! OBJ files have the ability to assign objects to groups, multiple
    //! groups at a time, and also to declare elements as belonging to
    //! a particular named object.
    //-*************************************************************************

    //! Called when an as-yet unseen group name is encountered.
    //! ...
    virtual void newGroup( const std::string &iGroupName ) {}


    //!	Called when active groups change
    //! The set of groups may be more than 1.
    virtual void activeGroups( const StringVec &iGroupNames ) {}


    //!	Called when an "o" line is encountered.
    //! ...
    virtual void activeObject( const std::string &iObjectName ) {}

    //!	Called when smoothing group changes -- note that Id == 0 means
    //!	no smoothing -- this is equivalent to the "s off" statement.
    //! the "s on" statement will set smoothing group to 1.
    virtual void smoothingGroup( int iSmoothingGroup ) {}

    //-*************************************************************************
    // Bevel/cinterp/dinterp
    //-*************************************************************************

    virtual void bevel( bool ) {}
    virtual void cinterp( bool ) {}
    virtual void dinterp( bool ) {}

    //-*************************************************************************
    // Material and map
    //-*************************************************************************
    virtual void mtllib( const std::string &iName ) {}
    virtual void maplib( const std::string &iName ) {}
    virtual void usemap( const std::string &iName ) {}
    virtual void usemtl( const std::string &iName ) {}

    //-*************************************************************************
    // Trace/Shadow objects
    //-*************************************************************************
    virtual void trace_obj( const std::string &iName ) {}
    virtual void shadow_obj( const std::string& iName ) {}

    //-*************************************************************************
    // Level of detail number
    //-*************************************************************************
    virtual void lod( int ) {}
};

} // End namespace WFObjConvert
} // End namespace AbcClients

#endif
