//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks Inc. and
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
// Industrial Light & Magic, nor the names of their contributors may be used
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

#include <Alembic/PdbIO/PdbIO.h>

namespace Alembic {
namespace PdbIO {

//-*****************************************************************************
// Use an anonymous namespace to protect against linker clashes with
// these temporary structures.
namespace {

//-*****************************************************************************
#define SWAP_2(x) ( (((x) & 0xff) << 8) | ((boost::uint16_t)(x) >> 8) )
#define SWAP_4(x) ( ((x) << 24) | \
         (((x) << 8) & 0x00ff0000) | \
         (((x) >> 8) & 0x0000ff00) | \
         ((x) >> 24) )
#define FIX_2(x) (*(boost::uint16_t *)&(x) = SWAP_2(*(boost::uint16_t *)&(x)))
#define FIX_4(x)   (*(boost::uint32_t *)&(x) = SWAP_4(*(boost::uint32_t *)&(x)))


//-*****************************************************************************
#define PDB_MAGIC 670

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// Padded structs for reading and writing PDB files
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
struct Header
{
    boost::int32_t             magic;
    boost::uint16_t           swap;
    boost::uint16_t           unused0;
    float32_t           version;  // typically ignored
    float32_t           time;  // typically ignored
    boost::uint32_t           numParticles;
    boost::uint32_t           numAttributes;
    
    boost::int8_t              UNUSED[36];

    // Reads into the obj ObjectInfo struct, (setting numParticles and
    // numAttributes), while reporting errors to the error handler
    void read( std::istream &istr, ObjectInfo &obj, ErrorHandler &err );

