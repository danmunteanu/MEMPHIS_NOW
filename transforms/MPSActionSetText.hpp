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

#ifndef MPSSETTOKEN_HPP
#define MPSSETTOKEN_HPP

#include "MPSActionBase.hpp"
#include "MPSCommon.hpp"

class MPSEngineBase;

class MPSActionSetText : public MPSActionBase
{
public:
    MPSActionSetText(MPSEngineBase& engine_base, const std::wstring& text);
    virtual ~MPSActionSetText ();

    virtual void operator() (MPSToken* token);
    virtual std::wstring to_string () const;
    virtual std::string id () const { return KActionSetTextID; }

    std::wstring text() const { return m_text; }

private:
    MPSEngineBase&      m_engine_base;
    std::wstring        m_text;
};

#endif // MPSSETTOKEN_HPP
