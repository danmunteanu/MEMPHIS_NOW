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

#include "MPSActionInsertSubtoken.hpp"
#include "MPSToken.hpp"
#include <sstream>

MPSActionInsertSubtoken::MPSActionInsertSubtoken(const std::wstring& text, int pos)
    : m_text_to_add (text),
      m_position (pos)
{
}

MPSActionInsertSubtoken::~MPSActionInsertSubtoken ()
{
}

void MPSActionInsertSubtoken::operator() (MPSToken* token)
{
    if (!token)
        return;

    token->insert_subtoken(m_text_to_add, m_position);
}

std::wstring MPSActionInsertSubtoken::to_string () const
{
    std::wstringstream ss;
    ss << m_position;
    return L"INSERT TEXT(\"" + m_text_to_add + L"\") ON POSITION " + ss.str();
}
