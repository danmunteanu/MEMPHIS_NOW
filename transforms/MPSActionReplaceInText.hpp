#ifndef MPSACTIONREPLACEINTEXT_HPP
#define MPSACTIONREPLACEINTEXT_HPP

#include "MPSActionBase.hpp"
#include "MPSCommon.hpp"
#include <string>

class MPSEngineBase;

class MPSActionReplaceInText : public MPSActionBase
{
public:
    MPSActionReplaceInText(MPSEngineBase& engine_base, const std::wstring& search_string, const std::wstring& replace_string);
    virtual ~MPSActionReplaceInText ();

    virtual void operator() (MPSToken* token);
    virtual std::wstring to_string () const;
    virtual std::string id () const { return KActionReplaceInTextID; }

    std::wstring search_string () const { return m_search_string; }
    std::wstring replace_string () const { return m_replace_string; }

private:
    MPSEngineBase&  m_engine_base;
    std::wstring    m_search_string;
    std::wstring    m_replace_string;
};

#endif // MPSACTIONREPLACEINTEXT_HPP
