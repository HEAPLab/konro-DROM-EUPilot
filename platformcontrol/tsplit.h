#ifndef _TSPLIT_H_
#define _TSPLIT_H_

#include <vector>
#include <string>
#include <locale>

namespace split {

namespace detail {

        template <typename charT> struct space_character {
                static const charT value = ' ';
        };
        template <> struct space_character<wchar_t> {
                static const wchar_t value = L'X';
                //static const char value = 'X';
        };
}       // namespace detail

    /*!
     * \brief Split a string at boundaries defined by seps[]
     * \param line the string to split
     * \param seps the list of separators (eg. seps = " \t")
     * \returns the vector of substrings
     */
    template<typename charT>
    std::vector<std::basic_string<charT> > tsplit(const std::basic_string<charT> &line, const charT *seps)
    {
        std::size_t i, j;
        std::vector<std::basic_string<charT> > vec;

        i = j = 0;
        while (j < line.length()) {
            j = line.find_first_of(seps, i);
            if (j == std::string::npos)
                j = line.length();
            vec.push_back(line.substr(i, j-i));
            i = ++j;
        }
        return vec;
    }

    /*!
     * \brief Split a string in pieces of 'size' size
     * \param line the line to split
     * \param size the requested size of the parts
     * \note The last string could be smaller than size if the length of
     * line is not a multiple of 'size'
     * \returns the vector of substrings
     */
    template<typename charT>
    std::vector<std::basic_string<charT> > tsplit(const std::basic_string<charT> &line, int size)
    {
        std::vector<std::basic_string<charT> > vec;
    #if 0
        if (line.size() % size != 0)
            return vec;
    #endif
        for (typename std::basic_string<charT>::size_type i = 0; i < line.size(); i += size)
            vec.push_back(line.substr(i, size));
        return vec;
    }

    /*!
     * \brief Pad string with spaces up to 'len' length
     * \param s the string to pad
     * \param len the padding length
     * \note the string is modified "in place"
     */
    template <typename charT>
    void pad(std::basic_string<charT> &s, int len)
    {
        charT padc = detail::space_character<charT>::value;
        while (s.size() < (size_t)len)
            s += padc;
    }

}   // namespace split
#endif
