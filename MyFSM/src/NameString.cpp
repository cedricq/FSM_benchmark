#include "NameString.h"

namespace FSM
{
    bool_t operator==(NameString const& lhs, NameString const& rhs) { return memcmp(lhs.data_, rhs.data_, NameString::LENGTH) == 0; }
    bool_t operator!=(NameString const& lhs, NameString const& rhs) { return memcmp(lhs.data_, rhs.data_, NameString::LENGTH) != 0 ; }
}

namespace std
{
    ostream& operator<<(ostream& os, FSM::NameString const& name)
    {
        os << name.c_str();
        return os;
    }
}
