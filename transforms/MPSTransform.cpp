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

#include "MPSTransform.hpp"
#include "MPSConditionBase.hpp"
#include "MPSActionBase.hpp"
#include "MPSToken.hpp"

MPSTransform::MPSTransform(
        MPSConditionBase*  condition,
        MPSActionBase*     action,
        bool leafs_only,
        bool enabled,
        const std::wstring& desc)
    :
      m_condition (condition),
      m_action (action),
      m_leafs_only (leafs_only),
      m_enabled (enabled),
      m_description (desc)
{
}

MPSTransform::~MPSTransform ()
{
    delete m_condition;
    delete m_action;

    m_condition = 0;
    m_action = 0;
}


void MPSTransform::operator () (MPSToken* token)
{
    if (m_condition == 0 || m_action == 0 || token == 0)
        return;

    bool apply = ((m_leafs_only) ? token->sub_tokens_empty() : true);

    if (apply && (*m_condition)(token))
        (*m_action) (token);
}

std::wstring MPSTransform::to_string () const
{
    if (!m_description.empty())
        return m_description;

    std::wstring str = L"when (";
    str += ((m_condition) ? m_condition->to_string() : L"FALSE");
    str += L") do (";
    str += ((m_action) ? m_action->to_string() : L"NOTHING") + L")";
    return str;
}
