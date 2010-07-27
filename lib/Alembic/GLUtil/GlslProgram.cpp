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

#include <Alembic/GLUtil/GlslProgram.h>

namespace Alembic{
namespace GLUtil {

//-*****************************************************************************
static GLuint initShader( const std::string &shaderName,
                          GLenum type,
                          const std::vector<std::string> &sources )
{
    GLuint id = 0;
    
    static const GLchar *shaderSources[32];
    GLsizei numSources = sources.size();
    assert( numSources > 0 );
    
    if ( numSources > 32 )
    {
        GLSL_THROW( "Can't compile shader: " << shaderName << std::endl
                    << "Too many shader sources: " << numSources
                    << ". Max = 32" );
    }

    std::vector<std::string> newSources( sources.size() );
    
    for ( int i = 0; i < numSources; ++i )
    {
        std::string oldSource = sources[i];
        std::string newSourceHdr = "#version 120\n";
        if ( boost::find_first( oldSource, "texture2DRect" ) ||
             boost::find_first( oldSource, "sampler2DRect" ) )
        {
            newSourceHdr += "#extension GL_ARB_texture_rectangle : enable";
        }
        //newSources[i] = newSourceHdr + oldSource;
        newSources[i] = oldSource;
        
        shaderSources[i] = ( const GLchar * )( newSources[i].c_str() );
    }
    
    for ( int i = numSources; i < 32; ++i )
    {
        shaderSources[i] = NULL;
    }
        
    id = glCreateShader( type );
    if ( id == 0 )
    {
        GLSL_THROW( "Could not create shader: " << shaderName );
    }

    glShaderSource( id, numSources, shaderSources, NULL );
    glCompileShader( id );
    GLint compiled = 0;
    glGetShaderiv( id, GL_COMPILE_STATUS, &compiled );
    if ( compiled != GL_TRUE )
    {
        GLint length = 0;
        glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length );
        std::vector<GLchar> log( length + 1 );
        glGetShaderInfoLog( id, length, &length, &(log[0]) );
        std::string strLog = ( const char * )&(log[0]);
        GLSL_THROW( "Compilation error in shader: " << shaderName
                    << std::endl << strLog );
    }

