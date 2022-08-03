#include "MPSEngine.hpp"
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "transforms/MPSActionChangeCase.hpp"

/*  Utility Structures */
struct MPSNotify
{
    void operator () (MPSEngineObserver* obs)
    {
        obs->notify();
    }
};
/*  End Utility Structures */

MPSEngine::MPSEngine()
    : m_master_token (0),
      m_rename_to (L""),

      //    selection related
      m_selected_subtoken (0),

      //    settings
      m_default_separators (L""),
      m_always_lowcase_extension (false)
{
}

MPSEngine::~MPSEngine()
{
}

void MPSEngine::set_default_separators (const std::wstring& separators)
{
    m_default_separators = separators;
}

std::wstring MPSEngine::remove_strings_from_text (const std::wstring& text)
{
    //  result
    std::wstring res = text;
    
    //  in case you want to repace with something other than empty string
    std::wstring to = L"";

    for (int idx = 0; idx < m_strings_to_remove.count(); ++idx) {
        QString from = m_strings_to_remove.at(idx);
        if (from.count() == 0)
            return res;

        size_t start_pos = 0;
        while((start_pos = res.find(from.toStdWString(), start_pos)) != std::string::npos) {
            res.replace(start_pos, from.count(), to);

            // In case 'to' contains 'from', like replacing 'x' with 'yx'
            start_pos += to.length();
        }

    }
    
    return res;
}


void MPSEngine::select_master_token (const std::wstring& file_name)
{
    //  an empty file name forces a deselect
    if (file_name.empty()) {
        m_master_token = 0;
        m_selected_subtoken = 0;
        m_rename_to.clear();

        return;
    }

    //  compute the hash for the file name
    boost::hash<std::wstring> hasher;
    size_t hash = hasher (file_name);

    MPSFilesToRenameMap::const_iterator iter = m_files_map.find (hash);
    if (iter != m_files_map.end()) {
        //  use data from the internal map
        m_master_token = iter->second.root;
        m_rename_to = iter->second.rename_to;
    } else {

        //  create new entry and add it into the internal map
        m_master_token = new MPSToken (NULL, file_name, m_default_separators, false);

        //  remove list of expressions
        std::wstring fn_clean = remove_strings_from_text(file_name);

        if (fn_clean != file_name) {
            //  create one subtoken and split that
            m_master_token->insert_subtoken(fn_clean, 0);

            //  get the first subtoken
            MPSToken* sub = *m_master_token->sub_tokens_begin();
            if (sub != 0) {
                //  must split the first subtoken, not the master token
                sub->set_separators(m_default_separators);
                sub->split();

                //  apply transforms
                if (is_apply_transforms())
                    apply_transforms (sub);
            }
        } else {
            m_master_token->split();

            //  apply transforms
            if (is_apply_transforms())
                apply_transforms (m_master_token);
        }

        //  reconstruct the output
        m_rename_to = reconstruct_output (m_master_token);

        //  add the entry        
        m_files_map.insert (MPSFilesToRenameMap::value_type (hash, MPSFilesMapStruct (m_master_token, m_rename_to)));
    }
    m_selected_subtoken = m_master_token;
}

void MPSEngine::select_subtoken (MPSToken* token, bool updateOutput)
{
    if (!token)
        return;

    m_selected_subtoken = token;
	
    if (updateOutput) {
        //  reconstruct the output
        m_rename_to = reconstruct_output (m_master_token);

    }

    //  notify observers that selected token has changed
    std::for_each (m_observers.begin(), m_observers.end(), MPSNotify ());
}

void MPSEngine::update_token (MPSToken*& token,
                              const std::wstring& text,
                              const std::wstring& separators,
                              bool discard,
                              bool force_update)
{
    if (!token)
        return;
		
    if (token->text() != text || token->separators() != separators || force_update) {
        MPSToken* parent = token->parent ();

        token->clear_subtokens ();
        token->set_parent (parent);
        token->set_text (text);
        token->set_separators (separators);
        token->set_discard (discard);
        token->split();
    } else if (token->is_discard() != discard || force_update) {
        if (token != m_master_token)    //  disallow discarding on root token - does not make sense
            token->set_discard(discard);
    }
}

