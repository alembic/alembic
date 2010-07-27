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
// EXAMPLE3 - INTRODUCTION
//
// Hello Alembic User! This is the third Example Usage file, and this one
// is going to demonstrate how to add your own compile-time "higher level"
// types, in the way that SimplePolyMesh and SimpleXform have been declared
// inside AlembicTraitsGeom.
//
// In the first example, we used AlembicTraitsGeom SimplePolyMesh for output
// and input, and in the second example we saw how to use Alembic's Objects
// and Properties to manipulate data without strong object-typing.
//
// In everyday practice, it is common to have objects which always have
// certain properties. In working with these, it is tedious to add an
// OProperty or an IProperty for each and every property you expect, every
// time you do so.  It's error-prone and tedious.  What you'd like to be
// able to do is declare a type of object, associated with a particular
// protocol, and then to declare what properties (by type and name) this
// object will have, and furthermore to expose these properties through a
// function call on the object itself - so, in the same way that
// SimpleXform has an "xform()" function which provides access to the
// M44dProperty "xform|local", we define a type and the properties, with
// names, that it'll have, and how to access them.
//
// We also allow for the specification of properties that are optional,
// and specification of properties that are non-animateable, and both!
//
// This API that we're about to tour is really a meta-API, a language for
// specifying data types, as opposed to a language for manipulating data.
// While it uses PreProcessor macros, the same things can be done without them,
// just more laboriously.
//
// We'll use the famous Bones Brigade Skate Team as the basis of our
// contrived example here.
// 
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
//-*****************************************************************************
// INCLUDES
//
// We need AlembicTraits and AlembicAsset for this example.
//-*****************************************************************************
//-*****************************************************************************

#include <AlembicTraits/AlembicTraits.h>
#include <AlembicAsset/AlembicAsset.h>

// Others
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//-*****************************************************************************
//-*****************************************************************************
// NAMESPACES
//
// AlembicTraits is in the AlembicAsset namespace. Just promote it!
//-*****************************************************************************
//-*****************************************************************************

namespace Abc = AlembicAsset;

//-*****************************************************************************
//-*****************************************************************************
// STRING TRAITS!
//
// Most of the type-specification we're about to do requires string literals
// to differentiate types - For example, the "XformLocalTrait" in
// AlembicTraitsGeom::SimpleXform is defined as "the non-optional,
// animateable M44dProperty that is named 'xform|local'". Similarly, the
// SimpleXform object itself is, "an object with the traits below and a
// protocol of 'AlembicSimpleXform_v0001'.
//
// So, this is just typeifying a few names.
// I use the suffix "Strait" for string traits.
//-*****************************************************************************
//-*****************************************************************************

// This strait will be used for the actual object protocol
ALEMBIC_STRING_TRAIT( BonesBrigadeProtocolStrait,
                      "AlembicBonesBrigade_v0001" );

// These straits are for the properties, one for each of the legendary
// skaters of yore.
ALEMBIC_STRING_TRAIT( SteveCaballeroStrait, "steve|caballero" );
ALEMBIC_STRING_TRAIT( TommyGuerreroStrait, "tommy|guerrero" );
ALEMBIC_STRING_TRAIT( TonyHawkStrait, "tony|hawk" );
ALEMBIC_STRING_TRAIT( MikeMcGillStrait, "mike|mcgill" );
ALEMBIC_STRING_TRAIT( LanceMountainStrait, "lance|mountain" );


//-*****************************************************************************
//-*****************************************************************************
// PROPERTY TRAITS!
//
// A Property Trait is basically a way of saying,
// "...has a property of THIS type with THIS name". You can also say
// that the property is optional, or that the flight is non-animated.
//
// An optional property will not cause the object to consider itself invalid
// if the property is absent, and yet allows you to still offer named access
// to the property via the class.
//
// A non-animated property is primarily a notational thing, though
// the 'makeAnimated' functions have been disabled.
//
// Note: PropertyTraits can be used by other objects - it's a nice
// shorthand to get a property and its desired name all wrapped up in one!
//-*****************************************************************************
//-*****************************************************************************

// Let's make the SteveCaballero a "normal" (non-optional, animateable)
// int property.
ALEMBIC_PROPERTY_TRAIT( SteveCaballeroTrait, IntProperty, 
                        SteveCaballeroStrait );

// Tommy Guerrero is optional. No slight to you, sir, Tommy. 
ALEMBIC_OPT_PROPERTY_TRAIT( TommyGuerreroTrait, FloatProperty,
                            TommyGuerreroStrait );

// Tony Hawk is non-animateable. This is a somewhat ironic statement.
// He's also a V3fArray property, which is interesting.
ALEMBIC_NONANIM_PROPERTY_TRAIT( TonyHawkTrait, V3fArrayProperty,
                                TonyHawkStrait );

// Mike McGill is a V2fArray property, both optional AND non-animateable.
ALEMBIC_OPT_NONANIM_PROPERTY_TRAIT( MikeMcGillTrait, V2fArrayProperty,
                                    MikeMcGillStrait );

