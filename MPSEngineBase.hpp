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

#ifndef MPSENGINEBASE_HPP
#define MPSENGINEBASE_HPP

#include <MPSToken.hpp>
#include <queue>

/*
 *  This class decouples the MPSCondition* and MPSAction* classes from the actual MPSEngine.
 *  They end up having access to exactly what they need from the MPSEngine instead of the whole functionality
*/
class MPSEngineBase
{
public:
    virtual ~MPSEngineBase() = 0;

    virtual void update (MPSToken* token) = 0;    

    //  this is public from the engine base, but becomes private in the actual engine, intended
    virtual void change_case (MPSToken* token, bool upcase, bool only_first, bool recursive) = 0;

    virtual bool is_token_current_root(const MPSToken* token) const = 0;

    //  error handling
    void push_message (const std::wstring& msg);
    bool has_messages () const;
    std::wstring pop_message ();
    void clear_messages ();

protected:
    //  store status & error messages until they are processed
    mutable std::queue<std::wstring>    m_messages;
};

#endif // MPSENGINEBASE_HPP
