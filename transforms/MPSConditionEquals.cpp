/*
 * This file is part of MemphisNow.
 *
 * MemphisNow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MemphisNow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MemphisNow.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "MPSConditionEquals.hpp"
#include "MPSToken.hpp"
#include "MPSEngineBase.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

MPSConditionEquals::MPSConditionEquals (
        MPSEngineBase& engine,
        const std::wstring& text,
        bool case_sensitive) :
    m_engine (engine),
    m_text (text),
    m_case_sensitive (case_sensitive)
{
}

MPSConditionEquals::~MPSConditionEquals ()
{
}

bool MPSConditionEquals::operator () (const MPSToken* token)
{
    if (!token)
        return false;

    BoostSeparator separ (L";");
    BoostTokenizer tokenizer (m_text, separ);
    BoostTokenizer::iterator iter = tokenizer.begin();
    bool equals = false;
    for ( ; iter != tokenizer.end (); ++iter) {
        if (m_case_sensitive)
            equals = token->text() == (*iter);
        else
            equals = boost::to_upper_copy (token->text()) == boost::to_upper_copy (*iter);

        if (equals)
            break;
    }
    return equals;
}

std::wstring MPSConditionEquals::to_string () const {
    std::wstring str;
    if (m_text.find(L";") != m_text.npos)
        str = L"TEXT IN";
    else
        str = L"TEXT EQUALS";

    str = str + L" \"" + m_text + L"\"";
    return str;
}
