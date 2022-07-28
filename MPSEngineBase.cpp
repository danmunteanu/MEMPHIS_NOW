#include "MPSEngineBase.hpp"

MPSEngineBase::~MPSEngineBase()
{
}

void MPSEngineBase::push_message (const std::wstring& msg)
{
    m_messages.push(msg);
}

bool MPSEngineBase::has_messages () const
{
    return !m_messages.empty();
}

std::wstring MPSEngineBase::pop_message ()
{
    std::wstring msg = m_messages.front();
    m_messages.pop();
    return msg;
}

void MPSEngineBase::clear_messages ()
{
    while (!m_messages.empty())
        m_messages.pop();
}
