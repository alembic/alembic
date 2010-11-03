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
void AbcExporter::NURBSHeader( MDagPath &dp )
{
    MFnNurbsSurface nurbs( dp.node() );

    int nu = nurbs.numCVsInU();
    int nv = nurbs.numCVsInV();

    int npoints = nu * nv;

    string protocol = ABC_PROTOCOL_NURBS;
    int protocolVersion = ABC_NURBS_VERSION;
    if ( hasUserProtocol( dp ) )
    {
        protocol = userProtocol( dp );
    }
    if ( hasUserProtocolVersion( dp ) )
    {
        protocolVersion = userProtocolVersion( dp );
    }

    m_writer->beginObject( m_objectName.asChar(),
                           protocol.c_str(), protocolVersion );

    // Only bother with all this if we're not writing a difference file
    // that should ONLY have matrices in it...
    if ( ! ( m_isDifferenceFile
            && m_diffMatrix
            && ! m_diffPoints ) )
    {
        MDoubleArray rawUknots, rawVknots;
        nurbs.getKnotsInU( rawUknots );
        nurbs.getKnotsInV( rawVknots );
        int uKnots = rawUknots.length() + 2; //spansU + 2 * degreeU - 1;
        int vKnots = rawVknots.length() + 2; //spansV + 2 * degreeV - 1;

        m_writer->beginComponent( ABC_COMPONENT_POINTS );
        m_writer->property( ABC_PROPERTY_POSITION, Abc::Float, npoints, 3 );

        // Only output points.weights property if this is NOT a difference file
        if ( ! m_isDifferenceFile )
        {
            m_writer->property( ABC_PROPERTY_WEIGHT,  Abc::Float, npoints, 1 );
        }

        m_writer->endComponent();

        // Only output surface component if this is NOT a difference file
        if ( ! m_isDifferenceFile )
        {
            m_writer->beginComponent( ABC_COMPONENT_SURFACE );
            m_writer->property( ABC_PROPERTY_DEGREE, Abc::Int, 2, 1 );
            m_writer->property( ABC_PROPERTY_UKNOTS, Abc::Float, uKnots, 1 );
            m_writer->property( ABC_PROPERTY_VKNOTS, Abc::Float, vKnots, 1 );
            m_writer->property( ABC_PROPERTY_URANGE, Abc::Float, 2, 1 );
            m_writer->property( ABC_PROPERTY_VRANGE, Abc::Float, 2, 1 );
            m_writer->property( ABC_PROPERTY_UFORM, Abc::Int, 1, 1 );
            m_writer->property( ABC_PROPERTY_VFORM, Abc::Int, 1, 1 );
            m_writer->endComponent();
        }
    }

    // Output the transform header, unless this is a difference file and
    // we didn't ask for difference matrices
    if ( ! ( m_isDifferenceFile && ! m_diffMatrix ) )
    {
        TransformHeader( dp );
    }

    m_writer->endObject();
}

//******************************************************************************
void AbcExporter::NURBSData( MDagPath &dp )
{
    // Only bother with all this if we're not writing a difference file
    // that should ONLY have matrices in it...
    if ( ! ( m_isDifferenceFile
            && m_diffMatrix
            && ! m_diffPoints ) )
    {
        MObject object = dp.node();
        MFnNurbsSurface nurbs( object );
        nurbs.updateSurface();

        int nu = nurbs.numCVsInU();
        int nv = nurbs.numCVsInV();

        if ( nu < 2 || nv < 2 )
        {
            if ( ! m_quiet )
            {
                MString error( "Ill-defined nurbs object: " );
                error += nurbs.name();
                MGlobal::displayError( error );
            }
            return;
        }

        vector<float> positions;
        vector<float> weights;
        MItSurfaceCV rawCVs(object, true); // v-major order for abc

        for (;!rawCVs.isDone(); rawCVs.nextRow())
        {
            for (;!rawCVs.isRowDone();rawCVs.next())
            {
                MPoint pt = rawCVs.position(MSpace::kObject);
                positions.push_back(float(pt.x));
                positions.push_back(float(pt.y));
                positions.push_back(float(pt.z));
                weights.push_back(float(pt.w));
            }
        }

        // output points.positions here
        m_writer->propertyDataInContainer( positions );

        // Only output points.weights property data and
        // surface component data if this is NOT a difference file
        if ( ! m_isDifferenceFile )
        {
            // output points.weights here
            m_writer->propertyDataInContainer(weights);

            //----------------------------------------------------------------------

            int uorder = nurbs.degreeU();
            int vorder = nurbs.degreeV();
            int degrees[2] = { uorder, vorder };

            m_writer->propertyData(degrees);

            MDoubleArray rawUknots, rawVknots;
            nurbs.getKnotsInU(rawUknots);
            nurbs.getKnotsInV(rawVknots);

            vector<float> uknots(rawUknots.length()+2); // TODO: Don't hard-code degree 3
            vector<float> vknots(rawVknots.length()+2);

            //
            // Double up end knots.
            //

            for (size_t i=0; i < rawUknots.length(); i++)
            {
                uknots[i+1] = float(rawUknots[i]);
            }

            uknots[0] = uknots[1];
            uknots.back() = uknots[uknots.size()-2];

            // same for v

            for (size_t i=0; i < rawVknots.length(); i++)
            {
                vknots[i+1] = float(rawVknots[i]);
            }

            vknots[0] = vknots[1];
            vknots.back() = vknots[vknots.size()-2];

            //

            float uRange[2];
            float vRange[2];

            double uMin_d, uMax_d, vMin_d, vMax_d;
            nurbs.getKnotDomain(uMin_d, uMax_d, vMin_d, vMax_d);
            float umin = float(uMin_d);
            float umax = float(uMax_d);
            float vmin = float(vMin_d);
            float vmax = float(vMax_d);

            uRange[0] = umin;
            uRange[1] = umax;
            vRange[0] = vmin;
            vRange[1] = vmax;

            // Normalize knots, if requested...
            if ( m_normalize )
            {
                if ( umax != umin )
                {
                    for ( std::vector<float>::iterator iter = uknots.begin();
                          iter != uknots.end(); ++iter )
                    {
                        (*iter) = ( (*iter) - umin ) / ( umax - umin );
                    }

                    uRange[0] = 0.0f;
                    uRange[1] = 1.0f;
                }

                if ( vmax != vmin )
                {
                    for ( std::vector<float>::iterator iter = vknots.begin();
                          iter != vknots.end(); ++iter )
                    {
                        (*iter) = ( (*iter) - vmin ) / ( vmax - vmin );
                    }

                    vRange[0] = 0.0f;
                    vRange[1] = 1.0f;
                }
            }

            // Write knots
            m_writer->propertyDataInContainer(uknots);
            m_writer->propertyDataInContainer(vknots);

            // Write range
            m_writer->propertyData(uRange);
            m_writer->propertyData(vRange);

            // Write form
            MFnNurbsSurface::Form formU = nurbs.formInU();
            MFnNurbsSurface::Form formV = nurbs.formInV();
            m_writer->propertyData( &formU );
            m_writer->propertyData( &formV );

        }  //  End if ( ! m_isDifference...
    }

    // Output the transform header, unless this is a difference file and
    // we didn't ask for difference matrices
    if ( ! ( m_isDifferenceFile && ! m_diffMatrix ) )
    {
        TransformData( dp );
    }
}
