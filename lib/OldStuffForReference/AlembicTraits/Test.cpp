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

#include <AlembicTraits/Test.h>
#include <iostream>

namespace Abc = AlembicAsset;

namespace AlembicTraits {

//-*****************************************************************************
// Test out our macros.

// String Traits.
ALEMBIC_STRING_TRAIT( BonesBrigadeProtocolStrait,
                      "AlembicBonesBrigade_v0001" );

ALEMBIC_STRING_TRAIT( SteveCaballeroStrait, "steve|caballero" );
ALEMBIC_STRING_TRAIT( TommyGuerreroStrait, "tommy|guerrero" );
ALEMBIC_STRING_TRAIT( TonyHawkStrait, "tony|hawk" );
ALEMBIC_STRING_TRAIT( MikeMcGillStrait, "mike|mcgill" );
ALEMBIC_STRING_TRAIT( LanceMountainStrait, "lance|mountain" );

// Property Traits
ALEMBIC_PROPERTY_TRAIT( SteveCaballeroTrait, IntProperty,
                        SteveCaballeroStrait );

ALEMBIC_OPT_PROPERTY_TRAIT( TommyGuerreroTrait, FloatProperty,
                            TommyGuerreroStrait );

ALEMBIC_NONANIM_PROPERTY_TRAIT( TonyHawkTrait, V3fArrayProperty,
                                TonyHawkStrait );

ALEMBIC_OPT_NONANIM_PROPERTY_TRAIT( MikeMcGillTrait, V2fArrayProperty,
                                    MikeMcGillStrait );

ALEMBIC_PROPERTY_TRAIT( LanceMountainTrait, Box3dProperty,
                        LanceMountainStrait );

// Amalgams.
ALEMBIC_AMALGAM_TRAIT5( BonesBrigadeTrait,
                        SteveCaballeroTrait, steve,
                        TommyGuerreroTrait, tommy,
                        TonyHawkTrait, tony,
                        MikeMcGillTrait, mike,
                        LanceMountainTrait, lance );

// Trait Objects.
ALEMBIC_TRAIT_OBJECT( BonesBrigade, BonesBrigadeTrait,
                      BonesBrigadeProtocolStrait );

//-*****************************************************************************
int BonesBrigadeTest( std::ostream &ostr )
{
    Abc::TimeSamplingInfo tinfo( Abc::kUniformTimeSampling,
                                 1.0/24.0,
                                 1.0/24.0 );

    Abc::OAsset oasset( "bonesBrigadeTest.abc", Abc::kThrowException );
    ostr << "Created Bones Brigade Test asset" << std::endl;
    {
        OBonesBrigade oBonesBrigade( oasset, "bonesBrigade",
                                     Abc::kThrowException );
        oBonesBrigade->steve().set( 5 );
        ostr << "Wrote Steve." << std::endl;
        
        oBonesBrigade->tommy().addOptional( Abc::kThrowException );
        oBonesBrigade->tommy()->makeAnimated( tinfo );
        oBonesBrigade->tommy()->set( 17.0f );
        oBonesBrigade->tommy()->setAnim( ( size_t )0, 16.0f );
        oBonesBrigade->tommy()->setAnim( ( size_t )1, 15.0f );
        oBonesBrigade->tommy()->setAnim( ( size_t )2, 14.0f );
        ostr << "Wrote Tommy." << std::endl;
        
        std::vector<Abc::V3f> points;
        points.push_back( Abc::V3f( 0.0f ) );
        points.push_back( Abc::V3f( 1.0f ) );
        points.push_back( Abc::V3f( 2.0f ) );
        points.push_back( Abc::V3f( 3.0f ) );
        oBonesBrigade->tony().set( ( const Abc::V3f * )&points.front(),
                                   points.size() );
        ostr << "Wrote Tony." << std::endl;
        
        std::vector<Abc::V2f> uvs;
        uvs.push_back( Abc::V2f( 0.0f ) );
        uvs.push_back( Abc::V2f( 1.0f ) );
        oBonesBrigade->mike().addOptional( Abc::kThrowException );
        oBonesBrigade->mike()->set( ( const Abc::V2f * )&uvs.front(),
                                    uvs.size() );
        ostr << "Wrote Mike." << std::endl;

        oBonesBrigade->lance().set( Abc::Box3d(
                                        Abc::V3d( -1.0 ),
                                        Abc::V3d( 1.0 ) ) );
        ostr << "Wrote Lance." << std::endl;
    }
    oasset.close( Abc::kThrowException );
    ostr << "Closed Bones Brigade Test asset" << std::endl;

    Abc::IAsset iasset( "bonesBrigadeTest.abc", Abc::kThrowException );
    ostr << "Read Bones Brigade Test asset" << std::endl;
    {
        IBonesBrigade inputBonesBrigade( iasset,
                                         "bonesBrigade",
                                         Abc::kThrowException );
        ostr << "Steve (should be 5): "
             << inputBonesBrigade->steve().get() << std::endl;

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
            return -1;
        }

        Abc::IV3fSample tonyRest = inputBonesBrigade->tony().get();
        ostr << "Tony's data: " << std::endl;
        for ( size_t idx = 0; idx < tonyRest.size(); ++idx )
        {
            ostr << tonyRest[idx] << " ";
        }
        ostr << std::endl;

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
            return -1;
        }
        
        Abc::Box3d lanceBox = inputBonesBrigade->lance().get();
        ostr << "Lance: "
             << lanceBox.min << " to " << lanceBox.max << std::endl;
    }
    iasset.close( Abc::kThrowException );
    ostr << "Closed Bones Brigade Test asset" << std::endl;

    return 0;
}

} // End namespace AlembicTraits
