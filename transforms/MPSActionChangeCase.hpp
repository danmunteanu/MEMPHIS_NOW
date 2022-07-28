#ifndef MPSACTIONUPCASE_HPP
#define MPSACTIONUPCASE_HPP

#include "MPSActionBase.hpp"
#include "MPSCommon.hpp"

class MPSEngineBase;

class MPSActionChangeCase : public MPSActionBase
{
public:
    MPSActionChangeCase(MPSEngineBase& engine_base, bool upcase, bool all, bool recursive);
    ~MPSActionChangeCase ();

    virtual void operator() (MPSToken* token);
    virtual std::wstring to_string () const;
    virtual std::string id () const { return KActionChangeCaseID; }

    bool is_upcase () const { return m_upcase; }
    bool is_only_first () const { return m_only_first; }
    bool is_recursive () const { return m_recursive; }

private:
    MPSEngineBase&  m_engine_base;
    bool            m_upcase;
    bool            m_only_first;
    bool            m_recursive;
};

#endif // MPSACTIONUPCASE_HPP
