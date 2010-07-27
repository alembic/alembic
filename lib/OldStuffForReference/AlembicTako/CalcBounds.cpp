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

#include <AlembicTako/CalcBounds.h>
#include <AlembicTako/Transform.h>
#include <AlembicTako/PolyMesh.h>
#include <AlembicTako/PointPrimitive.h>
#include <ImathBoxAlgo.h>
#include <ImathMatrixAlgo.h>

using namespace AlembicAsset;

namespace AlembicTako {

//-*****************************************************************************
// Bounds info.
struct BoundsInfo
{
    BoundsInfo()
      : xform( false ),
        inherit( true )
    {
        bounds.makeEmpty();
    }
    Box3d bounds;
    bool xform;
    bool inherit;
};

//-*****************************************************************************
BoundsInfo VisitObject( const IObject &obj,
                        const Time &sampTime )
{
    // Our return object.
    BoundsInfo ret;
    
    // Calculate the bounds of just this object, not including children,
    // and not transformed.
    Box3d selfBounds;
    selfBounds.makeEmpty();

    // Check to see if there's a poly mesh points trait. If so, use it.
    IPolyMeshPointsTrait pointsTrait( obj );
    if ( pointsTrait )
    {
        IV3fSample points = pointsTrait.getAnimLowerBound( sampTime );
        size_t numPoints = points.size();
        for ( size_t i = 0; i < numPoints; ++i )
        {
            selfBounds.extendBy( points[i] );
        }
    }

    // Check to see if there's a particle positions trait. If so, use it.
    IPointPrimitivePositionTrait positionTrait( obj );
    if ( positionTrait )
    {
        // Get points.
        IV3fSample points = positionTrait.getAnimLowerBound( sampTime );
        size_t numPoints = points.size();
        
        // Check to see if there is a width property.
        IPointPrimitiveWidthTrait widthTrait( obj );
        IFloatSample width = widthTrait.getAnimLowerBound( sampTime );
        if ( width.size() == numPoints )
        {
            for ( size_t i = 0; i < numPoints; ++i )
            {
                const V3f &p = points[i];
                V3f w( fabsf( width[i] ) );
                selfBounds.extendBy( Box3d( p - w,
                                            p + w ) );
            }
        }
        else
        {
            // Check for constant width, just in case.
            float cwidth = 0.0f;
            IPointPrimitiveConstantWidthTrait cwidthTrait( obj );
            if ( cwidthTrait )
            {
                cwidth = cwidthTrait.getAnimLowerBound( sampTime );
            }
            V3f cw( fabsf( cwidth ) );
            for ( size_t i = 0; i < numPoints; ++i )
            {
                const V3f &p = points[i];
                selfBounds.extendBy( Box3d( p - cw,
                                            p + cw ) );
            }
        }
    }

    // Get the transform, if it exists.
    M44f xform;
    xform.makeIdentity();
    ITransformStackTrait stackTrait( obj );
    if ( stackTrait )
    {
        ret.xform = true;
        
        IM44fSample stack =
            stackTrait.getAnimLowerBound( sampTime );
    
        // If anything went wrong, or if the stack
        // simply had no data, we'll stick with xform being identity.
        size_t numStackItems = stack.size();
        if ( numStackItems > 0 )
        {
            ITransformInheritTrait inhTrait( obj );
            if ( inhTrait && inhTrait.get() )
            {
                ret.inherit = true;
                
                // Inherit the whole stack.
                for ( size_t i = 0; i < numStackItems; ++i )
                {
                    xform *= stack[i];
                }
            }
            else
            {
                ret.inherit = false;
                
                // Just take the back of the stack.
                xform = stack[numStackItems-1];
            }
        }
    }

    // Cool! Transform selfBounds.
    if ( !selfBounds.isEmpty() )
    {
        selfBounds = Imath::transform( selfBounds, xform );
    }

    // In Tako, children are not affected by this transform...
    // or are they? Not sure - passing on an 'inherit' flag
    // and an 'xform' flag to inform this decision
    size_t numChildren = obj.numChildren();
    for ( size_t c = 0; c < numChildren; ++c )
    {
        IObject child( obj, c );
        BoundsInfo childBoundsInfo = VisitObject( child, sampTime );
        if ( childBoundsInfo.inherit )
        {
            // CJH: Also check childBoundsInfo.xform?
            if ( !childBoundsInfo.bounds.isEmpty() )
            {
                childBoundsInfo.bounds =
                    Imath::transform( childBoundsInfo.bounds,
                                      xform );
            }
        }

        selfBounds.extendBy( childBoundsInfo.bounds );
    }
    
    ret.bounds = selfBounds;
    return ret;
}

//-*****************************************************************************
//-*****************************************************************************
// Main Functions
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
Box3d CalcBounds( const IObject &obj, const Time &sampTime )
{
    return VisitObject( obj, sampTime ).bounds;
}

//-*****************************************************************************
Box3d CalcBounds( const IAsset &ast, const Time &sampTime )
{
    Box3d bounds;
    bounds.makeEmpty();
    size_t numChildren = ast.numChildren();
    for ( size_t c = 0; c < numChildren; ++c )
    {
        IObject child( ast, c );
        bounds.extendBy( CalcBounds( child, sampTime ) );
    }
    return bounds;
}

//-*****************************************************************************
Box3d CalcBounds( const std::string &fileName, const Time &sampTime )
{
    IAsset ast( fileName );
    return CalcBounds( ast, sampTime );
}

} // End namespace AlembicTako
