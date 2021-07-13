#include <pybind11/pybind11.h>

namespace py = pybind11;

// forwards
void register_iarchive(py::module_& module_handle);
void register_iobject(py::module_& module_handle);
void register_iscalarproperty( py::module_& module_handle );
void register_iarrayproperty(py::module_& module_handle);
void register_isampleselector(py::module_& module_handle);
// void register_coreabstracttypes(py::module_& module_handle);
// void register_utiltypes(py::module_& module_handle);
// void register_abctypes(py::module_& module_handle);
// void register_abccorelayer(py::module_& module_handle);
// void register_archiveinfo(py::module_& module_handle);
// void register_oarchive(py::module_& module_handle);
// void register_oobject(py::module_& module_handle);
void register_icompoundproperty(py::module_& module_handle);
// void register_oarrayproperty( py::module_& module_handle);
// void register_ocompoundproperty(py::module_& module_handle);
// void register_oscalarproperty( py::module_& module_handle );
// void register_itypedscalarproperty( py::module_& module_handle );
// void register_itypedarrayproperty( py::module_& module_handle );
// void register_otypedscalarproperty( py::module_& module_handle );
// void register_otypedarrayproperty( py::module_& module_handle );
// void register_typedpropertytraits(py::module_& module_handle);
// void register_abcgeomtypes(py::module_& module_handle);
// void register_visibility( py::module_& module_handle );
// void register_archivebounds( py::module_& module_handle );
// void register_ofaceset( py::module_& module_handle );


PYBIND11_MODULE(pybind_alembic, module_handle)
{
  // register_coreabstracttypes(module_handle);
  //
  // register_utiltypes(module_handle);
  //
  // register_abctypes(module_handle);
  //
  // register_abccorelayer(module_handle);

  // register_archiveinfo(module_handle);
  register_isampleselector(module_handle);
  register_icompoundproperty(module_handle);
  register_iarchive(module_handle);
  register_iarrayproperty(module_handle);
  // register_oarchive(module_handle);
  register_iobject(module_handle);
  // register_oobject(module_handle);
  // register_ocompoundproperty(module_handle);
  register_iscalarproperty( module_handle );
  // register_oscalarproperty( module_handle );
  // register_oarrayproperty(module_handle);
  // register_itypedscalarproperty( module_handle );
  // register_itypedarrayproperty( module_handle );
  // register_otypedscalarproperty( module_handle );
  // register_otypedarrayproperty( module_handle );
  // register_typedpropertytraits( module_handle );
  //
  // register_abcgeomtypes( module_handle );
  //
  // register_visibility( module_handle );
  // register_archivebounds( module_handle );
  //
  // register_ofaceset( module_handle );




  module_handle.doc() = "This is python module for alembic using pybind11";
}
