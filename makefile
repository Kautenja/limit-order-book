# an alias to the python command
PYTHON=python3

# build the LaiNES code, test the Python interface, and build
# the deployment package
all: test deployment

#
# MARK: Development
#

# test the CPP code
test_lib_limit_order_book:
	scons test

# build the CPP code
lib_limit_order_book: test_lib_limit_order_book
	scons

# run the Python test suite
test: lib_limit_order_book
	${PYTHON} -m unittest discover .

#
# MARK: Deployment
#

clean_dist:
	rm -rf build/ dist/ .eggs/ *.egg-info/ || true

clean_python_build:
	find . -name "*.pyc" -delete
	find . -name "__pycache__" -delete

clean_cpp_build:
	find . -name ".sconsign.dblite" -delete
	find . -name "build" | rm -rf
	find . -name "lib_limit_order_book.so" -delete

# clean the build directory
clean: clean_dist clean_python_build clean_cpp_build

# build the deployment package
deployment: clean
	${PYTHON} setup.py sdist bdist_wheel

# ship the deployment package to PyPi
ship: test deployment
	twine upload dist/*
