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

#ifndef MPSACTIONUPCASE_HPP
#define MPSACTIONUPCASE_HPP

#include "MPSActionBase.hpp"
#include "MPSCommon.hpp"

class MPSEngineBase;

class MPSActionChangeCase : public MPSActionBase
{
public:
    MPSActionChangeCase(MPSEngineBase& engine_base, bool upcase, bool all, bool recursive);
    ~MPSActionChangeCase ();

    virtual void operator() (MPSToken* token);
    virtual std::wstring to_string () const;
    virtual std::string id () const { return KActionChangeCaseID; }

    bool is_upcase () const { return m_upcase; }
    bool is_only_first () const { return m_only_first; }
    bool is_recursive () const { return m_recursive; }

private:
    MPSEngineBase&  m_engine_base;
    bool            m_upcase;
    bool            m_only_first;
    bool            m_recursive;
};

#endif // MPSACTIONUPCASE_HPP
