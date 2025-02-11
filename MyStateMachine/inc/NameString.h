#ifndef NAME_STRING_H
#define NAME_STRING_H

#include <numeric>
#include <array>
#include <string>
#include <cstring>

using char_t = unsigned char;
using bool_t = bool;

namespace FSM
{
    class NameString
    {
    public:
        constexpr static std::size_t LENGTH{64};

        constexpr NameString() = default;
        constexpr NameString(NameString const& other) = default;
        constexpr NameString& operator=(NameString const& other) = default;

        NameString(char_t const* str)
        {
            std::fill(data_, data_+LENGTH, 0);
            std::size_t const size = strnlen(str, LENGTH-1);
            std::copy(str, str + size, data_);
        }

        NameString(const std::string& str)
        {
            std::fill(data_, data_ + LENGTH, 0);
            std::size_t const toCopy = std::min(str.size(), LENGTH-1);
            std::copy(str.begin(), str.begin() + toCopy, data_);
        }

        NameString& operator=(std::string& str) { *this = NameString(str); return *this; }
        NameString& operator=(char_t const* str)      { *this = NameString(str); return *this; }
        ~NameString() = default;

        std::string str() const
        {
            std::string str;
            std::size_t const size = strnlen(data_, LENGTH);
            str.insert(str.begin(), data_, data_ + size);
            return str;
        }
        char_t const* c_str() const { return data_; }
        char_t* data()              { return data_; }
        char_t const* data() const  { return data_; }

        char_t const& operator[](int index) const   { return data_[index]; }

        char data_[LENGTH]{};
    };

    bool operator==(NameString const& lhs, NameString const& rhs);
    bool operator!=(NameString const& lhs, NameString const& rhs);
    bool operator< (NameString const& lhs, NameString const& rhs);

    void PrintTo(NameString const& name, std::ostream* os);
}

namespace std
{
    template<> struct hash<FSM::NameString>
    {
        std::size_t operator()(FSM::NameString const& k) const
        {
            return std::accumulate(k.data_, k.data_+FSM::NameString::LENGTH, 1, [](std::size_t h, auto c) { return h * 101 + c; });
        }
    };

    ostream& operator<<(ostream& os, FSM::NameString const& name);
}

#endif
