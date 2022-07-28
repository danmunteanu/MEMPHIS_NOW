#ifndef MPSSETTOKEN_HPP
#define MPSSETTOKEN_HPP

#include "MPSActionBase.hpp"
#include "MPSCommon.hpp"

class MPSEngineBase;

class MPSActionSetText : public MPSActionBase
{
public:
    MPSActionSetText(MPSEngineBase& engine_base, const std::wstring& text);
    virtual ~MPSActionSetText ();

    virtual void operator() (MPSToken* token);
    virtual std::wstring to_string () const;
    virtual std::string id () const { return KActionSetTextID; }

    std::wstring text() const { return m_text; }

private:
    MPSEngineBase&      m_engine_base;
    std::wstring        m_text;
};

#endif // MPSSETTOKEN_HPP
