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

#include "MPSActionReplaceInText.hpp"
#include "MPSEngineBase.hpp"
#include <MPSToken.hpp>
#include <boost/algorithm/string/replace.hpp>

MPSActionReplaceInText::MPSActionReplaceInText(MPSEngineBase& engine_base, const std::wstring& search_string, const std::wstring& replace_string) :
    m_engine_base (engine_base),
    m_search_string (search_string),
    m_replace_string (replace_string)
{
}

MPSActionReplaceInText::~MPSActionReplaceInText()
{
}

void MPSActionReplaceInText::operator() (MPSToken* token)
{
    if (!token)
        return;

    if (token->text().empty())
        return;

    std::wstring text = token->text();
    boost::replace_all (text, m_search_string, m_replace_string);
    token->set_text(text);
}

std::wstring MPSActionReplaceInText::to_string () const
{
    std::wstring str = L"REPLACE \"" + m_search_string + L"\" WITH \"" + m_replace_string + L"\"";
    return str;
}
