
#pragma once

#include <gsl/gsl>

namespace marcpawl {
namespace pointers {
template <class T, std::enable_if_t<std::is_pointer<T>::value, bool> = true>
using nonowner = T;

}
}

