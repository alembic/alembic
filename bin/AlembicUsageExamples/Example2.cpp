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

//-*****************************************************************************
//-*****************************************************************************
// EXAMPLE2 - INTRODUCTION
//
// Hello Alembic User! This is the second Example Usage file, and so we'll
// now dig a little deeper into the AlembicAsset base library. In the first
// example, we saw how to use the higher-level primitive "SimplePolyMesh"
// from the AlembicTraitsGeom library. The high-level primitives make it
// very easy to simply read or write a specific type of data with ease.
// However, those types are composed of the more general AlembicAsset base
// types, and significant flexibility is possible through the use of
// AlembicAsset by itself. This example will show how to manipulate
// Objects and Properties directly.
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
//-*****************************************************************************
// INCLUDES
//
// We only need AlembicAsset for this example.
//-*****************************************************************************
//-*****************************************************************************

#include <AlembicAsset/AlembicAsset.h>

// Others
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//-*****************************************************************************
//-*****************************************************************************
// NAMESPACES
//
// Promote!
//-*****************************************************************************
//-*****************************************************************************

namespace Abc = AlembicAsset;

//-*****************************************************************************
//-*****************************************************************************
// WRITE OUT SOME STRUCTURED DATA
//
// AlembicAsset consists of a very small set of primary abstractions.
// The first, which we discussed in the previous example, is the "Asset",
// which is basically just the top-level file in which this data all lives.
// AlembicAssets consist entirely of "Objects", which are entities that
// have a character string name, a character string "protocol", a list
// of children objects - representing the hierarchical nature of Alembic,
// and lastly, a list of "Properties", which are named, typed fields of
// data attached to the object.
//
// Let's write out an asset that has this topology.
//
// Asset fileName="example2.abc"
//   Object name="Europe", protocol="Continent"
//     FloatProperty name="surfaceArea"     - 10,180,000.0 km^2
//     LongProperty name="population"       - 731,000,000
//     FloatProperty name="populationDensity" - tbd
//     IntProperty name="numCountries" - 50
//
//     Object name="Spain", protocol="Country"
//       FloatProperty name="surfaceArea"   - 504,030 km^2
//       IntProperty name="population"      - 45,989,016
//       FloatProperty name="populationDensity" - tbd
//       IntProperty name="numProvinces" - 50
//
//       Object name="Barcelona", protocol="City"
//         FloatProperty name="surfaceArea" - 101.4 km^2
//         LongProperty name="population" - 3.186,461
//         FloatProperty name="populationDensity" - tbd
//         FloatProperty name="averageMonthlyTemperatur" - animated!
//             13.4, 14.6, 15.9, 17.6, 20.5, 24.2
//             27.5, 28.0, 25.5, 21.5, 17.0, 14.3
//
//     Object name="Portugal", protocol="Country"
//       FloatProperty name="surfaceArea" - 92,090 km^2
//       LongProperty name="population" - 10,707,924
//       FloatProperty name="populationDensity" - tbd
//       IntProperty name="numProvinces" - 11
//
// Thank you to Wikipedia for providing my example data!
// 
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// We'll use this function later, just forward-declaring it here.
void Example2_SetPopulationInfo( const Abc::OObject &object,
                                 float surfaceArea,
                                 long population );

