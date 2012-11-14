/*********************************
** Tsunagari Tile Engine        **
** python-optional.h            **
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

/*
 * Based on
 * http://stackoverflow.com/questions/6274822/boost-python-no-to-python-converter-found-for-stdstring
 */

#ifndef PYTHON_OPTIONAL_H
#define PYTHON_OPTIONAL_H

#include <boost/optional.hpp>
#include <boost/python.hpp>


namespace boost { namespace python {

template<typename T>
struct optional_ : private boost::noncopyable {
	struct conversion :
		public boost::python::converter::expected_from_python_type<T>
	{
		static PyObject* convert(boost::optional<T> const& value)
		{
			using namespace boost::python;
			return incref((value ? object(*value) : object()).ptr());
		}
	};

	static void* convertible(PyObject* obj) {
		using namespace boost::python;
		return obj == Py_None || extract<T>(obj).check() ? obj : NULL;
	}

	static void constructor(
		PyObject* obj,
		boost::python::converter::rvalue_from_python_stage1_data* data
	) {
		using namespace boost::python;
		void* const storage = reinterpret_cast<
			converter::rvalue_from_python_storage<optional<T> >*
		>(data)->storage.bytes;
		if (obj == Py_None) {
			new (storage) boost::optional<T>();
		} else {
			new (storage) boost::optional<T>(extract<T>(obj));
		}
		data->convertible = storage;
	}

	explicit optional_() {
		using namespace boost::python;
		if (!extract<boost::optional<T> >(object()).check()) {
			to_python_converter<boost::optional<T>, conversion, true>();
			converter::registry::push_back(
				&convertible,
				&constructor,
				type_id<boost::optional<T> >(),
				&conversion::get_pytype
			);
		}
	}
};

} } // namespace boost::python

#endif