void MPSEngine::update_selected_subtoken (
        const std::wstring& text,
        const std::wstring& separators,
        bool discard,
        bool force_update)
{
    if (!m_selected_subtoken)
        return;

    bool is_root = (m_selected_subtoken == m_master_token);
    update_token(m_selected_subtoken, text, separators, discard, force_update);
    if (is_root) m_master_token = m_selected_subtoken;

    //  reconstruct rename_to
    m_rename_to = reconstruct_output(m_master_token);

    //  update the new name in the map
    boost::hash<std::wstring> hasher;
    size_t hash = hasher (m_master_token->text());
    MPSFilesToRenameMap::iterator iter = m_files_map.find(hash);
    if (iter != m_files_map.end()) {
        iter->second.rename_to = m_rename_to;
    }
}

void MPSEngine::shift_selected_subtoken (EMPSDirection direction)
{
    if (!m_selected_subtoken)
        return;

    MPSToken* parent = m_selected_subtoken->parent();
    if (!parent)
        return;

    parent->shift_subtoken (m_selected_subtoken, direction);

    m_rename_to = reconstruct_output(m_master_token);
}

void MPSEngine::change_case (bool upcase, bool only_first, bool recursive)
{
    //  find the rename entry in the rename map first
    boost::hash<std::wstring> hasher;
    size_t hash = hasher (m_master_token->text());
    MPSFilesToRenameMap::iterator iter = m_files_map.find (hash);

    change_case(m_selected_subtoken, upcase, only_first, recursive);
    m_rename_to = reconstruct_output(m_master_token);
    if (iter != m_files_map.end())
        iter->second.rename_to = m_rename_to;
}

void MPSEngine::insert_text (const std::wstring& text_to_insert, EMPSDirection direction)
{
    if (!m_selected_subtoken)
        return;

    if (m_selected_subtoken == m_master_token)
        return;

    MPSToken* parent = m_selected_subtoken->parent();
    if (!parent)
        return;

    size_t sel_token_pos = 0;
    MPSTokensContainer::iterator iter = m_selected_subtoken->parent ()->sub_tokens_begin ();
    for ( ; iter != m_selected_subtoken->parent ()->sub_tokens_end (); ++iter) {
        if (*iter == m_selected_subtoken)
            break;
        ++sel_token_pos;
    }

    if (direction == ELeft)
        parent->insert_subtoken (text_to_insert, sel_token_pos);
    else
        parent->insert_subtoken (text_to_insert, sel_token_pos + 1);

    m_rename_to = reconstruct_output (m_master_token);
}

void MPSEngine::change_case (MPSToken* token, bool upcase, bool only_first, bool recursive)
{
    MPSActionChangeCase change_case (*this, upcase, !only_first, recursive);
    change_case (token);
}

std::wstring MPSEngine::reconstruct_output (MPSToken* token)
{    
    std::wstring name;
    std::wstring separator;
    if (token != 0) {
        if (token->sub_tokens_empty()) {
            if (!token->is_discard()) {
                bool is_first = (token == m_master_token->find_first_leaf_subtoken (false));
                bool is_last = (token == m_master_token->find_last_leaf_subtoken(false));
                separator = (is_first) ? L"" : ((is_last) ? L"." : L" ");
                name += separator;
                name += token->text();
            }
        } else {
            MPSTokensContainer::const_iterator iter = token->sub_tokens_begin();
            for ( ; iter != token->sub_tokens_end(); ) {
                name += reconstruct_output(*iter);
                ++iter;
            }
        }
    }
    return name;
}

bool MPSEngine::has_rename_to (const std::wstring& file_name, std::wstring& rename_to)
{
    bool has = false;

    //  compute the hash for the file name
    boost::hash<std::wstring> hasher;
    size_t hash = hasher (file_name);

    //  find it
    MPSFilesToRenameMap::const_iterator iter = m_files_map.find (hash);
    if (iter != m_files_map.end()) {
        rename_to = iter->second.rename_to;
        has = true;
    } else {
        rename_to.clear();
        has = false;
    }

    return has;
}

bool MPSEngine::has_files_to_rename () const
{
    MPSFilesToRenameMap::const_iterator iter = m_files_map.begin();
    for ( ; iter != m_files_map.end(); ++iter) {
        if (iter->second.root->text () != iter->second.rename_to)
            return true;
    }
    return false;
}

void MPSEngine::clear_files_map ()
{
    MPSFilesToRenameMap::iterator iter = m_files_map.begin();
    for ( ; iter != m_files_map.end(); ++iter) {
        delete iter->second.root;
        iter->second.root = 0;
        iter->second.rename_to.clear();
    }
    m_files_map.clear();
}