    return id;
}

//-*****************************************************************************
void GlslProgram::init( const std::vector<std::string> &vtxSources,
                        const std::vector<std::string> &frgSources )
{
    m_progId = glCreateProgram();
    if ( m_progId == 0 )
    {
        GLSL_THROW( "Couldn't allocate GLSL program: " << m_name );
    }

    m_vtxId = 0;
    if ( vtxSources.size() > 0 )
    {
        m_vtxId = initShader( m_name + "::vertex",
                              GL_VERTEX_SHADER, vtxSources );
        glAttachShader( m_progId, m_vtxId );
    }

    m_frgId = 0;
    if ( frgSources.size() > 0 )
    {
        m_frgId = initShader( m_name + "::fragment",
                              GL_FRAGMENT_SHADER, frgSources );
        glAttachShader( m_progId, m_frgId );
    }

    glLinkProgram( m_progId );
     
    GLint linked = 0;
    glGetProgramiv( m_progId, GL_LINK_STATUS, &linked );
    if ( linked != GL_TRUE )
    {
        GLint length = 0;
        glGetProgramiv( m_progId, GL_INFO_LOG_LENGTH, &length );
        
        std::vector<GLchar> log( length + 1 );
        glGetProgramInfoLog( m_progId, length, &length, &(log[0]) );
        std::string logStr = ( const std::string & )&(log[0]);
        GLSL_THROW( "Linking error in program: " << m_name
                    << std::endl << logStr );
    }

    GLint validate = 0;
    glValidateProgram( m_progId );
    glGetProgramiv( m_progId, GL_VALIDATE_STATUS, &validate );
    if ( validate != GL_TRUE )
    {
        GLSL_THROW( "Given vertex/fragment program: " << m_name
                    << "won't run on this hardware" );
    }
}

//-*****************************************************************************
GlslProgram::~GlslProgram()
{
    if ( m_progId > 0 )
    {
        glDeleteProgram( m_progId );
        m_progId = 0;
    }
    
    if ( m_vtxId > 0 )
    {
        glDeleteShader( m_vtxId );
        m_vtxId = 0;
    }
    
    if ( m_frgId > 0 )
    {
        glDeleteShader( m_frgId );
        m_frgId = 0;
    }
}

//-*****************************************************************************
void GlslProgram::use()
{
    glUseProgram( m_progId );
    bindUniforms();
    bindTextures();
}

//-*****************************************************************************
void GlslProgram::unuse()
{
    unbindTextures();
    glUseProgram( 0 );
}

//-*****************************************************************************
void GlslProgram::setTexture( const std::string &texName,
                              GLenum texTarget, GLuint texId )
{
    Texture tx;
    tx.first = texTarget;
    tx.second = texId;
    m_textures[texName] = tx;
}

//-*****************************************************************************
void GlslProgram::setUniform1f( const std::string &uname, float x )
{
    m_uniforms[uname].set( x );
}

//-*****************************************************************************
void GlslProgram::setUniform2f( const std::string &uname, float x, float y )
{
    m_uniforms[uname].set( x, y );
}

//-*****************************************************************************
void GlslProgram::setUniform2fv( const std::string &uname, const float *v )
{
    m_uniforms[uname].set( v[0], v[1] );
}

//-*****************************************************************************
void GlslProgram::setUniform3f( const std::string &uname, float x, float y, float z )
{
    m_uniforms[uname].set( x, y, z );
}

//-*****************************************************************************
void GlslProgram::setUniform3fv( const std::string &uname, const float *v )
{
    m_uniforms[uname].set( v[0], v[1], v[2] );
}

//-*****************************************************************************
void GlslProgram::setUniform4f( const std::string &uname, float x, float y,
                                float z, float w )
{
    m_uniforms[uname].set( x, y, z, w );
}

//-*****************************************************************************
void GlslProgram::setUniform4fv( const std::string &uname, const float *v )
{
    m_uniforms[uname].set( v[0], v[1], v[2], v[3] );
}

//-*****************************************************************************
void GlslProgram::bindUniforms()
{
    for ( Uniforms::iterator uiter = m_uniforms.begin();
          uiter != m_uniforms.end(); ++uiter )
    {
        const std::string &uname = (*uiter).first;
        const SimpleUniform &data = (*uiter).second;

        GLint uid = glGetUniformLocation( m_progId, uname.c_str() );
        if ( uid >= 0 )
        {
            switch ( data.size )
            {
            case 1: glUniform1f( uid, data.v[0] ); break;
            case 2: glUniform2f( uid, data.v[0], data.v[1] ); break;
            case 3: glUniform3f( uid, data.v[0], data.v[1],
                                 data.v[2] ); break;
            case 4: glUniform4f( uid, data.v[0], data.v[1],
                                 data.v[2], data.v[3] ); break;
            };
        }
        else
        {
            std::cerr << "WARNING: Program: " << m_name
                      << " has no uniform named: "
                      << uname << std::endl;
        }
    }
}

//-*****************************************************************************
void GlslProgram::bindTextures()
{
    m_bindings.clear();

    // This loop uniquely identifies a "texture unit"
    // with a named texture sampler in a shader. It makes
    // sure that no two pieces of data passed in share the
    // same textureId.
    // texUnit = ordered set of texture "slots", 0-N, available to programs.
    // texId = identifier of texture in the OpenGL state.
    // texTarget = 1D, 2D, 3D, RECT.
    for ( Textures::iterator titer = m_textures.begin();
          titer != m_textures.end(); ++titer )
    {
        const std::string &texName = (*titer).first;
        Texture tex = (*titer).second;
        const GLenum texTarget = tex.first;
        const GLuint texId = tex.second;

        int thisTexUnit = 0;
        int numBindings = m_bindings.size();
        int found;
        for ( found = 0; found < numBindings; ++found )
        {
            if ( m_bindings[found].second == texId )
            {
                break;
            }
        }

        if ( found < numBindings )
        {
            thisTexUnit = found;
        }
        else
        {
            thisTexUnit = numBindings;
            m_bindings.push_back( tex );
            glActiveTexture( GL_TEXTURE0 + thisTexUnit );
            glBindTexture( texTarget, texId );
            glEnable( texTarget );
        }

        GLint uid = glGetUniformLocation( m_progId, texName.c_str() );
        if ( uid >= 0 )
        {
            glUniform1i( uid, thisTexUnit );
        }
        else
        {
            std::cerr << "WARNING: Program: " << m_name
                      << " has no texture named: "
                      << texName << std::endl;
        }
    }
}

//-*****************************************************************************
void GlslProgram::unbindTextures()
{
    // It's unclear whether any of this is actually necessary.
    // It may actually be a huge performance hit.
    int numUnits = m_bindings.size();
    for ( int texUnit = 0; texUnit < numUnits; ++texUnit )
    {
        glActiveTexture( GL_TEXTURE0 + texUnit );
        GLenum texTarget = m_bindings[texUnit].first;
        glBindTexture( texTarget, 0 );
        glDisable( texTarget );
    }
    m_bindings.clear();
    glActiveTexture( GL_TEXTURE0 );
}

} // End namespace GLUtil
} // End namespace Alembic
