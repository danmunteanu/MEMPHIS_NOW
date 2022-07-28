#ifndef MPSHEADER_HPP
#define MPSHEADER_HPP

#include <boost/tokenizer.hpp>
#include <string>
#include <list>

typedef boost::char_separator<wchar_t> BoostSeparator;
typedef boost::tokenizer<BoostSeparator, std::wstring::const_iterator, std::wstring> BoostTokenizer;

class MPSToken;
typedef std::list<MPSToken*> MPSTokensContainer;

enum EMPSDirection {
    ELeft,
    ERight
};

class MPSToken {

public:
    MPSToken (	MPSToken* parent,
                const std::wstring& text = L"",
                const std::wstring& separators = L"",
                bool discard = false ) :
        m_parent (parent),
        m_text (text),
        m_separators (separators),
        m_discard (discard)
    {}

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
    MPSTokensContainer::const_iterator sub_tokens_const_begin () const { return m_sub_tokens.begin(); }
    MPSTokensContainer::const_iterator sub_tokens_const_end () const { return m_sub_tokens.end(); }
    MPSTokensContainer::iterator sub_tokens_begin () { return m_sub_tokens.begin(); }
    MPSTokensContainer::iterator sub_tokens_end () { return m_sub_tokens.end(); }
    bool sub_tokens_empty () const { return m_sub_tokens.empty(); }
    size_t count_subtokens () const { return m_sub_tokens.size(); }
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

    //  counts the number of siblings left or right
    int count_siblings (EMPSDirection direction) const;

    //  splits the root token into subtokens based on the current separators
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
    MPSTokensContainer      m_sub_tokens;		//	result after split into tokens using current separators
};

#endif // MPSHEADER_HPP
