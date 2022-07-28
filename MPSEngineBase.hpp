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
