//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#include <Foundation.h>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;

//-*****************************************************************************
void register_coreabstracttypes()
{
    // DataType
    //
    class_<AbcA::DataType>(
            "DataType",
            "The DataType class is a description of how an element of a "
            "sample in a Scalar or an Array property is stored",
            init<>( "Create an unknown DataType with extent 0" ) )
        .def( init<AbcU::PlainOldDataType, AbcU::uint8_t>(
                  ( arg( "plaintOldDataType"), arg( "extent" ) = 1 ),
                  "Create a DataType with the given pod type and extent" ) )
        .def( "getPod",
              &AbcA::DataType::getPod,
              "Return the PlainOldDataType enum" )
        .def( "setPod",
              &AbcA::DataType::setPod,
              ( arg( "plainOldDataType" ) ),
              "Set the PlainOldDataType enum" )
        .def( "getExtent",
              &AbcA::DataType::getExtent,
              "Return the 8-bit extent" )
        .def( "setExtent",
              &AbcA::DataType::setExtent,
              arg( "extent" ),
              "Set the 8-bit extent" )
        .def( "getNumBytes",
              &AbcA::DataType::getNumBytes,
              "Return the number of bytes occupied by a single datum")
        .def( self_ns::str( self_ns::self ) )
        .def( self == self )
        .def( self < self )
        ;

    // MetaData
    //
    class_<AbcA::MetaData>(
        "MetaData",
        "The MetaData class is an unordered, unique dictionary of strings for "
        "describing Protocol for Objects and Interpretation for Properties",
        init<>( "Create a MetaData with an empty dictionary" ) )
        .def( init<AbcA::MetaData&>(
                  ( arg( "metaData" ) ),
                  "Create a MetaData as a copy of the given MetaData" ) )
        .def( "set",
              &AbcA::MetaData::set,
              ( arg( "key" ), arg( "data" ) ),
              "Set a key/data pair (silently ovewrite an existing value)" )
        .def( "setReference",
              &Alembic::Abc::SetReference,
              "Helper function to tag this metadata as being a reference. See isReference() in PropertyHeader for the counterpart." )
        .def( "setUnique",
              &AbcA::MetaData::setUnique,
              ( arg( "key" ), arg( "data" ) ),
              "Set a key/data pair (throws an exception in attempt to change "
              "the value of an existing field, Setting the same value is fine" )
        .def( "get",
              &AbcA::MetaData::get,
              ( arg( "key" ) ),
              "Return the value of the given key or an empty string if it is "
              "not set")
        .def( "getRequired",
              &AbcA::MetaData::getRequired,
              ( arg( "key" ) ),
              "Return the value of the given key and throws an exception if "
              "it is not found" )
        .def( "append",
              &AbcA::MetaData::append,
              ( arg( "metaData" ) ),
              "Append the given MetaData. Duplicates are overwritten" )
        .def( "appendUnique",
              &AbcA::MetaData::appendUnique,
              ( arg( "metaData" ) ),
              "Append the given MetaData. Duplicate values will cause an "
              "exception to be thrown" )
        .def( "size",
              &AbcA::MetaData::size,
              "Return the size of the dictionary" )
        .def( "matches",
              &AbcA::MetaData::matches,
              ( arg( "metaData" ) ),
              "Return True if each of the fields in the given MetaData are "
              "found in this MetaData and have the same values" )
        .def( "matchesOverlap",
              &AbcA::MetaData::matchesOverlap,
              ( arg( "metaData" ) ),
              "Return True if, for each of the fields in the given MetaData "
              ", this MetaData has either no entry, or the same entry" )
        .def( "matchesExactly",
              &AbcA::MetaData::matchesExactly,
              ( arg( "metaData" ) ),
              "Return True if the given MetaData is exactly equal to this "
              "MetaData in every field" )
        .def( "serialize",
              &AbcA::MetaData::serialize,
              "Convert the contents of this MetaData into a single formatted "
              "string" )
        .def( "setIsUV",
              &AbcG::SetIsUV,
              "Set whether or not this metadata stores UVs." )
        .def( "setSourceName",
              &Alembic::Abc::SetSourceName,
              "Set a source name, for later retrieval via getSourceName()" )
        .def( "getSourceName",
              &Alembic::Abc::GetSourceName,
              "Get the stored source name from the metadata, if any." )
        .def( "__str__", &AbcA::MetaData::serialize )
        ;

    // ObjectHeader
    //
    AbcA::MetaData & ( AbcA::ObjectHeader::*getNonConstMetaData )() = \
        &AbcA::ObjectHeader::getMetaData;

    class_<AbcA::ObjectHeader >(
        "ObjectHeader",
        "The ObjectHeader is a collection of MetaData which helps define an "
        "Object",
        no_init )
        .def( "getName",
              &AbcA::ObjectHeader::getName,
              "Return the name of the object, which is unique amongst its "
              "siblings",
              return_value_policy<copy_const_reference>() )
        .def( "getFullName",
              &AbcA::ObjectHeader::getFullName,
              "Return the full name of the object, which is unique in the "
              "whole file",
              return_value_policy<copy_const_reference>() )
        .def( "getMetaData",
              getNonConstMetaData,
              "Return the MetaData of the object",
              return_internal_reference<>() )
        .def( "__str__", &AbcA::ObjectHeader::getFullName,
              return_value_policy<copy_const_reference>() )
        ;

    // PropertyHeader
    //
    class_<AbcA::PropertyHeader>(
        "PropertyHeader",
        "The PropertyHeader is a collection of MetaData which helps define a "
        "Property. It also acts as a key of getting an instance of a Property "
        "from a CompoundProperty",
        no_init )
        .def( "getName",
              &AbcA::PropertyHeader::getName,
              "Return the name of the property, which is unique amongst its "
              " siblings",
              return_value_policy<copy_const_reference>() )
        .def( "isScalar",
              &AbcA::PropertyHeader::isScalar,
              "Return True if the property is scalar" )
        .def( "isArray",
              &AbcA::PropertyHeader::isArray,
              "Return True if the property is array" )
        .def( "isCompound",
              &AbcA::PropertyHeader::isCompound,
              "Return True if the property is compound" )
        .def( "isSimple",
              &AbcA::PropertyHeader::isSimple,
              "Return True if the property is simple (non-compound)" )
        .def( "getMetaData",
              &AbcA::PropertyHeader::getMetaData,
              "Return the MetaData of the property",
              return_internal_reference<>() )
        .def( "getDataType",
              &AbcA::PropertyHeader::getDataType,
              "Return the DataType of the property. An exception will be "
              "thrown if this is called for a CompoundProperty",
              return_value_policy<copy_const_reference>() )
        .def( "isReference",
              &Alembic::Abc::isReference,
              "Return true if the property is tagged as being a reference" )
        .def( "isUV",
              &Alembic::AbcGeom::isUV,
              "Return true if the property is tagged as being UVs" )
        .def( "__str__", &AbcA::PropertyHeader::getName,
              return_value_policy<copy_const_reference>() )
        ;

    // TimeSamplingType
    //
    class_<AbcA::TimeSamplingType>(
        "TimeSamplingType",
        "The TimeSamplingType class controls how properties in Alembic relate "
        "time values to their samplig indices",
        init<>( "Create a uniform TimeSamplingType with the default time per "
                "cycle value of 1.0" ) )
        .def( init<AbcA::chrono_t>(
                   ( arg( "timePerCycle" ) ),
                  "Create a uniform TimeSamplingType with the given time per "
                  "cycle" ) )
        .def( init<AbcU::uint32_t, AbcA::chrono_t>(
                  ( arg( "numSamplePerCycle" ), arg( "timePerCycle" ) ),
                  "Create a cyclic TimeSamplingType with the given number of "
                  "samples per cycle and time per cycle" ) )
        .def( init<AbcA::TimeSamplingType::AcyclicFlag>(
                  ( arg( "acyclicFlag" ) ),
                  "Create an acyclic TimeSamplingType with the give acyclic "
                  "flag" ) )
        .def( "isUniform",
              &AbcA::TimeSamplingType::isUniform,
              "Return True if the sampling type is uniform (1 samples per "
              "cycle)" )
        .def( "isCyclic",
              &AbcA::TimeSamplingType::isCyclic,
              "Return True if the sampling type is cyclic (more than 1 samples "
              "per cycle)" )
        .def( "isAcyclic",
              &AbcA::TimeSamplingType::isAcyclic,
              "Return True of the sampling type is acyclic (infinit samples "
              "per cycle)" )
        .def( "getNumSamplesPerCycle",
              &AbcA::TimeSamplingType::getNumSamplesPerCycle,
              "Return the number of samples per cycle" )
        .def( "getTimePerCycle",
              &AbcA::TimeSamplingType::getTimePerCycle,
              "Return the time per cycle" )
        .def( "AcyclicNumSamples",
              &AbcA::TimeSamplingType::AcyclicNumSamples,
              "Return the predefined number of samples per cycle reserved for "
              "acyclic sampling type" )
        .staticmethod( "AcyclicNumSamples" )
        .def( "AcyclicTimePerCycle",
              &AbcA::TimeSamplingType::AcyclicTimePerCycle,
              "Return the predefined time per cycle reserved for acyclic "
              "sampling type" )
        .staticmethod( "AcyclicTimePerCycle" )
        .def( self_ns::str( self_ns::self ) )
        .def( self == self )
        ;

    // TimeSampling
    //
    struct TimeSamplingOverloads
    {
        static AbcA::index_t getFloorIndex( AbcA::TimeSampling& tSamp,
                                            AbcA::chrono_t iTime,
                                            AbcA::index_t iNumSamples )
        {
            return tSamp.getFloorIndex( iTime, iNumSamples ).first;
        }

        static AbcA::index_t getCeilIndex( AbcA::TimeSampling& tSamp,
                                           AbcA::chrono_t iTime,
                                           AbcA::index_t iNumSamples )
        {
            return tSamp.getCeilIndex( iTime, iNumSamples ).first;
        }

        static AbcA::index_t getNearIndex( AbcA::TimeSampling& tSamp,
                                           AbcA::chrono_t iTime,
                                           AbcA::index_t iNumSamples )
        {
            return tSamp.getNearIndex( iTime, iNumSamples ).first;
        }

        static std::vector<AbcA::chrono_t> getStoredTimes( AbcA::TimeSampling& tSamp )
        {
            return tSamp.getStoredTimes();
        }

    };

    class_<AbcA::TimeSampling, AbcA::TimeSamplingPtr>(
           "TimeSampling",
           "The TimeSampling class reports information about the time values "
           " that are associated with the samples written to a Property",
           init<>() )
        .def( init<AbcA::chrono_t, AbcA::chrono_t>(
                  ( arg( "timePerCycle" ), arg( "startTime" ) ),
                  "Create a uniform time sampling with the give time per cycle "
                  "and the given start time") )
        .def( init<AbcA::TimeSamplingType, std::vector<AbcA::chrono_t> >(
                  ( arg( "timeSamplingType" ), arg( "sampleTimes" ) ),
                  "Create a time sampling with the given TimeSamplingType and "
                  "the time samples per cycle" ) )
        .def( "getNumStoredTimes",
              &AbcA::TimeSampling::getNumStoredTimes,
              "Return the number of stored times samples" )
        .def( "getStoredTimes",
              TimeSamplingOverloads::getStoredTimes,
              "Return the stored times" )
        .def( "getTimeSamplingType",
              &AbcA::TimeSampling::getTimeSamplingType,
              "Return the TimeSamplingType of this class" )
        .def( "getSampleTime",
              &AbcA::TimeSampling::getSampleTime,
              ( arg( "index" ) ),
              "Return the time of the given sample index" )
        .def( "getFloorIndex",
              TimeSamplingOverloads::getFloorIndex,
              ( arg( "time" ), arg( "numSamples" ) ),
              "Find the largest valid index that as a time less than or equal "
              "to the given time" )
        .def( "getCeilIndex",
              TimeSamplingOverloads::getCeilIndex,
              ( arg( "time" ), arg( "numSamples" ) ),
              "Find the smallest valid index that has a time greater than or "
              "equal to the given time" )
        .def( "getNearIndex",
              TimeSamplingOverloads::getNearIndex,
              ( arg( "time" ), arg( "numSamples" ) ),
              "Find the valid index with the closest time to the given time" )
        .def( self == self )
        ;

    // TimeSampling time vector
    //
    class_<std::vector<AbcA::chrono_t> >(
        "TimeVector",
        "TimeVector class holds a list of the sampled times" )
        .def( vector_indexing_suite<std::vector<AbcA::chrono_t> >() );

    // TimeSamplingType Enum
    enum_<AbcA::TimeSamplingType::AcyclicFlag>( "AcyclicFlag" )
        .value( "kAcyclic", AbcA::TimeSamplingType::kAcyclic );
}
