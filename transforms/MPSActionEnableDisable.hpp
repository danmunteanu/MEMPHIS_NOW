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

#ifndef MPSACTIONDISCARD_HPP
#define MPSACTIONDISCARD_HPP

#include "MPSActionBase.hpp"
#include "MPSCommon.hpp"
#include "MPSEngineBase.hpp"

class MPSActionEnableDisable : public MPSActionBase
{
public:
    MPSActionEnableDisable(MPSEngineBase& engine_base, bool disable);
    virtual ~MPSActionEnableDisable();

    bool is_disable () const { return m_disable; }

    virtual std::string id () const { return KActionEnableDisableID; };
    virtual void operator() (MPSToken* token);
    virtual std::wstring to_string () const { return m_disable ? L"DISABLE" : L"ENABLE"; }

private:
    MPSEngineBase& m_engine_base;
    bool m_disable;
};

#endif // MPSACTIONDISCARD_HPP
