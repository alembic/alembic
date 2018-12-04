##-*****************************************************************************
##
## Copyright (c) 2009-2011,
##  Sony Pictures Imageworks, Inc. and
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
## *       Neither the name of Sony Pictures Imageworks, nor
## Industrial Light & Magic nor the names of their contributors may be used
## to endorse or promote products derived from this software without specific
## prior written permission.
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

import maya.OpenMaya as OpenMaya
import maya.OpenMayaFX as OpenMayaFX
import maya.cmds as cmds
import maya.mel
import os
import random
import pprint
import sys

from unittest import TestCase

# putting this here to remove dependance of util.py file (unavailable in my test environment)
def expandFileName(name):
    return os.getcwd() + os.path.sep + name

# Multi dirty write function for verbose to troubleshoot
def stdoutWrite( text ):
    return
    sys.__stdout__.write( "%s\n" % text )

# Main Test Class
class AnimPointPrimitive_nParticle_Test( TestCase):

    longMessage = True

    def setUp(self):

        cmds.file(new=True, force=True)
        self.__abc_files = []
        self.__test_files = []


    def tearDown(self):
        # Make sure to disable this function if you need to manualy check the particles data
        for f in self.__abc_files:
            os.remove(f)

        for f in self.__test_files:
            os.remove(f)

    def writeData(self, testData, fileName):
        # Writing the dataDict to file for manual particle data checking
        dataFile = expandFileName('%s.txt' % fileName )
        self.__test_files.append(dataFile)

        f = open(dataFile, "w")
        printData = pprint.pformat(testData)
        f.write( printData )
        f.close()

    def exportAbc(self, root, frameRange=(1,24), suffix="", **kargs):

        self.__abc_files.append(expandFileName('testAnimParticleReadWrite_%s.abc' % suffix))

        # seed on first frame
        cmds.currentTime(1)
        maya.mel.eval("seed(1)")
        
        # First, eval time from one to frameRange start to avoid a possible jump
        for i in range(1,frameRange[0]):
            cmds.currentTime(i)


        # Format argument for extra atrbutes
        argStr = " "
        for item in kargs:
            if isinstance(kargs[item], (tuple,list)):
                for element in kargs[item]:
                    argStr += " -%s %s" % ( item, element )
            else:
               argStr += " -%s %s" % ( item, kargs[item] )

        # Export
        stdoutWrite("writefile: %s" % self.__abc_files[-1])
        cmds.AbcExport(j='-fr %d %d -root %s %s -file ' % (frameRange[0], frameRange[1], root, argStr) + self.__abc_files[-1] )

    def importAbc(self):
        cmds.file(new=True, force=True)
        cmds.AbcImport(self.__abc_files[-1], mode='open')
        cmds.currentTime( 1, update=True )

    def compareAtFrame( self, inputData, particleFnList, frame, info=""):

        stdoutWrite( "%s: frame %s\n" % (info, frame))

        # Jump to Frame
        OpenMaya.MGlobal.viewFrame( frame );

        for particleFn in particleFnList:

            particleName = particleFn.name()

            # Force nucleus evaluation
            particleFn.evaluateDynamics( OpenMaya.MTime( frame ), False)

            for attr in inputData[frame][particleFn.name()].keys():
                stdoutWrite( "\t attr: %s" % attr)

                # Test if attribute exists
                self.failUnless( cmds.objExists( "%s.%s" % (particleFn.name(), attr ) ),
                                 msg=info + "At frame %d, %s, Attribute %s, doesn't exists" % (frame,particleName,attr) )

                abcAttrData = getParticleAttribute(particleFn, attr, abc=True)
                compareData = inputData[frame][particleFn.name()][attr]

                stdoutWrite( "\t\tabc : %s" % abcAttrData)
                stdoutWrite( "\t\tmaya: %s" % compareData)

                # Compare the number of element ( should be the same as particule number )
                self.failUnlessEqual( len(compareData), len(abcAttrData),
                                      msg=info + "At frame %d, %s, Attribute %s has %s value, should be %s" %
                                                        (frame, particleName, attr, len(abcAttrData), len(compareData) ) )

                for i in range(len(compareData)):

                    abcVals = abcAttrData[i]
                    if not isinstance(abcVals,(list,tuple)):
                        abcVals = [abcVals]
                    compareVals = compareData[i]
                    if not isinstance(compareVals,(list,tuple)):
                        compareVals = [compareVals]

                    # Compare number of value for each particle (==extent)
                    self.failUnlessEqual( len(abcVals), len(compareVals),
                                          msg=info + "At frame %d, %s[%d], Attribute %s, has %d values, should be %d" %
                                          (frame, particleName, i, attr, len(abcVals), len(compareVals) ) )

                    for j in range(len(compareVals)):
                        self.failUnlessAlmostEqual( abcVals[j], compareVals[j], 5,
                                                    msg=info + "At frame %d, %s[%d], Attribute %s[%d], has value %s, should be %s" %
                                                    (frame, particleName, i, attr, j, abcVals[j], compareVals[j] ) )


    def compareData(self, particleSystemShapes, inputData ):

        if not isinstance(particleSystemShapes,(list,tuple)):
            particleSystemShapes = [particleSystemShapes]

        particleFnList = []

        sel = OpenMaya.MSelectionList()
        # Get API objet
        for particleSystem in particleSystemShapes:
            sel.add( particleSystem )

        for i, particleSystem in enumerate(particleSystemShapes):
            obj = OpenMaya.MObject()
            sel.getDependNode( i, obj )
            particleFnList.append( OpenMayaFX.MFnParticleSystem( obj ) )

        frameList = sorted(inputData.keys())
        # Compare alembic data with particleData in playback order
        for frame in inputData.keys():
            self.compareAtFrame(inputData, particleFnList, frame)

        # do it again in a random way, alembic should be consistent when playing randomly or backward
        random.shuffle(frameList)
        for frame in frameList:
            self.compareAtFrame(inputData, particleFnList, frame, info="rand - ")


    def test_nParticle_1(self):
        # Simple particle emission with random lifespan and self collision
        # Radius si set for all particles ( kUniform scope )
        # ParticleShape has userAttribute that represente kUniform and kConstant attributes

        suffix = "01"

        cmds.file(new=True, force=True)

        # Start the test, we create a non trivial nParticle simulation
        range = (1,80)
        particleTransform, particleSystemShape = createParticleSystem_1()
        stdoutWrite("\t# Created %s %s" % (particleTransform,particleSystemShape))

        # To be able to check all particle value, we will feed a dict with all particle data
        stdoutWrite("# Get particle data")
        data = getData( particleSystemShape, frameRange=range, abc=False)

        # Write data to file for manual check
