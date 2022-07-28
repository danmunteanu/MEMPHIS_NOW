#include "MPSActionSetText.hpp"
#include "MPSToken.hpp"
#include "MPSEngineBase.hpp"

MPSActionSetText::MPSActionSetText(MPSEngineBase& engine_base, const std::wstring& text) :
    m_engine_base (engine_base),
    m_text(text)
{
}

MPSActionSetText::~MPSActionSetText ()
{
}

void MPSActionSetText::operator() (MPSToken* token)
{
    //  replace placeholder with token's text
    std::wstring str = m_text;   //  copy it in order to keep the original text
    size_t start_pos = str.find(KTextPlaceholder);
    if (start_pos != std::string::npos) {
        str.replace(start_pos, KTextPlaceholder.length(), token->text());
    }

    //  update the text
    token->set_text(str);
}

std::wstring MPSActionSetText::to_string () const
{
    return L"SET TEXT=\"" + m_text + L"\"";
}
