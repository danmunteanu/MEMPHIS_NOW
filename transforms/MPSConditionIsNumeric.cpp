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

#include "MPSConditionIsNumeric.hpp"
#include "MPSToken.hpp"
#include <boost/lexical_cast.hpp>

MPSConditionIsNumeric::MPSConditionIsNumeric()
{
}

MPSConditionIsNumeric::~MPSConditionIsNumeric ()
{
}

bool MPSConditionIsNumeric::operator () (const MPSToken* token)
{
    if (!token)
        return false;

    if (token->text().empty())
        return false;

    try {
        boost::lexical_cast<int> (token->text ());
    } catch (const boost::bad_lexical_cast&) {
        return false;
    }
    return true;
}

std::wstring MPSConditionIsNumeric::to_string () const
{
    return L"Text is numeric";
}