#         stdoutWrite( "# Writing particle data")
#         self.writeData(data, "test_nParticles_%s" % suffix )

        # now we can export
        stdoutWrite( "# Export abc")
        kargs = {"userAttrPrefix":"abcTest"}

        self.exportAbc( particleTransform, range, suffix, **kargs)

        # and reimport
        stdoutWrite( "# Import abc")
        self.importAbc()

        nParticleList = cmds.ls(type="nParticle")

        # Save data for alembic (for easy manual check)
        # Write data to file for manual check
#         stdoutWrite( "# Get abc data")
#         stdoutWrite("# Imported: %s " % nParticleList)
#
#         abcData = getData( nParticleList, frameRange=range, abc=True)
#         stdoutWrite( "# Writing abc data")
#         self.writeData(abcData, "test_nParticle_%s_abc" % suffix )

        # Now compare the live alembic with the data from the particle simulation
        stdoutWrite( "# Compare data")
        self.compareData( nParticleList, data )
        stdoutWrite( "# End")


    def test_nParticle_2(self):
        # Particles have a collision plane and rotation enabled
        # They also have random colors set on creation
        # The radius is varying per particle over time ( kVarying )

        suffix = "02"

        cmds.file(new=True, force=True)

        # Start the test, we create a non trivial nParticle simulation
        range = (1,80)

        particleTransform, particleSystemShape = createParticleSystem_2()

        # To be able to check all particle value, we will feed a dict with all particle data
        stdoutWrite("# Get particle data")
        data = getData( particleSystemShape, frameRange=range, abc=False)

        # Write data to file for manual check
#         stdoutWrite( "# Writing particle data")
#         self.writeData(data, "test_nParticles_%s" % suffix )

        # now we can export
        stdoutWrite( "# Export abc")
        self.exportAbc( particleTransform, range, suffix)


        # and reimport
        stdoutWrite( "# Import abc")
        self.importAbc()

        nParticleList = cmds.ls(type="nParticle")

        # Save data for alembic (for easy manual check)
        # Write data to file for manual check
