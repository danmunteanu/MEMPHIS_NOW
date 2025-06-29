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

#ifndef MPSACTIONSETSEPARATORS_HPP
#define MPSACTIONSETSEPARATORS_HPP

#include "MPSEngineBase.hpp"
#include "transforms/MPSActionBase.hpp"
#include "MPSCommon.hpp"
#include <string>

class MPSEngineBase;

class MPSActionSetSeparators : public MPSActionBase
{
public:
    MPSActionSetSeparators (MPSEngineBase& engine_base, const std::wstring& separators);
    ~MPSActionSetSeparators();

    virtual void operator() (MPSToken* token);
    virtual std::wstring to_string () const { return L"SET SEPARATORS=\"" + m_separators + L"\""; }
    virtual std::string id () const { return KActionSetSeparatorsID; }

    void set_separators (const std::wstring& separators) { m_separators = separators; }
    std::wstring separators () const { return m_separators; }

private:
    MPSEngineBase&  m_engine_base;
    std::wstring    m_separators;
};

#endif // MPSACTIONSETSEPARATORS_HPP
