#include "MPSActionInsertSubtoken.hpp"
#include "MPSToken.hpp"
#include <sstream>

MPSActionInsertSubtoken::MPSActionInsertSubtoken(const std::wstring& text, int pos)
    : m_text_to_add (text),
      m_position (pos)
{
}

MPSActionInsertSubtoken::~MPSActionInsertSubtoken ()
{
}

void MPSActionInsertSubtoken::operator() (MPSToken* token)
{
    if (!token)
        return;

    token->insert_subtoken(m_text_to_add, m_position);
}

std::wstring MPSActionInsertSubtoken::to_string () const
{
    std::wstringstream ss;
    ss << m_position;
    return L"INSERT TEXT(\"" + m_text_to_add + L"\") ON POSITION " + ss.str();
}
