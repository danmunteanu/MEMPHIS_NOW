#include "MPSConditionIsRoot.hpp"
#include "MPSEngineBase.hpp"

MPSConditionIsRoot::MPSConditionIsRoot(const MPSEngineBase& engine)
    : m_engine (engine)
{
}

MPSConditionIsRoot::~MPSConditionIsRoot ()
{
}

bool MPSConditionIsRoot::operator() (const MPSToken* token)
{
    return (!token) ? false : m_engine.is_token_current_root(token);
}

std::wstring MPSConditionIsRoot::to_string () const
{
    return L"IS ROOT";
}
