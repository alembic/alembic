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


// *****************************************************************************
void AbcExporter::CurveHeader( MDagPath &dp )
{
    MFnNurbsCurve curve( dp.node() );

    int npoints = curve.numCVs();

    string protocol = "NURBSCurve";
    int protocolVersion = 1;
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
        MDoubleArray rawVknots;
        curve.getKnots( rawVknots );

        int vKnots = rawVknots.length() + 2; //spansU + 2 * degreeU - 1;

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
            m_writer->property( ABC_PROPERTY_DEGREE, Abc::Int, 1, 1 );
            m_writer->property( ABC_PROPERTY_VKNOTS, Abc::Float, vKnots, 1 );
            m_writer->property( ABC_PROPERTY_VRANGE, Abc::Float, 2, 1 );
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


// *****************************************************************************
void AbcExporter::CurveData( MDagPath &dp )
{
    // Only bother with all this if we're not writing a difference file
    // that should ONLY have matrices in it...
    if ( ! ( m_isDifferenceFile
            && m_diffMatrix
            && ! m_diffPoints ) )
    {
        MObject object = dp.node();
        MFnNurbsCurve curve( dp.node() );

        int npoints = curve.numCVs();

        if ( npoints < 2 )
        {
            if ( ! m_quiet )
            {
                MString error( "Ill-defined curve object: " );
                error += curve  .name();
                MGlobal::displayError( error );
            }
            return;
        }

        vector<float> positions;
        vector<float> weights;
        MItCurveCV rawCVs( object );

        for (;!rawCVs.isDone(); rawCVs.next())
        {
            MPoint pt = rawCVs.position(MSpace::kObject);
            positions.push_back(float(pt.x));
            positions.push_back(float(pt.y));
            positions.push_back(float(pt.z));
            weights.push_back(float(pt.w));
        }

        // output points.positions here
        m_writer->propertyDataInContainer(positions);

        // Only output points.weights property data and
        // surface component data if this is NOT a difference file
        if ( ! m_isDifferenceFile )
        {
            // output points.weights here
            m_writer->propertyDataInContainer(weights);

            //----------------------------------------------------------------------

            int vDegree = curve.degree();
            m_writer->propertyData(&vDegree);

            MDoubleArray rawVknots;
            curve.getKnots(rawVknots);

            // TODO: Don't hard-code degree 3
            vector<float> vknots(rawVknots.length()+2);

            //
            // Double up end knots.
            //
            for (size_t i=0; i < rawVknots.length(); i++)
            {
                vknots[i+1] = float(rawVknots[i]);
            }
            vknots[0] = vknots[1];
            vknots.back() = vknots[vknots.size()-2];

            //

            float vRange[2];

            double vMin_d, vMax_d;
            curve.getKnotDomain(vMin_d, vMax_d);
            float vmin = float(vMin_d);
            float vmax = float(vMax_d);
            vRange[0] = vmin;
            vRange[1] = vmax;

            // Normalize knots, if requested...
            if ( m_normalize )
            {
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
            m_writer->propertyDataInContainer(vknots);

            // Write range
            m_writer->propertyData(vRange);

            // Write form
            MFnNurbsCurve::Form formV = curve.form();
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