bool MPSEngine::rename_one (    const std::wstring& path,
                                const std::wstring& src_file,
                                const std::wstring& dst_file,
                                bool update_map_entry ) const
{
    using namespace boost::filesystem;

    std::wstring err_msg = L"Failed to rename file \"" + src_file + L"\" because of error ";

    //  construct source path
    std::wstring src = path;
    src += L"\\";
    src += src_file;

    //  construct destination path
    std::wstring dst = path;
    dst += L"\\";
    dst += dst_file;

    //  does src_file file exist?
    if (!exists(src)) {
        err_msg += L"[Source file does not exist]";
        m_messages.push(err_msg);
        return false;
    }

    //  is src_file a regular file?
    if (!is_regular_file(src)) {
        err_msg += L"[Source file is not regular]";
        m_messages.push(err_msg);
        return false;
    }

    //  if dst_file empty?
    if (dst.empty()) {
        err_msg += L"[Destination file name is empty]";
        m_messages.push(err_msg);
        return false;
    }

    //  does src_file equal dst_file?
    if (src == dst) {
        err_msg += L"[Source file is same as destination file]";
        m_messages.push(err_msg);
        return false;
    }


    //  don't let it overwrite an already existing file unless it's an upcase change
    if (exists(dst) && !boost::iequals (src, dst)) {
        err_msg += L"[A file with that name already exists in this folder]";
        m_messages.push (err_msg);
        return false;
    }

    //  rename
    try {
        rename(src, dst);

        if (update_map_entry) {
            //  search for a map entry for src and update the LHS
            boost::hash<std::wstring> hasher;
            size_t hash = hasher (src_file);

            //  find it
            MPSFilesToRenameMap::const_iterator iter = m_files_map.find (hash);
            if (iter != m_files_map.end()) {
                MPSFilesMapStruct record = (*iter).second;
                record.root->set_separators (L"");
                record.root->set_text (dst_file);
            }
        }

        err_msg.clear ();
        err_msg += L"Renamed \"";
        err_msg += src_file;
        err_msg += L"\" to \"";
        err_msg += dst_file;
        err_msg += L"\"";
        m_messages.push (err_msg);
    } catch (boost::filesystem::filesystem_error& fserr) {
        std::string err = fserr.what();
        std::wstring werr;
        werr.assign(err.begin(), err.end());
        err_msg += L"[" + werr + L"]";
        m_messages.push(err_msg);
        return false;
    }

    return true;
}

void MPSEngine::rename_all (const std::wstring& path) const
{
    using namespace boost::filesystem;

    MPSFilesToRenameMap::const_iterator iter = m_files_map.begin();
    for ( ; iter != m_files_map.end(); ++iter) {
        std::wstring src = (*iter).second.root->text();
        src = path + L"\\" + src;   //  prepend folder path

        std::wstring dst = (*iter).second.rename_to;
        dst = path + L"\\" + dst;   //  prepend folder path

        //  is dst empty?
        if (((*iter).second.rename_to).empty ()) {
            std::wstring err_msg = L"Failed to rename [";
            err_msg += (*iter).second.root->text();
            err_msg += L"], destination file name is null string.";
            m_messages.push(err_msg);
            continue;
        }

        //  skip same source and destination
        if ((*iter).second.root->text() == (*iter).second.rename_to) {
            std::wstring err_msg = L"Skipped rename [";
            err_msg += (*iter).second.root->text();
            err_msg += L"], destination same as source";
            m_messages.push(err_msg);
            continue;
        }

        //  dst should not exist, but allow case changes updates
        if (exists(dst) && !boost::iequals ((*iter).second.root->text(), (*iter).second.rename_to)) {
            std::wstring err_msg = L"Failed to rename [";
            err_msg += (*iter).second.root->text();
            err_msg += L"] to [";
            err_msg += (*iter).second.rename_to;
            err_msg += L"], a file with that name already exists in current folder.";
            m_messages.push(err_msg);
            continue;
        }

        //  rename one file
        (void) rename_one (path, (*iter).second.root->text(), (*iter).second.rename_to, true);
    }
}

void MPSEngine::update (MPSToken* token)
{
    //  force update token
    update_token(token, token->text(), token->separators(), token->is_discard(), true);
}

bool MPSEngine::is_token_current_root(const MPSToken* token) const
{
    if (!token)
        return false;

    if (!m_master_token)
        return false;

    return (token == m_master_token);
}