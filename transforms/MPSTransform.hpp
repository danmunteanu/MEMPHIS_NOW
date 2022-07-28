#ifndef MPSTRANSFORM_HPP
#define MPSTRANSFORM_HPP

#include <vector>
#include <string>

class MPSConditionBase;
class MPSActionBase;
class MPSToken;

class MPSTransform
{
public:
    MPSTransform(MPSConditionBase*  condition,
                 MPSActionBase*     action,
                 bool leafs_only = true,
                 bool enabled = true,
                 const std::wstring& desc = L"");
    ~MPSTransform ();

    //  apply the transform on a token
    void operator () (MPSToken* token);

    void set_leafs_only (bool leafs_only) { m_leafs_only = leafs_only; }
    bool is_leafs_only () const { return m_leafs_only; }

    void set_enabled (bool enabled) { m_enabled = enabled; }
    bool is_enabled () const { return m_enabled; }

    void set_description (const std::wstring& description) { m_description = description; }
    std::wstring description () const { return m_description; }

    const MPSConditionBase* condition () const { return m_condition; }
    const MPSActionBase* action() const { return m_action; }

    //  string description
    std::wstring to_string () const;

private:
    MPSConditionBase* 	m_condition;
    MPSActionBase* 		m_action;

    bool				m_leafs_only;
    bool                m_enabled;

    std::wstring        m_description;
};

typedef std::vector<MPSTransform*> MPSTransformsHolder;

#endif // MPSTRANSFORM_HPP
