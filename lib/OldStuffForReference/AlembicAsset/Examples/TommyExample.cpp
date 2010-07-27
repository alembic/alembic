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

#include <AlembicAsset/AlembicAsset.h>
#include <iostream>

//-*****************************************************************************
// Abbreviate.
namespace Abc = AlembicAsset;

//-*****************************************************************************
// Print out the center of mass of a piece of geo in a mesh.
int main( int argc, char *argv[] )
{
    Abc::IContext context;
    Abc::IAssetHandle asset( context, "ironMan2_sc273_v09.abc" );
    Abc::IV3fSPropertyHandle centerOfMass(
        asset, "/IronMan/LeftArm/Shoulder/MetalPlate:centerOfMass" );
    
    std::cout << "Rest pose value for center of mass: "
              << (*centerOfMass).restPoseValue() << std::endl;

    if ( (*centerOfMass).isAnimated() )
    {
        size_t numTimeSamps = (*centerOfMass).numSamples();
        for ( size_t samp = 0; samp < numTimeSamps; ++samp )
        {
            std::cout << "Center of mass at time: "
                      << (*centerOfMass).sampleTime( samp )
                      << " = "
                      << (*centerOfMass).sample( samp )
                      << std::endl;
        }
    }
}

//-*****************************************************************************
void longerForm()
{
    // The context contains the various global caches that are used
    // during reading. It can span multiple assets, or be specific per-object,
    // if desired.

    // This example shows how you would get the V3f value from an animated
    // property at time t, when you know that the object lives in:
    // 
    // /IronMan/LeftArm/Shoulder/MetalPlate:centerOfMass
    // 
    Abc::IContext context;
    Abc::IAssetHandle asset( context, "ironMan2_sc273_v09.abc" );
    Abc::IV3fPropertyHandle CenterOfMass;

    // Scoped
    {
        Abc::IObjectHandle IronMan( asset, "IronMan" );
        Abc::IObjectHandle LeftArm( IronMan, "LeftArm" );
        Abc::IObjectHandle Shoulder( LeftArm, "Shoulder" );
        Abc::IObjectHandle MetalPlate( Shoulder, "MetalPlate" );
        
        // We know that centerOfMass is a singular property (just one value
        // per time), and that it has V3f type.
        CenterOfMass.reset( MetalPlate, "centerOfMass" );
    }
    
    // Print rest-pose value
    std::cout << "Rest pose value for center of mass: "
              << (*CenterOfMass).restPoseValue() << std::endl;

    // If animated, print animated values.
    if ( (*CenterOfMass).isAnimated() )
    {
        size_t numTimeSamps = (*CenterOfMass).numSamples();
        for ( size_t samp = 0; samp < numTimeSamps; ++samp )
        {
            std::cout << "Center of mass at time: "
                      << (*CenterOfMass).sampleTime( samp )
                      << " = "
                      << (*CenterOfMass).sample( samp )
                      << std::endl;
        }
    }

}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// This is a writing example. There are no partial writes, and writes should
// be done at least partially in order.
// Animated Properties must have the number of samples set in advance.

//-*****************************************************************************
// This is just some pretend class that represents the concept of a property
// from some data source - perhaps maya, or a sim tool.
// The normal export process would be in three phases:
// 
// 1. discover objects to be exported, & bind them to appropriate Alembic
// OObjects or OProperties (or wrappers around such).
//
// 2. set rest pose values.
//
// 3. loop over time samples and set animated values for animated properties.
//-*****************************************************************************
class ExternalProperty
{
public:
    ExternalProperty() {}
    virtual ~ExternalProperty() {}

    enum ValueType
    {
        kFloat,
        kDouble,
        kFloat3,
        kDouble3
    };

    virtual std::string name() const = 0;
    virtual ValueType valueType() const = 0;
    
    virtual bool isAnimated() = 0;
    
    virtual void getValue( void *ptrToFill ) = 0;
    virtual void getAnimValue( double secs, void *ptrToFill ) = 0;
};

//-*****************************************************************************
// For the purposes of writing, one would probably bind together
// the external properties with the instance of the alembic OProperty
struct PropWritingPair
{
    ExternalProperty *extProp;
    OPropertyHandle abcProp;
};
typedef std::vector<PropWritingPair> PropWritingPairs;

//-*****************************************************************************
// This function simulates discovery or searching of external properties
// to be exported and creation of the corresponding OProperties.
void HandleProperty( ExternalProperty *prop, PropWritingPairs &pairs )
{
    

}

