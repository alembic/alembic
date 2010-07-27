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

#ifndef _AlembicHDF5_AlembicHDF5_h_
#define _AlembicHDF5_AlembicHDF5_h_

#include <AlembicHDF5/Foundation.h>
#include <AlembicHDF5/Exception.h>
#include <AlembicHDF5/Assert.h>
#include <AlembicHDF5/BaseObject.h>
#include <AlembicHDF5/PropertyList.h>
#include <AlembicHDF5/Datatype.h>
#include <AlembicHDF5/Dataspace.h>
#include <AlembicHDF5/AttributedObject.h>
#include <AlembicHDF5/ParentObject.h>
#include <AlembicHDF5/Group.h>
#include <AlembicHDF5/File.h>
#include <AlembicHDF5/BufferedObject.h>
#include <AlembicHDF5/Attribute.h>
#include <AlembicHDF5/Dataset.h>

//-*****************************************************************************
// Object Inheritance
// 
// BaseObject
// --NamedObject
//   --Attribute (secondarily derived from BufferedObject)
//   --AttributedObject
//     --ParentObject (may have children objects)
//       --File
//       --Group
//     --Dataset (secondarily derived from BufferedObject)
// --Datatype (though there are named Datatypes in HDF5, we don't use them)
// --Dataspace
// --PropertyList

#endif
