#ifndef MPSENGINE_HPP
#define MPSENGINE_HPP

#include "MPSToken.hpp"
#include "MPSEngineObserver.hpp"
#include "MPSTransformsContainer.hpp"
#include <string>
#include <map>
#include <QVector>
#include <QString>

struct MPSFilesMapStruct {
    MPSToken*       root;
    std::wstring    rename_to;

    explicit MPSFilesMapStruct (MPSToken* token = 0, const std::wstring& rename = L"")
        : root (token), rename_to (rename) {}
};

typedef std::map<size_t, MPSFilesMapStruct> MPSFilesToRenameMap;

class MPSEngine : public MPSTransformsContainer
{
public:
    MPSEngine();
    ~MPSEngine();

    void set_default_separators (const std::wstring& separators);
    std::wstring default_separators () const { return m_default_separators; }

    void set_always_lowcase_extension (bool lowcase) { m_always_lowcase_extension = lowcase; }
    bool is_always_lowcase_extension () const { return m_always_lowcase_extension; }

    /*
     *  Tries to find the file_name in the internal map based on its hash value
     *  If it's found, it will select the stored root token
     *  If not, it will add an entry into the internal map and select the new root token
     *  Passing an empty string forces a deselect
     */
    void select_master_token (const std::wstring& file_name);

    const MPSToken* master_token () const { return m_master_token; }

    std::wstring current_rename_to () const { return m_rename_to; }

    //  subtoken methods
    void select_subtoken (MPSToken* token, bool updateOutput = true);

    const MPSToken* selected_subtoken () const { return m_selected_subtoken; }

    void update_selected_subtoken (
            const std::wstring& text,
            const std::wstring& separators,
            bool discard,
            bool force_update = false
    );

    //  shift selected subtoken
    void shift_selected_subtoken (EMPSDirection direction);

    //  upcase or lowcase first or all
    void change_case (bool upcase, bool only_first, bool recursive);

    //  insert text to left or right
    void insert_text (const std::wstring& text_to_insert, EMPSDirection direction);

    //  files map access
    bool has_rename_to (const std::wstring& file_name, std::wstring& rename_to); //  check if have rename to entry in map
    bool is_files_map_empty () const { return m_files_map.empty(); }
    bool has_files_to_rename () const;
    void clear_files_map ();

    //  remove strings
    void add_string_to_remove(const QString& rem_str) { m_strings_to_remove.push_back(rem_str); }
    const QString& get_string_to_remove_at (int idx) const { return m_strings_to_remove.at(idx); }
    int count_strings_to_remove() const { return m_strings_to_remove.count(); }
    void clear_strings_to_remove () { m_strings_to_remove.clear(); }
    std::wstring remove_strings_from_text (const std::wstring& text);

    //  renames one file
    bool rename_one (   const std::wstring& folder,
                        const std::wstring& src_file,
                        const std::wstring& dst_file,
                        bool update_map_entry = false) const;

    //  renames all files from the map
    void rename_all (const std::wstring& path) const;

    //  observer utils
    void add_observer (MPSEngineObserver* obs) { m_observers.push_back(obs); }
    void clear_observers () { m_observers.clear(); }

    //  from MPSEngineBase
    virtual void update (MPSToken* token);
    virtual bool is_token_current_root (const MPSToken* token) const;

private:    //  methods
    //  (re)constructs the rename to name for the current root token
    std::wstring reconstruct_output (MPSToken* token);

    //  in the engine base this method is public, making it private here is intended
    void change_case (MPSToken* token, bool upcase, bool only_first, bool recursive);

    void update_token (MPSToken*& token,
                       const std::wstring& text,
                       const std::wstring& separators,
                       bool discard,
                       bool force_update);

private:
    //  the renaming list
    MPSFilesToRenameMap     m_files_map;

    //  the next members are stored for convenience, they will always be filled in from the new / selected root token
    MPSToken*               m_master_token;           //  points to current root token from files map
    std::wstring            m_rename_to;

    //  strings to remove
    QVector<QString>        m_strings_to_remove;

    //  currently selected subtoken in the current root token
    MPSToken*               m_selected_subtoken;

    //  engine settings
    std::wstring            m_default_separators;
    bool                    m_always_lowcase_extension;

    //  state observers
    std::vector<MPSEngineObserver*> m_observers;
};

#endif // MPSENGINE_HPP
