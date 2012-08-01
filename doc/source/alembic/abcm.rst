:mod:`alembic.AbcMaterial`
==========================

AbcMaterial is a module which sits above Abc. It is not dependent upon AbcGeom -- 
nor is AbcGeom currently dependent upon it. It provides schema and object types for 
representing materials and material bindings.

Explanation of terms

target: a string representing a target application or renderer by name and agreed-upon 
convention. Examples include "prman," "arnold" and "maya." The library could also 
include a document of conventions for interchange via a reserved "abc" target value.
(More on this later).

shader type: a string representing a shader classification by name and agreed-upon 
convention. Examples include "surface," "displacement" and "light." This could also 
express things like "coshader_taco" in which the value text following the "coshader_" 
prefix is used as a coshader instance name.

shader name: a string identifying a shader object within a specified target and shader
type. Examples include "paintedplastic" and "spot_light_v12."

assignment: binding of a material object onto another object via an alembic 
object path

inheritance/flattening: combining the contributions of multiple material definitions. 
The common case is the flattening of a hierarchy of material objects following 
assignment. It can also reference to the combination of a locally defined material 
with that of an assigned material.

.. automodule:: alembic.AbcMaterial
   :members:
   :undoc-members:
