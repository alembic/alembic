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

#ifndef _Alembic_GLUtil_GlslProgram_h_
#define _Alembic_GLUtil_GlslProgram_h_

#include <Alembic/GLUtil/Foundation.h>

namespace Alembic {
namespace GLUtil {

//-*****************************************************************************
// Use these to catch recoverable shader compilation and linking errors.
class GlslException : public Alembic::Util::Exception
{
public:
    GlslException() : Alembic::Util::Exception() {}
    GlslException( const std::string &str )
      : Alembic::Util::Exception( str ) {}
};

//-*****************************************************************************
#define GLSL_THROW( TEXT )                                      \
do                                                              \
{                                                               \
    std::stringstream sstr;                                     \
    sstr << TEXT;                                               \
    Alembic::GLUtil::GlslException exc( sstr.str() );           \
    throw exc;                                                  \
}                                                               \
while( 0 )


//-*****************************************************************************
// GlslProgram
// Manages fragment program state using GLSL and OpenGL.
// This class supports a single vertex program,
// a single fragment program and its parameters.
class GlslProgram
{
protected:
    void init( const std::vector<std::string> &vtxSources,
               const std::vector<std::string> &frgSources );
    
public:
    GlslProgram( const std::string &nme,
                 const std::vector<std::string> &vtxSources,
                 const std::vector<std::string> &frgSources )
      : m_name( nme ),
        m_progId( 0 ),
        m_vtxId( 0 ),
        m_frgId( 0 )
    {
        init( vtxSources, frgSources );
    }

    GlslProgram( const std::string &nme,
                 const std::string &vtxSource,
                 const std::string &frgSource )
      : m_name( nme ),
        m_progId( 0 ),
        m_vtxId( 0 ),
        m_frgId( 0 )
    {
        std::vector<std::string> vtx;
        vtx.push_back( vtxSource );
        std::vector<std::string> frg;
        frg.push_back( frgSource );
        init( vtx, frg );
    }

    ~GlslProgram();
    
    const std::string &name() const { return m_name; }
    
    GLuint id() const { return m_progId; }

    //-*************************************************************************
    // These parameter settings must be done BEFORE calling "use".
    // They can be set at any time, the state is held in this program class.
    
    void setTexture( const std::string &texName,
                     GLenum texTarget,
                     GLuint texId );

    void setTexture1d( const std::string &texName,
                       GLuint texId )
    {
        setTexture( texName, GL_TEXTURE_1D, texId );
    }
    
    void setTexture2d( const std::string &texName,
                       GLuint texId )
    {
        setTexture( texName, GL_TEXTURE_2D, texId );
    }
    
    void setTexture3d( const std::string &texName,
                       GLuint texId )
    {
        setTexture( texName, GL_TEXTURE_3D, texId );
    }

    void setTextureRect( const std::string &texName,
                         GLuint texId )
    {
        setTexture( texName, GL_TEXTURE_RECTANGLE_ARB, texId );
    }

    void setUniform1f( const std::string &uname, float x );
    void setUniform2fv( const std::string &uname, const float *v );
    void setUniform2f( const std::string &uname, float x, float y );
    void setUniform3fv( const std::string &uname, const float *v );
    void setUniform3f( const std::string &uname,
                       float x, float y, float z );
    void setUniform4fv( const std::string &uname, const float *v );
    void setUniform4f( const std::string &uname,
                       float x, float y, float z, float w );

    //-*************************************************************************
    // Use turns the program on and sets all if its uniforms and textures.
    // Unuse turns it off.
    void use();
    void unuse();

protected:
    void bindUniforms();
    void bindTextures();
    void unbindTextures();
    
    typedef std::pair<GLenum, GLuint> Texture;
    typedef std::vector<Texture> TextureBindings;
    typedef std::map<std::string, Texture> Textures;
    
    struct SimpleUniform
    {
        int size;
        GLfloat v[4];
        
        SimpleUniform() : size( 0 )
        { v[0] = v[1] = v[2] = v[3] = 0.0f; }

        void reset()
        {
            size = 0;
            v[0] = v[1] = v[2] = v[3] = 0.0f;
        }

        void set( float f0 )
        {
            reset();
            size = 1;
            v[0] = f0;
        }

        void set( float f0, float f1 )
        {
            reset();
            size = 2;
            v[0] = f0;
            v[1] = f1;
        }

        void set( float f0, float f1, float f2 )
        {
            reset();
            size = 3;
            v[0] = f0;
            v[1] = f1;
            v[2] = f2;
        }

        void set( float f0, float f1, float f2, float f3 )
        {
            reset();
            size = 4;
            v[0] = f0;
            v[1] = f1;
            v[2] = f2;
            v[3] = f3;
        }   
    };
    
    typedef std::map<std::string, SimpleUniform> Uniforms;

    std::string m_name;
    GLuint m_progId;
    GLuint m_vtxId;
    GLuint m_frgId;

    Textures m_textures;
    TextureBindings m_bindings;
    Uniforms m_uniforms;
};

} // End namespace GLUtil
} // End namespace Alembic

#endif
