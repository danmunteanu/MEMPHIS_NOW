#ifndef MPSTRANSFORMINFO_HPP
#define MPSTRANSFORMINFO_HPP

#include <string>

struct MPSTransformInfo
{
    int             condition_index;
    std::wstring    condition_text;
    bool            condition_checks[2];

    int             action_index;
    std::wstring    action_texts[2];
    bool            action_checks[4];
    int             action_position;

    bool            leafs_only;
    bool            enabled;
    std::wstring    description;

    MPSTransformInfo ();
    void reset ();
};

#endif // MPSTRANSFORMINFO_HPP
