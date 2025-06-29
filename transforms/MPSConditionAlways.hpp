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

#ifndef MPSCONDITIONALWAYS_HPP
#define MPSCONDITIONALWAYS_HPP

#include "MPSConditionBase.hpp"
#include "MPSCommon.hpp"

class MPSConditionAlways : public MPSConditionBase
{
public:
    MPSConditionAlways();
    virtual ~MPSConditionAlways();

    virtual bool operator () (const MPSToken*) { return true; }
    virtual std::string id () const { return KConditionAlwaysID; }
    virtual std::wstring to_string () const { return L"ALWAYS TRUE"; }
};

#endif // MPSCONDITIONALWAYS_HPP
