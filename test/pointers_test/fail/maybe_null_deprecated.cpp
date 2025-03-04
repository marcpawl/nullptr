#include "marcpawl/pointers/ptr.hpp"
#include <string>

int* get_deprecated( 
    marcpawl::pointers::maybe_null<int*> maybe)
{
	return maybe.get();
}

size_t arrow_deprecated( 
    marcpawl::pointers::maybe_null<std::string*> maybe)
{
	return maybe->size();
}

size_t star_deprecated( 
    marcpawl::pointers::maybe_null<std::string*> maybe)
{
	return (*maybe).size();
}

