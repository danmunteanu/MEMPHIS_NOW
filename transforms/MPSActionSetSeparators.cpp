#include "MPSActionSetSeparators.hpp"
#include <MPSToken.hpp>

MPSActionSetSeparators::MPSActionSetSeparators(MPSEngineBase& engine_base, const std::wstring& separators) :
    m_engine_base (engine_base),
    m_separators (separators)
{
}

MPSActionSetSeparators::~MPSActionSetSeparators()
{
}

void MPSActionSetSeparators::operator() (MPSToken* token)
{
    if (!token)
        return;

    token->set_separators(m_separators);

    m_engine_base.update(token);
}
