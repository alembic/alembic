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
