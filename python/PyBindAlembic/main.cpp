#include <pybind11/pybind11.h>
#include <iostream>
#include <Foundation.h>

namespace py = pybind11;
using namespace Abc;

// forwards
void register_typedarraysampleconverters(py::module_& module_handle);

void register_iarchive(py::module_& module_handle);
void register_iobject(py::module_& module_handle);
void register_iscalarproperty( py::module_& module_handle );
void register_iarrayproperty(py::module_& module_handle);
void register_isampleselector(py::module_& module_handle);
void register_coreabstracttypes(py::module_& module_handle);
void register_utiltypes(py::module_& module_handle);
void register_abctypes(py::module_& module_handle);
void register_abccorelayer(py::module_& module_handle);
void register_archiveinfo(py::module_& module_handle);
void register_oarchive(py::module_& module_handle);
void register_oobject(py::module_& module_handle);
void register_icompoundproperty(py::module_& module_handle);
void register_oarrayproperty( py::module_& module_handle);
void register_ocompoundproperty(py::module_& module_handle);
void register_oscalarproperty( py::module_& module_handle );
void register_itypedscalarproperty( py::module_& module_handle );
void register_itypedarrayproperty( py::module_& module_handle );
void register_otypedscalarproperty( py::module_& module_handle );
void register_otypedarrayproperty( py::module_& module_handle );
void register_typedpropertytraits(py::module_& module_handle);
void register_abcgeomtypes(py::module_& module_handle);

void register_visibility( py::module_& module_handle );
void register_archivebounds( py::module_& module_handle );

void register_ofaceset( py::module_& module_handle );
void register_ifaceset( py::module_& module_handle );

void register_igeomparam(py::module_& module_handle);
void register_ogeomparam(py::module_& module_handle);

void register_igeombase(py::module_& module_handle);

void register_opolymesh(py::module_& module_handle);
void register_ipolymesh(py::module_& module_handle);

void register_oxform(py::module_& module_handle);
void register_ixform(py::module_& module_handle);

void register_xformop(py::module_& module_handle);
void register_filmbackxformop(py::module_& module_handle);
void register_xformsample(py::module_& module_handle);
void register_ipoints(py::module_& module_handle);
void register_opoints(py::module_& module_handle);
void register_icurves(py::module_& module_handle);
void register_ocurves(py::module_& module_handle);
void register_isubd(py::module_& module_handle);
void register_osubd(py::module_& module_handle);
void register_inupatch(py::module_& module_handle);
void register_onupatch(py::module_& module_handle);
void register_icamera(py::module_& module_handle);
void register_ocamera(py::module_& module_handle);
void register_camerasample(py::module_& module_handle);
void register_ilight(py::module_& module_handle);
void register_olight(py::module_& module_handle);
void register_icollections(py::module_& module_handle);
void register_ocollections(py::module_& module_handle);

void register_imaterial(py::module_& module_handle);
void register_omaterial(py::module_& module_handle);
void register_materialflatten(py::module_& module_handle);
void register_materialassignment(py::module_& module_handle);


PYBIND11_MODULE(alembic, module_handle)
{
    py::module imath = py::module::import("imath");


    py::object scope = py::module(module_handle);
    scope.attr("__path__") = "alembic";

    {
        const char* scopeName = "AbcCoreAbstract";
        const char* moduleName = "alembic.AbcCoreAbstract";
        auto module = py::reinterpret_borrow<py::module>(PyImport_AddModule(moduleName));
        scope.attr(scopeName) = module;

        register_coreabstracttypes(module);
    }

    {
        const char* scopeName = "Util";
        const char* moduleName = "alembic.Util";
        auto module = py::reinterpret_borrow<py::module>(PyImport_AddModule(moduleName));
        scope.attr(scopeName) = module;

        register_utiltypes(module);
    }

    {
      const char* scopeName = "Abc";
      const char* moduleName = "alembic.Abc";
      auto module = py::reinterpret_borrow<py::module>(PyImport_AddModule(moduleName));
      scope.attr(scopeName) = module;

      register_abctypes(module);
      register_typedarraysampleconverters(module);

      // registered here because the utils within are only really needed when
      // creating object and properties
      register_abccorelayer(module);

      register_archiveinfo(module);
      register_iarchive(module);
      register_oarchive(module);
      register_iobject(module);
      register_oobject(module);
      register_icompoundproperty(module);
      register_ocompoundproperty(module);
      register_isampleselector(module);
      register_iscalarproperty( module );
      register_iarrayproperty(module);
      register_oscalarproperty( module );
      register_oarrayproperty(module);
      register_itypedscalarproperty( module );
      register_itypedarrayproperty( module );
      register_otypedscalarproperty( module );
      register_otypedarrayproperty( module );
      register_typedpropertytraits( module );
    }

    {
        const char* scopeName = "AbcGeom";
        const char* moduleName = "alembic.AbcGeom";
        auto module = py::reinterpret_borrow<py::module>(PyImport_AddModule(moduleName));
        scope.attr(scopeName) = module;

        register_abcgeomtypes( module );

        register_visibility( module );
        register_archivebounds( module );

        register_ofaceset( module );
        register_ifaceset( module );

        register_igeomparam( module );
        register_ogeomparam( module );

        register_igeombase( module );
        register_ipolymesh( module );
        register_opolymesh( module );

        register_oxform( module );
        register_ixform( module );
        register_xformop( module );
        register_filmbackxformop( module );
        register_xformsample( module );

        register_ipoints( module );
        register_opoints( module );

        register_icurves( module );
        register_ocurves( module );

        register_isubd( module );
        register_osubd( module );

        register_inupatch( module );
        register_onupatch( module );

        register_icamera( module );
        register_ocamera( module );
        register_camerasample( module );

        register_ilight( module );
        register_olight( module );
   }

   {
        const char* scopeName = "AbcCollection";
        const char* moduleName = "alembic.AbcCollection";
        auto module = py::reinterpret_borrow<py::module>(PyImport_AddModule(moduleName));
        scope.attr(scopeName) = module;

        register_icollections( module );
        register_ocollections( module );
   }

   {
        const char* scopeName = "AbcMaterial";
        const char* moduleName = "alembic.AbcMaterial";
        auto module = py::reinterpret_borrow<py::module>(PyImport_AddModule(moduleName));
        scope.attr(scopeName) = module;

        register_imaterial( module );
        register_omaterial(module);
        register_materialflatten(module);
        register_materialassignment(module);
   }

    module_handle.doc() = "Python module for alembic using pybind11";
}
