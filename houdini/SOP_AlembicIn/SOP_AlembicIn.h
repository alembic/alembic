#ifndef _SOP_ALEMBICIN_H_
#define _SOP_ALEMBICIN_H_

#include <UT/UT_Version.h>
#include <UT/UT_Interrupt.h>
#include <SOP/SOP_Node.h>

#include <map>

#include <Alembic/AbcGeom/All.h>
using namespace Alembic::AbcGeom;

// The Houdini Geometry libraries changed radically between H11 and H12.
#if UT_MAJOR_VERSION_INT >= 12
    #include <GA/GA_AttributeRef.h>
#else
    #include <GB/GB_AttributeRef.h>
    // These typedefs allow us to use the GA types from within H11 code
    typedef GB_AttributeRef GA_ROAttributeRef;
    typedef GB_AttributeRef GA_RWAttributeRef;
    typedef GB_PrimitiveGroup GA_PrimitiveGroup;
    typedef int GA_Offset;
    
    enum GA_Storage
    {
        GA_STORE_REAL32,
        GA_STORE_INT32,
        GA_STORE_STRING
    };
    enum GA_TypeInfo
    {
        GA_TYPE_VOID,
        GA_TYPE_POINT,
        GA_TYPE_VECTOR,
        GA_TYPE_NORMAL,
        GA_TYPE_COLOR,
    };
    inline bool	GAisIntStorage(GA_Storage s)
    {
        return s == GA_STORE_INT32;
    
    }
    inline bool	GAisFloatStorage(GA_Storage s)
    {
        return s == GA_STORE_REAL32;
    }
#endif

class SOP_AlembicIn : public SOP_Node
{
public:
    //--------------------------------------------------------------------------
    // Standard hdk declarations
    static OP_Node *myConstructor(OP_Network *net, const char *name,
            OP_Operator *entry);
    static PRM_Template myTemplateList[];
    typedef std::vector<std::string> PathList;
    
protected:
    //--------------------------------------------------------------------------
    // Standard hdk declarations
    SOP_AlembicIn(OP_Network *net, const char *name, OP_Operator *op);
    virtual ~SOP_AlembicIn();
    virtual bool unloadData();
    virtual OP_ERROR cookMySop(OP_Context &context);
    virtual void nodeUnlocked();
    
private:
    
    GA_ROAttributeRef attachDetailStringData(const std::string &attrName,
            const std::string &value);
    
    
    struct Args
    {
        double abcTime;
        size_t pointCount;
        size_t primCount;
        bool includeXform;
        bool isConstant;            // Attributes are constant
        bool isTopologyConstant;    // Flag whether topology is constant
        bool reusePrimitives;       // Reuse existing primitives
        
        // Attribute name map
        const std::map<std::string,std::string> *nameMap;
        
        
        UT_Interrupt *boss;
    };
    
    class InterruptedException : public std::runtime_error
    {
    public:
         InterruptedException( const std::string & what_arg )
                : std::runtime_error( what_arg ){}
    };
    
    
    void walkObject( Args & args, IObject parent, const ObjectHeader &ohead,
            PathList::const_iterator I, PathList::const_iterator E,
                    M44d parentXform, bool parentXformIsConstant);
    
    
    
    std::string getFullName( IObject object );
    
    
    
    void buildSubD( Args & args, ISubD & subd, M44d parentXform, bool parentXformIsConstant);
    void buildPolyMesh( Args & args, IPolyMesh & polymesh, M44d parentXform, bool parentXformIsConstant);
    
    GA_PrimitiveGroup * buildMesh(const std::string & groupName,
            P3fArraySamplePtr positions,
            Int32ArraySamplePtr counts,
            Int32ArraySamplePtr indicies,
            size_t startPointIdx);
    
    GA_PrimitiveGroup * reuseMesh(const std::string &groupName,
            P3fArraySamplePtr positions, size_t startPointIdx);
    
    bool addOrFindTextureAttribute(GEO_AttributeOwner owner,
            GA_RWAttributeRef & attrIdx);
    
    bool addOrFindNormalAttribute(GEO_AttributeOwner owner,
            GA_RWAttributeRef & attrIdx);
    
    void addUVs(Args & args, IV2fGeomParam param,
            size_t startPointIdx, size_t endPointIdx,
            size_t startPrimIdx, size_t endPrimIdx);
    
    void addNormals(Args & args, IN3fGeomParam param,
            size_t startPointIdx, size_t endPointIdx,
            size_t startPrimIdx, size_t endPrimIdx,
            bool parentXformIsConstant);
    
    void addArbitraryGeomParams(Args & args,
            ICompoundProperty parent,
            size_t startPointIdx, size_t endPointIdx,
            size_t startPrimIdx, size_t endPrimIdx,
            bool parentXformIsConstant);
    
    template <typename geomParamT, typename podT>
    void processArbitraryGeomParam(
            Args & args,
            ICompoundProperty parent,
            const PropertyHeader & propHeader,
            GA_Storage attrStorage,
            GA_TypeInfo attrTypeInfo,
            const GA_RWAttributeRef & existingAttr,
            size_t startPointIdx,
            size_t endPointIdx,
            size_t startPrimIdx,
            size_t endPrimIdx,
            bool parentXformIsConstant);
    
    template <typename geomParamSampleT, typename podT>
    void applyArbitraryGeomParamSample(
            geomParamSampleT & paramSample,
            const GA_RWAttributeRef & attrIdx,
            size_t totalExtent,
            size_t startPointIdx,
            size_t endPointIdx,
            size_t startPrimIdx,
            size_t endPrimIdx);
    
    UT_String myFileObjectCache;
    bool myTopologyConstant;
    bool myEntireSceneIsConstant;
    std::map<std::string, size_t> myPrimitiveCountCache;
};

#endif
