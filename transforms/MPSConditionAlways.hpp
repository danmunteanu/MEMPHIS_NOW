#ifndef MPSCONDITIONALWAYS_HPP
#define MPSCONDITIONALWAYS_HPP

#include "MPSConditionBase.hpp"
#include "MPSCommon.hpp"

class MPSConditionAlways : public MPSConditionBase
{
public:
    MPSConditionAlways();
    virtual ~MPSConditionAlways();

    virtual bool operator () (const MPSToken*) { return true; }
    virtual std::string id () const { return KConditionAlwaysID; }
    virtual std::wstring to_string () const { return L"ALWAYS TRUE"; }
};

#endif // MPSCONDITIONALWAYS_HPP
