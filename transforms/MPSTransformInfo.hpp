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

#ifndef MPSTRANSFORMINFO_HPP
#define MPSTRANSFORMINFO_HPP

#include <string>

struct MPSTransformInfo
{
    int             condition_index;
    std::wstring    condition_text;
    bool            condition_checks[2];

    int             action_index;
    std::wstring    action_texts[2];
    bool            action_checks[4];
    int             action_position;

    bool            leafs_only;
    bool            enabled;
    std::wstring    description;

    MPSTransformInfo ();
    void reset ();
};

#endif // MPSTRANSFORMINFO_HPP
