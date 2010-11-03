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

#ifndef _AlembicAbcExport_SimpleNull_h_
#define _AlembicAbcExport_SimpleNull_h_

#include "Foundation.h"
#include "Exportable.h"

namespace AlembicAbcExport {

//-*****************************************************************************
// A SimpleNull is an object that has no particular attributes, but because
// it may have children, it has a bounding box.
template <class ABC_OBJECT>
class BaseSimpleNull : public Exportable
{
public:
    typedef ABC_OBJECT alembic_object_type;
    typedef BaseSimpleNull<ABC_OBJECT> this_type;

protected:
    BaseSimpleNull( Exportable &parent,
                    MDagPath &dpath,
                    MObject &nde,
                    const std::string &nme,
                    const Abc::TimeSamplingInfo &tinfo );
public:

    virtual bool valid() const;
    virtual void internalClose()
    {
        m_alembicObject.close( Abc::kThrowException );
        m_alembicObject.release();
        m_boundsTrait.release();
    }

protected:
    virtual Abc::Box3d internalWriteSample( const Abc::Time &sampTime,
                                            const Abc::Box3d &childBounds );

private:
    virtual const Abc::OObject &alembicObject();

protected:
    ABC_OBJECT m_alembicObject;
    Atg::OBoundsLocalTrait m_boundsTrait;
};

//-*****************************************************************************
// typedef BaseSimpleNull<Atg::OSimpleNull> SimpleNull;

//-*****************************************************************************
//-*****************************************************************************
template <class ABC_OBJECT>
inline BaseSimpleNull<ABC_OBJECT>::BaseSimpleNull
(
    Exportable &parent,
    MDagPath &dpath,
    MObject &nde,
    const std::string &nme,
    const Abc::TimeSamplingInfo &tinfo
)
  : Exportable( dpath, nde, nme ),
    m_alembicObject( parent.alembicParentObject(), nme,
                     Abc::kThrowException )
{
    if ( m_alembicObject )
    {
        m_boundsTrait = m_alembicObject->bounds();
    }
    
    if ( tinfo )
    {
        m_boundsTrait.makeAnimated( tinfo );
    }
}

//-*****************************************************************************
template <class ABC_OBJECT>
bool BaseSimpleNull<ABC_OBJECT>::valid() const
{
    return ( bool )m_alembicObject;
}

//-*****************************************************************************
template <class ABC_OBJECT>
Abc::Box3d BaseSimpleNull<ABC_OBJECT>::internalWriteSample
(
    const Abc::Time &sampTime,
    const Abc::Box3d &childBounds
)
{
    if ( m_boundsTrait )
    {
        if ( sampTime.isRestTime() )
        {
            // std::cout << "Writing BaseSimpleNull rest pose"
            //          << std::endl;
            m_boundsTrait.set( childBounds );
        }
        else
        {
            // std::cout << "Writing BaseSimpleNull anim pose at time: "
            //           << sampTime << std::endl;
            if ( m_boundsTrait.isAnimated() )
            {
                m_boundsTrait.setAnim( sampTime, childBounds );
            }
        }
    }
    
    return childBounds;
}

//-*****************************************************************************
template <class ABC_OBJECT>
const Abc::OObject &BaseSimpleNull<ABC_OBJECT>::alembicObject()
{
    return m_alembicObject;
}

} // End namespace AlembicAbcExport

#endif