// Lastly, Lance Mountain is a regular Box3d property.
ALEMBIC_PROPERTY_TRAIT( LanceMountainTrait, Box3dProperty,
                        LanceMountainStrait );


//-*****************************************************************************
//-*****************************************************************************
// AMALGAMS!
//
// An amalgam is an amalgamation of other traits into a single trait, along
// with the creation of member functions with which to access each of the
// member traits.
// 
// We want there to be one and only one 'master' trait for each trait object,
// so we combine them all up to a top trait - in this case, the
// "BonesBrigadeTrait".
//-*****************************************************************************
//-*****************************************************************************


//-*****************************************************************************
// There are macros for amalgams from 2 to 6 - the underlying classes
// support up to 10 subtraits.  The intention is that things will be
// grouped somewhat hierarchically.
//
// Also, these macros, and even the Traits library in general, are just
// conveniences - they're not strictly required for Alembic (as the second
// example showed).
ALEMBIC_AMALGAM_TRAIT5(

    // Name of the trait. This is a type name, and can be instanced.
    BonesBrigadeTrait,

    // First of 5 (since we called TRAIT5) members.
    // We're saying we want member0 to be the SteveCaballeroTrait, and
    // accessed via a member function named "steve".
    SteveCaballeroTrait, steve,

    // to access Tommy, tommmy()
    TommyGuerreroTrait, tommy,

    // to access Tony, tony()
    TonyHawkTrait, tony,

    // to access Mike, mike()
    MikeMcGillTrait, mike,

    // to access Lance, lance()
    LanceMountainTrait, lance );

//-*****************************************************************************
//-*****************************************************************************
// TRAIT OBJECTS!!!
//
// An AlembicTrait object is an Object that has one (and only one) trait
// associated with it. To instantiate one of these objects as an ostream,
// you automatically create the non-optional properties, and you have
// "addOptional" support for the optional ones.  To instantiate one
// of these objects as an istream, you're declaring that the object
// must have the specified protocol, AND it must have all of the features
// of the trait.
//
// Amalgams are used to combine things together into a single, top-level
// trait. This has the secondary side benefit of being able to say,
// "I wish my object to have all the features of the BonesBrigadeObject,
//  but with some other protocol and additional stuff." by simply
//  instantiating the BonesBrigadeTrait without creating a BonesBrigade object.
// Similarily, using the BonesBrigadeTrait on an input object is a way of
// asking, "can this object behave like a BonesBrigade object"?, regardless
// of protocol.
//-*****************************************************************************
//-*****************************************************************************

// Make the BonesBrigade object - this will actually create the objects
// OBonesBrigade and IBonesBrigade. The trait functions above also created
// OBonesBrigadeTrait, IBonesBrigadeTrait, etc.
ALEMBIC_TRAIT_OBJECT(

    // Name of the new object types
    // Actually OBonesBrigade and IBonesBrigade
    BonesBrigade,

    // Name of the trait defined above that defines this trait object.
    // The macros above actually create OBonesBrigadeTrait and
    // IBonesBrigadeTrait
    BonesBrigadeTrait,

    // The string trait corresponding to our protocol
    BonesBrigadeProtocolStrait );

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// TEST BONES BRIGADE
//
// I'll keep the comments inside to a minimum, since they've been explained
// in previous examples.
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
void BonesBrigade_Out( std::ostream &ostr )
{
    Abc::TimeSamplingInfo tinfo( Abc::kUniformTimeSampling,
                                 1.0/24.0,
                                 1.0/24.0 );

    // Create an output asset to give this a try with.
    Abc::OAsset oasset( "bonesBrigadeTest.abc", Abc::kThrowException );
    ostr << "Created Bones Brigade Test asset" << std::endl;
    
    // Create the bones brigade object. Note that I don't need to
    // specify a protocol!
    OBonesBrigade oBonesBrigade( oasset, "bonesBrigade",
                                 Abc::kThrowException );

    // Now, see that I can access the various properties defined
    // above via the member functions that I requested in my
    // amalgam specifications.
    // Here, we simply set steve's rest value to 5.
    // The -> notation denotes that we are talking to the
    // "trait associated with the object", rather than the object.
    // The long form of this is:
    // oBonesBrigade.trait().steve().set( 5 );
    oBonesBrigade->steve().set( 5 );
    ostr << "Wrote Steve." << std::endl;

    // Tommy is an optional property, so we have to add him before
    // doing anything.
    oBonesBrigade->tommy().addOptional( Abc::kThrowException );

    // Because tommy is an optional property, we have to use the
    // pointer notation to talk to the underlying property. This is
    // because it has the option of not existing!
    // Here we're going to animate tommy as well.
    oBonesBrigade->tommy()->makeAnimated( tinfo );
    oBonesBrigade->tommy()->set( 17.0f );
    oBonesBrigade->tommy()->setAnim( ( size_t )0, 16.0f );
    oBonesBrigade->tommy()->setAnim( ( size_t )1, 15.0f );
    oBonesBrigade->tommy()->setAnim( ( size_t )2, 14.0f );
    ostr << "Wrote Tommy." << std::endl;

    // Tony is a V3fArray property, so we need to make a quick
    // array to set him with.
    std::vector<Abc::V3f> points;
    points.push_back( Abc::V3f( 0.0f ) );
    points.push_back( Abc::V3f( 1.0f ) );
    points.push_back( Abc::V3f( 2.0f ) );
    points.push_back( Abc::V3f( 3.0f ) );
    oBonesBrigade->tony().set( ( const Abc::V3f * )&points.front(),
                               points.size() );
    ostr << "Wrote Tony." << std::endl;

    // Mike is also an array, so make data for him too.
    // Because Mike is optional, we add him.
    std::vector<Abc::V2f> uvs;
    uvs.push_back( Abc::V2f( 0.0f ) );
    uvs.push_back( Abc::V2f( 1.0f ) );
    oBonesBrigade->mike().addOptional( Abc::kThrowException );
    oBonesBrigade->mike()->set( ( const Abc::V2f * )&uvs.front(),
                                uvs.size() );
    ostr << "Wrote Mike." << std::endl;

    // Lastly, set Lance
    oBonesBrigade->lance().set( Abc::Box3d(
                                    Abc::V3d( -1.0 ),
                                    Abc::V3d( 1.0 ) ) );
    ostr << "Wrote Lance." << std::endl;

    // Close up shop!
    // We technically don't need to do this, it'll close itself
    // when we leave this scope, but here we can make it throw
    // an exception if there's a problem.
    oasset.close( Abc::kThrowException );
    ostr << "Closed Bones Brigade Test asset\n\n\n" << std::endl;
}

