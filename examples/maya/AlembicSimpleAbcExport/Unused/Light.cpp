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

//-*****************************************************************************
// Some portions of this file are:
//
//  Copyright (c) 2009, Tweak Software
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
//     * Neither the name of the Tweak Software nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
//
//  THIS SOFTWARE IS PROVIDED BY Tweak Software ''AS IS'' AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL Tweak Software BE LIABLE FOR
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
//  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//
//-*****************************************************************************


//******************************************************************************
void AbcExporter::LightHeader( MDagPath &dp, int protocolVersion )
{
    MDagPath parent = dp;
    parent.pop();
    m_writer->intern( m_objectName.asChar() );

    m_writer->beginObject( m_objectName.asChar(), "light", protocolVersion );

    // Output the transform header, unless this is a difference file and
    // we didn't ask for difference matrices
    if ( ! ( m_isDifferenceFile && ! m_diffMatrix ) )
    {
        TransformHeader( dp );
    }

    m_writer->endObject();
}

//******************************************************************************
void AbcExporter::LightData( MDagPath &dp )
{
    MFnLight light( dp.node() );
    MStatus status;

//     const char *shader = NULL;
//     if ( dp.hasFn( MFn::kAmbientLight ) ) shader = "ambient_lgt";
//     else if ( dp.hasFn( MFn::kAreaLight ) ) shader = "area_lgt";
//     else if ( dp.hasFn( MFn::kDirectionalLight ) ) shader = "distant_lgt";
//     else if ( dp.hasFn( MFn::kPointLight ) ) shader = "point_lgt";
//     else if ( dp.hasFn( MFn::kSpotLight ) ) shader = "mayaspot_lgt";
//
//     assert( shader );
//
//     // Properties common to all light types:
//     int shaderId = m_writer->lookup( shader );
//     m_writer->propertyData( &shaderId );
//
//     float intensity = light.intensity();
//     m_writer->propertyData( &intensity );
//
//     float color[3];
//     light.color().get( color );
//     m_writer->propertyData( color );
//
//     float Ldiff = 1.0;
//     if ( !light.lightDiffuse( ) ){
//         //this doesn't work for some reason    Ldiff = 0.0;
//     }
//     m_writer->propertyData( &Ldiff );
//
//     float Lspec = 1.0;
//     if ( !light.lightSpecular( ) ){
//         //this doesn't work for some reason    Lspec = 0.0;
//     }
//     m_writer->propertyData( &Lspec );
//
//     // Type-specific properties:
//     if ( dp.hasFn( MFn::kAmbientLight ) )
//     {
//         // None
//     }
//     else if ( dp.hasFn( MFn::kAreaLight ) )
//     {
//         MFnAreaLight area( dp.node() );
//
//         float falloff = (float)area.decayRate();
//         m_writer->propertyData( &falloff );
//     }
//     else if ( dp.hasFn( MFn::kDirectionalLight ) )
//     {
//         // None
//     }
//     else if ( dp.hasFn( MFn::kPointLight ) )
//     {
//         MFnPointLight point( dp.node() );
//
//         float falloff = (float)point.decayRate();
//         m_writer->propertyData( &falloff );
//     }
//     else if ( dp.hasFn( MFn::kSpotLight ) )
//     {
//         MFnSpotLight spot( dp.node() );
//         MFnSpotLight::MDecayRegion decayRegion = MFnSpotLight::kFirst;
//         MFnSpotLight::MBarnDoor    barnDoor    = MFnSpotLight::kRight;
//
//         float falloff = (float)spot.decayRate();
//         m_writer->propertyData( &falloff );
//
//         float coneangle = spot.coneAngle() * 180.0f / M_PI;
//         m_writer->propertyData( &coneangle );
//
//         float conedeltaangle = spot.penumbraAngle() * 180.0f / M_PI;
//         m_writer->propertyData( &conedeltaangle );
//
//         float beamdistribution = (float)spot.dropOff();
//         m_writer->propertyData( &beamdistribution );
//
//         float usedecayregions = (float)spot.useDecayRegions( );
//         m_writer->propertyData( &usedecayregions );
//
//         float decayreg1start = (float)spot.startDistance( decayRegion );
//         m_writer->propertyData( &decayreg1start );
//
//         float decayreg1end = (float)spot.endDistance( decayRegion );
//         m_writer->propertyData( &decayreg1end );
//
//         decayRegion = MFnSpotLight::kSecond;
//         float decayreg2start = (float)spot.startDistance( decayRegion );
//         m_writer->propertyData( &decayreg2start );
//
//         float decayreg2end = (float)spot.endDistance( decayRegion );
//         m_writer->propertyData( &decayreg2end );
//
//         decayRegion = MFnSpotLight::kThird;
//         float decayreg3start = (float)spot.startDistance( decayRegion );
//         m_writer->propertyData( &decayreg3start );
//
//         float decayreg3end = (float)spot.endDistance( decayRegion );
//         m_writer->propertyData( &decayreg3end );
//
//         float usebarndoors = (float)spot.barnDoors( );
//         m_writer->propertyData( &usebarndoors );
//
//         barnDoor = MFnSpotLight::kRight;
//         float barnanglepx = (float)spot.barnDoorAngle( barnDoor );
//         m_writer->propertyData( &barnanglepx );
//
//         barnDoor = MFnSpotLight::kLeft;
//         float barnanglenx = (float)spot.barnDoorAngle( barnDoor );
//         m_writer->propertyData( &barnanglenx );
//
//         barnDoor = MFnSpotLight::kTop;
//         float barnanglepy = (float)spot.barnDoorAngle( barnDoor );
//         m_writer->propertyData( &barnanglepy );
//
//         barnDoor = MFnSpotLight::kBottom;
//         float barnangleny = (float)spot.barnDoorAngle( barnDoor );
//         m_writer->propertyData( &barnangleny );
//
//         float shadowcolor[3];
//         spot.shadowColor().get( shadowcolor );
//         m_writer->propertyData( shadowcolor );
//
//         float usedepthmap = (float)spot.MFnNonExtendedLight::useDepthMapShadows( );
//         m_writer->propertyData( &usedepthmap );
//
//         float shadowres = (float)spot.MFnNonExtendedLight::depthMapResolution( &status );
//         m_writer->propertyData( &shadowres );
//
//         float shadowblur = (float)spot.MFnNonExtendedLight::depthMapFilterSize( &status );
//         m_writer->propertyData( &shadowblur );
//
//         float shadowbias = (float)spot.MFnNonExtendedLight::depthMapBias( &status );
//         m_writer->propertyData( &shadowbias );
//
//         float useRayTrace = (float)spot.useRayTraceShadows( );
//         m_writer->propertyData( &useRayTrace );
//
//         float shadowradius = (float)spot.MFnNonExtendedLight::shadowRadius( );
//         m_writer->propertyData( &shadowradius );
//
//         float shadowsamples = (float)spot.numShadowSamples( );
//         m_writer->propertyData( &shadowsamples );
//     }


    // Output the transform, unless this is a difference file and
    // we didn't ask for difference matrices
    if ( ! ( m_isDifferenceFile && ! m_diffMatrix ) )
    {
        TransformData( dp );
    }
}