#         stdoutWrite( "# Get abc data")
#         stdoutWrite("# Imported: %s " % nParticleList)
#         abcData = getData( nParticleList, frameRange=range, abc=True)
#         stdoutWrite( "# Writing abc data")
#         self.writeData(abcData, "test_nParticles_%s_abc" % suffix)

        # Now compare the live alembic with the data from the particle simulation
        stdoutWrite( "# Compare data")
        self.compareData( nParticleList, data )
        stdoutWrite( "# End")


    def test_nParticles_multiple(self):
        # Scene with both two first particles simulation to test multi points cloud in one alembic

        suffix = "multi"

        cmds.file(new=True, force=True)

        # Start the test, we create a non trivial nParticle simulation
        range = (1,80)

        particleTransform1, particleSystemShape1 = createParticleSystem_1()
        particleTransform2, particleSystemShape2 = createParticleSystem_2()

        root = cmds.group(particleTransform1, particleTransform2)

        # To be able to check all particle value, we will feed a dict with all particle data
        stdoutWrite("# Get particle data")
        data = getData( (particleSystemShape1,particleSystemShape2), frameRange=range, abc=False)

        # Write data to file for manual check
#         stdoutWrite( "# Writing particle data")
#         self.writeData(data, "test_nParticles_%s" % suffix )

        # now we can export
        kargs = {"userAttrPrefix":"abcTest"}
        stdoutWrite( "# Export abc")
        self.exportAbc( root, range, suffix, **kargs)

        # and reimport
        stdoutWrite( "# Import abc")
        self.importAbc()

        nParticleList = cmds.ls(type="nParticle")

        # Save data for alembic (for easy manual check)
        # Write data to file for manual check
#         stdoutWrite( "# Get abc data")
#         stdoutWrite("# Imported: %s " % nParticleList)
#         abcData = getData( nParticleList, frameRange=range, abc=True)
#         stdoutWrite( "# Writing abc data")
#         self.writeData(abcData, "test_nParticles_%s_abc" % suffix)

        # Now compare the live alembic with the data from the particle simulation
        stdoutWrite( "# Compare data")
        self.compareData( nParticleList, data )
        stdoutWrite( "# End")

    def test_nParticles_goal(self):
        # Particles are attracted to a deformed goal object and have a constant number of particle over time
        # Due to the nature of the goal, they have non consecutive ids

        suffix = "static_goal"

        cmds.file(new=True, force=True)

        # Start the test, we create a non trivial nParticle simulation
        range = (1,80)

        particleTransform, particleSystemShape = createParticleSystem_goal()

        # To be able to check all particle value, we will feed a dict with all particle data
        stdoutWrite("# Get particle data")
        data = getData( particleSystemShape, frameRange=range, abc=False)

        # Write data to file for manual check
#         stdoutWrite( "# Writing particle data")
#         self.writeData(data, "test_nParticles_%s" % suffix )

        # now we can export
        stdoutWrite( "# Export abc")
        self.exportAbc( particleTransform, range, suffix)

        # and reimport
        stdoutWrite( "# Import abc")
        self.importAbc()

        nParticleList = cmds.ls(type="nParticle")

        # Save data for alembic (for easy manual check)
        # Write data to file for manual check
#         stdoutWrite( "# Get abc data")
#         stdoutWrite("# Imported: %s " % nParticleList)
#         abcData = getData( nParticleList, frameRange=range, abc=True)
#         stdoutWrite( "# Writing abc data")
#         self.writeData(abcData, "test_nParticles_%s_abc" % suffix)

        # Now compare the live alembic with the data from the particle simulation
        stdoutWrite( "# Compare data")
        self.compareData( nParticleList, data )
        stdoutWrite( "# End")

###
# Functions performing maya task are outside the unitest object to be able to call them from a gui maya for testing

