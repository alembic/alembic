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

#include "Xform.h"
#include "Request.h"
#include "RiUtil.h"
#include "AbcUtil.h"

namespace AlembicRiPlugin {

//-*****************************************************************************
//-*****************************************************************************
// Instantiate Attribute State!
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
void XformInstantiateAttributeState( Request &rq,
                                     const Atg::IXformLocalTrait &xform )
{
    if ( rq.restPose() )
    {
        Abc::M44d local;
        // CJH: Technically this mutex lock is unnecessary.
        {
            MutexLock mlock( rq.mutex() );
            local = xform.get();
        }
        ConcatTransform( local );
    }
    else
    {
        const TimeSamples &tsamps = rq.timeSamples();
        const float motionSampleBias = rq.motionSampleBias();
        const float motionSampleGain = rq.motionSampleGain();

        if ( tsamps.size() > 1 )
        {
            MotionBegin( tsamps, motionSampleBias, motionSampleGain );
        }

        // No interpolation for now. Just lower bounds.
        for ( TimeSamples::const_iterator iter = tsamps.begin();
              iter != tsamps.end(); ++iter )
        {
            Abc::M44d local;
            // CJH: Technically, this mutex lock is unnecessary.
            {
                MutexLock mlock( rq.mutex() );
                local = xform.getAnimLowerBound( (*iter) );
            }
            ConcatTransform( local );
        }

        if ( tsamps.size() > 1 )
        {
            RiMotionEnd();
        }
    }
}

//-*****************************************************************************
//-*****************************************************************************
// XFORM
//-*****************************************************************************
//-*****************************************************************************
Xform::Xform( Factory &factory,
              ParentObject &parent,
              const std::string &objName )
  : ParentObject(),
    m_xform( parent.alembicParentObject(), objName )
{
    ParentObject::init( factory, m_xform );
}

//-*****************************************************************************
const Abc::IParentObject &Xform::alembicParentObject() const throw()
{
    return ( const Abc::IParentObject & )m_xform;
}

//-*****************************************************************************
void Xform::instantiateAttributeState( Request &rq )
{
    XformInstantiateAttributeState( rq, m_xform->xform() );
}

} // End namespace AlembicRiPlugin

