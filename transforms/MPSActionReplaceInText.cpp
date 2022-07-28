#include "MPSActionReplaceInText.hpp"
#include "MPSEngineBase.hpp"
#include <MPSToken.hpp>
#include <boost/algorithm/string/replace.hpp>

MPSActionReplaceInText::MPSActionReplaceInText(MPSEngineBase& engine_base, const std::wstring& search_string, const std::wstring& replace_string) :
    m_engine_base (engine_base),
    m_search_string (search_string),
    m_replace_string (replace_string)
{
}

MPSActionReplaceInText::~MPSActionReplaceInText()
{
}

void MPSActionReplaceInText::operator() (MPSToken* token)
{
    if (!token)
        return;

    if (token->text().empty())
        return;

    std::wstring text = token->text();
    boost::replace_all (text, m_search_string, m_replace_string);
    token->set_text(text);
}

std::wstring MPSActionReplaceInText::to_string () const
{
    std::wstring str = L"REPLACE \"" + m_search_string + L"\" WITH \"" + m_replace_string + L"\"";
    return str;
}
