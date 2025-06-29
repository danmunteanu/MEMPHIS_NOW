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

#ifndef MPSCONDITIONEQUALS_HPP
#define MPSCONDITIONEQUALS_HPP

#include "MPSConditionBase.hpp"
#include "MPSCommon.hpp"

class MPSEngineBase;

//  class testing if token's text equals a predefined text
class MPSConditionEquals : public MPSConditionBase
{
public:
    explicit MPSConditionEquals (
            MPSEngineBase& engine,
            const std::wstring& text = L"",
            bool case_sensitive = false);
    virtual ~MPSConditionEquals ();

    virtual bool operator () (const MPSToken* token);
    virtual std::string id () const { return KConditionEqualsID; }
    virtual std::wstring to_string () const;

    void set_text (const std::wstring& text) { m_text = text; }
    std::wstring text () const { return m_text; }

    void set_case_sensitive (bool case_sensitive) { m_case_sensitive = case_sensitive; }
    bool is_case_sensitive () const { return m_case_sensitive; }

private:
    MPSEngineBase&  m_engine;
    std::wstring    m_text;
    bool            m_case_sensitive;
};

#endif // MPSCONDITIONEQUALS_HPP
