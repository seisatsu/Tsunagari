namespace Validator {
	enum VALUE_TYPE {
		NONE, // Has no content. Element only.
		INT, // Integer data.
		FLOAT, // Float data.
		STRING // String data.
	};

	enum NODE_TYPE {
		ELEM,
		ATTR
	};
	
	enum NODE_FLAGS {
		OPTIONAL,
		REQUIRED,
		SINGLE,
		MULTI
	};

	struct NODE {
		std::string name; // Node name.
		VALUE_TYPE vtype; // Type of value inside node.
		std::string value; // Value or text data.
		unsigned flags;
	
		virtual NODE_TYPE ntype();
	};

	struct ELEMENT : NODE {
		NODE_TYPE ntype() {
			return ELEM;
		}
		
		bool validate(XMLDoc* doc);
		
		std::map<std::string, ATTRIBUTE> attrs; // Element's attributes, if any.
		std::map<std::string, std::vector<ELEMENT> > elems; // Child elements, if any.
	};
	
	struct ATTRIBUTE : NODE {
		NODE_TYPE ntype() {
			return ATTR;
		}
	};
}


Element(REQUIRED | SINGLE, "whatever", NONE,
	Element(OPTIONAL | MULTI, "num two", NONE,
		Attribute
		Element
		Element
			Element
	),
	Attribute(REQUIRED, "at", INT)
)


