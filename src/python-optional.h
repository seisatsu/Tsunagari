/*********************************
** Tsunagari Tile Engine        **
** python-optional.h            **
** Copyright 2011-2012 OmegaSDG **
*********************************/

/*
 * Based on
 * http://stackoverflow.com/questions/6274822/boost-python-no-to-python-converter-found-for-stdstring
 */

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

