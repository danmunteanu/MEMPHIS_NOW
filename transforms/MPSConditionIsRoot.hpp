#ifndef MPSCONDITIONISROOT_HPP
#define MPSCONDITIONISROOT_HPP

#include "MPSConditionBase.hpp"
#include "MPSCommon.hpp"

class MPSEngineBase;

class MPSConditionIsRoot : public MPSConditionBase
{
public:
    MPSConditionIsRoot(const MPSEngineBase& engine);
    virtual ~MPSConditionIsRoot ();

    virtual bool operator() (const MPSToken* token);
    virtual std::string id () const { return KConditionIsRootID; }
    virtual std::wstring to_string () const;

private:
    const MPSEngineBase&    m_engine;
};

#endif // MPSCONDITIONISROOT_HPP