def createParticleSystem_1():
    # Creates a very basic particle system with custom attribute

    cmds.playbackOptions(maxTime=180)

    emitter = cmds.emitter(dx=1, dy=0, dz=0, sp=0.1, pos=(0, 5, 0),rate=20)[0]

    # nParticle creation depends on an optoinVar value, make sure we use the default one
    cmds.optionVar( sv=("NParticleStyle","Points") )

    particleSystem, particleSystemShape = cmds.nParticle(n="nParticle_test_1")
    cmds.setAttr('%s.lfm' % particleSystemShape, 0) # live forever
    cmds.setAttr('%s.particleRenderType' % particleSystemShape, 7) # Blobby, to see radius
    cmds.connectDynamic( particleSystemShape, em=emitter)

    # Keyframe the radius parametter to have a single sample but varying in time ( abc kUniformScope )
    cmds.setKeyframe( "%s.radius" % particleSystem, v=.2, t=1)
    cmds.setKeyframe( "%s.radius" % particleSystem, v=1.0, t=24)
    cmds.setKeyframe( "%s.radius" % particleSystem, v=2.0, t=50)
    cmds.setKeyframe( "%s.radius" % particleSystem, v=.5, t=100)

    # Create a custom attribute that will be handled by AbcExport as uniform double
    cmds.addAttr( particleSystemShape, ln="abcTestUniformDoublePP", at="double")
    exp = "%s.abcTestUniformDoublePP = frame;\n" % particleSystemShape

    # Create a custom attribute that will be handled by AbcExport as constant double
    cmds.addAttr( particleSystemShape, ln="abcTestConstantDoublePP", at="double")
    cmds.setAttr( "%s.abcTestConstantDoublePP" % particleSystemShape, 0x2697 )

    # Create a custom attribute that will be handled by AbcExport as a uniform vector
    cmds.addAttr( particleSystemShape, ln="abcTestUniformVectorPP", at="double3")
    cmds.addAttr( particleSystemShape, ln="abcTestUniformVectorPPX", p="abcTestUniformVectorPP")
    cmds.addAttr( particleSystemShape, ln="abcTestUniformVectorPPY", p="abcTestUniformVectorPP")
    cmds.addAttr( particleSystemShape, ln="abcTestUniformVectorPPZ", p="abcTestUniformVectorPP")
    exp += "%s.abcTestUniformVectorPPX = frame;\n" % particleSystemShape
    exp += "%s.abcTestUniformVectorPPY = time;\n" % particleSystemShape
    exp += "%s.abcTestUniformVectorPPZ = 9.81;\n" % particleSystemShape

    # Create a custom attribute that will be handled by AbcExport as a constant vector
    cmds.addAttr( particleSystemShape, ln="abcTestConstantVectorPP", at="double3")
    cmds.addAttr( particleSystemShape, ln="abcTestConstantVectorPPX", p="abcTestConstantVectorPP")
    cmds.addAttr( particleSystemShape, ln="abcTestConstantVectorPPY", p="abcTestConstantVectorPP")
    cmds.addAttr( particleSystemShape, ln="abcTestConstantVectorPPZ", p="abcTestConstantVectorPP")
    cmds.setAttr( "%s.abcTestConstantVectorPP" % particleSystemShape, 0x2697, 3.141592654, 6.02e23 )

    cmds.expression( particleSystemShape, s=exp, o="", ae=1, uc="all" )

    return particleSystem, particleSystemShape


