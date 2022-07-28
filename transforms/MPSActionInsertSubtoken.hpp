#ifndef MPSACTIONINSERTSUBTOKEN_HPP
#define MPSACTIONINSERTSUBTOKEN_HPP

#include <string>
#include "MPSActionBase.hpp"
#include "MPSCommon.hpp"

class MPSActionInsertSubtoken : public MPSActionBase
{
public:
    MPSActionInsertSubtoken(const std::wstring& text, int pos);
    virtual ~MPSActionInsertSubtoken ();

    virtual void operator() (MPSToken* token);
    virtual std::wstring to_string () const;
    virtual std::string id () const { return KActionInsertSubtokenID; }

    std::wstring text_to_add () const { return m_text_to_add; }
    int position () const { return m_position; }

private:
    std::wstring    m_text_to_add;
    int             m_position;
};

#endif // MPSACTIONINSERTSUBTOKEN_HPP
