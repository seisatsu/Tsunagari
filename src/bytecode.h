/***************************************
** Tsunagari Tile Engine              **
** python-script.h                    **
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

#ifndef PYTHON_SCRIPT_H
#define PYTHON_SCRIPT_H

#include <string>

#include <boost/shared_ptr.hpp>

// from Python
#include <pyport.h>
#include <object.h>
#include <code.h>

class Bytecode
{
public:
	//! Compile a Python script. Must provide both a representative
	//! filename for any error messages along with a string containing the
	//! body of code to compile. Prints any errors on failure.
	Bytecode(const std::string& filename, const std::string& sourcecode);
	~Bytecode();

	//! Run a compiled Python script. Returns false on runtime error and
	//! prints the error.
	bool execute();

	//! Returns true if this object was constructed correctly.
	bool valid() const;

	const std::string& filename() const;

private:
	std::string fn;

	PyCodeObject* code;
	PyObject* locals;
};

typedef boost::shared_ptr<Bytecode> BytecodeRef;

#endif