def createParticleSystem_2():
    # Creates a point emitter, with a collide plane and some PP attributes

    cmds.playbackOptions(maxTime=180)

    emitter = cmds.emitter(dx=1, dy=0, dz=0, sp=0.1, pos=(0, 5, 0),rate=20)[0]

    # nParticle creation depends on an optionVar value, make sure we use the default one
    cmds.optionVar( sv=("NParticleStyle","Points") )

    particleSystem, particleSystemShape = cmds.nParticle(n="nParticle_test_2")
    cmds.setAttr('%s.lfm' % particleSystemShape, 2.0) # random range
    cmds.setAttr('%s.lifespan' % particleSystemShape, 3)
    cmds.setAttr('%s.lifespanRandom' % particleSystemShape, 4)
    cmds.setAttr('%s.friction' % particleSystemShape, 0.01)
    cmds.setAttr('%s.bounce' % particleSystemShape, 0.5)
    cmds.setAttr('%s.rotationFriction' % particleSystemShape, 0.98)
    cmds.setAttr('%s.pointMass' % particleSystemShape, 4)
    cmds.setAttr('%s.computeRotation' % particleSystemShape, True)
    cmds.setAttr('%s.particleRenderType' % particleSystemShape, 7) # Blobby, to see radius
    cmds.setAttr('%s.selfCollide' % particleSystemShape, True)
    cmds.connectDynamic( particleSystemShape, em=emitter)


    # add Custom Attributes
    # rgb
    if not cmds.objExists( "%s.rgbPP" % particleSystemShape):
        cmds.addAttr( particleSystemShape ,ln="rgbPP",dt="vectorArray")
    else:
        # Disconnect possible thing in rgbPP
        input = cmds.connectionInfo("%s.rgbPP" % particleSystemShape,sourceFromDestination=True)
        if input:
            cmds.disconnectAttr(input,"%s.rgbPP" % particleSystemShape)

    cmds.dynExpression( particleSystem, s="\nfloat $r = rand(1.0);float $g = rand(1.0);float $b = rand(1.0);\n%s.rgbPP = <<$r,$g,$b>>;" % particleSystemShape, c=True)

    # radius
    if not cmds.objExists( "%s.radiusPP" % particleSystemShape):
        cmds.addAttr( particleSystemShape, ln="radiusPP",dt="doubleArray")
    cmds.dynExpression( particleSystem, s="seed(%s.id);\n%s.radiusPP = .2 + time * .2 * rand(.2,1.5)" % (particleSystemShape,particleSystemShape), runtimeAfterDynamics=True)

    # rotatePP (enabled by "compute rotation", the attribute would be created by attribut editor's first constrcution in a gui maya)
    if not cmds.objExists( "%s.rotationPP" % particleSystemShape):
        cmds.addAttr( particleSystemShape, ln="rotationPP",dt="vectorArray")

    # add collision plane
    plane = cmds.polyPlane( w=30,h=30,sx=10,sy=10,ax=[0, 1, .5], cuv=2)
    cmds.select(plane[0])
    maya.mel.eval("makePassiveCollider")
    cmds.select(clear=True)
    cmds.setAttr( "nRigidShape1.friction", 0.25)
    cmds.setAttr( "nRigidShape1.stickiness", 0.0)

    return particleSystem, particleSystemShape

def createParticleSystem_goal():
    # Create a constant sized particle object following a deformed torus shape (using goal)

    cmds.file(new=True, force=True)

    # Create a torus, we will use it as goal
    xrez = 14
    yrez = 8
    goalTorus = cmds.polyTorus( r=1, sr=0.5, tw=0, sx=xrez, sy=yrez, ax=(0, 1, 0), cuv=1, ch=0)[0]

    # nParticle creation depends on an optoinVar value, make sure we use the default one
    cmds.optionVar( sv=("NParticleStyle","Points") )

    # Emit a lot of particle, the actual number will be limited to the max particle attribute in the particleShape
    emitter = cmds.emitter(dx=1, dy=0, dz=0, sp=0.1, pos=(0, 5, 0),rate=2500)[0]
    particleSystem, particleSystemShape = cmds.nParticle(n="nParticle_test_goal")
    cmds.setAttr('%s.lfm' % particleSystemShape, 0) # live forever
    cmds.setAttr('%s.particleRenderType' % particleSystemShape, 7) # Blobby, to see radius
    cmds.setAttr('%s.maxCount' % particleSystemShape, xrez*yrez) # max count is the number of vertices on the torus

    cmds.connectDynamic( particleSystemShape, em=emitter)

    # Create Goal
    cmds.goal(particleSystem, w=1, utr=0, g=goalTorus);


    # Create Initial state to we start with the correct amount of particle
    # NOTE: When using this script in command line, the first frame is not correctly evaluated and the particleShape is empty
    # This doesn't happens in interactive maya
    for i in range(1, 10):
        cmds.currentTime(i)
    cmds.saveInitialState(particleSystemShape)
    cmds.currentTime(1)

    bend, bendHandle = cmds.nonLinear( goalTorus, type="bend", lowBound=-1, highBound=1, curvature=0)

    cmds.setAttr( "%s.rotateZ" % bendHandle, 90)
    cmds.setKeyframe( "%s.curvature" % bend, v=0, t=1, inTangentType="flat", outTangentType="flat")
    cmds.setKeyframe( "%s.curvature" % bend, v=-130, t=12, inTangentType="flat", outTangentType="flat")
    cmds.setKeyframe( "%s.curvature" % bend, v=130, t=24, inTangentType="flat", outTangentType="flat")

    # Make the bend animation loop
    cmds.setInfinity( bend, poi="oscillate", attribute="curvature")

    return particleSystem, particleSystemShape



