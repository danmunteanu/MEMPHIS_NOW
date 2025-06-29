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

#include "MPSActionSetText.hpp"
#include "MPSToken.hpp"
#include "MPSEngineBase.hpp"

MPSActionSetText::MPSActionSetText(MPSEngineBase& engine_base, const std::wstring& text) :
    m_engine_base (engine_base),
    m_text(text)
{
}

MPSActionSetText::~MPSActionSetText ()
{
}

void MPSActionSetText::operator() (MPSToken* token)
{
    //  replace placeholder with token's text
    std::wstring str = m_text;   //  copy it in order to keep the original text
    size_t start_pos = str.find(KTextPlaceholder);
    if (start_pos != std::string::npos) {
        str.replace(start_pos, KTextPlaceholder.length(), token->text());
    }

    //  update the text
    token->set_text(str);
}

std::wstring MPSActionSetText::to_string () const
{
    return L"SET TEXT=\"" + m_text + L"\"";
}
