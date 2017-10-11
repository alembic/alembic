//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <Alembic/AbcGeom/ISubD.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
MeshTopologyVariance ISubDSchema::getTopologyVariance() const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISubDSchema::getTopologyVariance()" );

    if ( m_faceIndicesProperty.isConstant() && m_faceCountsProperty.isConstant() )
    {
        if ( m_positionsProperty.isConstant() )
        {
            return kConstantTopology;
        }
        else
        {
            return kHomogenousTopology;
        }
    }
    else
    {
        return kHeterogenousTopology;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw
    return kConstantTopology;
}

//-*****************************************************************************
size_t ISubDSchema::getNumSamples() const
{
    size_t max = 0;

    for ( size_t i = 0 ; i < this->getNumProperties() ; i++ )
    {
        const AbcA::PropertyHeader& ph = this->getPropertyHeader( i );
        if ( ph.isArray() )
        {
            max = std::max( max,
                            Abc::IArrayProperty(
                                this->getPtr(),
                                ph.getName() ).getNumSamples() );
        }
        else if ( ph.isScalar() )
        {
            max = std::max( max,
                            Abc::IScalarProperty(
                                this->getPtr(),
                                ph.getName() ).getNumSamples() );
        }
    }

    return max;
}

//-*****************************************************************************
void ISubDSchema::get( ISubDSchema::Sample &oSample,
                       const Abc::ISampleSelector &iSS ) const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISubDSchema::get()" );

    m_positionsProperty.get( oSample.m_positions, iSS );
    m_faceIndicesProperty.get( oSample.m_faceIndices, iSS );
    m_faceCountsProperty.get( oSample.m_faceCounts, iSS );

    if ( m_faceVaryingInterpolateBoundaryProperty )
    {
        m_faceVaryingInterpolateBoundaryProperty.get(
            oSample.m_faceVaryingInterpolateBoundary, iSS );
    }
    else
    {
        oSample.m_faceVaryingInterpolateBoundary = 0;
    }

    if ( m_faceVaryingPropagateCornersProperty )
    {
        m_faceVaryingPropagateCornersProperty.get(
            oSample.m_faceVaryingPropagateCorners, iSS );
    }
    else
    {
        oSample.m_faceVaryingPropagateCorners = 0;
    }

    if ( m_interpolateBoundaryProperty )
    {
        m_interpolateBoundaryProperty.get( oSample.m_interpolateBoundary, iSS );
    }
    else
    {
        oSample.m_interpolateBoundary = 0;
    }

    m_selfBoundsProperty.get( oSample.m_selfBounds, iSS );

    if ( m_creaseIndicesProperty )
    { m_creaseIndicesProperty.get( oSample.m_creaseIndices, iSS ); }

    if ( m_creaseLengthsProperty )
    { m_creaseLengthsProperty.get( oSample.m_creaseLengths, iSS ); }

    if ( m_creaseSharpnessesProperty )
    { m_creaseSharpnessesProperty.get( oSample.m_creaseSharpnesses, iSS ); }

    if ( m_cornerIndicesProperty )
    { m_cornerIndicesProperty.get( oSample.m_cornerIndices, iSS ); }

    if ( m_cornerSharpnessesProperty )
    { m_cornerSharpnessesProperty.get( oSample.m_cornerSharpnesses, iSS ); }

    if ( m_holesProperty )
    { m_holesProperty.get( oSample.m_holes, iSS ); }

    if ( m_subdSchemeProperty )
    {
        m_subdSchemeProperty.get( oSample.m_subdScheme, iSS );
    }
    else
    {
        oSample.m_subdScheme = "catmull-clark";
    }

    if ( m_velocitiesProperty && m_velocitiesProperty.getNumSamples() > 0 )
    { m_velocitiesProperty.get( oSample.m_velocities, iSS ); }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
const ISubDSchema &
ISubDSchema::operator=(const ISubDSchema & rhs)
{
    IGeomBaseSchema<SubDSchemaInfo>::operator=(rhs);

    m_positionsProperty = rhs.m_positionsProperty;
    m_velocitiesProperty = rhs.m_velocitiesProperty;
    m_faceIndicesProperty = rhs.m_faceIndicesProperty;
    m_faceCountsProperty = rhs.m_faceCountsProperty;
    m_faceVaryingInterpolateBoundaryProperty =
        rhs.m_faceVaryingInterpolateBoundaryProperty;
    m_faceVaryingPropagateCornersProperty =
        rhs.m_faceVaryingPropagateCornersProperty;
    m_interpolateBoundaryProperty = rhs.m_interpolateBoundaryProperty;
    m_creaseIndicesProperty = rhs.m_creaseIndicesProperty;
    m_creaseLengthsProperty = rhs.m_creaseLengthsProperty;
    m_creaseSharpnessesProperty = rhs.m_creaseSharpnessesProperty;
    m_cornerIndicesProperty = rhs.m_cornerIndicesProperty;
    m_cornerSharpnessesProperty = rhs.m_cornerSharpnessesProperty;
    m_holesProperty = rhs.m_holesProperty;
    m_subdSchemeProperty = rhs.m_subdSchemeProperty;
    m_uvsParam = rhs.m_uvsParam;
    m_faceVaryingInterpolateBoundaryProperty =
        rhs.m_faceVaryingInterpolateBoundaryProperty;

    // lock, reset
    Alembic::Util::scoped_lock l(m_faceSetsMutex);
    m_faceSetsLoaded = false;
    m_faceSets.clear();
    return *this;
}

