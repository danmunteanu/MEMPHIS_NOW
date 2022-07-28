#ifndef MPSCONDITIONEQUALS_HPP
#define MPSCONDITIONEQUALS_HPP

#include "MPSConditionBase.hpp"
#include "MPSCommon.hpp"

class MPSEngineBase;

//  class testing if token's text equals a predefined text
class MPSConditionEquals : public MPSConditionBase
{
public:
    explicit MPSConditionEquals (
            MPSEngineBase& engine,
            const std::wstring& text = L"",
            bool case_sensitive = false);
    virtual ~MPSConditionEquals ();

    virtual bool operator () (const MPSToken* token);
    virtual std::string id () const { return KConditionEqualsID; }
    virtual std::wstring to_string () const;

    void set_text (const std::wstring& text) { m_text = text; }
    std::wstring text () const { return m_text; }

    void set_case_sensitive (bool case_sensitive) { m_case_sensitive = case_sensitive; }
    bool is_case_sensitive () const { return m_case_sensitive; }

private:
    MPSEngineBase&  m_engine;
    std::wstring    m_text;
    bool            m_case_sensitive;
};

#endif // MPSCONDITIONEQUALS_HPP
