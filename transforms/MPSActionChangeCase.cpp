#include "MPSActionChangeCase.hpp"
#include "MPSEngineBase.hpp"

MPSActionChangeCase::MPSActionChangeCase(MPSEngineBase& engine_base, bool upcase, bool all, bool recursive)
    : m_engine_base (engine_base),
      m_upcase (upcase),
      m_only_first (!all),
      m_recursive (recursive)
{
}

MPSActionChangeCase::~MPSActionChangeCase()
{
}

void MPSActionChangeCase::operator() (MPSToken* token)
{
    if (!token)
        return;

    bool is_root = m_engine_base.is_token_current_root(token);

    std::wstring text = token->text();
    if (!text.empty()) {
        int (*fun_case) (int) = m_upcase ? toupper : tolower;

        if (!is_root || (is_root && token->count_subtokens () == 0)) {
            if (m_only_first) {
                //  find first letter
                size_t idx = 0;
                for ( ; idx < text.length(); ++idx) {
                    if ((text[idx] >= 'a' && text[idx] <= 'z') ||
                        (text[idx] >= 'A' && text[idx] <= 'Z'))
                        break;
                }
                if (idx < text.length()) {
                    text[idx] = fun_case (text[idx]);
                    token->set_text(text);
                }
            } else {
                std::transform(text.begin(), text.end(), text.begin(), fun_case);
                token->set_text(text);
            }
        }
        if (m_recursive) {
            MPSTokensContainer::iterator iter = token->sub_tokens_begin();
            for ( ; iter != token->sub_tokens_end(); ++iter) {
                this->operator ()(*iter);
            }
        }
    }
}

std::wstring MPSActionChangeCase::to_string () const
{
    std::wstring str = (m_upcase) ? L"UPCASE" : L"LOWCASE";
    str += L" ";
    str += (m_only_first ? L"FIRST" : L"ALL");
    return str;
}
