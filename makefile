# an alias to the python command
PYTHON=python3

# test the C++ and Python code and build the deployment package
all: test deployment

#
# MARK: Development
#

# test the CPP code
test_lib_limit_order_book:
	scons test

# build the shared library into the Python package
lib_limit_order_book: test_lib_limit_order_book
	scons limit_order_book

# run the Python test suite
test: lib_limit_order_book
	${PYTHON} -m unittest discover .

#
# MARK: Deployment
#

# remove the build directory, which holds all C++ and Python build outputs,
# and the setuptools metadata that cannot live there
clean_build:
	rm -rf .build *.egg-info

# remove Python bytecode and the shared library copied into the package
clean_python_build:
	find limit_order_book -name "*.pyc" -delete
	find limit_order_book -name "__pycache__" -type d -prune -exec rm -rf {} +
	rm -f limit_order_book/lib_limit_order_book*

# clean all build outputs
clean: clean_build clean_python_build

# build the deployment package
deployment: clean
	${PYTHON} -m build --outdir .build/dist

# ship the deployment package to PyPi
ship: test deployment
	twine upload .build/dist/*
