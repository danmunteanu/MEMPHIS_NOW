#ifndef MPSACTIONSETSEPARATORS_HPP
#define MPSACTIONSETSEPARATORS_HPP

#include "MPSEngineBase.hpp"
#include "transforms/MPSActionBase.hpp"
#include "MPSCommon.hpp"
#include <string>

class MPSEngineBase;

class MPSActionSetSeparators : public MPSActionBase
{
public:
    MPSActionSetSeparators (MPSEngineBase& engine_base, const std::wstring& separators);
    ~MPSActionSetSeparators();

    virtual void operator() (MPSToken* token);
    virtual std::wstring to_string () const { return L"SET SEPARATORS=\"" + m_separators + L"\""; }
    virtual std::string id () const { return KActionSetSeparatorsID; }

    void set_separators (const std::wstring& separators) { m_separators = separators; }
    std::wstring separators () const { return m_separators; }

private:
    MPSEngineBase&  m_engine_base;
    std::wstring    m_separators;
};

#endif // MPSACTIONSETSEPARATORS_HPP
