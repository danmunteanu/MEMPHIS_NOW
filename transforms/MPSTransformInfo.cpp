#include "MPSTransformInfo.hpp"

MPSTransformInfo::MPSTransformInfo () :
    condition_index (-1),
    condition_text (L""),
    action_index (-1),
    action_position (0),
    leafs_only (true),
    enabled (true)
{
    condition_checks[0] = false;
    condition_checks[1] = false;
    action_checks[0] = false;
    action_checks[1] = false;
    action_checks[2] = false;
}

void MPSTransformInfo::reset () {
    condition_index = -1;
    condition_text.clear();
    condition_checks[0] = false;
    condition_checks[1] = false;

    action_index = -1;
    action_texts[0].clear();
    action_texts[1].clear();
    action_checks[0] = false;
    action_checks[1] = false;
    action_checks[2] = false;
    action_checks[3] = false;
    action_position = 0;

    leafs_only = true;
    enabled = true;
    description.clear();
}
