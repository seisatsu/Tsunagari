/***************************************
** Tsunagari Tile Engine              **
** python.h                           **
** Copyright 2011-2013 PariahSoft LLC **
***************************************/

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

#ifndef PYTHON_H
#define PYTHON_H

#include <string>

typedef struct _object PyObject;

extern int inPythonScript;


//! Initialize Python libraries for use.
bool pythonInit();

//! Free resources used by Python libraries and uninitialize them.
void pythonFinalize();

//! Print last error received within Python.
void pythonErr();


bool pythonPrependPath(const std::string& path);
bool pythonRmPath(const std::string& path);


//! Access to global namespace shared by all Python scripts.
//! Object is borrowed, so don't Py_DECREF it.
PyObject* pythonGlobals();

//! Bind a C++ object into the global Python namespace.
template<class T>
void pythonSetGlobal(const std::string& name, T pointer);

template<class Fn>
void pythonAddFunction(const std::string& name, Fn fn);

void pythonDumpGlobals();

#endif

