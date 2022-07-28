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
#define Reminder __FILE__ "(" $Line ") : Reminder: "

#endif // MPSCOMMON_HPP
