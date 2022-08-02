#ifndef MPSMAINWINDOW_HPP
#define MPSMAINWINDOW_HPP

#include "ui_MemphisNowWindow.h"
#include "MPSEngine.hpp"

#include <QtCore/QDir>
#include <QtGui/QFileSystemModel>

//  forward declarations to prevent includes at this level
class QDialog;
class QComboBox;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QLabel;
class QGraphicsScene;
class QMediaPlayer;
class QDomElement;

//  enum used for transforms dialog logic
enum MPSDialogState {
    ENoState = 0,
    EAddState,
    EEditState
};

/*  Class used to allow only numeric values on editors */
class MPSNumericValidator : public QValidator {
    virtual QValidator::State validate ( QString & input, int & pos ) const;
};

class MPSMainWindow : public QMainWindow, public MPSEngineObserver
{
    Q_OBJECT
    
public:
    explicit MPSMainWindow(QWidget *parent = 0);
    
    //  from MPSEngineObserver
    void notify ();

private:
    void create_transform_dialog ();

    bool can_enable_save_button (QString& out_message) const;

    void update_dialog_message (const QString& msg);

    //  sets up the transforms dialog state based on the settings from info
    void load_dialog_state (const QString& title, const MPSTransformInfo& info);

    //  manually connects events to handlers
    void connect_events();

    void create_view_models ();

    //  get the label for the drive
    QString get_drive_label(const QString& drive);

    //  load editable drives into drives combo
    void load_drives_list ();

    //  update tree navigator based on currently selected drive
    void change_drive(int idx);

    void on_current_file_changed (const QModelIndex&);

    //  updates all global settings with values from the engine
    void load_global_settings ();

    void update_rename_to_entry ();

    void update_renames_list ();

    void update_rename_and_discard_area ();

    void update_selected_token_details ();

    void update_selected_token_transforms ();

    void update_shift_buttons ();

    void update_transforms_buttons ();

    void create_default_transforms ();

    void add_char_to_token (char one_char);

    int get_confirmation (const QString& message);

    //  scene handling
    void reconstruct_scene ();
    void construct_scene (
        QGraphicsScene* scene,
        const MPSToken* token,
        int& orig_x_offset,
        int& orig_y_offset,
        int& width,
        int& height
    );

    //  console related
    void log_to_console (const QString& err_msg);

private slots:  //  event handlers for tranforms dialog
    void on_cmbConditionsIndexChanged (int idx);
    void on_cmbActionsIndexChanged(int idx);
    void on_editActionsTextTextEdited (const QString& text);
    void on_editActionsPosTextChanged (const QString& text);
    void on_editCondTextTextEdited (const QString& text);
    void on_btnSaveClicked();
    void on_btnCloseClicked();

private slots:    //  event handlers for main window
    void on_cmbDrives_currentIndexChanged(int index);
    void on_tvFolders_clicked(const QModelIndex& index);
    void on_filesModelDirectoryLoaded (QString);    
    void on_btnAddTransform_clicked();   
    void on_btnEditTransform_clicked();
    void on_btnRemoveTransform_clicked();
    void on_btnClearTransforms_clicked();
    void on_lstTransforms_doubleClicked(const QModelIndex &index);
    void on_editSelectedTokenText_returnPressed();
    void on_editSelectedTokenSeparators_returnPressed();
    void on_btnClearTokenSeparators_clicked();
    void on_btnDefaultTokenSeparators_clicked();
    void on_btnShiftTokenLeft_clicked();
    void on_btnShiftTokenRight_clicked();
    void on_chkTokenDiscarded_stateChanged(int);
    void on_btnDiscardOne_clicked();
    void on_btnDiscardAll_clicked();
    void on_btnRenameOne_clicked();
    void on_btnRenameAll_clicked();
    void on_btnUpcase_clicked();
    void on_btnLowcase_clicked();
    void on_chkEnableTransforms_stateChanged(int);
    void on_listFilesVerticalSliderValueChanged (int value);
    void on_lstFilesHorizontalSliderValueChanged (int value);
    void on_lstRenameToVerticalSliderValueChanged (int value);
    void on_lstRenameToHorizontalSliderValueChanged (int value);
    void on_editDefaultSeparators_textEdited(const QString&);
    void on_lstFiles_entered(const QModelIndex &index);
    void on_lstFiles_pressed(const QModelIndex &index);
    void on_btnEnableDisableTransform_clicked();
    void on_btnMoveUpTransform_clicked();
    void on_btnMoveDownTransform_clicked();
    void on_lstTransforms_currentRowChanged(int);
    void on_btnAddRemLeft_toggled(bool checked);
    void on_btnAddRemRight_toggled(bool checked);
    void on_btnAddDot_clicked();
    void on_btnAddDash_clicked();
    void on_btnAddParanthesis_clicked();
    void on_btnAddBracket_clicked();
    void on_btnClearConsole_clicked();
    void on_btnListRenames_clicked();
    void on_btnInsertLeft_clicked();
    void on_btnInsertRight_clicked();
    void on_btnClearInsertText_clicked();
    void on_btnLoadTransChain_clicked();
    void on_close(QCloseEvent *event);    

private:
    Ui::MPSMainWindow       m_gui_obj;
    QGraphicsScene*         m_scene;
    QGraphicsTextItem*      m_txt_no_sel;

    MPSEngine               m_core_engine;        

    QStringList             m_drives_list;
    QFileSystemModel*       m_folders_model;
    QFileSystemModel*       m_files_model;
    QIcon                   m_icon_warn;

    //  settings
    bool                    m_show_groups;
    MPSDialogState          m_trans_dlg_state;

    //  validator for cmbAddRemChar
    MPSNumericValidator     m_numeric_validator;

private:    //  Transform Dialog
    QDialog*                m_dlg_trans;
    QComboBox*              m_dlg_trans_cmb_condition;
    QComboBox*              m_dlg_trans_cmb_action;
    QPushButton*            m_dlg_trans_btn_save;
    QPushButton*            m_dlg_trans_btn_close;
    QLineEdit*              m_dlg_trans_edit_cond_text;
    QCheckBox*              m_dlg_trans_chk_case_sens;
    QLineEdit*              m_dlg_trans_edit_act_text_1;
    QLineEdit*              m_dlg_trans_edit_act_text_2;
    QLabel*                 m_dlg_trans_lbl_msg;
    QCheckBox*              m_dlg_trans_chk_upcase;
    QCheckBox*              m_dlg_trans_chk_case_all;
    QCheckBox*              m_dlg_trans_chk_recursive;
    QCheckBox*              m_dlg_trans_chk_disable;
    QLineEdit*              m_dlg_trans_edit_pos;
    QLineEdit*              m_dlg_trans_edit_desc;
    QCheckBox*              m_dlg_trans_chk_leafs_only;
    QCheckBox*              m_dlg_trans_chk_enabled;
};

#endif // MPSMAINWINDOW_HPP
