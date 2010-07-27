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

#include <Alembic/Asset/AssetAll.h>
#include <iostream>

//-*****************************************************************************
using namespace Alembic::Asset;

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    Alembic::Asset::Init();

    //-*************************************************************************
    // DECLARE THE ASSET AND THE OBJECTS
    //-*************************************************************************

    // Make an asset named "biteMe.abc".
    OAsset asset( "biteMe.abc" );

    // Create an object at the top level of the asset named
    // 'someBloke', with protocol 'timmy'
    OObject object = OObject( asset, "someBloke", "timmy" );

    // Create properties on object named 'firstAttr', 'secondAttr', etc.
    // Pass the time sampling info to indicate that they are animated.
    OFloatProperty a1( object, "firstAttr" );
    OFloatProperty a2( object, "secondAttr" ); 

    // Create an object named 'randyQuaid' with protocol 'inexplicable'
    // as a child of object (the someBloke(timmy) one).
    OObject object2( object, "randyQuaid", "inexplicable" );

    // Create some attributes on object2 - thirdAttr is an ARRAY of floats.
    // Very exciting.
    OFloatArrayProperty a3( object2, "thirdAttr" );
    OIntProperty a4( object2, "fourthAttr" );
    OFloatProperty a5( object2, "fifthAttr" );

    //-*************************************************************************
    // SET THE REST VALUES FOR THE PROPERTIES
    //-*************************************************************************
    a1.set( 9.5f );
    a2.set( 8.5f );

    // Some float array data for a3.
    float stuff[16] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f,
                        8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f };
    
    a3.set( stuff, 16 );
    a4.set( 19 );
    a5.set( 5.5f );

    //-*************************************************************************
    // SET THE ANIMATED VALUES FOR THE PROPERTIES
    //-*************************************************************************
    // Looping over 'time samples', which map to time as described by the
    // TimeSampling info passed into the property constructors.
    // Create info about what kind of time samples our properties will use.
    // Time-sampling information is specified per-property.
    TimeSamplingInfo tsi( kUniformTimeSampling,

                          // First sample seconds
                          0.0,

                          // First sample interval
                          1.0 / 24.0 );
    a1.makeAnimated( tsi );
    a2.makeAnimated( tsi );
    a3.makeAnimated( tsi );
    a4.makeAnimated( tsi );
    a5.makeAnimated( tsi );
    for ( size_t samp = 0; samp < 4; ++samp )
    {
        a1.setAnim( samp, 9.5f + ( float )samp );
        a2.setAnim( samp, 8.5f + ( float )samp );

        // Prepare new array of data for the float-array attribute.
        //for ( int j = 0; j < 16; ++j )
        //{
        //    stuff[j] = ( float )( j + samp );
        //}
        
        a3.setAnim( samp, stuff, 16 );
        a4.setAnim( samp, 72 + ( int )samp );
        a5.setAnim( samp, 9.5f + ( float )samp );
    }

    //-*************************************************************************
    // UM, THAT'S IT.
    //-*************************************************************************
    return 0;
}
