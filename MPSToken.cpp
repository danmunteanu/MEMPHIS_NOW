#include "MPSToken.hpp"
#include "MPSCommon.hpp"
#include <regex>

MPSToken::~MPSToken ()
{
    cleanup_token(*this);
}

void MPSToken::set_parent (MPSToken* parent)
{
    if (!parent)
        return;

    if (parent->is_subtoken(this))
        m_parent = parent;
}

void MPSToken::shift_subtoken (MPSToken* sub_token, EMPSDirection direction)
{
    if (!sub_token)
        return;

    if (m_sub_tokens.empty())
        return;

    MPSTokensContainer::iterator iter = m_sub_tokens.begin();
    for ( ; iter != m_sub_tokens.end(); ++iter) {
        if (*iter == sub_token) {
            if (ELeft == direction) {
                if (iter == m_sub_tokens.begin())
                    return; //  shift left requested on first subtoken, do nothing

                MPSTokensContainer::iterator left = iter;
                --left;

                std::swap (*left, *iter);
                return;

            } else if (ERight == direction) {
                MPSTokensContainer::iterator right = iter;
                ++right;
                if (right == m_sub_tokens.end())
                    return; //  shift right requested on last subtoken, do nothing

                std::swap (*iter, *right);
                return;

            }
        }
    }
}

void MPSToken::split()
{
    if (m_separators.empty())
        return;

    //  check https://www.journaldev.com/37223/tokenize-string-c-plus-plus


    BoostSeparator separ (m_separators.c_str());
    BoostTokenizer tokenizer (m_text, separ);

    cleanup_token (*this);

    //  does it have only one token?
    BoostTokenizer::iterator iter = tokenizer.begin();
    for ( ; iter != tokenizer.end (); ) {
        bool equals = (m_text == std::wstring (*iter));

        ++iter;
        if (iter == tokenizer.end() && equals) {
            return; //  no subtokens, bail out
        } else {
            break;  //  has more than one token, add them
        }
    }
    
    iter = tokenizer.begin();
    int count = 0;
    for ( ; iter != tokenizer.end(); ++iter) {
        //	create sub-tokens
        MPSToken* sub_token = new MPSToken (this, *iter, L"", m_discard);
        m_sub_tokens.push_back(sub_token);
        ++count;
    }
}

void MPSToken::clear_subtokens ()
{
    cleanup_token (*this);
}

void MPSToken::insert_subtoken (const std::wstring& text, size_t pos)
{
    if (pos == KLastSubtokenPosition) {
        //  add on last position
        m_sub_tokens.push_back(new MPSToken (this, text));
    } else {
        size_t idx = 0;
        MPSTokensContainer::iterator iter = m_sub_tokens.begin();
        for ( ; (iter != m_sub_tokens.end()) && (idx != pos); ++iter, ++idx);
        if (iter == m_sub_tokens.end())
            m_sub_tokens.push_back(new MPSToken (this, text));
        else
            m_sub_tokens.insert(iter, new MPSToken (this, text));
    }
}

void MPSToken::set_text (const std::wstring& text)
{
    m_text = text;    
}

void MPSToken::set_separators (const std::wstring& separ)
{
    m_separators = separ;
    if (m_separators.empty()) {
        cleanup_token(*this);
    }
}

void MPSToken::set_discard (bool discard)
{
    discard_token(*this, discard);
}

void MPSToken::cleanup_token (MPSToken& token)
{
    if (!token.m_sub_tokens.empty()) {
        //	we have some sub-tokens, need to delete them
        for (MPSTokensContainer::iterator iter = token.m_sub_tokens.begin(); iter != token.m_sub_tokens.end(); ++iter) {
            if (*iter != 0) {
                cleanup_token (*(*iter));
                delete (*iter);
                *iter = 0;
            }
        }
        token.m_sub_tokens.clear();
    }
}

void MPSToken::discard_token (MPSToken& token, bool discard)
{
    token.m_discard = discard;
    MPSTokensContainer::iterator iter = token.sub_tokens_begin();
    for ( ; iter != token.sub_tokens_end(); ++iter) {
        if (*iter != 0)
            discard_token(*(*iter), discard);
    }
}

bool MPSToken::is_subtoken (const MPSToken* token) const
{
    MPSTokensContainer::const_iterator pos;
    pos = std::find(m_sub_tokens.begin(), m_sub_tokens.end(), token);
    return (pos != m_sub_tokens.end());
}

const MPSToken* MPSToken::last_subtoken () const
{
    if (m_sub_tokens.empty())
        return 0;

    const MPSToken* last = 0;
    MPSTokensContainer::const_iterator iter = m_sub_tokens.begin();
    for ( ; iter != m_sub_tokens.end();) {
        last = (*iter);
        ++iter;
    }
    return last;
}

const MPSToken* MPSToken::find_first_leaf_subtoken (bool include_discarded) const
{
    return find_first_leaf_subtoken(this, include_discarded);
}

const MPSToken* MPSToken::find_last_leaf_subtoken (bool include_discarded) const
{
    return find_last_leaf_subtoken(this, include_discarded);
}

const MPSToken* MPSToken::find_last_leaf_subtoken (const MPSToken* token, bool include_discarded) const
{
    if (!token)
        return 0;

    if (token->sub_tokens_empty())
        return token;

    const MPSToken* last = 0;
    MPSTokensContainer::const_iterator iter = token->sub_tokens_const_begin();
    for ( ; iter != token->sub_tokens_const_end(); ) {

        //  find only tokens which are not discarded
        bool discard_cond = (include_discarded) || (!include_discarded && !(*iter)->is_discard());
        if (*iter != 0 && discard_cond) {
            last = *iter;
        }

        ++iter;

        //  was it the last subttoken?
        if (iter == token->sub_tokens_const_end()) {
            if (last != 0 && !last->sub_tokens_empty()) {
                last = find_last_leaf_subtoken(last, include_discarded);
            }
        }
    }
    return last;
}

const MPSToken* MPSToken::find_first_leaf_subtoken (const MPSToken* token, bool include_discarded) const
{
    if (!token)
        return 0;

    if (token->sub_tokens_empty())
        return token;

    const MPSToken* first = 0;

    //  find first token which is not discarded
    MPSTokensContainer::const_iterator iter = token->sub_tokens_const_begin();
    for ( ; iter != token->sub_tokens_const_end(); ++ iter) {
        bool discard_cond = (include_discarded) || (!include_discarded && !(*iter)->is_discard());
        if (*iter && discard_cond) {
            first = *iter;
            break;
        }
    }

    //  if it has subtokens, find first undiscarded token in that group
    if (iter != token->sub_tokens_const_end() &&
        *iter &&
        !(*iter)->sub_tokens_empty())
    {
        first = find_first_leaf_subtoken(*iter, include_discarded);
    }

    return first;
}
