#ifndef MPSACTIONDISCARD_HPP
#define MPSACTIONDISCARD_HPP

#include "MPSActionBase.hpp"
#include "MPSCommon.hpp"
#include "MPSEngineBase.hpp"

class MPSActionEnableDisable : public MPSActionBase
{
public:
    MPSActionEnableDisable(MPSEngineBase& engine_base, bool disable);
    virtual ~MPSActionEnableDisable();

    bool is_disable () const { return m_disable; }

    virtual std::string id () const { return KActionEnableDisableID; };
    virtual void operator() (MPSToken* token);
    virtual std::wstring to_string () const { return m_disable ? L"DISABLE" : L"ENABLE"; }

private:
    MPSEngineBase& m_engine_base;
    bool m_disable;
};

#endif // MPSACTIONDISCARD_HPP