//-*****************************************************************************
void Example2_DataOut( void )
{
    // Create an OAsset - just like in the previous example
    Abc::OAsset asset( "example2.abc", Abc::kThrowException );

    // Create an object named "Europe", with protocol "Continent".
    Abc::OObject europe(

        // Created as a child of the asset, just like in the first
        // example.
        asset,

        // Name of the object
        "Europe",

        // Protocol for the object. The protocol string is a way of
        // describing what KIND of object you are - it is a type
        // identifier. However, in Alembic, we support the
        // idea of "duck typing" - which is to say, if it walks like
        // a duck, talks like a duck, and quacks like a duck, it is
        // a duck. In this sense, regardless of what the protocol
        // says, if you're looking for something that has the properties
        // "surfaceArea", "population", and "populationDensity",
        // you can grab those properties and use them, regardless of
        // what the protocol says, and in this way, the protocol is
        // essentially just a "hint".  We'll exercise this by
        // using the "SetPopulationInfo" function above in a polymorphic
        // way.
        "Continent",

        // And throw an exception if there is a problem.
        Abc::kThrowException );

    // Create an int property of "Europe" describing how many countries
    // it has, and immediately set it to its value.
    Abc::OIntProperty europeNumCountries( europe, "numCountries" );
    europeNumCountries.set( 50 );

    // Create an object named "Spain", with protocol, "Country",
    // as a child of Europe.
    Abc::OObject spain( europe, "Spain", "Country", Abc::kThrowException );

    // Create an int property of "Spain" describing how many provinces
    // it has, and immediately set it to its value.
    Abc::OIntProperty spainNumProvinces( spain, "numProvinces" );
    spainNumProvinces.set( 50 );

    // Create an object named "Barcelona", with protocol, "City",
    // as a child of Spain.
    Abc::OObject barcelona( spain, "Barcelona", "City",
                            Abc::kThrowException );

    // Create an animated float property of barcelona describing its
    // average monthly temperature, in degrees centigrade.
    Abc::OFloatProperty avgMonthTemp( barcelona,
                                      "averageMonthlyTemperature",
                                      Abc::kThrowException );

    // Rest pose is somewhat meaningless in this case. 
    avgMonthTemp.set( 0.0f );

    // Animated!
    static const Abc::seconds_t DaysToSeconds = 60.0 * 60.0 * 24.0;
    Abc::TimeSamplingInfo tinfo(

        // Variable time sampling means that every sample also stores
        // a time value. They must be written in ascending order.
        Abc::kVariableTimeSampling );
    
    avgMonthTemp.makeAnimated( tinfo );

    // Okay, now set the animated values!
    // January - put the sample near the middle of the month
    // instead of the end.
    Abc::seconds_t daysSoFar = 31.0 - 15.0;
    avgMonthTemp.setAnim( 0, DaysToSeconds * daysSoFar, 13.4f );

    // Feburary - non leap year.
    daysSoFar += 28.0;
    avgMonthTemp.setAnim( 1, DaysToSeconds * daysSoFar, 14.6f );

    // March
    daysSoFar += 31.0;
    avgMonthTemp.setAnim( 2, DaysToSeconds * daysSoFar, 15.9f );

    // April
    daysSoFar += 30.0;
    avgMonthTemp.setAnim( 3, DaysToSeconds * daysSoFar, 17.6f );

    // May
    daysSoFar += 31.0;
    avgMonthTemp.setAnim( 4, DaysToSeconds * daysSoFar, 20.5f );

    // June
    daysSoFar += 30.0;
    avgMonthTemp.setAnim( 5, DaysToSeconds * daysSoFar, 24.2f );

    // July
    daysSoFar += 31.0;
    avgMonthTemp.setAnim( 6, DaysToSeconds * daysSoFar, 27.5f );

    // August
    daysSoFar += 31.0;
    avgMonthTemp.setAnim( 7, DaysToSeconds * daysSoFar, 28.0f );

    // September
    daysSoFar += 30.0;
    avgMonthTemp.setAnim( 8, DaysToSeconds * daysSoFar, 25.5f );

    // October
    daysSoFar += 31.0;
    avgMonthTemp.setAnim( 9, DaysToSeconds * daysSoFar, 21.5f );

    // November
    daysSoFar += 30.0;
    avgMonthTemp.setAnim( 10, DaysToSeconds * daysSoFar, 17.0f );

    // December
    daysSoFar += 31.0;
    avgMonthTemp.setAnim( 11, DaysToSeconds * daysSoFar, 14.3f );

    // Create another country, "Portugal".
    // as a child of Europe.
    Abc::OObject portugal( europe, "Portugal", "Country",
                           Abc::kThrowException );

    // Create an int property of "Portugal" describing how many provinces
    // it has, and immediately set it to its value.
    Abc::OIntProperty portugalNumProvinces( portugal, "numProvinces" );
    portugalNumProvinces.set( 11 );

    // Okay, this is pretty cool. We'll show off the duck-typing idea
    // by passing each of these objects to the "SetPopulationInfo" function,
    // even though some are Continents, some are Countries, and some are
    // Cities.
    Example2_SetPopulationInfo( europe, 10180000.0f, 731000000 );
    Example2_SetPopulationInfo( spain, 504030.0f, 45989016 );
    Example2_SetPopulationInfo( barcelona, 101.4f, 3186461 );
    Example2_SetPopulationInfo( portugal, 92090.0f, 10707924 );

    // The objects close themselves when they go out of scope, so, bravo!
    std::cout << "Wrote Alembic Asset: \"example2.abc\"" << std::endl;
}

//-*****************************************************************************
void Example2_SetPopulationInfo( const Abc::OObject &object,
                                 float SurfaceArea,
                                 long Population )
{
    // Create a float property for surfaceArea and set it.
    Abc::OFloatProperty surfaceArea( object, "surfaceArea",
                                     Abc::kThrowException );
    surfaceArea.set( SurfaceArea );

    // Create a long property for population and set it.
    Abc::OLongProperty population( object, "population",
                                   Abc::kThrowException );
    population.set( Population );

    // Create an average population density property and set it.
    Abc::OFloatProperty popDens( object, "populationDensity",
                                 Abc::kThrowException );
    popDens.set( (( float )Population)/SurfaceArea );

    // Properties close themselves on scope exit. Hoot!
}

