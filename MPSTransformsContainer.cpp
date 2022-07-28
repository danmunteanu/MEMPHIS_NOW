#include "MPSTransformsContainer.hpp"
#include "transforms/MPSConditionAlways.hpp"
#include "transforms/MPSConditionEquals.hpp"
#include "transforms/MPSConditionIsRoot.hpp"
#include "transforms/MPSConditionIsNumeric.hpp"
#include "transforms/MPSActionEnableDisable.hpp"
#include "transforms/MPSActionSetSeparators.hpp"
#include "transforms/MPSActionChangeCase.hpp"
#include "transforms/MPSActionSetText.hpp"
#include "transforms/MPSActionInsertSubtoken.hpp"
#include "transforms/MPSActionReplaceInText.hpp"

using namespace boost::tuples;

struct MPSDeleteTransform {
    void operator () (MPSTransform*& transform)
    {
        delete transform;
        transform = 0;
    }
};

MPSTransformsContainer::MPSTransformsContainer()
    : m_apply_transforms (true)
{
    //  build conditions vector
    m_avail_cond_info.push_back(MPSAvailableConditions::value_type(KConditionAlwaysID,  0,  &MPSTransformsContainer::create_condition_true));
    m_avail_cond_info.push_back(MPSAvailableConditions::value_type(KConditionEqualsID,  1,  &MPSTransformsContainer::create_condition_equals));
    m_avail_cond_info.push_back(MPSAvailableConditions::value_type(KConditionIsRootID,  2,  &MPSTransformsContainer::create_condition_is_root));
    m_avail_cond_info.push_back(MPSAvailableConditions::value_type(KConditionIsNumericID, 3,  &MPSTransformsContainer::create_condition_is_numeric));

    //  build actions vector
    m_avail_act_info.push_back(MPSAvailableActions::value_type(KActionEnableDisableID,  0,  &MPSTransformsContainer::create_action_enable_disable));
    m_avail_act_info.push_back(MPSAvailableActions::value_type(KActionChangeCaseID,     1,  &MPSTransformsContainer::create_action_change_case));
    m_avail_act_info.push_back(MPSAvailableActions::value_type(KActionSetTextID,        2,  &MPSTransformsContainer::create_action_set_text));
    m_avail_act_info.push_back(MPSAvailableActions::value_type(KActionSetSeparatorsID,  3,  &MPSTransformsContainer::create_action_set_separators));
    m_avail_act_info.push_back(MPSAvailableActions::value_type(KActionInsertSubtokenID, 4,  &MPSTransformsContainer::create_action_insert_subtoken));
    m_avail_act_info.push_back(MPSAvailableActions::value_type(KActionReplaceInTextID,  5,  &MPSTransformsContainer::create_action_replace_in_text));
}

MPSTransformsContainer::~MPSTransformsContainer()
{
    clear_transforms();
}

//  access to conditions
int MPSTransformsContainer::count_available_conditions () const
{
    return m_avail_cond_info.size();
}

std::string MPSTransformsContainer::get_available_condition_name_at (int idx) const
{
    if (idx < 0 || idx >= static_cast<int> (m_avail_cond_info.size()))
        return "";

    return get<0> (m_avail_cond_info.at(idx));
}

int MPSTransformsContainer::get_available_condition_index (const std::string& id) const
{
    MPSAvailableConditions::const_iterator iter;
    for (iter = m_avail_cond_info.begin(); iter != m_avail_cond_info.end(); ++iter) {
        if (get<0> (*iter) == id)
            break;
    }
    return (iter != m_avail_cond_info.end()) ? get<1> (*iter) : -1;
}

//  access to actions
int MPSTransformsContainer::count_available_actions () const
{
    return m_avail_act_info.size();
}

std::string MPSTransformsContainer::get_available_action_name_at (int idx) const
{
    if (idx < 0 || idx >= static_cast<int> (m_avail_act_info.size()))
        return "";

    return get<0> (m_avail_act_info.at(idx));
}

int MPSTransformsContainer::get_available_action_index (const std::string& id) const
{
    MPSAvailableActions::const_iterator iter;
    for (iter = m_avail_act_info.begin(); iter != m_avail_act_info.end(); ++iter) {
        if (get<0> (*iter) == id)
            break;
    }
    return (iter != m_avail_act_info.end()) ? get<1> (*iter) : -1;
}

