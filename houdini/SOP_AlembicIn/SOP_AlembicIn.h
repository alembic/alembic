//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#ifndef SOP_ALEMBICIN_H
#define SOP_ALEMBICIN_H

#include <UT/UT_Version.h>
#include <UT/UT_Interrupt.h>
#include <SOP/SOP_Node.h>

#include <map>

#include <Alembic/AbcGeom/All.h>
using namespace Alembic::AbcGeom;

    #include <GA/GA_AttributeRef.h>

/// The sop_IAlembicWalker needs to be in a public namespace for forward
/// declarations.
class sop_IAlembicWalker;

/// SOP to read Alembic geometry
class SOP_AlembicIn : public SOP_Node
{
public:
    //--------------------------------------------------------------------------
    // Standard hdk declarations
    static OP_Node *myConstructor(OP_Network *net, const char *name,
            OP_Operator *entry);
    static PRM_Template myTemplateList[];
    typedef std::vector<std::string> PathList;

    /// Clear a file out of the read-cache.  If the filename is null, the
    /// entire cache is cleared.
    static void	clearCacheFile(const std::vector<std::string> *filenames=NULL);
    void appendFileNames(std::vector<std::string> &filenames, fpreal t);

protected:
    //--------------------------------------------------------------------------
    // Standard hdk declarations
    SOP_AlembicIn(OP_Network *net, const char *name, OP_Operator *op);
    virtual ~SOP_AlembicIn();
    virtual bool updateParmsFlags();
    virtual bool unloadData();
    virtual OP_ERROR cookMySop(OP_Context &context);
    virtual void nodeUnlocked();

private:

    GA_ROAttributeRef attachDetailStringData(const std::string &attrName,
            const std::string &value);

    struct Args
    {
        double abcTime;
        GA_Offset pointCount;
        GA_Offset primCount;
        bool includeXform;
        bool isConstant;            // Attributes are constant
        bool isTopologyConstant;    // Flag whether topology is constant
        bool reusePrimitives;       // Reuse existing primitives

	// normally set to 0 but useful for interpolation of
	// varying GeomParams across NuPatch
	int activePatchRows;
	int activePatchCols;

        // Attribute name map
        const std::map<std::string,std::string> *nameMap;

        UT_Interrupt *boss;
    };

    struct ArgsRowColumnReset
    {
	Args &m_args;

	ArgsRowColumnReset(Args &args, int rows, int cols)
	: m_args(args)
	{
	    m_args.activePatchRows = rows;
	    m_args.activePatchCols = cols;
	}
	~ArgsRowColumnReset()
	{
	    m_args.activePatchRows = m_args.activePatchCols = 0;
	}
    };

    class InterruptedException : public std::runtime_error
    {
    public:
         InterruptedException( const std::string & what_arg )
                : std::runtime_error( what_arg ){}
    };


    void walkObject( Args & args, sop_IAlembicWalker &pathBuf,
	    IObject parent, const ObjectHeader &ohead,
            PathList::const_iterator I, PathList::const_iterator E,
                    M44d parentXform, bool parentXformIsConstant);



    std::string getFullName( IObject object );



    void buildSubD( Args & args, ISubD & subd, M44d parentXform, bool parentXformIsConstant);
    void buildPolyMesh( Args & args, IPolyMesh & polymesh, M44d parentXform, bool parentXformIsConstant);
    void buildNuPatch( Args & args, INuPatch & nupatch, M44d parentXform, bool paretnXformIsConstant);
    void buildCurves( Args & args, ICurves & curves, M44d parentXform, bool parentXformIsConstant);
    void buildPoints( Args & args, IPoints & curves, M44d parentXform, bool parentXformIsConstant);

    GA_PrimitiveGroup * buildMesh(const std::string & groupName,
            P3fArraySamplePtr positions,
            Int32ArraySamplePtr counts,
            Int32ArraySamplePtr indices);

    GA_PrimitiveGroup * buildCurves(const std::string & groupName,
            P3fArraySamplePtr positions,
            Int32ArraySamplePtr counts);

    GA_PrimitiveGroup * buildPoints(const std::string & groupName,
	    P3fArraySamplePtr positions,
	    UInt64ArraySamplePtr ids,
	    V3fArraySamplePtr velocities);

    GA_PrimitiveGroup * reuseMesh(const std::string &groupName,
            P3fArraySamplePtr positions, GA_Offset startPointOfs);

    bool addOrFindWidthAttribute(GA_AttributeOwner owner,
            GA_RWAttributeRef & attrIdx);

    bool addOrFindTextureAttribute(GA_AttributeOwner owner,
            GA_RWAttributeRef & attrIdx);

    bool addOrFindNormalAttribute(GA_AttributeOwner owner,
            GA_RWAttributeRef & attrIdx);

    void addWidths(Args &args, IFloatGeomParam param,
            GA_Offset startPointOfs, GA_Offset endPointOfs,
            GA_Offset startPrimOfs, GA_Offset endPrimOfs);

    void addUVs(Args & args, IV2fGeomParam param,
            GA_Offset startPointOfs, GA_Offset endPointOfs,
            GA_Offset startPrimOfs, GA_Offset endPrimOfs);

    void addNormals(Args & args, IN3fGeomParam param,
            GA_Offset startPointOfs, GA_Offset endPointOfs,
            GA_Offset startPrimOfs, GA_Offset endPrimOfs,
            bool parentXformIsConstant);

    void addArbitraryGeomParams(Args & args,
            ICompoundProperty parent,
            GA_Offset startPointOfs, GA_Offset endPointOfs,
            GA_Offset startPrimOfs, GA_Offset endPrimOfs,
            bool parentXformIsConstant);

    template <typename geomParamT, typename podT>
    void processArbitraryGeomParam(
            Args & args,
            ICompoundProperty parent,
            const PropertyHeader & propHeader,
            GA_Storage attrStorage,
            GA_TypeInfo attrTypeInfo,
            const GA_RWAttributeRef & existingAttr,
            GA_Offset startPointOfs,
            GA_Offset endPointOfs,
            GA_Offset startPrimOfs,
            GA_Offset endPrimOfs,
            bool parentXformIsConstant);

    template <typename geomParamSampleT, typename podT>
    void applyArbitraryGeomParamSample(
	    Args & args,
            geomParamSampleT & paramSample,
            const GA_RWAttributeRef & attrIdx,
            size_t totalExtent,
            GA_Offset startPointOfs,
            GA_Offset endPointOfs,
            GA_Offset startPrimOfs,
            GA_Offset endPrimOfs);

    UT_String myFileObjectCache;
    bool myTopologyConstant;
    bool myEntireSceneIsConstant;
    std::map<std::string, GA_Offset> myPrimitiveCountCache;
    int myConstantPointCount;	// Point count for constant topology
    int myConstantPrimitiveCount; // Primitive count for constant topology
    int myConstantUniqueId; // Detail unique id for constant topology
};

#endif