//-*****************************************************************************
//-*****************************************************************************
// READING IN A CUSTOM DATA LAYOUT
//
// Let's just do a simple thing where we find out what the likely
// temperature in barcelona, in spain, in europe, will be on my birthday,
// november 6th.
//
// We'll be cavalier with this, and not introspect at all, but just jump
// right to what we want.
//
// This example also highlights another Alembic design principle - we don't
// put interpolation code down into the low-level objects (or even the
// AlembicTraitsGeom objects) because there are so many ways to interpolate,
// and we don't want to attempt (and fail) to exhaustively cover them. We
// instead provide the necessary lower & upper bound functions on time
// sampling, and the host applications can interpolate as they see fit.
//-*****************************************************************************
//-*****************************************************************************
void Example2_DataIn( void )
{
    // IAsset, just like Example1.
    Abc::IAsset asset( "example2.abc", Abc::kThrowException );

    // Get Europe.
    // Note that I don't bother to specify a desired protocol, meaning
    // I don't care what the protocol is - I'm just after the object
    // hierarchy. Useful!
    Abc::IObject europe( asset, "Europe", Abc::kThrowException );

    // Get Spain.
    Abc::IObject spain( europe, "Spain", Abc::kThrowException );

    // Get Barcelona
    Abc::IObject barcelona( spain, "Barcelona", Abc::kThrowException );

    // Get the avgMonthlyTemp function.
    Abc::IFloatProperty avgMonthTemp( barcelona,
                                      "averageMonthlyTemperature",
                                      Abc::kThrowException );

    // Get the time sampling.
    // Use the AAH5_ASSERT macro to throw an exception in case of error.
    const Abc::ITimeSampling *tsamp = avgMonthTemp.timeSampling();
    AAH5_ASSERT( tsamp,
                 "Expected monthly temperature to be animated!" );
    
    // Get my birthday in seconds.
    Abc::seconds_t november6 =
        ( 365.0 - 31.0 - 30.0 + 6.0 ) * 24.0 * 60.0 * 60.0;
    std::cout << "November 6th, in seconds: " << november6 << std::endl;

    // Get lower sample.
    size_t lowerSampleIdx = tsamp->lowerBoundSample( november6 );
    Abc::seconds_t lowerSampleSecs = tsamp->sampleSeconds( lowerSampleIdx );
    float lowerSampleTemp = avgMonthTemp.getAnim( lowerSampleIdx );
    std::cout << "Lower Sample: "
              << lowerSampleIdx << ", "
              << lowerSampleSecs << ", "
              << lowerSampleTemp << std::endl;

    // Get upper sample
    size_t upperSampleIdx = tsamp->upperBoundSample( november6 );
    Abc::seconds_t upperSampleSecs = tsamp->sampleSeconds( upperSampleIdx );
    float upperSampleTemp = avgMonthTemp.getAnim( upperSampleIdx );
    std::cout << "Upper Sample: "
              << upperSampleIdx << ", "
              << upperSampleSecs << ", "
              << upperSampleTemp << std::endl;

    // They shouldn't be the same. Use Alembic's assert macro.
    // An interpolator class could make this more elegant.
    AAH5_ASSERT( lowerSampleIdx < upperSampleIdx &&
                 lowerSampleSecs < upperSampleSecs,
                 "November 6th is in middle of the animated keyframe range" );

    // Figure it out!
    float tempNov6Interp = ( november6 - lowerSampleSecs ) /
        ( upperSampleSecs - lowerSampleSecs );
    float tempNov6 = lowerSampleTemp +
        tempNov6Interp * ( upperSampleTemp - lowerSampleTemp );

    std::cout << "Probable temperature, in degrees centigrade, of Barcelona "
              << "on November 6th: " << tempNov6
              << std::endl;
}
    
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MAIN FUNCTION!
// The only important thing here is to remember to do Abc::Init.
// I'm not going to bother with exceptions, since I have no actions I
// could do to deal with them. If something goes wrong, it will cheerfully
// crash and print the exception information.
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
int main( int argc, char *argv[] )
{
    // Initialize Alembic. Must be called before _ANY_ Alembic functions
    // may be used.
    Abc::Init();

    // Data out
    Example2_DataOut();

    // And back in.
    Example2_DataIn();

    return 0;
}
