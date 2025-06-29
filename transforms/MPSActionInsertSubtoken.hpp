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

#ifndef MPSACTIONINSERTSUBTOKEN_HPP
#define MPSACTIONINSERTSUBTOKEN_HPP

#include <string>
#include "MPSActionBase.hpp"
#include "MPSCommon.hpp"

class MPSActionInsertSubtoken : public MPSActionBase
{
public:
    MPSActionInsertSubtoken(const std::wstring& text, int pos);
    virtual ~MPSActionInsertSubtoken ();

    virtual void operator() (MPSToken* token);
    virtual std::wstring to_string () const;
    virtual std::string id () const { return KActionInsertSubtokenID; }

    std::wstring text_to_add () const { return m_text_to_add; }
    int position () const { return m_position; }

private:
    std::wstring    m_text_to_add;
    int             m_position;
};

#endif // MPSACTIONINSERTSUBTOKEN_HPP
