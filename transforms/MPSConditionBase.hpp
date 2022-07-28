#ifndef MPSCONDITIONBASE_HPP
#define MPSCONDITIONBASE_HPP

#include <string>

class MPSToken;

//  abstract base class for all conditions
class MPSConditionBase
{
public:
    virtual ~MPSConditionBase ();
    virtual bool operator() (const MPSToken* token) = 0;
    virtual std::string id () const = 0;
    virtual std::wstring to_string () const = 0;
};

#endif // MPSCONDITIONBASE_HPP
