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

#ifndef _SOP_ALEMBICIN_H_
#define _SOP_ALEMBICIN_H_

#include <SOP/SOP_Node.h>
#include <GB/GB_AttributeRef.h>

#include <Alembic/AbcGeom/All.h>
using namespace Alembic::AbcGeom;


class SOP_AlembicIn : public SOP_Node
{
public:
    //--------------------------------------------------------------------------
    // Standard hdk declarations
    static OP_Node *myConstructor(OP_Network *net, const char *name,
            OP_Operator *entry);
    static PRM_Template myTemplateList[];
    
protected:
    //--------------------------------------------------------------------------
    // Standard hdk declarations
    SOP_AlembicIn(OP_Network *net, const char *name, OP_Operator *op);
    virtual ~SOP_AlembicIn();
    virtual bool unloadData();
    virtual OP_ERROR cookMySop(OP_Context &context);
    virtual void nodeUnlocked();
    
private:
    
    GB_AttributeRef attachDetailStringData(const std::string &attrName,
            const std::string &value);
    
    
    struct Args
    {
        double abcTime;
        bool includeXform;
    };
    
    typedef std::vector<std::string> PathList;
    void walkObject( Args & args, IObject parent, const ObjectHeader &ohead,
            PathList::const_iterator I, PathList::const_iterator E,
                    M44d parentXform);
    
    
    
    std::string getFullName( IObject object );
    
    
    
    void buildSubD( Args & args, ISubD & subd, M44d parentXform);
    void buildPolyMesh( Args & args, IPolyMesh & polymesh, M44d parentXform);
    
    GB_PrimitiveGroup * buildMesh(const std::string & groupName,
            V3fArraySamplePtr positions, Int32ArraySamplePtr counts,
                    Int32ArraySamplePtr indicies);
    
    bool addOrFindTextureAttribute(GEO_AttributeOwner owner,
            GB_AttributeRef & attrIdx);
    bool addOrFindNormalAttribute(GEO_AttributeOwner owner,
            GB_AttributeRef & attrIdx);
    
    void addUVs(Args & args, IV2fGeomParam param,
             size_t startPointIdx, size_t startPrimIdx);
    
    void addNormals(Args & args, IN3fGeomParam param,
             size_t startPointIdx, size_t startPrimIdx);
    
    void addArbitraryGeomParams(Args & args,
            ICompoundProperty parent,
            size_t startPointIdx,
            size_t startPrimIdx);
    
    template <typename geomParamT, typename podT>
    void processArbitraryGeomParam(
            Args & args,
            ICompoundProperty parent,
            const PropertyHeader & propHeader,
            GB_AttribType attrType,
            const GB_AttributeRef & existingAttr,
            size_t startPointIdx,
            size_t startPrimIdx
            );
    
    template <typename geomParamSampleT, typename podT>
    void applyArbitraryGeomParamSample(
            geomParamSampleT & paramSample,
            const GB_AttributeRef & attrIdx,
            size_t totalExtent,
            size_t startPointIdx,
            size_t startPrimIdx
            );
};








#endif
