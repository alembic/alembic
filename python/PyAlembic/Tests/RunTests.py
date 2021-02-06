import sys, os, glob, unittest


# Make sure we're in the correct directory
main_dir = os.path.dirname(os.path.abspath(sys.argv[0]))
os.chdir( main_dir )

print(sys.argv)
# TODO
# For now, hack the path we import from
# for alembic and imath paths and make sure they come
# before any other paths
sys.path.insert(1, sys.argv[1])
sys.path.insert(1, sys.argv[2])


testFiles = sys.argv[3:]

if not testFiles:
    testFiles = glob.glob('test*.py')

# Load all the tests
suite = unittest.TestSuite()

for file in testFiles:
    name = os.path.splitext(file)[0]
    __import__(name)
    test = unittest.defaultTestLoader.loadTestsFromName(name)
    suite.addTest(test)


# Run the tests
runner = unittest.TextTestRunner(verbosity=2)
runner.run(suite)
