//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#ifndef ABC_STITCHER_UTIL_H
#define ABC_STITCHER_UTIL_H

#include <string>
#include <vector>
#include <Alembic/Abc/ICompoundProperty.h>
#include <Alembic/Abc/OCompoundProperty.h>

typedef std::vector< Alembic::Abc::ICompoundProperty > ICompoundPropertyVec;

class TimeAndSamplesMap
{
public:
    TimeAndSamplesMap() {m_isVerbose = false;};

    void add(Alembic::AbcCoreAbstract::TimeSamplingPtr iTime,
             std::size_t iNumSamples);

    Alembic::AbcCoreAbstract::TimeSamplingPtr get(
        Alembic::AbcCoreAbstract::TimeSamplingPtr iTime,
        std::size_t & oNumSamples) const;

    void setVerbose(bool isVerbose){m_isVerbose = isVerbose;};
    bool isVerbose() const {return m_isVerbose;};

private:
    std::vector< Alembic::AbcCoreAbstract::TimeSamplingPtr > mTimeSampling;
    std::vector< std::size_t > mExpectedSamples;
    bool m_isVerbose;
};

Alembic::AbcCoreAbstract::index_t
getIndexSample(Alembic::AbcCoreAbstract::index_t iCurOutIndex,
    Alembic::AbcCoreAbstract::TimeSamplingPtr iOutTime,
    Alembic::AbcCoreAbstract::index_t iInNumSamples,
    Alembic::AbcCoreAbstract::TimeSamplingPtr iInTime,
    Alembic::AbcCoreAbstract::index_t & oNumEmpty);

void checkAcyclic(const Alembic::AbcCoreAbstract::TimeSamplingType & tsType,
                  const std::string & fullNodeName);

void stitchArrayProp(const Alembic::AbcCoreAbstract::PropertyHeader & propHeader,
                     const ICompoundPropertyVec & iCompoundProps,
                     Alembic::Abc::OCompoundProperty & oCompoundProp,
                     const TimeAndSamplesMap & iTimeMap);

void stitchScalarProp(const Alembic::AbcCoreAbstract::PropertyHeader & propHeader,
                      const ICompoundPropertyVec & iCompoundProps,
                      Alembic::Abc::OCompoundProperty & oCompoundProp,
                      const TimeAndSamplesMap & iTimeMap);

void stitchCompoundProp(ICompoundPropertyVec & iCompoundProps,
                        Alembic::Abc::OCompoundProperty & oCompoundProp,
                        const TimeAndSamplesMap & iTimeMap);


#endif // ABC_STITCHER_UTIL_H