def getData( particleSystemShapes, frameRange=(1,24), abc=False, customAttribute=[]):
    '''
    Create a dict containing data

    exemple:

    dict = { frame : { attibute : [data] }

     3: "nParticle_test_1Shape" : {'id': [0.0],
                                  'position':  [(-0.0025255996733903885, 4.990767002105713, -0.0024415384978055954)],
                                  'radius':    [0.20000000298023224],
                                  'rgbPP':       [(0.44199632268870914, 0.2631281241639378, 0.6541382528648185)],
                                  'rotationPP':  [(0.0, -0.0, 0.0)],
                                  'velocity':    [(-0.6160576343536377, 0.1030367910861969, -0.5955522060394287)]},
    '''

    if not isinstance(particleSystemShapes, (list,tuple)):
        particleSystemShapes = [particleSystemShapes]

    # Get API objet
    particleFnList = []
    sel = OpenMaya.MSelectionList()
    for particleSystem in particleSystemShapes:
        sel.add( particleSystem )

    for i, particleSystem in enumerate(particleSystemShapes):
        obj = OpenMaya.MObject()
        stdoutWrite( "%d: %s" % (i, particleSystem))
        sel.getDependNode( i, obj )
        particleFnList.append( OpenMayaFX.MFnParticleSystem( obj ) )

    testData = {}

    # seed on first frame (doing the same before alembic export to correctly match randomly generated attributes)
    cmds.currentTime(1)
    maya.mel.eval("seed(1)")


    frameList = range(frameRange[0],frameRange[1])

    perParticleAttr = ( "particleId",
                        "position",
                        "acceleration",
                        "velocity",
                        "radius",

                        "force",
                        "rgbPP",
                        "mass",
                        "age",
                        "lifespanPP")
    for item in customAttribute:
        perParticleAttr.append(item)

    # Now eval the frameList
    for frame in frameList:

        stdoutWrite("fame: %d" % frame)

        # Jump to frame
        OpenMaya.MGlobal.viewFrame(frame)
        testData[frame] = {}

        for particleFn in particleFnList:

            stdoutWrite("\t: %s" % particleFn.name())
            # Force evaluate dynamics
            particleFn.evaluateDynamics( OpenMaya.MTime(frame), False)

            testData[frame][particleFn.name()] = {}

            # Get all perParticle attributes for current Shape
            for attr in perParticleAttr:
                if cmds.objExists( "%s.%s" % (particleFn.name(), attr) ):
                    testData[frame][particleFn.name()][attr] = getParticleAttribute( particleFn, attr, abc=abc)

    return testData

def getAPIVectorAttribute( particleFn, attribute ):
    # Use specific function on MFnParticleSystem
    assert( isinstance(particleFn, OpenMayaFX.MFnParticleSystem ))

    # Get a MVectorArray but return a python list of tuple
    vectorArray = OpenMaya.MVectorArray()
    getattr( particleFn, attribute )( vectorArray )
    if not vectorArray:
        vectorArray = []

    returnList = []

    for i in range( vectorArray.length() ):
        returnList.append([])
        for j in range(3):
            returnList[i].append( vectorArray[i][j] )

    return returnList

def getAPIDoubleAttribute( particleFn, attribute ):
    # Use specific function on MFnParticleSystem
    assert( isinstance(particleFn, OpenMayaFX.MFnParticleSystem ))

    # Get a MDoubleArray but return a python list of double
    doubleArray = OpenMaya.MDoubleArray()
    getattr( particleFn, attribute )( doubleArray )
    if not doubleArray:
        doubleArray = []

    return list(doubleArray)

def getAPIIntAttribute( particleFn, attribute ):
    # Use specific function on MFnParticleSystem
    assert( isinstance(particleFn, OpenMayaFX.MFnParticleSystem ))

    # Get a MIntArray but return a python list of int
    intArray = OpenMaya.MIntArray()
    getattr( particleFn, attribute )( intArray )
    if not intArray:
        intArray = []

    return list(intArray)

def getAPIVectorAttributePP( particleFn, attribute ):
    # Use specific function on MFnParticleSystem
    assert( isinstance(particleFn, OpenMayaFX.MFnParticleSystem ))

    # Get a MVectorArray but return a python list of tuple
    vectorArray = OpenMaya.MVectorArray()
    particleFn.getPerParticleAttribute( attribute, vectorArray )
    if not vectorArray:
        vectorArray = []

    returnList = []

    for i in range( vectorArray.length() ):
        returnList.append([])
        for j in range(3):
            returnList[i].append( vectorArray[i][j] )

    return returnList

