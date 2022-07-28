#include "MPSConditionEquals.hpp"
#include "MPSToken.hpp"
#include "MPSEngineBase.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

MPSConditionEquals::MPSConditionEquals (
        MPSEngineBase& engine,
        const std::wstring& text,
        bool case_sensitive) :
    m_engine (engine),
    m_text (text),
    m_case_sensitive (case_sensitive)
{
}

MPSConditionEquals::~MPSConditionEquals ()
{
}

bool MPSConditionEquals::operator () (const MPSToken* token)
{
    if (!token)
        return false;

    BoostSeparator separ (L";");
    BoostTokenizer tokenizer (m_text, separ);
    BoostTokenizer::iterator iter = tokenizer.begin();
    bool equals = false;
    for ( ; iter != tokenizer.end (); ++iter) {
        if (m_case_sensitive)
            equals = token->text() == (*iter);
        else
            equals = boost::to_upper_copy (token->text()) == boost::to_upper_copy (*iter);

        if (equals)
            break;
    }
    return equals;
}

std::wstring MPSConditionEquals::to_string () const {
    std::wstring str;
    if (m_text.find(L";") != m_text.npos)
        str = L"TEXT IN";
    else
        str = L"TEXT EQUALS";

    str = str + L" \"" + m_text + L"\"";
    return str;
}