//  create transforms
void MPSTransformsContainer::create_and_add_transform (const MPSTransformInfo& info)
{
    MPSConditionBase* condition = ((*this).*(get<2> (m_avail_cond_info.at(info.condition_index)))) (info);
    MPSActionBase* action = ((*this).*(get<2> (m_avail_act_info.at(info.action_index)))) (info);

    m_transforms.push_back(new MPSTransform(condition, action, info.leafs_only, info.enabled, info.description));
}

std::wstring MPSTransformsContainer::get_transform_description_at (int idx) const
{
    return m_transforms.at(idx)->to_string ();
}

MPSTransformInfo MPSTransformsContainer::get_transform_information_at (int idx) const
{
    MPSTransformInfo info;

    MPSTransform* trans = m_transforms.at (idx);
    if (!trans || !trans->condition() || !trans->action())
        return info;

    //  CONDITIONS
    info.condition_index = get_available_condition_index(trans->condition()->id());

    //  check if we need to get cond equals info
    if (trans->condition()->id() == KConditionEqualsID) {
        const MPSConditionEquals* cond_eq = dynamic_cast<const MPSConditionEquals*> (trans->condition());
        info.condition_text = cond_eq->text();        
        info.condition_checks[0] = cond_eq->is_case_sensitive();        
    }

    //  ACTIONS
    //  load actions info
    info.action_index = get_available_action_index(trans->action()->id());

    //  check if we need to save the text or other information
    if (trans->action()->id() == KActionSetSeparatorsID) {
        info.action_texts[0] = (dynamic_cast<const MPSActionSetSeparators*> (trans->action()))->separators();
    }

    if (trans->action()->id() == KActionSetTextID) {
        info.action_texts[0] = (dynamic_cast<const MPSActionSetText*> (trans->action()))->text();
    }

    //  do we need to fill in the checkboxes?
    if (trans->action()->id() == KActionChangeCaseID) {
        const MPSActionChangeCase* act_case = dynamic_cast<const MPSActionChangeCase*> (trans->action());
        info.action_checks[0] = act_case->is_upcase();
        info.action_checks[1] = !act_case->is_only_first();
        info.action_checks[2] = act_case->is_recursive ();
    }

    //  discard checkbox?
    if (trans->action()->id() == KActionEnableDisableID) {
        const MPSActionEnableDisable* act_ena_dis = dynamic_cast<const MPSActionEnableDisable*> (trans->action());
        info.action_checks[3] = act_ena_dis->is_disable();
    }

    //  get the text for insert subtoken
    if (trans->action()->id() == KActionInsertSubtokenID) {
        const MPSActionInsertSubtoken* act_insert = dynamic_cast<const MPSActionInsertSubtoken*> (trans->action());
        info.action_texts[0] = act_insert->text_to_add();
        info.action_position = act_insert->position();
    }

    //  get the text for replace in text
    if (trans->action()->id() == KActionReplaceInTextID) {
        const MPSActionReplaceInText* act_replace = dynamic_cast<const MPSActionReplaceInText*> (trans->action());
        info.action_texts[0] = act_replace->search_string();
        info.action_texts[1] = act_replace->replace_string();
    }

    //  description
    info.description = trans->description();

    //  leafs only?
    info.leafs_only = trans->is_leafs_only();

    //  enabled?
    info.enabled = trans->is_enabled();

    return info;
}

void MPSTransformsContainer::update_transform_information_at (int idx, const MPSTransformInfo& info)
{
    if (idx == -1)
        return;

    //  remove previous entry
    delete m_transforms[idx];
    m_transforms[idx] = 0;

    MPSConditionBase* condition = ((*this).*(get<2> (m_avail_cond_info.at(info.condition_index)))) (info);
    MPSActionBase* action = ((*this).*(get<2> (m_avail_act_info.at(info.action_index)))) (info);
    m_transforms[idx] = new MPSTransform(condition, action, info.leafs_only, info.enabled, info.description);
}

void MPSTransformsContainer::toggle_transform_enabled_at (int idx)
{
    m_transforms[idx]->set_enabled (!m_transforms[idx]->is_enabled ());
}