//-*****************************************************************************
// Just try loading it in.
void BonesBrigade_In( std::ostream &ostr )
{
    // Read it back in.
    Abc::IAsset iasset( "bonesBrigadeTest.abc", Abc::kThrowException );
    ostr << "Read Bones Brigade Test asset" << std::endl;

    // This will make sure that not only is the asset there,
    // and named "bonesBrigade", but has the right protocol and all
    // the right properties, as we have declared.
    IBonesBrigade inputBonesBrigade( iasset, "bonesBrigade",
                                     Abc::kThrowException );

    // Get steve's data.
    ostr << "Steve (should be 5): "
         << inputBonesBrigade->steve().get() << std::endl;

    // Because Tommy is an optional property, he may not exist. Here's
    // how you check.
    if ( inputBonesBrigade->tommy() )
    {
        ostr << "Tommy exists. (As he should) " << std::endl
             << "Is he animated (should be yes)? "
             << inputBonesBrigade->tommy().isAnimated()
             << std::endl;
        
        ostr << "Tommy rest (should be 17.0): "
             << inputBonesBrigade->tommy().get()
             << std::endl
             << "Tommy sample 0 (should be 16.0): "
             << inputBonesBrigade->tommy().getAnim( ( size_t )0 )
             << std::endl
             << "Tommy sample 1 (should be 15.0): "
             << inputBonesBrigade->tommy().getAnim( ( size_t )1 )
             << std::endl
             << "Tommy sample 2 (should be 14.0): "
             << inputBonesBrigade->tommy().getAnim( ( size_t )2 )
             << std::endl;
    }
    else
    {
        std::cerr << "ERROR: Tommy should have existed. Boo."
                  << std::endl;
        exit( -1 );
    }

    // Get tony's array data.
    Abc::IV3fSample tonyRest = inputBonesBrigade->tony().get();
    ostr << "Tony's data: " << std::endl;
    for ( size_t idx = 0; idx < tonyRest.size(); ++idx )
    {
        ostr << tonyRest[idx] << " ";
    }
    ostr << std::endl;

    // Mike is an optional property, so get his data.
    // He's an array too!
    if ( inputBonesBrigade->mike() )
    {
        ostr << "Mike exists. (As he should) " << std::endl;
        
        Abc::IV2fSample mikeRest = inputBonesBrigade->mike().get();
        ostr << "Mike's data: " << std::endl;
        for ( size_t idx = 0; idx < mikeRest.size(); ++idx )
        {
            ostr << mikeRest[idx] << " ";
        }
        ostr << std::endl;
    }
    else
    {
        std::cerr << "ERROR: Mike should have existed. Boo."
                  << std::endl;
        exit( -1 );
    }

    // Finally, get Lance's data.
    Abc::Box3d lanceBox = inputBonesBrigade->lance().get();
    ostr << "Lance: "
         << lanceBox.min << " to " << lanceBox.max << std::endl;

    // And again, close the asset, this time explicitly, to see if
    // any exceptions occur.
    iasset.close( Abc::kThrowException );
    ostr << "Closed Bones Brigade Test asset" << std::endl;
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
    BonesBrigade_Out( std::cout );

    // And back in.
    BonesBrigade_In( std::cout );

    return 0;
}
