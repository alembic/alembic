import sys, os, glob, unittest


cwd = os.getcwd()

# Make sure we're in the correct directory to get the tests
main_dir = os.path.dirname(os.path.abspath(sys.argv[0]))
os.chdir( main_dir )

print(sys.argv)


# For now, add the path we import from
# for alembic and imath paths and make sure they come
# before any other paths
sys.path.insert(1, os.path.abspath(sys.argv[1]))
sys.path.insert(1, os.path.abspath(sys.argv[2]))


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


# lets set it back before running so our Alembic files get dumped into
# our original working directory, instead of the source of our tests
os.chdir( cwd )

# Run the tests
runner = unittest.TextTestRunner(verbosity=2)
result = runner.run(suite)

if not result.wasSuccessful():
    exit(1)