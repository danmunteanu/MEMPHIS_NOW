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

#ifndef MPSCOMMON_HPP
#define MPSCOMMON_HPP

#include <string>

//  conditions' IDs
static const std::string KConditionAlwaysID = "Always";
static const std::string KConditionIsRootID = "IsRoot";
static const std::string KConditionIsNumericID = "IsNumeric";
static const std::string KConditionEqualsID = "Equals";

//  actions' IDs
static const std::string KActionSetTextID = "SetText";
static const std::string KActionSetSeparatorsID = "SetSeparators";
static const std::string KActionReplaceInTextID = "Replace";
static const std::string KActionInsertSubtokenID = "Insert";
static const std::string KActionEnableDisableID = "EnableDisable";
static const std::string KActionChangeCaseID = "ChangeCase";

//  common constants
static const std::wstring KTextPlaceholder = L"[T]";
static const size_t KLastSubtokenPosition = static_cast <size_t> (-1);

// Statements like:
 // #pragma message(Reminder "Fix this problem!")
 // Which will cause messages like:
 // C:\Source\Project\main.cpp(47): Reminder: Fix this problem!
 // to show up during compiles. Note that you can NOT use the
 // words "error" or "warning" in your reminders, since it will
 // make the IDE think it should abort execution. You can double
 // click on these messages and jump to the line in question.

#define Stringize( L )     #L 
#define MakeString( M, L ) M(L)
#define $Line MakeString( Stringize, __LINE__ )
#define FixReminder __FILE__ "(" $Line ") : FixReminder: "

#endif // MPSCOMMON_HPP
