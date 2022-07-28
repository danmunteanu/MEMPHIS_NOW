#ifndef MPSTRANSFORMS_HPP
#define MPSTRANSFORMS_HPP

#include <string>

class MPSToken;

class MPSActionBase
{
public:
    virtual ~MPSActionBase ();

    virtual void operator() (MPSToken* token) = 0;
    virtual std::wstring to_string () const = 0;
    virtual std::string id () const = 0;    
};

#endif // MPSTRANSFORMS_HPP
