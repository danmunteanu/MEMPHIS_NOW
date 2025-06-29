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
