MicMac v2 Python3 API
=====================


Dependencies
------------

As admin:

    apt install python3-pip doxygen

As user:

    pip3 install pybind11 wheel


Compilation
-----------

First, compile MMv1 and MMv2.

Then, in 'apib11' directory:

    make

The module can be used from this directory without installation (this local version has priority to installed modules).

Installation
------------

    make install

Distribution
------------

The file dist/MMVII-*.whl, created at compilation, can be distributed to machines with same OS, architecture and python version.

This file contains all the necessary files to run the module:
MMVII does not have to be installed on the machine to use the python module.

It can be installed with:

    pip3 install MMVII-*.whl

Upgrade pip if needed:

    python3 -m pip install --upgrade pip



Usage
-----

```python
    import MMVII
```

The built-in Python help system can be used to have information about the API.

See 'examples' directory for use cases.



Binding Conventions
-------------------
  - Implement python binding in a .cpp files that matches th C++ header.<p> (i.e. a binding for a C++ class that is declared in MMVII_MyClass.h we'll be implemented in py_MMVII_MyClass.cpp)
  - Class names are the same than in C++ but without the leading 'c'
  - functions, methods, properties are the same than in C++ but with initial letter in lower case (and 'm' removed from class variables name)

