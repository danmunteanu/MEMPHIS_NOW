#include "MPSActionEnableDisable.hpp"
#include "MPSToken.hpp"

MPSActionEnableDisable::MPSActionEnableDisable(MPSEngineBase& engine_base, bool disable)
    : m_engine_base (engine_base), m_disable (disable)
{
}

MPSActionEnableDisable::~MPSActionEnableDisable()
{
}

void MPSActionEnableDisable::operator() (MPSToken* token)
{
    if (!token)
        return;

    if (!m_engine_base.is_token_current_root(token))
        token->set_discard (m_disable);
}
