#include "MPSConditionIsNumeric.hpp"
#include "MPSToken.hpp"
#include <boost/lexical_cast.hpp>

MPSConditionIsNumeric::MPSConditionIsNumeric()
{
}

MPSConditionIsNumeric::~MPSConditionIsNumeric ()
{
}

bool MPSConditionIsNumeric::operator () (const MPSToken* token)
{
    if (!token)
        return false;

    if (token->text().empty())
        return false;

    try {
        boost::lexical_cast<int> (token->text ());
    } catch (const boost::bad_lexical_cast&) {
        return false;
    }
    return true;
}

std::wstring MPSConditionIsNumeric::to_string () const
{
    return L"Text is numeric";
}
