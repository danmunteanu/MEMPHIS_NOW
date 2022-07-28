#ifndef MPSTRANSFORMSCONTAINER_HPP
#define MPSTRANSFORMSCONTAINER_HPP

#include <string>
#include "MPSEngineBase.hpp"
#include "transforms/MPSTransform.hpp"
#include "transforms/MPSTransformInfo.hpp"
#include "boost/tuple/tuple.hpp"

class MPSTransformsContainer;

//  condition defs
typedef MPSConditionBase* (MPSTransformsContainer::*MPSConditionCreator) (const MPSTransformInfo& info);
typedef boost::tuples::tuple<std::string, int, MPSConditionCreator> MPSConditionInfoTuple;
typedef std::vector<MPSConditionInfoTuple> MPSAvailableConditions;

//  action defs
typedef MPSActionBase* (MPSTransformsContainer::*MPSActionCreator) (const MPSTransformInfo& info);
typedef boost::tuples::tuple<std::string, int, MPSActionCreator> MPSActionInfoTuple;
typedef std::vector<MPSActionInfoTuple> MPSAvailableActions;

class MPSTransformsContainer : public MPSEngineBase
{
public:
    MPSTransformsContainer();
    ~MPSTransformsContainer();

    void set_apply_transforms (bool apply) { m_apply_transforms = apply; }
    bool is_apply_transforms () const { return m_apply_transforms; }

    int count_available_conditions () const;

    //  returns condtion name or empty string if index is invalid
    std::string get_available_condition_name_at (int idx) const;

    // returns index for condition id or -1 if condition id is not stored in available conditions
    int get_available_condition_index (const std::string& id) const;

    int count_available_actions () const;

    //  gets the action name available at index idx or empty string if not found
    std::string get_available_action_name_at (int idx) const;

    //  gets the index for the specified action or -1 if action id is not stored in available actions
    int get_available_action_index (const std::string& id) const;


    //  transforms
    void create_and_add_transform (const MPSTransformInfo& info);
    int count_transforms () const { return m_transforms.size(); }
    std::wstring get_transform_description_at (int idx) const;
    MPSTransformInfo get_transform_information_at (int idx) const;
    void update_transform_information_at (int idx, const MPSTransformInfo& info);
    void toggle_transform_enabled_at (int idx);
    void move_up_transform_at (int idx);
    void move_down_transform_at (int idx);
    void remove_transform_at (int idx);
    void clear_transforms ();

    void apply_transforms (MPSToken* token);

protected:
    //  create conditions
    MPSConditionBase*   create_condition_true (const MPSTransformInfo& info);
    MPSConditionBase*   create_condition_equals (const MPSTransformInfo& info);
    MPSConditionBase*   create_condition_is_root (const MPSTransformInfo& info);
    MPSConditionBase*   create_condition_is_numeric (const MPSTransformInfo&);

    //  create actions
    MPSActionBase*      create_action_enable_disable (const MPSTransformInfo& info);
    MPSActionBase*      create_action_set_separators (const MPSTransformInfo& info);
    MPSActionBase*      create_action_change_case (const MPSTransformInfo& info);
    MPSActionBase*      create_action_set_text (const MPSTransformInfo& info);
    MPSActionBase*      create_action_replace_in_text (const MPSTransformInfo& info);
    MPSActionBase*      create_action_insert_subtoken (const MPSTransformInfo& info);

private:
    //  transforms info
    MPSAvailableConditions  m_avail_cond_info;
    MPSAvailableActions     m_avail_act_info;
    MPSTransformsHolder     m_transforms;
    bool                    m_apply_transforms;
};

#endif // MPSTRANSFORMSCONTAINER_HPP