void MPSTransformsContainer::move_up_transform_at (int idx)
{
    if (idx < 0 || static_cast<size_t> (idx) >= m_transforms.size())
        return;

    MPSTransformsHolder::iterator iter = m_transforms.begin();
    MPSTransformsHolder::iterator prev = iter;
    int pos = 0;

    for (;;) {
        if (iter == m_transforms.end())
            break;

        if (pos == idx)
            break;

        prev = iter;
        ++iter;
        ++pos;
    }

    if (iter != m_transforms.end() && pos != 0) {
        std::swap (*prev, *iter);
    }
}

void MPSTransformsContainer::move_down_transform_at (int idx)
{
    if (idx < 0 || static_cast <size_t> (idx) >= m_transforms.size())
        return;

    MPSTransformsHolder::iterator iter = m_transforms.begin();
    MPSTransformsHolder::iterator next = m_transforms.begin();
    ++next;
    int pos = 0;

    for (;;) {
        if (iter == m_transforms.end())
            break;

        if (pos == idx)
            break;

        //  advance
        ++pos;
        ++iter;

        if (iter != m_transforms.end()) {
            //  get next position
            next = iter;
            ++next;
        }
    }

    if (next != m_transforms.end()) {
        std::swap(*iter, *next);
    }
}

void MPSTransformsContainer::remove_transform_at (int idx)
{
    MPSTransformsHolder::iterator iter = m_transforms.begin () + idx;
    delete (*iter);
    m_transforms.erase(iter);
}

void MPSTransformsContainer::clear_transforms ()
{
    MPSDeleteTransform delete_transform;
    std::for_each(m_transforms.begin(), m_transforms.end(), delete_transform);
    m_transforms.clear();
}

MPSConditionBase*   MPSTransformsContainer::create_condition_true (const MPSTransformInfo&)
{
    return new MPSConditionAlways;
}

MPSConditionBase*   MPSTransformsContainer::create_condition_equals (const MPSTransformInfo& info)
{
    return new MPSConditionEquals (*this, info.condition_text, info.condition_checks[0]);
}

MPSConditionBase*   MPSTransformsContainer::create_condition_is_root (const MPSTransformInfo&)
{
    return new MPSConditionIsRoot (*this);
}

MPSConditionBase*   MPSTransformsContainer::create_condition_is_numeric (const MPSTransformInfo&)
{
    return new MPSConditionIsNumeric;
}

MPSActionBase*      MPSTransformsContainer::create_action_enable_disable (const MPSTransformInfo& info)
{
    return new MPSActionEnableDisable (*this, info.action_checks[3]);
}

MPSActionBase*      MPSTransformsContainer::create_action_set_separators (const MPSTransformInfo& info)
{
    return new MPSActionSetSeparators (*this, info.action_texts[0]);
}

MPSActionBase*      MPSTransformsContainer::create_action_change_case (const MPSTransformInfo& info)
{
    return new MPSActionChangeCase (*this, info.action_checks[0], info.action_checks[1], info.action_checks[2]);
}

MPSActionBase*      MPSTransformsContainer::create_action_set_text (const MPSTransformInfo& info)
{
    return new MPSActionSetText (*this, info.action_texts[0]);
}

MPSActionBase*      MPSTransformsContainer::create_action_replace_in_text (const MPSTransformInfo& info)
{
    return new MPSActionReplaceInText (*this, info.action_texts[0], info.action_texts[1]);
}

MPSActionBase*      MPSTransformsContainer::create_action_insert_subtoken (const MPSTransformInfo& info)
{
    return new MPSActionInsertSubtoken (info.action_texts[0], info.action_position);
}

void MPSTransformsContainer::apply_transforms (MPSToken* token)
{
    if (!token)
        return;

    //  apply transforms on current token
    MPSTransformsHolder::iterator trans_iter = m_transforms.begin();
    for ( ; trans_iter != m_transforms.end(); ++trans_iter) {
        MPSTransform* trans = (*trans_iter);
        if (trans && trans->is_enabled())
            (*trans) (token);
    }

    //  apply on subtokens
    if (!token->sub_tokens_empty()) {
        MPSTokensContainer::iterator iter = token->sub_tokens_begin();
        for ( ; iter != token->sub_tokens_end(); ++iter) {
            apply_transforms (*iter);
        }
    }
}