def getAPIDoubleAttributePP( particleFn, attribute ):
    assert( isinstance(particleFn, OpenMayaFX.MFnParticleSystem ))

    # Get a MDoubleArray but return a python list of double
    doubleArray = OpenMaya.MDoubleArray()
    particleFn.getPerParticleAttribute( attribute, doubleArray )
    if not doubleArray:
        doubleArray = []

    return list(doubleArray)

def getAPIIntAttributePP( particleFn, attribute ):
    assert( isinstance(particleFn, OpenMayaFX.MFnParticleSystem ))

    # Get a MIntArray but return a python list of int
    intArray = OpenMaya.MIntArray()
    particleFn.getPerParticleAttribute( attribute, intArray )
    if not intArray:
        intArray = []

    return list(intArray)


def getParticleAttribute( particleFn, attribute, abc=False ):
    # When possible, use API call to get the attribute
    # else, use cmds
    assert( isinstance(particleFn, OpenMayaFX.MFnParticleSystem ))

    try:
        # Handle attribute that have specific function in MFnParticleShape
        if attribute == "position":
            return getAPIVectorAttribute(particleFn, attribute)

        elif attribute == "particleIds":
            return getAPIIntAttribute(particleFn, attribute)

        elif attribute == "velocity":
            return getAPIVectorAttribute(particleFn, attribute)

        elif attribute == "acceleration":
            return getAPIVectorAttribute(particleFn, attribute)

        elif attribute == "velocity":
            return getAPIVectorAttribute(particleFn, attribute)

        elif attribute == "radius":
        # Special Handling
            value = None
            # get Radius in the same way it is done in MayaPointPrimitiveWrite
            if particleFn.hasAttribute("radiusPP"):
                radiusPPPlug = particleFn.findPlug("radiusPP", True )
                value = getAPIDoubleAttribute( particleFn, "radius" )
            else:
                # No radius PP was found
                value = particleFn.findPlug("radius").asDouble()

            # Alembic will always feed the particle with radiusPP
            # To be able to compare the resulting alembic with the source maya particle
            # we need to always write the radius data as a list of the size of the particle object
            if not isinstance(value,(list,tuple)):
                value = [value]

            if len(value) == 1 and particleFn.count() > 0:
                value = value * particleFn.count()
                return value
            elif particleFn.count() == 0:
                return []
            else:
                return value

        elif attribute == "age":
            return getAPIDoubleAttribute(particleFn, attribute)

        elif attribute == "lifespan":
            if particleFn.hasLifespan():
                return getAPIDoubleAttribute(particleFn, attribute)
            else:
                return []

        elif attribute == "opacity":
            if particleFn.hasOpacity():
                return getAPIDoubleAttribute(particleFn, attribute)
            else:
                return []

        elif attribute == "mass":
            return getAPIDoubleAttribute(particleFn, attribute)

        elif attribute == "emission":
            if particleFn.hasEmission():
                return getAPIVectorAttribute(particleFn, attribute)
            else:
                return []

        elif attribute == "rgb":
            if particleFn.hasRgb():
                return getAPIVectorAttribute(particleFn, attribute)
            else:
                return []

        # Handle API perParticle call
        elif particleFn.isPerParticleIntAttribute( attribute ):
            return getAPIIntAttributePP( particleFn, attribute )

        elif particleFn.isPerParticleDoubleAttribute( attribute ):
            return getAPIDoubleAttributePP( particleFn, attribute )

        elif particleFn.isPerParticleVectorAttribute( attribute ):
            return getAPIVectorAttributePP( particleFn, attribute )

        elif attribute.startswith("abcTest") and not abc: # ignore when getting data for alembicObject
            # Special handling for test purpose
            # thoses attributes are created a simple attribute in maya but are translated as AbcGeom param, either
            # kUniform or kConstant and will be imported back in maya as PP attribute.
            # to be able to correctly compare them here, we need to write them as array, like any other PP attribute in maya

            value = cmds.getAttr( "%s.%s" % (particleFn.name(), attribute) ) or []

            if not isinstance(value,(list,tuple)):
                value = [value]

            if len(value) == 1 and particleFn.count() > 0:
                value = value * particleFn.count()
                return value
            elif particleFn.count() == 0:
                return []
            else:
                return value
        else:
            return cmds.getAttr( "%s.%s" % (particleFn.name(), attribute) ) or []

    except RuntimeError:
        # If it fails, use rusty cmds
        return cmds.getAttr( "%s.%s" % (particleFn.name(), attribute) ) or []
