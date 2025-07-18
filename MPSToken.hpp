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

#ifndef MPSHEADER_HPP
#define MPSHEADER_HPP

#include <boost/tokenizer.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <list>

typedef boost::char_separator<wchar_t> BoostSeparator;
typedef boost::tokenizer<BoostSeparator, std::wstring::const_iterator, std::wstring> BoostTokenizer;

class MPSToken;
typedef std::list<MPSToken*> MPSTokensContainer;

typedef std::list<boost::shared_ptr<MPSToken> > MPSTokensSharedContainer;

enum EMPSDirection {
    ELeft,
    ERight
};

class MPSToken {

public:
    explicit MPSToken(MPSToken* parent = NULL,
        const std::wstring& text = L"",
        const std::wstring& separators = L"",
        bool discard = false);

    ~MPSToken ();

    /* Will set parent only if this is the list of the new parent's subtokens
     * Check the pointer returned by parent () to be sure it has been set */
    void set_parent (MPSToken* parent);
    MPSToken* parent () const { return m_parent; }

    void set_text (const std::wstring& text);
    std::wstring text () const { return m_text; }

    void set_separators (const std::wstring& separ);
    std::wstring separators () const { return m_separators; }

    void set_discard (bool discard);
    bool is_discard () const { return m_discard; }

    //  access to subtokens
    MPSTokensContainer::const_iterator subtokens_const_begin () const { return m_subtokens.begin(); }
    MPSTokensContainer::const_iterator subtokens_const_end () const { return m_subtokens.end(); }
    MPSTokensContainer::iterator sub_tokens_begin () { return m_subtokens.begin(); }
    MPSTokensContainer::iterator sub_tokens_end () { return m_subtokens.end(); }
    bool sub_tokens_empty () const { return m_subtokens.empty(); }
    size_t count_subtokens () const { return m_subtokens.size(); }
    void clear_subtokens ();

    /*  checks if token is in the list of subtokens */
    bool is_subtoken (const MPSToken* token) const;

    /* finds last subtoken, regardless if it's disabled or not*/
    const MPSToken* last_subtoken () const;

    const MPSToken* find_first_leaf_subtoken (bool include_discarded) const;
    const MPSToken* find_last_leaf_subtoken (bool include_discarded) const;

    /*  creates and inserts a new subtoken at the specified position
     *  if pos is after the maximum possible position, it will insert on the last position
     *  similarly, if pos is equal to KLastSubtokenPosition, it will insert on the last position
     */
    void insert_subtoken (const std::wstring& text, size_t pos);

    /*  shifts the specified subtoken to the desired position - left or right
     *  does nothing if it is null
     *  if sub_token is leftmost, does nothing
     *  if sub_token is rightmost, does nothing
     */
    void shift_subtoken (MPSToken* sub_token, EMPSDirection direction);

    //  splits this token into subtokens based on current separators
    void split ();

private:
    static void cleanup_token (MPSToken& token);
    static void discard_token (MPSToken& token, bool discard);

private:
    const MPSToken* find_first_leaf_subtoken (const MPSToken* token, bool include_discarded) const;
    const MPSToken* find_last_leaf_subtoken (const MPSToken* token, bool include_discarded) const;

private:
    MPSToken*               m_parent;           //  who does this subtoken belong to; this is NULL if current token is root
    std::wstring            m_text;             //	original text
    std::wstring            m_separators;       //	current separators
    bool                    m_discard;			//	flag to indicate if we're discarding this one or not
    MPSTokensContainer      m_subtokens;		//	result after split into tokens using current separators
	MPSTokensSharedContainer m_subtokens_shared; //  shared pointers to subtokens, if needed
};

#endif // MPSHEADER_HPP
