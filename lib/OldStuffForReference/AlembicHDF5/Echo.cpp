//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#include <AlembicHDF5/AlembicHDF5.h>
#include <AlembicHDF5/Echo.h>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/iostreams/filtering_stream.hpp>

namespace bio = boost::iostreams;

namespace AlembicHDF5 {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// NEED BOOST IOSTREAMS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

class indentFilter : public bio::output_filter
{
public:
    indentFilter() : m_indent( 0 ) {}

    int indent() const { return m_indent; }
    void setIndent( int i )
    {
        m_indent = std::max( i, 0 );
        //std::cout << "This: " << ( void * )this
        //          << " NEW indent: " << m_indent << std::endl;
    }

    void increment( int i ) { setIndent( m_indent + i ); }
    void decrement( int i ) { setIndent( m_indent - i ); }

    template<typename Sink>
    bool put( Sink &dest, int c )
    {
        bool ret = boost::iostreams::put( dest, c );
        if ( c == '\n' )
        {
            //std::cout << "This: " << ( void * )this
            //          << " indent: " << m_indent << std::endl;
            for ( int i = 0; i < m_indent; ++i )
            {
                ret = boost::iostreams::put( dest, ' ' );
            }
        }
        return ret;
    }

protected:
    int m_indent;
};

//-*****************************************************************************
struct IOBundle : boost::noncopyable
{
private:
    indentFilter indent;
public:
    IOBundle( std::ostream &ostr )
    {
        out.push( indent );
        out.push( ostr );
    }

    void increment( int i )
    {
        out << std::flush;
        out.component<indentFilter>( 0 )->increment( i );
    }
    
    void decrement( int i )
    {
        out << std::flush;
        out.component<indentFilter>( 0 )->decrement( i );
    }
    
    bio::filtering_ostream out;
};

//-*****************************************************************************
static void VisitComments( const BaseObject &obj,
                           std::ostream &ostr )
{
    std::string cmt = obj.comment();
    if ( cmt.size() > 0 && cmt != "" )
    {
        ostr << "Comment: \"" << cmt << "\"" << std::endl;
    }
}

//-*****************************************************************************
static void VisitDimensions( const Dimensions &dims,
                             const std::string &open,
                             const std::string &close,
                             std::ostream &ostr )
{
    for ( Dimensions::const_iterator diter = dims.begin();
          diter != dims.end(); ++diter )
    {
        ostr << open << (*diter) << close;
    }
}

//-*****************************************************************************
static void VisitType( const Datatype &dtype,
                       std::ostream &ostr )
{
    H5T_class_t tClass = dtype.getClass();

    /*if ( tClass == H5T_STRING ||
         tClass == H5T_BITFIELD ||
         tClass == H5T_OPAQUE ||
         tClass == H5T_COMPOUND ||
         tClass == H5T_ENUM ||
         tClass == H5T_VLEN )
    {
        ostr << "UNSUPPORTED DATATYPE ";
        return;
    }
    else*/ if ( tClass == H5T_STRING )
    {
        if ( H5Tis_variable_str( dtype.id() ) )
        {
            ostr << "H5T_STRING(variable,len="
                 << dtype.getSize()
                 << ")";
            return;
        }
        else
        {
            ostr << "H5T_STRING(fixed,len="
                 << dtype.getSize()
                 << ")";

        }
        return;
    }
    else if ( tClass == H5T_BITFIELD )
    {
        ostr << "UNSUPPORTED DATATYPE (BITFIELD) ";
        return;
    }
    else if ( tClass == H5T_OPAQUE )
    {
        ostr << "UNSUPPORTED DATATYPE (OPAQUE) ";
        return;
    }
    else if ( tClass == H5T_COMPOUND )
    {
        ostr << "UNSUPPORTED DATATYPE (COMPOUND) ";
        return;
    }
    else if ( tClass == H5T_ENUM )
    {
        ostr << "UNSUPPORTED DATATYPE (ENUM) ";
        return;
    }
    else if ( tClass == H5T_VLEN )
    {
        ostr << "UNSUPPORTED DATATYPE (VLEN) ";
        return;
    }
    else if ( tClass == H5T_REFERENCE )
    {
        ostr << "Reference";
        return;
    }
    else if ( tClass == H5T_ARRAY )
    {
        // Arrayed classes. Get the super type.
        VisitType( dtype.getSuper(), ostr );
        Dimensions dims;
        dtype.getArrayDims( dims );
        VisitDimensions( dims, "[", "]", ostr );
        return;
    }
    
    Datatype::flag_t predefFlags = 0;
    
    if ( tClass == H5T_INTEGER )
    {
        predefFlags |= Datatype::kInteger;
        
        if ( dtype.isSigned() )
        {
            predefFlags |= Datatype::kSigned;
        }
        else
        {
            predefFlags |= Datatype::kUnsigned;
        }
    }
    else if ( tClass == H5T_FLOAT )
    {
        predefFlags |= Datatype::kFloat;
    }
    
    int bytes = dtype.getSize();
    if ( bytes == 1 )
    {
        predefFlags |= Datatype::kBytes1;
    }
    else if ( bytes == 2 )
    {
        predefFlags |= Datatype::kBytes2;
    }
    else if ( bytes == 4 )
    {
        predefFlags |= Datatype::kBytes4;
    }
    else if ( bytes == 8 )
    {
        predefFlags |= Datatype::kBytes8;
    }
    
    if ( dtype.isBigEndian() )
    {
        predefFlags |= Datatype::kBig;
    }
    else
    {
        predefFlags |= Datatype::kLittle;
    }

    ostr << dtype.predefinedName( predefFlags );
}

//-*****************************************************************************
static void VisitAttr( const Attribute &attr,
                       std::ostream &ostr )
{
    ostr << "Attribute: " << attr.name() << " ";
    Datatype dtype( attr );
    VisitType( dtype, ostr );
    Dataspace dspace( attr );
    Dimensions dims;
    dspace.getSimpleExtentDims( dims );
    VisitDimensions( dims, "{", "}", ostr );
    ostr << std::endl;
    VisitComments( attr, ostr );
}

//-*****************************************************************************
static void VisitAttributes( const AttributedObject &lobj,
                             IOBundle &io )
{
    // Get Attrs.
    std::ostream &ostr = io.out;
    size_t numAttrs = lobj.numAttributes();
    if ( numAttrs > 0 )
    {
        ostr << "ATTRIBUTES" << std::endl
             << "{";
        io.increment( 2 );
        ostr << std::endl;
        for ( size_t a = 0; a < numAttrs; ++a )
        {
            Attribute attr( lobj, a );
            VisitAttr( attr, ostr );
        }
        io.decrement( 2 );
        ostr << std::endl << "}" << std::endl;
    }
}

//-*****************************************************************************
static void VisitDataset( const Dataset &dset,
                          IOBundle &io )
{
    std::ostream &ostr = io.out;
    
    ostr << "DATASET: " << dset.name() << std::endl
         << "{";
    io.increment( 2 );
    ostr << std::endl;
    VisitComments( dset, ostr );
    VisitAttributes( dset, io );
    
    Datatype dtype( dset );
    VisitType( dtype, ostr );
    Dataspace dspace( dset );
    Dimensions dims;
    dspace.getSimpleExtentDims( dims );
    VisitDimensions( dims, "{", "}", ostr );

    io.decrement( 2 );
    ostr << std::endl << "}" << std::endl;
}

//-*****************************************************************************
struct Visitor
{
    Visitor( const ParentObject &p, IOBundle &o )
      : parent( p ), io( o ) {}
    const ParentObject &parent;
    IOBundle &io;
};

//-*****************************************************************************
static void VisitGroup( const Group &g, IOBundle &io );

//-*****************************************************************************
static herr_t
VisitAllCB( hid_t group,
            const char *name,
            const H5L_info_t *linfo,
            void *op_data )
{
    hid_t       obj;
    Visitor *visit = ( Visitor * )op_data;
    ABCH5_ASSERT( visit != NULL,
                  "Null visitor passed to iteration op." );
    std::ostream &ostr = visit->io.out;
    const ParentObject &parent = visit->parent;
    herr_t status;

    if ( linfo->type == H5L_TYPE_HARD )
    {
        //std::cout << "Link: " << name << " is a HARD_LINK" << std::endl;
        
        // Stat the object
        H5O_info_t  oinfo;
        status = H5Oget_info_by_name( parent.id(),
                                      name, &oinfo, H5P_DEFAULT );
        ABCH5_ASSERT( status >= 0,
                      "H50get_info_by_name failed for object: " << name );
        Group newGrp;
        Dataset newDset;

        switch ( oinfo.type )
        {
        case H5O_TYPE_GROUP:
            newGrp.open( parent, name );
            VisitGroup( newGrp, visit->io );
            break;

        case H5O_TYPE_DATASET:
            newDset.open( parent, name );
            VisitDataset( newDset, visit->io );
            break;

        case H5O_TYPE_NAMED_DATATYPE:
            ostr << "NAMED DATATYPES UNSUPPORTED" << std::endl;
            break;

        default:
            ostr << "UNKNOWN OBJECT: " << name << std::endl;
        }
    }
    else if ( linfo->type == H5L_TYPE_SOFT )
    {
        ostr << "SOFT LINKS UNSUPPORTED" << std::endl;
    }
    else if ( linfo->type == H5L_TYPE_EXTERNAL )
    {
        ostr << "EXTERNAL LINKS UNSUPPORTED" << std::endl;
    }
    else
    {
        ostr << "USERDEF LINKS UNSUPPORTED" << std::endl;
    }

    return 0;
}

//-*****************************************************************************
static void VisitGroup( const Group &g,
                        IOBundle &io )
{
    std::ostream &ostr = io.out;
    
    ostr << "GROUP: " << g.name() << std::endl
         << "{";
    io.increment( 2 );
    ostr << std::endl;
    VisitComments( g, ostr );
    VisitAttributes( g, io );

    ostr << "CHILDREN: " << std::endl
         << "{";
    io.increment( 2 );
    ostr << std::endl;

    Visitor v( g, io );
    hsize_t idx = 0;
    herr_t status = H5Literate( g.id(),
                                H5_INDEX_NAME,
                                H5_ITER_NATIVE,
                                &idx,
                                VisitAllCB,
                                ( void * )&v );
    ABCH5_ASSERT( status >= 0,
                  "H5Literate failed." );
    io.decrement( 2 );
    ostr << std::endl;
    ostr << "}";
    io.decrement( 2 );
    ostr << std::endl;
    ostr << "}" << std::endl;
}

//-*****************************************************************************
static void VisitFile( const File &f,
                       IOBundle &io )
{
    std::ostream &ostr = io.out;
    ostr << "FILE: " << f.name() << std::endl
         << "{";
    io.increment( 2 );
    ostr << std::endl;
    VisitComments( f, ostr );
    VisitAttributes( f, io );

    // Get root group.
    Group grp( f, "/", kOpen );
    VisitGroup( grp, io );

    io.decrement( 2 );
    ostr << std::endl << "}" << std::endl;
}

//-*****************************************************************************
void Echo( const std::string &fileName,
           std::ostream &ostr )
{
    IOBundle io( ostr );
    
    File file( fileName, kOpen, H5F_ACC_RDONLY );
    VisitFile( file, io );
}

} // End namespace AlembicHDF5
