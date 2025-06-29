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

#include "MPSActionSetSeparators.hpp"
#include <MPSToken.hpp>

MPSActionSetSeparators::MPSActionSetSeparators(MPSEngineBase& engine_base, const std::wstring& separators) :
    m_engine_base (engine_base),
    m_separators (separators)
{
}

MPSActionSetSeparators::~MPSActionSetSeparators()
{
}

void MPSActionSetSeparators::operator() (MPSToken* token)
{
    if (!token)
        return;

    token->set_separators(m_separators);

    m_engine_base.update(token);
}