    // Writes from the obj ObjectInfo struct, reporting errors to error-handler.
    void write( std::ostream &ostr, const ObjectInfo &obj, ErrorHandler &err );
};

//-*****************************************************************************
void Header::read( std::istream &istr,
                   ObjectInfo &obj,
                   ErrorHandler &err )
{
    istr.read( ( char * )this, sizeof( Header ) );
    
    if ( !istr )
    {
        err.handle( "Premature EOF while reading header" );
        return;
    }
    
    if ( swap != 1 )
    {
        FIX_4( magic );
        FIX_4( numParticles );
        FIX_4( numAttributes );
        obj.swapped = true;
    }
    else
    {
        obj.swapped = false;
    }

    if ( magic != PDB_MAGIC )
    {
        err.handle(
            ( boost::format( "Bad magic number: %d. Pdb magic is %d." )
              % magic % PDB_MAGIC ).str() );
        return;
    }
    
    obj.numParticles = numParticles;
    obj.numAttributes = numAttributes;
}

//-*****************************************************************************
void Header::write( std::ostream &ostr,
                    const ObjectInfo &obj,
                    ErrorHandler &err )
{
    memset( ( void * )this, 0, sizeof( Header ) );

    magic = PDB_MAGIC;
    swap = 1;
    numParticles = obj.numParticles;
    numAttributes = obj.numAttributes;

    if ( obj.swapped )
    {
        FIX_4( magic );
        FIX_2( swap );
        FIX_4( numParticles );
        FIX_4( numAttributes );
    }

    ostr.write( ( const char * )this, sizeof( Header ) );

    if ( !ostr )
    {
        err.handle( "Stream failure while writing header" );
        return;
    }
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// None of the first ChannelPrefix is used.
struct ChannelPrefix1
{
    boost::int8_t      unused0;
    boost::int16_t     unused1;
    boost::int8_t      unused2;
    boost::int8_t      unused3;

    void read( std::istream &istr,
               const ObjectInfo &obj, // unused, here for consistency
               AttributeInfo &attr,
               ErrorHandler &err )
    {
        istr.read( ( char * )this, sizeof( ChannelPrefix1 ) );

        if ( !istr )
        {
            err.handle(
                ( boost::format( "Premature EOF while reading attribute: %d" )
                  % attr.name ).str() );
            return;
        }
    }

    void write( std::ostream &ostr,
                const ObjectInfo &obj, // Unused, here for consistency
                const AttributeInfo &attr,
                ErrorHandler &err )
    {
        memset( ( void * )this, 0, sizeof( ChannelPrefix1 ) );
        ostr.write( ( const char * )this, sizeof( ChannelPrefix1 ) );

        if ( !ostr )
        {
            err.handle(
                ( boost::format( "Error while writing attribute: %d" )
                  % attr.name ).str() );
            return;
        }
    }
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// Of the second channel prefix, only the "type" field is used, and it can only
// have values 1, 2, or 3.
// As of GCC-4.0, the unused 4 & 5 needed to be changed for correct word
// alignment.
struct ChannelPrefix2
{
    boost::int32_t     unused0;
    
    boost::int32_t     type;

    boost::int32_t     unused1;
    boost::int32_t     unused2;
    boost::int32_t     unused3;
    // CJH boost::int8_t      unused4;
    // CJH boost::int8_t      unused5;
    boost::int16_t     unused4;
    boost::int16_t     unused5;
    boost::int32_t     unused6;
    boost::int32_t     unused7;
    boost::int32_t     unused8;

    void read( std::istream &istr,
               const ObjectInfo &obj,
               AttributeInfo &attr,
               ErrorHandler &err )
    {
        istr.read( ( char * )this, sizeof( ChannelPrefix2 ) );

        if ( !istr )
        {
            err.handle(
                ( boost::format( "Premature EOF while reading attribute: %s" )
                  % attr.name ).str() );
            return;
        }

        if ( obj.swapped )
        {
            FIX_4( type );
        }

        if ( type != ( boost::int32_t )k_VectorAttribute &&
             type != ( boost::int32_t )k_FloatAttribute &&
             type != ( boost::int32_t )k_IntAttribute )
        {
            err.handle(
                ( boost::format( "Corrupt type: %d in attribute: %s" )
                  % type % attr.name ).str() );
            return;
        }

        attr.type = ( AttributeType )type;
    }

    void write( std::ostream &ostr,
                const ObjectInfo &obj,
                const AttributeInfo &attr,
                ErrorHandler &err )
    {
        memset( ( void * )this, 0, sizeof( ChannelPrefix2 ) );
        type = ( boost::int32_t )attr.type;
        if ( obj.swapped )
        {
            FIX_4( type );
        }

        ostr.write( ( const char * )this, sizeof( ChannelPrefix2 ) );

        if ( !ostr )
        {
            err.handle(
                ( boost::format( "Failure while writing attribute: %s" )
                  % attr.name ).str() );
            return;
        }
    }
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
struct ChannelSuffix
{
    boost::int32_t     unused0;
    
    boost::uint32_t   dataSize;

    boost::int32_t     unused1;
    boost::int32_t     unused2;
    boost::int32_t     unused3;

    void read( std::istream &istr,
               const ObjectInfo &obj,
               const AttributeInfo &attr,
               ErrorHandler &err )
    {
        istr.read( ( char * )this, sizeof( ChannelSuffix ) );

        if ( !istr )
        {
            err.handle(
                ( boost::format( "Premature EOF while reading attribute: %s" )
                  % attr.name ).str() );
            return;
        }

        if ( obj.swapped )
        {
            FIX_4( dataSize );
        }

        if ( dataSize != bytesPerElement( attr.type ) )
        {
            err.handle(
                ( boost::format( "Corrupt data size: %d in attribute: %s" )
                  % dataSize % attr.name ).str() );
            return;
        }
    }

    void write( std::ostream &ostr,
                const ObjectInfo &obj,
                const AttributeInfo &attr,
                ErrorHandler &err )
    {
        memset( ( void * )this, 0, sizeof( ChannelSuffix ) );
        dataSize = ( boost::uint32_t )bytesPerElement( attr.type );
        if ( obj.swapped )
        {
            FIX_4( dataSize );
        }

        ostr.write( ( const char * )this, sizeof( ChannelSuffix ) );
        
        if ( !ostr )
        {
            err.handle(
                ( boost::format( "Failure while writing attribute: %s" )
                  % attr.name ).str() );
            return;
        }
    }
};

//-*****************************************************************************
// CJH: Dear reader. I am aware the following string read & write routines are
// lame. To be fixed.

//-*****************************************************************************
// Read a null terminated ASCII string from a stream.
// There's GOT to be a better way of doing this.
static inline void readString( std::istream &stream, std::string &into )
{
    into.erase();
    while ( !( stream.fail() ) )
    {
        int c = stream.get();
        if ( c == 0 ||
             c == EOF )
        {
            return;
        }
        else
        {
	    into.push_back(c);
            //into.append( ( size_t )1, ( char )c );
        }
    }
}

//-*****************************************************************************
// Write a null terminated ASCII string to a stream.
// There's GOT to be a better way of doing this.
static inline void writeString( std::ostream &stream, const std::string &outof )
{
    for ( int c = 0; c < outof.size(); ++c )
    {
        stream.put( outof[c] );
    }
    stream.put( 0 );
}

} // End anonymous namespace

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
void Read( std::istream &istr, Sink &snk, ErrorHandler &err )
{
    // Header reads into object info.
    ObjectInfo obj;
    {
        Header header;
        header.read( istr, obj, err );
    }

    snk.beginObjectRead( obj );

    for ( int attr = 0; attr < obj.numAttributes; ++attr )
    {
        AttributeInfo attrInfo;
        attrInfo.index = attr;

        // Read prefixes. This puts info into attrInfo
        {
            ChannelPrefix1 pre1;
            ChannelPrefix2 pre2;
            pre1.read( istr, obj, attrInfo, err );
            pre2.read( istr, obj, attrInfo, err );
        }

        // Read channel name
        readString( istr, attrInfo.name );

        // Read suffix
        // Nothing changed, just verified.
        {
            ChannelSuffix post;
            post.read( istr, obj, attrInfo, err );
        }
        
        // okay, get some stuff.
        size_t numBytes = bytesPerElement( attrInfo.type ) * obj.numParticles;

        if ( !istr )
        {
            err.handle(
                ( boost::format( "Premature EOF while reading attribute: %s" )
                  % attrInfo.name ).str() );
            return;
        }

        // Ask the snk for a data buffer.
        // If the snk returns NULL, simply skip it
        union
        {
            float32_t *f;
            boost::int32_t *i;
            char *c;
        } buffers;

        buffers.c = snk.beginAttributeRead( obj, attrInfo );

        if ( !buffers.c )
        {
            // Skip it.
            istr.ignore( numBytes );
        }
        else
        {
            istr.read( buffers.c, numBytes );
            if ( !istr )
            {
                err.handle(
                    ( boost::format( "Premature EOF while reading "
                                     "attribute: %s" )
                      % attrInfo.name ).str() );
            }
            
            if ( obj.swapped )
            {
                size_t numWords = numBytes / 4;
                for ( size_t w = 0; w < numWords; ++w )
                {
                    // Dangerous, but whatever.
                    FIX_4( (buffers.i[w]) );
                }
            }

            snk.endAttributeRead( obj, attrInfo );
        }
    } // End of loop over attributes

    snk.endObjectRead( obj );
}

//-*****************************************************************************
void Read( const std::string &fileName,
           Sink &sink,
           ErrorHandler &err )
{
    std::ifstream ifstr( fileName.c_str() );
    if ( !ifstr )
    {
        err.handle(
            ( boost::format( "Could not open file: %s for reading." )
              % fileName ).str() );
        return;
    }

    Read( ifstr, sink, err );

    ifstr.close();
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
void Write( std::ostream &ostr,
            Source &src,
            ErrorHandler &err,
            bool swapped )
{
    ObjectInfo obj;
    obj.swapped = swapped;
    src.beginObjectWrite( obj );
    {
        Header header;
        header.write( ostr, obj, err );
    }

    for ( int attr = 0; attr < obj.numAttributes; ++attr )
    {
        AttributeInfo attrInfo;
        attrInfo.index = attr;

        const char *buffer =
            src.beginAttributeWrite( obj, attrInfo );

        // Write prefixes.
        {
            ChannelPrefix1 pre1;
            ChannelPrefix2 pre2;
            pre1.write( ostr, obj, attrInfo, err );
            pre2.write( ostr, obj, attrInfo, err );
        }

        // Write channel name
        writeString( ostr, attrInfo.name );

        // Write suffix.
        {
            ChannelSuffix post;
            post.write( ostr, obj, attrInfo, err );
        }

        // Get data.
        size_t numBytes = bytesPerElement( attrInfo.type ) * obj.numParticles;

        // We already have the buffer.
        if ( !buffer )
        {
            // Write numBytes of zero.
            for ( size_t b = 0; b < numBytes; ++b )
            {
                ostr.put( 0 );
            }
        }
        else
        {
            if ( !obj.swapped )
            {
                ostr.write( buffer, numBytes );
            }
            else
            {
                size_t numWords = numBytes / 4;
                const boost::int32_t *ibuf = ( const boost::int32_t * )buffer;
                for ( size_t w = 0; w < numWords; ++w )
                {
                    boost::int32_t i = ibuf[w];
                    FIX_4( i );
                    ostr.write( ( const char * )&i, 4 );
                }
            }   
        }
        
        if ( !ostr )
        {
            err.handle(
                ( boost::format( "Premature EOF while writing attribute: %s" )
                  % attrInfo.name ).str() );
            return;
        }

        // Wrap it up...
        src.endAttributeWrite( obj, attrInfo );
    }

    // Wrap it up...
    src.endObjectWrite( obj );

    if ( !ostr )
    {
        err.handle(
            ( boost::format( "Something bad happened while writing PDB" )
              ).str() );
    }
}

//-*****************************************************************************
void Write( const std::string &fileName,
            Source &src,
            ErrorHandler &err,
            bool swapped )
{
    std::ofstream ofstr( fileName.c_str() );
    if ( !ofstr )
    {
        err.handle(
            ( boost::format( "Could not open file: %s for writing." )
              % fileName ).str() );
        return;
    }
    
    Write( ofstr, src, err, swapped );

    ofstr.close();
}

} // End namespace PdbIO
} // End namespace Alembic


