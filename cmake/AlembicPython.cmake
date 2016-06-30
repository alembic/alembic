##-*****************************************************************************
##
## Copyright (c) 2009-2016,
##  Sony Pictures Imageworks Inc. and
##  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
##
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are
## met:
## *       Redistributions of source code must retain the above copyright
## notice, this list of conditions and the following disclaimer.
## *       Redistributions in binary form must reproduce the above
## copyright notice, this list of conditions and the following disclaimer
## in the documentation and/or other materials provided with the
## distribution.
## *       Neither the name of Industrial Light & Magic nor the names of
## its contributors may be used to endorse or promote products derived
## from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
## OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##-*****************************************************************************

FIND_PACKAGE ( PythonLibs REQUIRED )
FIND_PACKAGE ( PythonInterp REQUIRED )
IF(PYTHONLIBS_FOUND)
    SET(ALEMBIC_PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS})
    SET(ALEMBIC_PYTHON_LIBRARY ${PYTHON_LIBRARIES})
ENDIF()

SET( CopyScriptFile ${CMAKE_SOURCE_DIR}/cmake/CopyScriptFile.py )

#-******************************************************************************
# A Python Script just needs to be copied to the same directory in the
# binary source tree
MACRO(ADD_PYTHON_SCRIPT ScriptFile)

  # Get the various filename permutations
  GET_FILENAME_COMPONENT( ScriptFileNoDirectory ${ScriptFile} NAME )
  GET_FILENAME_COMPONENT( ScriptFileFullPath ${ScriptFile} ABSOLUTE )

  # Create an output filename.
  SET( OutputFile ${CMAKE_CURRENT_BINARY_DIR}/${ScriptFileNoDirectory} )

  # This is the compile target
  ADD_CUSTOM_TARGET( ${ScriptFile}_Copy ALL
                      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ScriptFileFullPath} ${OutputFile}
                      DEPENDS ${ScriptFileFullPath} )
ENDMACRO(ADD_PYTHON_SCRIPT)

#-******************************************************************************
# With installation. Python scripts should go into
# ${CMAKE_INSTALL_PREFIX}/bin
#
MACRO(ADD_PYTHON_SCRIPT_INST ScriptFile)
  ADD_PYTHON_SCRIPT( ${ScriptFile} )
  INSTALL( FILES ${ScriptFile}
           DESTINATION bin
           PERMISSIONS
           OWNER_READ OWNER_EXECUTE
           GROUP_READ GROUP_EXECUTE
           WORLD_READ WORLD_EXECUTE )
ENDMACRO(ADD_PYTHON_SCRIPT_INST)

#-******************************************************************************
# A Python Module is a single .py file that needs to be copied to the same
# directory in the library source tree
# We enforce our Test-Driven-Development by AUTOMATICALLY adding a unit
# test.
MACRO(ADD_PYTHON_MODULE ModuleFile ParentModuleName )

  # Get the various filename permutations
  GET_FILENAME_COMPONENT( ModuleFileNoDirectory ${ModuleFile} NAME )
  GET_FILENAME_COMPONENT( ModuleFileFullPath ${ModuleFile} ABSOLUTE )
  GET_FILENAME_COMPONENT( ModuleFileExt ${ModuleFile} EXT )

  # Check that the extension is .py
  IF( NOT ( ${ModuleFileExt} STREQUAL ".py" ) )
    MESSAGE( FATAL_ERROR "Python Module Filenames must end in \".py\": ${ModuleFile}" )
  ENDIF()

  # Create an output filename.
  SET( OutputFile ${CMAKE_CURRENT_BINARY_DIR}/${ModuleFileNoDirectory} )

  # The "CopyScriptFile" is a python script in the alembic directories
  # that allow us to copy files in a cross-platform way.

  # This is the compile target

  ADD_CUSTOM_TARGET( ${ModuleFile}_Copy ALL
                      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ModuleFileFullPath} ${OutputFile}
                      DEPENDS ${ModuleFileFullPath} )

  # Get the RunPythonTest script
  SET( RunPythonTest ${CMAKE_SOURCE_DIR}/cmake/RunPythonTest )

  # Create a test name.
  SET( TestName ${ParentModuleName}/${ModuleFileNoDirectory}_TEST )

  # The build root is ${PROJECT_BINARY_DIR}
  ADD_TEST( NAME ${TestName}
            COMMAND ${PYTHON_EXECUTABLE}
                    ${OutputFile} ${ARGN} )
  # These tests don't always return something other than zero when they
  # fail. They do print:
  # "***Test Failed***"
  # So I shall search for this
  SET_TESTS_PROPERTIES( ${TestName} PROPERTIES
                        FAIL_REGULAR_EXPRESSION "\\*\\*\\*Test Failed\\*\\*\\*" )
ENDMACRO(ADD_PYTHON_MODULE)

#-******************************************************************************
# This macro does everything above and also does an install
# Stuff is installed into ${CMAKE_INSTALL_PREFIX}/python/${ParentModuleName}/MODULE
MACRO(ADD_PYTHON_MODULE_INST ModuleFile ParentModuleName )
  ADD_PYTHON_MODULE( ${ModuleFile} ${ParentModuleName} )
  INSTALL( FILES ${ModuleFile}
           DESTINATION lib/python/${ParentModuleName}
           PERMISSIONS OWNER_READ GROUP_READ WORLD_READ )
ENDMACRO(ADD_PYTHON_MODULE_INST)

#-******************************************************************************
# This is a testing infrastructure
# The 'RunPythonTest' script sets up the python paths correctly and then
# runs the script.
MACRO(ADD_PYTHON_EXIT0_SCRIPT_TEST PythonScript)

  # Get the python script full path
  GET_FILENAME_COMPONENT( PythonScriptNoDirectory ${PythonScript} NAME )
  GET_FILENAME_COMPONENT( PythonScriptFullPath ${PythonScript} ABSOLUTE )

  # Get the RunPythonTest script
  SET( RunPythonTest ${CMAKE_SOURCE_DIR}/cmake/RunPythonTest )

  # Fiddle with the Test Name. We expect exit0 scripts to end with
  STRING( REGEX MATCH "_Test$" RegexOutput ${PythonScriptNoDirectory} )
  IF( ${RegexOutput} STREQUAL "_Test" )
    STRING( REGEX REPLACE "_Test$" "_TEST" TestName ${PythonScriptNoDirectory} )
  ELSE()
    SET( TestName ${PythonScriptNoDirectory}_TEST )
  ENDIF()

  # The build root is just ${PROJECT_BINARY_DIR}
  ADD_TEST( NAME ${TestName}
            COMMAND ${RunPythonTest} ${PROJECT_BINARY_DIR}
                    ${PythonScriptFullPath} ${ARGN} )
ENDMACRO(ADD_PYTHON_EXIT0_SCRIPT_TEST)
