##-*****************************************************************************
##
## Copyright (c) 2009-2015,
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

SET(TEST_LIBS
    Alembic::Alembic
    ${HDF5_LIBRARIES}
    ${ALEMBIC_ILMBASE_LIBS}
    ${ALEMBIC_PYILMBASE_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_THREAD_LIBRARY}
    ${ZLIB_LIBRARIES}
    ${EXTERNAL_MATH_LIBS}
)

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/python/PyAlembic/Tests")

ADD_EXECUTABLE(PyAlembic_Test
               testAlembicImport.cpp
               main.cpp
)

ADD_DEPENDENCIES(PyAlembic_Test alembic)

TARGET_LINK_LIBRARIES(PyAlembic_Test Alembic::Alembic ${ALEMBIC_PYILMBASE_LIBS})

ADD_TEST(PyAlembic_Python_TEST PyAlembic_Test)

GET_FILENAME_COMPONENT(BOOST_LIBRARY_DIR ${Boost_PYTHON_LIBRARY} PATH)
GET_FILENAME_COMPONENT(ILMBASE_LIBRARY_DIR ${ALEMBIC_ILMBASE_IMATH_LIB} PATH)

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/run_tests.in"
    "${CMAKE_CURRENT_BINARY_DIR}/run_tests"
    @ONLY
)