//-*****************************************************************************
void ISubDSchema::init( const Abc::Argument &iArg0,
                        const Abc::Argument &iArg1 )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISubDSchema::init()" );

    AbcA::CompoundPropertyReaderPtr _this = this->getPtr();

    Abc::Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );

    // no matching so we pick up old assets written as V3f
    m_positionsProperty = Abc::IP3fArrayProperty( _this, "P", kNoMatching,
        args.getErrorHandlerPolicy() );

    m_faceIndicesProperty = Abc::IInt32ArrayProperty( _this, ".faceIndices",
                                                      iArg0, iArg1 );
    m_faceCountsProperty = Abc::IInt32ArrayProperty( _this, ".faceCounts",
                                                     iArg0, iArg1 );

    if ( this->getPropertyHeader(".faceVaryingInterpolateBoundary") != NULL )
    {
        m_faceVaryingInterpolateBoundaryProperty =
            Abc::IInt32Property( _this, ".faceVaryingInterpolateBoundary",
                                 iArg0, iArg1 );
    }

    if ( this->getPropertyHeader(".faceVaryingPropagateCorners") != NULL )
    {
        m_faceVaryingPropagateCornersProperty =
            Abc::IInt32Property( _this, ".faceVaryingPropagateCorners",
                                 iArg0, iArg1 );
    }

    if ( this->getPropertyHeader(".interpolateBoundary") != NULL )
    {
        m_interpolateBoundaryProperty =
            Abc::IInt32Property( _this, ".interpolateBoundary", iArg0, iArg1 );
    }

    // creases, corners, and holes optionally exist
    if ( this->getPropertyHeader(".creaseIndices") != NULL)
    {
        m_creaseIndicesProperty = Abc::IInt32ArrayProperty( _this,
            ".creaseIndices", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader(".creaseLengths") != NULL)
    {
        m_creaseLengthsProperty = Abc::IInt32ArrayProperty( _this,
            ".creaseLengths", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader(".creaseSharpnesses") != NULL)
    {
        m_creaseSharpnessesProperty =
             Abc::IFloatArrayProperty( _this,
                                       ".creaseSharpnesses", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader(".cornerIndices") != NULL)
    {
        m_cornerIndicesProperty =
            Abc::IInt32ArrayProperty( _this, ".cornerIndices", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader(".cornerSharpnesses") != NULL)
    {
        m_cornerSharpnessesProperty =
            Abc::IFloatArrayProperty( _this, ".cornerSharpnesses",
                                      iArg0, iArg1 );
    }

    if ( this->getPropertyHeader(".holes") != NULL)
    {
        m_holesProperty =
            Abc::IInt32ArrayProperty( _this, ".holes", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader(".scheme") != NULL)
    {
        m_subdSchemeProperty = Abc::IStringProperty( _this, ".scheme",
                                                     iArg0, iArg1 );
    }

    // none of the things below here are guaranteed to exist
    if ( this->getPropertyHeader( "uv" ) != NULL )
    {
        m_uvsParam = IV2fGeomParam( _this, "uv", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader( ".velocities" ) != NULL )
    {
        m_velocitiesProperty = Abc::IV3fArrayProperty( _this, ".velocities",
                                                       iArg0, iArg1 );
    }

    m_faceSetsLoaded = false;

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void ISubDSchema::getFaceSetNames (std::vector <std::string> & oFaceSetNames)
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISubDSchema::getFaceSetNames()" );

    Alembic::Util::scoped_lock l(m_faceSetsMutex);
    loadFaceSetNames();

    for (std::map<std::string, IFaceSet>::const_iterator faceSetIter =
        m_faceSets.begin(); faceSetIter != m_faceSets.end(); ++faceSetIter)
    {
        oFaceSetNames.push_back( faceSetIter->first );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void ISubDSchema::loadFaceSetNames()
{
    // Caller must ensure they have locked m_faceSetsMutex.
    // (allows us to use non-recursive mutex)
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISubDSchema::loadFaceSetNames()" );

    if (!m_faceSetsLoaded)
    {
        // iterate over childHeaders, and if header matches
        // FaceSet add to our vec
        IObject _thisObject = getObject();

        size_t numChildren = _thisObject.getNumChildren();
        for ( size_t childIndex = 0 ; childIndex < numChildren; childIndex++ )
        {
            ObjectHeader const & header = _thisObject.getChildHeader (childIndex);
            if ( IFaceSet::matches( header ) )
            {
                // start out with an empty (invalid IFaceSet)
                // accessor later on will create real IFaceSet object.
                m_faceSets [header.getName ()] = IFaceSet ();
            }
        }
        m_faceSetsLoaded = true;
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
bool
ISubDSchema::hasFaceSet( const std::string &faceSetName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISubDSchema::hasFaceSet (faceSetName)" );

    Alembic::Util::scoped_lock l(m_faceSetsMutex);
    if (!m_faceSetsLoaded)
    {
        loadFaceSetNames();
    }

    return (m_faceSets.find (faceSetName) != m_faceSets.end ());

    ALEMBIC_ABC_SAFE_CALL_END();

    return false;
}

//-*****************************************************************************
IFaceSet
ISubDSchema::getFaceSet( const std::string &iFaceSetName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISubDSchema::getFaceSet()" );
    Alembic::Util::scoped_lock l(m_faceSetsMutex);
    if (!m_faceSetsLoaded)
    {
        loadFaceSetNames();
    }

    ABCA_ASSERT( m_faceSets.find (iFaceSetName) != m_faceSets.end (),
        "The requested FaceSet name can't be found in SubD.");

    if (!m_faceSets [iFaceSetName])
    {
        IObject thisObj = getObject();
        // We haven't yet loaded the faceSet, so create/load it
        m_faceSets [iFaceSetName] = IFaceSet ( thisObj, iFaceSetName );
    }

    return m_faceSets [iFaceSetName];

    ALEMBIC_ABC_SAFE_CALL_END();

    IFaceSet empty;
    return empty;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcGeom
} // End namespace Alembic
