#include <bstring.h>
#include <mexcept.h>

exception::exception (const exception& exarg) MSIPL_THROW {
    	desc = ((alloced = exarg.alloced) ?
		new string (exarg.what ()) : 0);
	};
//exception::exception (const char* str) MSIPL_THROW {
//	    desc = new string (str);
//	    alloced = true;
//	};
			 
//exception& exception::operator= (const exception& exarg) MSIPL_THROW {
//	    if (this != &exarg) {
//	        if (alloced) delete (string*)desc;
//	        desc = ((alloced = exarg.alloced) ? new string (exarg.what ()) : 0);
//	    }
//	    return *this;
//	};

//exception::~exception () MSIPL_THROW {
//	    if (alloced) delete (string*)desc;
//	};

