#ifndef MPSCONDITIONISNUMERIC_HPP
#define MPSCONDITIONISNUMERIC_HPP

#include "MPSConditionBase.hpp"
#include "MPSCommon.hpp"

class MPSConditionIsNumeric : public MPSConditionBase
{
public:
    MPSConditionIsNumeric();
    virtual ~MPSConditionIsNumeric ();

    virtual bool operator () (const MPSToken* token);
    virtual std::string id () const { return KConditionIsNumericID; }
    virtual std::wstring to_string () const;
};

#endif // MPSCONDITIONISNUMERIC_HPP
