/*********************************
** Tsunagari Tile Engine        **
** scriptinst.h                 **
** Copyright 2011-2012 OmegaSDG **
*********************************/

// "OmegaSDG" is defined as Michael D. Reiley and Paul Merrill.

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

#ifndef SCRIPTINST_H
#define SCRIPTINST_H

#include <string>

#include <boost/python/object.hpp>
#include <boost/variant.hpp>

class ScriptInst
{
public:
	ScriptInst();
	ScriptInst(const std::string& strloc);
	ScriptInst(boost::python::object pyfn);

	// context = domain for errors msgs
	bool validate(const std::string& context);
	bool invoke();

private:
	struct strref {
		std::string filename;
		std::string funcname;
		PyCodeObject* funccall;
	};

	boost::variant<
		void*,
		strref,
		boost::python::object
	> data;

	friend struct validate_visitor;
	friend struct invoke_visitor;
	friend struct topython_visitor;
	friend struct scriptinst_to_python;
};

void exportScriptInst();

#endif
