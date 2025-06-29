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

#include "MPSTransformInfo.hpp"

MPSTransformInfo::MPSTransformInfo () :
    condition_index (-1),
    condition_text (L""),
    action_index (-1),
    action_position (0),
    leafs_only (true),
    enabled (true)
{
    condition_checks[0] = false;
    condition_checks[1] = false;
    action_checks[0] = false;
    action_checks[1] = false;
    action_checks[2] = false;
}

void MPSTransformInfo::reset () {
    condition_index = -1;
    condition_text.clear();
    condition_checks[0] = false;
    condition_checks[1] = false;

    action_index = -1;
    action_texts[0].clear();
    action_texts[1].clear();
    action_checks[0] = false;
    action_checks[1] = false;
    action_checks[2] = false;
    action_checks[3] = false;
    action_position = 0;

    leafs_only = true;
    enabled = true;
    description.clear();
}
