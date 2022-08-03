#include "MPSMainWindow.hpp"
#include "MPSCommon.hpp"
#include "MPSTokenWidget.hpp"
#include "boost/tuple/tuple.hpp"
#include <string>
#include <fstream>
#include <windows.h>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QScrollBar>
#include <QtGui/QPainter>


#include <QVector>
//#include <QUrl>


//  confirmation dialog messages
const QString KDiscardAllMessage = "You are going to lose all your changes.\nAre you sure?";
const QString KRenameAllMessage = "You are going to physically rename all modified files.\nDo you really want to proceed?";
const QString KClearTransformsMessage = "You are going to remove all transforms.\nDo you REALLY want to proceed?";
const QString KLoadDefaultTransformsMessage = "You are going to load the default transforms.\nThis is going to REMOVE all current transforms, proceed?";

//  characters available to add
const QString KCharactersToAdd[] = { ".", "-", "&", "(", ")", "[", "]" };
const QString KDefaultSeparators = " _,.";

//  scene drawing
const int KInterTokenHorizontalSpace = 20;
const int KInterTokenVerticalSpace = 35;

//  red color template
const QString KRedColorTemplate = "<font color='red'>%2</font>";

//  max number of digits in position editor
const int KMaxDigitsInEditor = 2;

//  enable / disable transfom texts
const char KDisableTransformText[] = "■";
const char KEnableTransformText[] = "●";

//  audio extensions
const char* KAudioFileExtensions[] = { "*.mp3", "*.flac", "*.wav", "*.ogg", "*.m4a" };

#ifdef QT_DEBUG
const QString KDefaultXMLPath = "c:\\devel\\memphis.xml";
#else
const QString KDefaultXMLPath = "memphis.xml";
#endif

const QString KLabelPlay = "Play";
const QString KLabelStop = "Stop";

QValidator::State MPSNumericValidator::validate ( QString & input, int & pos ) const
{
    (void) pos;

    bool is_valid = false;

    //  should not contain any spaces
    is_valid = !input.contains(' ');

    //  limit the size on the input
    is_valid = is_valid && input.count() <= KMaxDigitsInEditor;

    //  if no spaces and allowed number of digits, test conversion
    if (is_valid && input.count() > 0)
        input.toInt(&is_valid, 10);   //  convert to base 10; is_valid is set to false if fails

    return (is_valid) ? QValidator::Acceptable : QValidator::Invalid;
}

MPSMainWindow::MPSMainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_icon_warn (":/MemphisNow/icons/warn.png"),
    m_show_groups (false),
    m_dlg_trans (0)
{
    //  construct user interface
    m_gui_obj.setupUi(this);

    //  create the scene
    m_scene = new QGraphicsScene;
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);    
    m_gui_obj.viewScene->setScene(m_scene);

    //  setup the header on the folders tree view to scroll depending on the item size
    m_gui_obj.tvFolders->header()->setStretchLastSection(false);
    m_gui_obj.tvFolders->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    create_transform_dialog ();

    create_view_models();

    //  set this as an observer of the engine - used for selecting token widgets
    m_core_engine.add_observer(this);
    
    //  Create and add default transforms
    create_default_transforms();
    for (int idx = 0; idx < m_core_engine.count_transforms(); ++idx) {
        QString item_text = QString::number(idx + 1) + ". " + QString::fromStdWString(m_core_engine.get_transform_description_at(idx));
        m_gui_obj.lstTransforms->addItem(item_text);
    }

    load_global_settings();

    //  should be called only after all GUI objects have been initialised
    connect_events();

    load_drives_list();

    update_rename_and_discard_area();
    update_selected_token_details();
    update_selected_token_transforms();
    update_transforms_buttons();

    log_to_console("MPS started.");
}

void MPSMainWindow::notify ()
{
    m_scene->invalidate();

    update_selected_token_details();
    update_selected_token_transforms();
    update_rename_to_entry();
    update_rename_and_discard_area();
}

void MPSMainWindow::create_transform_dialog ()
{
    /*******************************
    **** START Condition Section ***
    ********************************/
    QHBoxLayout* layout_cond = new QHBoxLayout;

    //  label
    QLabel* lbl_cond = new QLabel ("On Condition:");
    lbl_cond->setFixedWidth(70);
    layout_cond->addWidget(lbl_cond, 0, Qt::AlignRight);

    //  combo condition
    m_dlg_trans_cmb_condition = new QComboBox;
    m_dlg_trans_cmb_condition->setMinimumWidth(105);
    layout_cond->addWidget(m_dlg_trans_cmb_condition);

    //  dynamic condition widget
    QWidget* wid_dyn_cond = new QWidget;
    wid_dyn_cond->setFixedSize(205, 65);
        QVBoxLayout* dyn_cond_layout = new QVBoxLayout;
        m_dlg_trans_edit_cond_text = new QLineEdit;
        m_dlg_trans_edit_cond_text->setText("");
        m_dlg_trans_edit_cond_text->setVisible(false);
        dyn_cond_layout->addWidget(m_dlg_trans_edit_cond_text);

        m_dlg_trans_chk_case_sens = new QCheckBox;
        m_dlg_trans_chk_case_sens->setText("Case sensitive");
        m_dlg_trans_chk_case_sens->setVisible(false);
        dyn_cond_layout->addWidget(m_dlg_trans_chk_case_sens);
    wid_dyn_cond->setLayout(dyn_cond_layout);
    layout_cond->addWidget(wid_dyn_cond);
    /*****************************
    **** END Condition Section ***
    ******************************/

    //  separator for conditions & actions
    QFrame* sep_cond_act = new QFrame();
    sep_cond_act->setFrameShape(QFrame::HLine);
    sep_cond_act->setFrameShadow(QFrame::Sunken);

    /****************************
    **** START Action Section ***
    *****************************/
    QHBoxLayout* layout_act = new QHBoxLayout;

    //  label
    QLabel* lbl_act = new QLabel ("Do Action:");
    lbl_act->setFixedWidth(70);
    layout_act->addWidget(lbl_act, 0, Qt::AlignRight);

    //  combo action
    m_dlg_trans_cmb_action = new QComboBox;
    m_dlg_trans_cmb_action->setMinimumWidth(105);
    layout_act->addWidget(m_dlg_trans_cmb_action);

    //  dynamic action widget
    QWidget* wid_dyn_act = new QWidget;
    wid_dyn_act->setFixedSize(205, 60);
        QHBoxLayout* dyn_act_layout = new QHBoxLayout;

        m_dlg_trans_edit_act_text_1 = new QLineEdit;
        m_dlg_trans_edit_act_text_1->setText("");
        m_dlg_trans_edit_act_text_1->setVisible(false);
        dyn_act_layout->addWidget(m_dlg_trans_edit_act_text_1);

        //  second editbox is used for action Replace In Text
        m_dlg_trans_edit_act_text_2 = new QLineEdit;
        m_dlg_trans_edit_act_text_2->setText("");
        m_dlg_trans_edit_act_text_2->setVisible(false);
        dyn_act_layout->addWidget(m_dlg_trans_edit_act_text_2);

        m_dlg_trans_chk_upcase = new QCheckBox;
        m_dlg_trans_chk_upcase->setText("Upcase");
        m_dlg_trans_chk_upcase->setVisible(false);
        dyn_act_layout->addWidget(m_dlg_trans_chk_upcase);

        m_dlg_trans_chk_case_all = new QCheckBox;
        m_dlg_trans_chk_case_all->setText("All");
        m_dlg_trans_chk_case_all->setVisible(false);
        dyn_act_layout->addWidget(m_dlg_trans_chk_case_all);

        m_dlg_trans_chk_recursive = new QCheckBox;
        m_dlg_trans_chk_recursive->setText("Recursive");
        m_dlg_trans_chk_recursive->setVisible(false);
        dyn_act_layout->addWidget(m_dlg_trans_chk_recursive);

        m_dlg_trans_chk_disable = new QCheckBox;
        m_dlg_trans_chk_disable->setText("Disable");
        m_dlg_trans_chk_disable->setVisible(false);
        dyn_act_layout->addWidget(m_dlg_trans_chk_disable);

        m_dlg_trans_edit_pos = new QLineEdit;
        m_dlg_trans_edit_pos->setValidator(&m_numeric_validator);
        m_dlg_trans_edit_pos->setVisible(false);
        m_dlg_trans_edit_pos->setMaximumWidth(25);
        m_dlg_trans_edit_pos->setAlignment(Qt::AlignHCenter);
        m_dlg_trans_edit_pos->setToolTip("Position on which to insert subtoken");
        dyn_act_layout->addWidget(m_dlg_trans_edit_pos);
    wid_dyn_act->setLayout(dyn_act_layout);
    layout_act->addWidget(wid_dyn_act);
    /**************************
    **** END Action Section ***
    ***************************/

    //  separator for actions & description
    QFrame* sep_act_desc = new QFrame();
    sep_act_desc->setFrameShape(QFrame::HLine);
    sep_act_desc->setFrameShadow(QFrame::Sunken);

    /*********************************
    **** START Description Section ***
    **********************************/
    QHBoxLayout* layout_desc = new QHBoxLayout;

    QLabel* lbl_desc = new QLabel ("Description: ");
    lbl_desc->setFixedWidth(70);
    layout_desc->addWidget(lbl_desc, 0, Qt::AlignRight);

    m_dlg_trans_edit_desc = new QLineEdit;
    layout_desc->addWidget(m_dlg_trans_edit_desc);

    layout_desc->insertSpacing(2, 60);
    /*******************************
    **** END Description Section ***
    ********************************/

    //  separator for actions & bottom layout
    QFrame* sep_bottom = new QFrame();
    sep_bottom->setFrameShape(QFrame::HLine);
    sep_bottom->setFrameShadow(QFrame::Sunken);

    //  create the message label
    m_dlg_trans_lbl_msg = new QLabel("");
    QFont font = m_dlg_trans_lbl_msg->font();
    font.setItalic(true);
    m_dlg_trans_lbl_msg->setFont(font);

    /***************************
    **** START Bottom Layout ***
    ****************************/
    QHBoxLayout* bottom_layout = new QHBoxLayout;
        m_dlg_trans_chk_leafs_only = new QCheckBox ("Apply On Leafs Only");
        bottom_layout->addWidget(m_dlg_trans_chk_leafs_only);

        m_dlg_trans_chk_enabled = new QCheckBox ("Enabled");
        bottom_layout->addWidget(m_dlg_trans_chk_enabled);

        m_dlg_trans_btn_save = new QPushButton;
        m_dlg_trans_btn_save->setText("Save");
        m_dlg_trans_btn_save->setFixedWidth(75);
        m_dlg_trans_btn_save->setEnabled(false);
        bottom_layout->addWidget(m_dlg_trans_btn_save);

        m_dlg_trans_btn_close = new QPushButton;
        m_dlg_trans_btn_close->setText("Cancel");
        m_dlg_trans_btn_close->setFixedWidth(75);
        bottom_layout->addWidget(m_dlg_trans_btn_close);
    /*************************
    **** END Bottom Layout ***
    **************************/

    //  add all layouts
    QVBoxLayout* layout_vertical = new QVBoxLayout;
    layout_vertical->addLayout(layout_cond);
    layout_vertical->addWidget(sep_cond_act);
    layout_vertical->addLayout(layout_act);
    layout_vertical->addWidget(sep_act_desc);
    layout_vertical->addLayout(layout_desc);
    layout_vertical->addWidget(sep_bottom);
    layout_vertical->addWidget(m_dlg_trans_lbl_msg, 0, Qt::AlignRight);
    layout_vertical->insertSpacing(4, 10);
    layout_vertical->insertSpacing(6, 10);
    layout_vertical->addLayout(bottom_layout);
    layout_vertical->setSizeConstraint(QLayout::SetFixedSize);

    m_dlg_trans = new QDialog;
    m_dlg_trans->setModal(true);
    m_dlg_trans->setLayout(layout_vertical);

    //  load conditions
    for (int idx = 0; idx < m_core_engine.count_available_conditions(); ++idx)
        m_dlg_trans_cmb_condition->addItem(m_core_engine.get_available_condition_name_at(idx).c_str());

    //  load actions
    for (int idx = 0; idx < m_core_engine.count_available_actions(); ++idx)
        m_dlg_trans_cmb_action->addItem(m_core_engine.get_available_action_name_at(idx).c_str());
}

void MPSMainWindow::load_dialog_state (const QString& title, const MPSTransformInfo& info)
{
    m_dlg_trans->setWindowTitle(title);

    m_dlg_trans_cmb_condition->setCurrentIndex(-1); //  force deselect
    m_dlg_trans_cmb_condition->setCurrentIndex(info.condition_index);

    bool is_cond_equals = (info.condition_index == m_core_engine.get_available_condition_index(KConditionEqualsID));
    m_dlg_trans_edit_cond_text->setVisible(is_cond_equals);
    m_dlg_trans_edit_cond_text->setText(QString::fromStdWString(info.condition_text));
    if (is_cond_equals)
        m_dlg_trans_edit_cond_text->setFocus();

    m_dlg_trans_chk_case_sens->setVisible(is_cond_equals);
    m_dlg_trans_chk_case_sens->setChecked(info.condition_checks[0]);

    m_dlg_trans_cmb_action->setCurrentIndex(-1); //  force deselect
    m_dlg_trans_cmb_action->setCurrentIndex(info.action_index);
    bool is_act_text_1_hidden = (
        info.action_index == -1 ||  // not selected
        info.action_index == m_core_engine.get_available_action_index(KActionEnableDisableID) ||
        info.action_index == m_core_engine.get_available_action_index(KActionChangeCaseID)
    );
    m_dlg_trans_edit_act_text_1->setVisible(!is_act_text_1_hidden);
    m_dlg_trans_edit_act_text_1->setText(QString::fromStdWString(info.action_texts[0]));

    bool is_act_text_2_hidden = (
        info.action_index == -1 ||  // not selected
        info.action_index != m_core_engine.get_available_action_index(KActionReplaceInTextID)
    );
    m_dlg_trans_edit_act_text_2->setVisible(!is_act_text_2_hidden);
    m_dlg_trans_edit_act_text_2->setText(QString::fromStdWString(info.action_texts[1]));

    //  case change checkboxes
    m_dlg_trans_chk_upcase->setChecked(info.action_checks[0]);
    m_dlg_trans_chk_case_all->setChecked(info.action_checks[1]);
    m_dlg_trans_chk_recursive->setChecked(info.action_checks[2]);

    //  set discard checkbox
    m_dlg_trans_chk_disable->setChecked(info.action_checks[3]);

    //  description
    m_dlg_trans_edit_desc->setText(QString::fromStdWString(info.description));

    //  we can set the leafs only flag only if something is selected and the condition is not (IS ROOT)
    bool can_use_leafs_only_check = (
        info.condition_index != -1 &&
        info.condition_index != m_core_engine.get_available_condition_index(KConditionIsRootID)
    );
    m_dlg_trans_chk_leafs_only->setChecked(info.leafs_only);
    m_dlg_trans_chk_leafs_only->setEnabled(can_use_leafs_only_check);

    m_dlg_trans_chk_enabled->setChecked(info.enabled);
}

/** BEGIN utility methods **/
void MPSMainWindow::connect_events ()
{
    connect(m_files_model, SIGNAL(directoryLoaded(QString)), this, SLOT(on_filesModelDirectoryLoaded(QString)));

    //  dialog events
    connect(m_dlg_trans_cmb_condition,        SIGNAL(currentIndexChanged(int)),           this,   SLOT(on_cmbConditionsIndexChanged(int)));
    connect(m_dlg_trans_cmb_action,           SIGNAL(currentIndexChanged(int)),           this,   SLOT(on_cmbActionsIndexChanged(int)));
    connect(m_dlg_trans_edit_cond_text,       SIGNAL(textEdited(const QString&)),         this,   SLOT(on_editCondTextTextEdited(const QString&)));
    connect(m_dlg_trans_edit_act_text_1,      SIGNAL(textEdited(const QString&)),         this,   SLOT(on_editActionsTextTextEdited(QString)));
    connect(m_dlg_trans_edit_pos,             SIGNAL(textChanged(QString)),               this,   SLOT(on_editActionsPosTextChanged(const QString&)));
    connect(m_dlg_trans_btn_close,            SIGNAL(clicked()),                          this,   SLOT(on_btnCloseClicked()));
    connect(m_dlg_trans_btn_save,             SIGNAL(clicked()),                          this,   SLOT(on_btnSaveClicked()));

    //  connect scroll bars' motion
    QScrollBar* files_vert_scroll = m_gui_obj.lstFiles->verticalScrollBar();
    QScrollBar* files_horiz_scroll = m_gui_obj.lstFiles->horizontalScrollBar();
    QScrollBar* rename_to_vert_scroll = m_gui_obj.lstRenames->verticalScrollBar();
    QScrollBar* rename_to_horiz_scroll = m_gui_obj.lstRenames->horizontalScrollBar();
    connect(files_vert_scroll,              SIGNAL(valueChanged(int)),          this,   SLOT(on_listFilesVerticalSliderValueChanged(int)));
    connect(files_horiz_scroll,             SIGNAL(valueChanged(int)),          this,   SLOT(on_lstFilesHorizontalSliderValueChanged(int)));
    connect(rename_to_vert_scroll,          SIGNAL(valueChanged(int)),          this,   SLOT(on_lstRenameToVerticalSliderValueChanged(int)));
    connect(rename_to_horiz_scroll,         SIGNAL(valueChanged(int)),          this,   SLOT(on_lstRenameToHorizontalSliderValueChanged(int)));
}

void MPSMainWindow::create_view_models ()
{
    //  create and setup folders models
    m_folders_model = new QFileSystemModel(this);
    m_folders_model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);    
    m_gui_obj.tvFolders->setModel(m_folders_model);
    m_gui_obj.tvFolders->hideColumn(1);
    m_gui_obj.tvFolders->hideColumn(2);
    m_gui_obj.tvFolders->hideColumn(3);

    //  expand on click
    QObject::connect(
        m_gui_obj.tvFolders, SIGNAL(clicked(const QModelIndex &)),
        m_gui_obj.tvFolders, SLOT(expand(const QModelIndex &))
    );

    //  construct the audio file filters
    QStringList filter_audio_only;
    for (size_t idx = 0; idx < sizeof(KAudioFileExtensions) / sizeof(*KAudioFileExtensions); ++idx) {
        filter_audio_only << KAudioFileExtensions[idx];
    }

    //  create and setup files model
    m_files_model = new QFileSystemModel(this);
    m_files_model->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    m_files_model->setNameFilters (filter_audio_only);
    m_files_model->setNameFilterDisables(false);

    m_gui_obj.lstFiles->setModel(m_files_model);
    m_gui_obj.lstFiles->setEditTriggers(QAbstractItemView::NoEditTriggers);  

    //  load last path
    /*xQString lastPath = "C:\\Users\\USER\\Desktop\\Dan\\NOVA_MUZICA";
    QModelIndex index = m_folders_model->setRootPath(lastPath);    
    m_gui_obj.tvFolders->setRootIndex(index);
    */

}

QString MPSMainWindow::get_drive_label(const QString& drive)
{
    WCHAR szVolumeName[256] ;
    WCHAR szFileSystemName[256];
    DWORD dwSerialNumber = 0;
    DWORD dwMaxFileNameLength=256;
    DWORD dwFileSystemFlags=0;
    bool ret = GetVolumeInformation(
        (WCHAR*) drive.utf16(),
        szVolumeName,
        256,
        &dwSerialNumber,
        &dwMaxFileNameLength,
        &dwFileSystemFlags,
        szFileSystemName,
        256);
    if (!ret) return QString("");
    QString name = QString::fromUtf16 ((const ushort*) szVolumeName);
    (void) name.trimmed();

    return name;
}

void MPSMainWindow::load_drives_list ()
{
    //  TODO: possible bug here - what if there is an un-editable drive between 2 editable drives?
    m_drives_list.clear();
    foreach( QFileInfo drive, QDir::drives()) {
        m_drives_list.append(drive.absolutePath());
    }

    //  load the combobox
    foreach (QString drive, m_drives_list) {
        UINT type = GetDriveTypeA(drive.toStdString().c_str());
        if (type == DRIVE_FIXED || type == DRIVE_REMOVABLE) {
            //  drop the forward slash
            std::string std_drive = drive.toStdString() + " ";
            std::string std_label = get_drive_label(drive).toStdString();

            //  remove the forward slash
            size_t slash_pos = std_drive.find("/");
            if (slash_pos != std_drive.npos) {
                std_drive = std_drive.replace(slash_pos, 1, "");
            }

            std::string drive_and_label = std_drive;
            if (!std_label.empty())
                drive_and_label += "(" + std_label + ") ";
            else {
                drive_and_label += "[no label]";
            }

            m_gui_obj.cmbDrives->addItem(drive_and_label.c_str());
        }
    }

    if (m_gui_obj.cmbDrives->count() > 0)
        m_gui_obj.cmbDrives->setCurrentIndex(0);
}

void MPSMainWindow::change_drive(int drive_index)
{
    if (drive_index == -1)
        return;

    QString root_path = m_drives_list.at(drive_index);

    m_gui_obj.tvFolders->setRootIndex(m_folders_model->setRootPath(root_path));
    m_gui_obj.lstFiles->setRootIndex (m_files_model->setRootPath(root_path));

    m_gui_obj.editFolder->setText(QDir::toNativeSeparators(root_path));

    //  deselect files list
    m_gui_obj.lstFiles->setCurrentIndex(QModelIndex());
    m_gui_obj.lstRenames->setCurrentRow(-1);

    //  deselect, clear map, reconstruct scene
    m_core_engine.select_master_token(L"");
    m_core_engine.clear_files_map();
    reconstruct_scene();

    //  update ui elements
    update_selected_token_details();
    update_selected_token_transforms();
    update_rename_and_discard_area();
}

void MPSMainWindow::on_current_file_changed (const QModelIndex& index)
{
    if (index.row() == -1)
        return;

    QString qsFileName = m_files_model->fileName(index);
    std::wstring file_name = qsFileName.toStdWString();

    m_core_engine.select_master_token (file_name);

    if (m_core_engine.master_token()->text() != m_core_engine.current_rename_to()) {
        m_gui_obj.lstRenames->item(index.row())->setText(QString::fromStdWString(m_core_engine.current_rename_to()));
        m_gui_obj.lstRenames->item(index.row())->setIcon(m_icon_warn);
    }

    reconstruct_scene();
    update_rename_and_discard_area();
    update_selected_token_details();
    update_selected_token_transforms();

    //  log the engine errors
    while (m_core_engine.has_messages())
        log_to_console(QString::fromStdWString(m_core_engine.pop_message()));
}

void MPSMainWindow::update_rename_to_entry ()
{
    int idx = m_gui_obj.lstFiles->currentIndex().row();
    if (idx == -1)
        return;
    m_gui_obj.lstRenames->item(idx)->setText(QString::fromStdWString(m_core_engine.current_rename_to()));
    if (m_core_engine.master_token()->text() == m_core_engine.current_rename_to()) {
        //  restore original icon
        QVariant variant = m_files_model->data(m_gui_obj.lstFiles->currentIndex(), QFileSystemModel::FileIconRole);
        m_gui_obj.lstRenames->item(idx)->setIcon(variant.value<QIcon> ());
    } else {
        //  set warning icon
        m_gui_obj.lstRenames->item(idx)->setIcon(m_icon_warn);
    }
}

void MPSMainWindow::update_renames_list ()
{
    m_gui_obj.lstRenames->clear();

    QModelIndex modelIndex = m_files_model->index(m_files_model->rootPath());
    for (int idx = 0; idx < m_files_model->rowCount(modelIndex); ++idx) {        
        QModelIndex child = m_files_model->index(idx, modelIndex.column(), modelIndex);

        //  get the file name
        QString fileName = m_files_model->fileName(child);

        //  check the entry in engine's map
        std::wstring rename_to;
        bool has_rename = m_core_engine.has_rename_to(fileName.toStdWString(), rename_to);
        QString qstr_rename_to = QString::fromStdWString(rename_to);

        if (has_rename)
            //  use rename to entry
            m_gui_obj.lstRenames->addItem(qstr_rename_to);
        else
            //  use same file name and icon
            m_gui_obj.lstRenames->addItem(fileName);

        if (has_rename) {
            if (fileName == qstr_rename_to) {
                //  same
                QVariant variant = m_files_model->data(child, QFileSystemModel::FileIconRole);
                m_gui_obj.lstRenames->item(idx)->setIcon(QIcon (variant.value<QIcon> ()));
            } else {
                m_gui_obj.lstRenames->item(idx)->setIcon(m_icon_warn);
            }
        } else {
            QVariant variant = m_files_model->data(child, QFileSystemModel::FileIconRole);
            m_gui_obj.lstRenames->item(idx)->setIcon(QIcon (variant.value<QIcon> ()));
        }
    }
    
}

void MPSMainWindow::load_global_settings ()
{
    m_gui_obj.editDefaultSeparators->setText(KDefaultSeparators);

    m_core_engine.set_default_separators(KDefaultSeparators.toStdWString());

    m_gui_obj.chkAlwaysLowcaseExt->setChecked(m_core_engine.is_always_lowcase_extension());

    m_gui_obj.chkEnableTransforms->setChecked(m_core_engine.is_apply_transforms());

    m_gui_obj.lstTransforms->clear();
    for (int idx = 0; idx < m_core_engine.count_transforms(); ++idx) {
        QString item_text = QString::number(idx + 1) + ". " + QString::fromStdWString(m_core_engine.get_transform_description_at(idx));
        m_gui_obj.lstTransforms->addItem(item_text);
    }
}

void MPSMainWindow::update_rename_and_discard_area ()
{
    bool enable_one = (
        m_core_engine.master_token() != 0 &&
        m_core_engine.master_token()->text() != m_core_engine.current_rename_to()
    );
    bool enable_all = m_core_engine.has_files_to_rename();

    m_gui_obj.btnRenameOne->setEnabled(enable_one);
    m_gui_obj.btnDiscardOne->setEnabled(enable_one);

    m_gui_obj.btnRenameAll->setEnabled(enable_all);
    m_gui_obj.btnDiscardAll->setEnabled(enable_all);

    bool has_entries = !m_core_engine.is_files_map_empty();
    m_gui_obj.btnListRenames->setEnabled(has_entries);
    m_gui_obj.chkFullList->setEnabled(has_entries);
}

void MPSMainWindow::update_selected_token_details ()
{
    const MPSToken* token = m_core_engine.selected_subtoken();
    if (!token) {
        //  clear & disable
        m_gui_obj.editSelectedTokenText->setText("");
        m_gui_obj.editSelectedTokenSeparators->setText("");
        m_gui_obj.grpSelectedTokenDetails->setEnabled(false);
        return;
    }

    bool is_root = m_core_engine.is_token_current_root(token);

    m_gui_obj.editSelectedTokenText->setText(QString::fromStdWString(token->text()));
    if (is_root)
        m_gui_obj.editSelectedTokenText->setCursorPosition(0);

    m_gui_obj.editSelectedTokenText->setReadOnly(is_root && !token->sub_tokens_empty());

    m_gui_obj.editSelectedTokenSeparators->setText(QString::fromStdWString(token->separators()));

    m_gui_obj.chkTokenDiscarded->setEnabled(!is_root);
    m_gui_obj.chkTokenDiscarded->setChecked(!token->is_discard());

    //  decide whether to enable the shifter buttons
    if (is_root) {
        m_gui_obj.btnShiftTokenLeft->setEnabled(false);
        m_gui_obj.btnShiftTokenRight->setEnabled(false);
    } else {
        update_shift_buttons();
    }

    m_gui_obj.grpSelectedTokenDetails->setEnabled(true);

    if (!is_root || (is_root && token->sub_tokens_empty()))
        m_gui_obj.editSelectedTokenText->setFocus();
    else
        m_gui_obj.editSelectedTokenSeparators->setFocus();
}

void MPSMainWindow::update_selected_token_transforms ()
{
    const MPSToken* token = m_core_engine.selected_subtoken();
    if (!token) {
        m_gui_obj.grpSelectedTokenTransforms->setEnabled(false);
        return;
    }

    bool is_root = m_core_engine.is_token_current_root(m_core_engine.selected_subtoken());
    m_gui_obj.btnAddRemLeft->setEnabled(!is_root);
    m_gui_obj.btnAddRemRight->setEnabled(!is_root);
    m_gui_obj.btnAddDot->setEnabled(!is_root);
    m_gui_obj.btnAddDash->setEnabled(!is_root);
    m_gui_obj.btnAddParanthesis->setEnabled(!is_root);
    m_gui_obj.btnAddBracket->setEnabled(!is_root);
    m_gui_obj.btnInsertLeft->setEnabled (!is_root);
    m_gui_obj.btnInsertRight->setEnabled (!is_root);
    m_gui_obj.editTextToInsert->setEnabled (!is_root);
    m_gui_obj.btnClearInsertText->setEnabled (!is_root);
    m_gui_obj.grpSelectedTokenTransforms->setEnabled(true);
}

void MPSMainWindow::update_shift_buttons ()
{
    const MPSToken* token = m_core_engine.selected_subtoken();
    if (!token)
        return;

    if (!token->parent())
        return;

    if (!token->parent()->sub_tokens_empty()) {
        bool is_first_subtoken = (*token->parent()->subtokens_const_begin()) == token;
        m_gui_obj.btnShiftTokenLeft->setEnabled(!is_first_subtoken);

        bool is_last_subtoken = token == token->parent()->last_subtoken();
        m_gui_obj.btnShiftTokenRight->setEnabled(!is_last_subtoken);
    } else {
        m_gui_obj.btnShiftTokenLeft->setEnabled(false);
        m_gui_obj.btnShiftTokenRight->setEnabled(false);
    }
}

void MPSMainWindow::update_transforms_buttons ()
{
    int row = m_gui_obj.lstTransforms->currentRow();

    m_gui_obj.btnRemoveTransform->setEnabled(row != -1);
    m_gui_obj.btnEditTransform->setEnabled(row != -1);
    m_gui_obj.btnEnableDisableTransform->setEnabled(row != -1);
    if (row >= 0 && row < m_gui_obj.lstTransforms->count()) {
        MPSTransformInfo info = m_core_engine.get_transform_information_at(row);
        m_gui_obj.btnEnableDisableTransform->setText(QString (info.enabled ? KDisableTransformText : KEnableTransformText));
    }
    m_gui_obj.btnMoveUpTransform->setEnabled(row != -1 && row > 0);
    m_gui_obj.btnMoveDownTransform->setEnabled(row != -1 && row < m_gui_obj.lstTransforms->count() - 1);
}

void MPSMainWindow::create_default_transforms ()
{
    int idx_cond_root       = m_core_engine.get_available_condition_index (KConditionIsRootID);
    int idx_cond_equals     = m_core_engine.get_available_condition_index (KConditionEqualsID);
    int idx_cond_true       = m_core_engine.get_available_condition_index (KConditionAlwaysID);
    int idx_cond_is_num     = m_core_engine.get_available_condition_index (KConditionIsNumericID);
    int idx_act_discard     = m_core_engine.get_available_action_index (KActionEnableDisableID);
    int idx_act_chg_case    = m_core_engine.get_available_action_index (KActionChangeCaseID);
    int idx_act_set_text    = m_core_engine.get_available_action_index (KActionSetTextID);
    int idx_act_rep_text    = m_core_engine.get_available_action_index (KActionReplaceInTextID);

    //  Apply recursive upcase first on root token
    MPSTransformInfo info;
    info.condition_index = idx_cond_root;
    info.action_index = idx_act_chg_case;
    info.action_checks[0] = true;       //  upcase?
    info.action_checks[1] = false;    //  all?
    info.action_checks[2] = true;     //  recursive?
    info.leafs_only = false;
    info.description = L"Upcase all words";
    m_core_engine.create_and_add_transform(info);

    //  discard numeric tokens like 01, 23, 568, etc...
    info.reset();
    info.condition_index = idx_cond_is_num;
    info.action_index = idx_act_discard;
    info.action_checks[0] = true;
    info.action_checks[3] = true;
    info.leafs_only = true;
    info.description = L"Disable all numbers";
    m_core_engine.create_and_add_transform(info);

    //  lowcase extensions (TBD in another way)
    info.reset();
    info.condition_index = idx_cond_equals;
    info.condition_text = L"mp3;m4a;flac;wav";
    info.action_index = idx_act_chg_case;
    info.action_checks[0] = false;  //  upcase?
    info.action_checks[1] = true; //  all?
    info.action_checks[2] = true; //  recursive?
    info.description = L"Lowcase extension (workaround)";
    m_core_engine.create_and_add_transform(info);

    //  upcase several strings
    info.reset();
    info.condition_index = idx_cond_equals;
    info.action_index = idx_act_chg_case;
    info.action_checks[0] = true; //  upcase?
    info.action_checks[1] = true; //  all?
    info.action_checks[2] = true; //  recursive?
    info.condition_text = L"dj;atb";
    info.description = L"Fully upcase 'dj', 'atb'";
    m_core_engine.create_and_add_transform(info);

    //  add dot after tokens in list
    info.reset();
    info.condition_index = idx_cond_equals;
    info.condition_text = L"feat;vs;pres;incl;ft;mr;dr";
    info.action_index = idx_act_set_text;
    info.action_texts[0] = L"[T].";
    info.description = L"Append dot to list of words";
    m_core_engine.create_and_add_transform(info);

    boost::tuples::tuple<std::wstring, std::wstring, std::wstring> tuples[] = {
        boost::make_tuple (L"hi-q",  L"Hi-Q",   L"Update hi-q to Hi-Q"),
        boost::make_tuple (L"nsync", L"N-Sync", L"Update nsync to N-Sync"),
        boost::make_tuple (L"youre", L"You're", L"Update youre to You're"),
        boost::make_tuple (L"its",   L"It's",   L"Update its to It's"),
        boost::make_tuple (L"mccoy", L"McCoy",  L"Update mccoy to McCoy"),
    };
    for (size_t idx = 0; idx < sizeof(tuples) / sizeof(*tuples); ++idx) {
        info.reset();
        info.condition_index = idx_cond_equals;
        info.action_index = idx_act_set_text;
        info.condition_text = boost::get<0> (tuples[idx]);
        info.action_texts[0] = boost::get<1> (tuples[idx]);
        info.description = boost::get<2> (tuples[idx]);
        m_core_engine.create_and_add_transform(info);
    }

    //  replace unwanted strings
    info.reset();
    info.condition_index = idx_cond_true;
    info.action_index = idx_act_rep_text;
    info.action_texts[0] = L"<text to search>";
    info.action_texts[1] = L"";
    info.description = L"Replace unwanted strings";
    m_core_engine.create_and_add_transform(info);

    //  discard Multiple
    info.reset();
    info.condition_index = idx_cond_equals;
    info.condition_text = L"www;radiofly;ws;(www;primemusic;CD;R;[pleer;Net];com];(PrimeMusic;cc);(NaitiMp3;Ru);";
    info.condition_checks[0] = false;
    info.action_index = idx_act_discard;
    info.action_checks[3] = true;
    info.description = L"Discard Multiple";
    m_core_engine.create_and_add_transform(info);
}

void MPSMainWindow::add_char_to_token (char one_char)
{
    if (m_core_engine.selected_subtoken() == 0)
        return;

    if (m_core_engine.is_token_current_root(m_core_engine.selected_subtoken()))
        return;

    std::wstring str_to_add;
    str_to_add += one_char;
    std::wstring text = m_core_engine.selected_subtoken()->text();

    //  check if we need to add the blank or not
    bool add_blank = (str_to_add.at(0) == L'-');
    int num_to_take = 1;
    if (add_blank) {
        if (m_gui_obj.btnAddRemLeft->isChecked())
            str_to_add += L' ';
        else if (m_gui_obj.btnAddRemRight->isChecked())
            str_to_add = L' ' + str_to_add;
        ++num_to_take;
    }

    if (!text.empty()) {
        if (m_gui_obj.btnAddRemLeft->isChecked()) {
            std::wstring first = text.substr(0, num_to_take);
            if (first != str_to_add)
                text = str_to_add + text;
            else
                text = text.substr(num_to_take);
        } else if (m_gui_obj.btnAddRemRight->isChecked()) {
            if (text.size() > 1) {
                //  taking last 2 characters, so need to have size >= 2
                std::wstring last_two = text.substr(text.length() - num_to_take, num_to_take);
                if (last_two != str_to_add) {
                    text += str_to_add;
                } else {
                    text = text.substr(0, text.length() - num_to_take);
                }
            } else  {   //  size can be only 1
                text += str_to_add;
            }
        }
        m_core_engine.update_selected_subtoken(text, m_core_engine.selected_subtoken()->separators(), m_core_engine.selected_subtoken()->is_discard());
    }

    m_scene->invalidate();
    m_gui_obj.editSelectedTokenText->setText(QString::fromStdWString(text));

    update_rename_to_entry();
    update_rename_and_discard_area();

    m_scene->clear();
    reconstruct_scene();
}

int MPSMainWindow::get_confirmation (const QString& message)
{
    return QMessageBox::warning(this, "Double check time!", message, QMessageBox::Yes, QMessageBox::Cancel);
}

void MPSMainWindow::reconstruct_scene ()
{
    m_scene->clear();

    //  add the "No file selected" text to the scene
    QFont font;
    font.setFamily ("Helvetica");
    font.setPixelSize (24);
    QString text = "[No File Selected]";
    m_txt_no_sel = m_scene->addText (text, font);
    m_txt_no_sel->setDefaultTextColor (Qt::white);
    m_txt_no_sel->setPos (-100, -35);
    m_txt_no_sel->setVisible (false);

    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    construct_scene (m_scene, m_core_engine.master_token(), x, y, w, h);
    m_scene->setSceneRect(0, -KInterTokenVerticalSpace, w, h + KInterTokenVerticalSpace);
}

void MPSMainWindow::construct_scene (
        QGraphicsScene* scene,
        const MPSToken* token,
        int& orig_x_offset,
        int& orig_y_offset,
        int& width,
        int& height
) {
    if (!token)
        return;

    bool select = (token == m_core_engine.selected_subtoken());

    if (token->sub_tokens_empty()) {
        //  construct the widget for the subtoken
        MPSTokenWidget* item = new MPSTokenWidget (
                    const_cast<MPSToken*> (token),
                    m_core_engine,
                    KInterTokenHorizontalSpace,
                    KInterTokenVerticalSpace);
        item->setPos(orig_x_offset, orig_y_offset);
        item->set_selected(select);
        scene->addItem(item);

        //  collect dimensions
        width = item->geometry().width();
        height = item->geometry().height();
    } else {
        //  construct group widget
        MPSTokenWidget* group = new MPSTokenWidget (
                    const_cast<MPSToken*> (token),
                    m_core_engine,
                    KInterTokenHorizontalSpace,
                    KInterTokenVerticalSpace);
        group->set_selected(select);
        scene->addItem(group);

        //  local version
        int x_offset = orig_x_offset;
        int y_offset = orig_y_offset + group->geometry().height() + KInterTokenVerticalSpace;
        int subtoken_width = 0;
        int subtoken_height = 0;

        int group_width = 0;
        int max_height = 0;

        //  store positions for connector lines
        QPointF first_subtoken_pos;
        QPointF last_subtoken_pos;

        //  iterate subtokens
        MPSTokensContainer::const_iterator iter = token->subtokens_const_begin();
        for ( ; iter != token->subtokens_const_end(); ++iter) {
            construct_scene(
                scene,
                (*iter), //iter->second,
                x_offset, y_offset,
                subtoken_width, subtoken_height
            );

            //  add connector line coming up from the subtoken
            scene->addLine(
                QLineF(
                    x_offset + subtoken_width / 2,
                    orig_y_offset + group->geometry().height() + KInterTokenVerticalSpace / 2,
                    x_offset + subtoken_width / 2,
                    orig_y_offset + group->geometry().height() + KInterTokenVerticalSpace
                ),
                QPen(Qt::white)
            );

            //  store first subtoken's pos for connector line
            if (iter == token->subtokens_const_begin()) {
                first_subtoken_pos.setX(x_offset + subtoken_width / 2);
                first_subtoken_pos.setY(y_offset - KInterTokenVerticalSpace / 2);
            }

            //  store last subtoken's pos for connector line
            if (*iter == token->last_subtoken()) {
                last_subtoken_pos.setX(x_offset + subtoken_width / 2);
                last_subtoken_pos.setY(y_offset - KInterTokenVerticalSpace / 2);
            }

            //  prepare X for next subtoken
            x_offset += subtoken_width + KInterTokenHorizontalSpace;

            group_width += subtoken_width + KInterTokenHorizontalSpace;
            if (max_height < subtoken_height)
                max_height = subtoken_height;
        }

        //  draw horizontal line connecting line coming down from group to lines coming up from subtokens
        if (first_subtoken_pos != last_subtoken_pos) {
            scene->addLine(
                QLineF(
                    first_subtoken_pos.x(),
                    first_subtoken_pos.y(),
                    last_subtoken_pos.x(),
                    last_subtoken_pos.y()
                ),
                QPen(Qt::white)
            );
        } else {
            scene->addLine(
                QLineF(
                    first_subtoken_pos.x(),
                    first_subtoken_pos.y(),
                    orig_x_offset + group->geometry().width() / 2,
                    orig_y_offset + group->geometry().height() + KInterTokenVerticalSpace / 2
                ),
                QPen(Qt::white)
            );
        }

        //  added 1 extra inter token space, subtract it if group width is positive
        if (group_width != 0)
            group_width -= KInterTokenHorizontalSpace;

        //  feedback group dimensions
        width = (group_width > group->geometry().width()) ? group_width : group->geometry().width();    //  pick the largest width
        height += group->geometry().height() + KInterTokenVerticalSpace + max_height;

        //  only now can we set the x position for the group and draw the connector in its right position
        group->setPos(orig_x_offset + width / 2 - group->geometry().width() / 2, orig_y_offset);

        //  add connector line going down from group subtoken
        scene->addLine(
            QLineF(
                group->x() + group->geometry().width() / 2,
                orig_y_offset + group->geometry().height(),
                group->x() + group->geometry().width() / 2,
                orig_y_offset + group->geometry().height() + KInterTokenVerticalSpace / 2
            ),
            QPen(Qt::white)
        );
    }
}

void MPSMainWindow::log_to_console (const QString& err_msg)
{
    QTextCursor text_cursor = QTextCursor (m_gui_obj.editConsole->document());
    text_cursor.movePosition(QTextCursor::End);
    text_cursor.insertText(">-" + err_msg + "\n");
    QScrollBar* bar = m_gui_obj.editConsole->verticalScrollBar ();
    bar->setValue (bar->maximum ());
}
/** END utility methods **/

/**  BEGIN event handlers **/
void MPSMainWindow::on_cmbDrives_currentIndexChanged(int index)
{
    change_drive(index);
}

void MPSMainWindow::on_tvFolders_clicked(const QModelIndex &index)
{
    //  update files tree view with new root path
    QString root_path = m_folders_model->fileInfo(index).absoluteFilePath();
    m_gui_obj.lstFiles->setRootIndex (m_files_model->setRootPath(root_path));
    m_gui_obj.lstRenames->clear ();

    if (m_gui_obj.editFolder->text() != root_path) {
        //  disable the lists until folder is completely loaded
        m_gui_obj.lstFiles->setEnabled(false);
        m_gui_obj.lstRenames->setEnabled(false);
    }

    // clear the scene & engine
    m_scene->clear();
    m_core_engine.clear_files_map();
    m_core_engine.select_master_token(L"");    //  force resetting the root index

    //  update list buttons only after clearing engine state
    update_rename_and_discard_area();

    update_selected_token_details();
    update_selected_token_transforms();

    //  update current folder
    m_gui_obj.editFolder->setText(QDir::toNativeSeparators(root_path));

    //  reset scroll bars
    m_gui_obj.lstFiles->verticalScrollBar()->setValue(0);
    m_gui_obj.lstFiles->horizontalScrollBar()->setValue(0);

    //  reset selection in lists
    m_gui_obj.lstFiles->setCurrentIndex(QModelIndex());
    m_gui_obj.lstRenames->setCurrentRow(-1);
}

void MPSMainWindow::on_lstFiles_entered(const QModelIndex &index)
{
    on_current_file_changed(index);
}

void MPSMainWindow::on_lstFiles_pressed(const QModelIndex &index)
{
    on_current_file_changed(index);
}

void MPSMainWindow::on_filesModelDirectoryLoaded (QString)
{
    //  sort the model before filling in the renames' list
    m_files_model->sort(0);

    update_renames_list ();

    m_gui_obj.lstFiles->setEnabled(true);
    m_gui_obj.lstRenames->setEnabled(true);
}

void MPSMainWindow::on_listFilesVerticalSliderValueChanged (int value)
{
    m_gui_obj.lstRenames->verticalScrollBar()->setValue(value);
}

void MPSMainWindow::on_lstFilesHorizontalSliderValueChanged (int value)
{
    m_gui_obj.lstRenames->horizontalScrollBar()->setValue(value);
}

void MPSMainWindow::on_lstRenameToVerticalSliderValueChanged (int value)
{
    m_gui_obj.lstFiles->verticalScrollBar()->setValue(value);
}

void MPSMainWindow::on_lstRenameToHorizontalSliderValueChanged (int value)
{
    m_gui_obj.lstFiles->horizontalScrollBar()->setValue(value);
}

void MPSMainWindow::on_btnAddTransform_clicked()
{
    MPSTransformInfo info;
    info.leafs_only = false;
    load_dialog_state("Add Transform", info);

    m_trans_dlg_state = EAddState;
    m_dlg_trans->show();
}

void MPSMainWindow::on_cmbConditionsIndexChanged (int idx)
{
    bool is_cond_equals = (idx == m_core_engine.get_available_condition_index(KConditionEqualsID));
    m_dlg_trans_edit_cond_text->setVisible(is_cond_equals);
    m_dlg_trans_chk_case_sens->setVisible(is_cond_equals);

    //  leafs only check
    bool can_use_leafs_only_check = (idx != -1 && idx != m_core_engine.get_available_condition_index(KConditionIsRootID));
    m_dlg_trans_chk_leafs_only->setChecked(can_use_leafs_only_check);
    m_dlg_trans_chk_leafs_only->setEnabled(can_use_leafs_only_check);

    //  save button
    QString msg;
    m_dlg_trans_btn_save->setEnabled(can_enable_save_button(msg));
    update_dialog_message(msg);

    if (is_cond_equals)
        m_dlg_trans_edit_cond_text->setFocus();
}

void MPSMainWindow::on_cmbActionsIndexChanged(int idx)
{
    //  action text
    int idx_act_set_sep = m_core_engine.get_available_action_index (KActionSetSeparatorsID);
    int idx_act_set_text = m_core_engine.get_available_action_index (KActionSetTextID);
    int idx_act_chg_case = m_core_engine.get_available_action_index (KActionChangeCaseID);
    int idx_act_ena_dis = m_core_engine.get_available_action_index (KActionEnableDisableID);
    int idx_act_insert = m_core_engine.get_available_action_index (KActionInsertSubtokenID);
    int idx_act_replace = m_core_engine.get_available_action_index (KActionReplaceInTextID);

    bool enable_act_text = (idx == idx_act_set_sep || idx == idx_act_set_text || idx == idx_act_insert || idx == idx_act_replace);
    m_dlg_trans_edit_act_text_1->setVisible (enable_act_text);
    m_dlg_trans_edit_act_text_2->setVisible (idx == idx_act_replace);

    //  discard checkbox
    bool is_discard = (idx == idx_act_ena_dis);
    m_dlg_trans_chk_disable->setVisible(is_discard);

    //  case checkboxes
    bool is_change_case = (idx == idx_act_chg_case);
    m_dlg_trans_chk_upcase->setVisible(is_change_case);
    m_dlg_trans_chk_case_all->setVisible(is_change_case);
    m_dlg_trans_chk_recursive->setVisible(is_change_case);

    //  insert subtoken position edit
    m_dlg_trans_edit_pos->setVisible(idx == idx_act_insert);

    QString msg;
    m_dlg_trans_btn_save->setEnabled(can_enable_save_button(msg));
    update_dialog_message(msg);

    if (enable_act_text)
        m_dlg_trans_edit_act_text_1->setFocus();
}

void MPSMainWindow::on_editCondTextTextEdited (const QString& text)
{
    Q_UNUSED(text);
    QString msg;
    m_dlg_trans_btn_save->setEnabled(can_enable_save_button(msg));
    update_dialog_message(msg);
}

void MPSMainWindow::on_editActionsTextTextEdited (const QString& text)
{
    Q_UNUSED(text);
    QString msg;
    m_dlg_trans_btn_save->setEnabled(can_enable_save_button(msg));
    update_dialog_message(msg);
}

void MPSMainWindow::on_editActionsPosTextChanged (const QString& text)
{
    Q_UNUSED(text);
    QString msg;
    m_dlg_trans_btn_save->setEnabled(can_enable_save_button(msg));
    update_dialog_message(msg);
}

void MPSMainWindow::update_dialog_message (const QString& msg)
{
    m_dlg_trans_lbl_msg->setText(KRedColorTemplate.arg(msg));
    m_dlg_trans_lbl_msg->hide();
    m_dlg_trans_lbl_msg->show();
}

void MPSMainWindow::on_btnSaveClicked()
{
    //  construct the information out of the dialog state
    MPSTransformInfo info;
    info.condition_index = m_dlg_trans_cmb_condition->currentIndex();
    info.condition_text = m_dlg_trans_edit_cond_text->text().toStdWString();
    info.condition_checks[0] = m_dlg_trans_chk_case_sens->isChecked();
    info.action_index = m_dlg_trans_cmb_action->currentIndex();
    info.action_texts[0] = m_dlg_trans_edit_act_text_1->text().toStdWString();
    info.action_texts[1] = m_dlg_trans_edit_act_text_2->text().toStdWString();
    info.action_checks[0] = m_dlg_trans_chk_upcase->isChecked();
    info.action_checks[1] = m_dlg_trans_chk_case_all->isChecked();
    info.action_checks[2] = m_dlg_trans_chk_recursive->isChecked();
    info.action_checks[3] = m_dlg_trans_chk_disable->isChecked();
    info.action_position = m_dlg_trans_edit_pos->text().toInt();
    info.description = m_dlg_trans_edit_desc->text().toStdWString();
    info.leafs_only = m_dlg_trans_chk_leafs_only->isChecked();
    info.enabled = m_dlg_trans_chk_enabled->isChecked();

    int modified_row = -1;
    if (m_trans_dlg_state == EAddState) {
        m_core_engine.create_and_add_transform(info);
        QString item_text = QString::number(m_core_engine.count_transforms()) + ". " + QString::fromStdWString(m_core_engine.get_transform_description_at(m_core_engine.count_transforms() - 1));
        m_gui_obj.lstTransforms->addItem (item_text);
        modified_row = m_gui_obj.lstTransforms->count () - 1;
    } else if (m_trans_dlg_state == EEditState) {
        modified_row = m_gui_obj.lstTransforms->currentRow();
        m_core_engine.update_transform_information_at(modified_row, info);
        QString item_text = QString::number(modified_row + 1) + ". " + QString::fromStdWString(m_core_engine.get_transform_description_at(modified_row));
        m_gui_obj.lstTransforms->currentItem()->setText(item_text);
    }
    if (modified_row != -1) {
        //  update the button
        m_gui_obj.btnEnableDisableTransform->setText(QString (info.enabled ? KDisableTransformText : KEnableTransformText));

        //  update the line
        QFont font = m_gui_obj.lstTransforms->item(modified_row)->font();
        font.setStrikeOut(!info.enabled);
        m_gui_obj.lstTransforms->item(modified_row)->setFont(font);
    }

    m_dlg_trans->close();
}

void MPSMainWindow::on_btnCloseClicked()
{
    m_dlg_trans->close();
}

bool MPSMainWindow::can_enable_save_button (QString& red_message) const
{
    red_message = "";

    //  get condition & action indices
    int cond_idx = m_dlg_trans_cmb_condition->currentIndex();
    int act_idx = m_dlg_trans_cmb_action->currentIndex();

    int idx_always = m_core_engine.get_available_condition_index(KConditionAlwaysID);
    int idx_equals = m_core_engine.get_available_condition_index(KConditionEqualsID);
    int idx_is_root = m_core_engine.get_available_condition_index(KConditionIsRootID);

    int idx_disc = m_core_engine.get_available_action_index(KActionEnableDisableID);
    int idx_set_text = m_core_engine.get_available_action_index(KActionSetTextID);
    int idx_insert = m_core_engine.get_available_action_index(KActionInsertSubtokenID);

    //  indices must not be -1
    bool can_enable = (cond_idx != -1 && act_idx != -1);
    if (!can_enable) {
        red_message = "Select one condition and one action.";
        return can_enable;
    }

    //  if we're on condition equals, then the text cannot be null
    can_enable = can_enable && (
        (cond_idx != idx_equals) ||
        ((cond_idx == idx_equals) && (m_dlg_trans_edit_cond_text->text() != ""))
    );
    if (!can_enable) {
        red_message = "When the selected condition is [";
        red_message += KConditionEqualsID.c_str();
        red_message += "], the condition text cannot be empty.";
        return can_enable;
    }

    //  if we're on action discard, the condition cannot be (IS ROOT)
    can_enable = can_enable && (
        (cond_idx != idx_is_root) ||
        ((cond_idx == idx_is_root) && (act_idx != idx_disc))
    );
    if (!can_enable) {
        red_message = "When the selected condition is [";
        red_message += KConditionIsRootID.c_str();
        red_message += "], the action cannot be [";
        red_message += KActionEnableDisableID.c_str();
        red_message += "].";
        return can_enable;
    }

    //  if we're on action set text, then text cannot be empty
    can_enable = can_enable && (
        (act_idx != idx_set_text) ||
        ((act_idx == idx_set_text) && (m_dlg_trans_edit_act_text_1->text() != ""))
    );
    if (!can_enable) {
        red_message = "When the selected action is [";
        red_message += KActionSetTextID.c_str();
        red_message += "], the action text cannot be empty.";
        return can_enable;
    }

    //  if we're on action Insert Subtoken, the condition cannot be Always
    can_enable = can_enable && (
        (act_idx != idx_insert) ||
        ((act_idx == idx_insert) && (cond_idx != idx_always))
    );
    if (!can_enable) {
        red_message = "When the selected condition is [";
        red_message += KConditionAlwaysID.c_str();
        red_message += "], the action cannot be [";
        red_message += KActionInsertSubtokenID.c_str();
        red_message += "].";
        return can_enable;
    }

    //  if we're on action Insert Subtoken, then text cannot be empty
    can_enable = can_enable && (
        (act_idx != idx_insert) ||
        ((act_idx == idx_insert) && (m_dlg_trans_edit_act_text_1->text() != ""))
    );
    if (!can_enable) {
        red_message = "When the selected action is [";
        red_message += KActionInsertSubtokenID.c_str();
        red_message += "], the action text cannot be empty.";
        return can_enable;
    }

    //  if we're on action Insert Subtoken, the position cannot be empty
    can_enable = can_enable && (
        (act_idx != idx_insert) ||
        ((act_idx == idx_insert) && (m_dlg_trans_edit_pos->text() != ""))
    );
    if (!can_enable) {
        red_message = "When the selected action is [";
        red_message += KActionInsertSubtokenID.c_str();
        red_message += "], the position cannot be empty.";
        return can_enable;
    }

    return can_enable;
}

void MPSMainWindow::on_btnEditTransform_clicked()
{
    int idx = m_gui_obj.lstTransforms->currentRow();
    if (idx == -1)
        return;

    MPSTransformInfo info = m_core_engine.get_transform_information_at(idx);
    load_dialog_state("Edit Transform", info);

    QString msg;
    m_dlg_trans_btn_save->setEnabled(can_enable_save_button(msg));
    update_dialog_message(msg);

    m_trans_dlg_state = EEditState;
    m_dlg_trans->show();
}

void MPSMainWindow::on_btnRemoveTransform_clicked()
{
    int idx = m_gui_obj.lstTransforms->currentRow();

    //  remove the list item
    delete m_gui_obj.lstTransforms->takeItem(m_gui_obj.lstTransforms->currentRow());

    //  remove it from the engine
    m_core_engine.remove_transform_at(idx);

    //  go and update the counter on next transforms
    for (int kdx = (idx > 0 ? idx - 1 : 0); kdx < m_core_engine.count_transforms (); ++kdx) {
        QString item_text = QString::number(kdx + 1) + ". " + QString::fromStdWString(m_core_engine.get_transform_description_at(kdx));
        m_gui_obj.lstTransforms->item (kdx)->setText (item_text);
    }
}

void MPSMainWindow::on_btnClearTransforms_clicked()
{
    if (get_confirmation(KClearTransformsMessage) == QMessageBox::Cancel)
        return;

    m_core_engine.clear_transforms();
    m_gui_obj.lstTransforms->clear();
}

void MPSMainWindow::on_lstTransforms_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    on_btnEditTransform_clicked();
}

void MPSMainWindow::on_editSelectedTokenText_returnPressed()
{
    if (m_gui_obj.editSelectedTokenText->text().isEmpty())
        return;

    m_core_engine.update_selected_subtoken(
                m_gui_obj.editSelectedTokenText->text().toStdWString(),
                m_core_engine.selected_subtoken()->separators(),
                m_core_engine.selected_subtoken()->is_discard()
    );
    reconstruct_scene();

    update_rename_to_entry();

    update_rename_and_discard_area();
}

void MPSMainWindow::on_editSelectedTokenSeparators_returnPressed()
{
    m_core_engine.update_selected_subtoken(
        m_core_engine.selected_subtoken()->text(),
        m_gui_obj.editSelectedTokenSeparators->text().toStdWString(),
        m_core_engine.selected_subtoken()->is_discard()
    );
    reconstruct_scene();

    update_rename_to_entry();

    update_rename_and_discard_area();
}

void MPSMainWindow::on_btnClearTokenSeparators_clicked()
{
    m_core_engine.update_selected_subtoken(
        m_core_engine.selected_subtoken()->text(),
        L"",
        m_core_engine.selected_subtoken()->is_discard()
    );

    m_gui_obj.editSelectedTokenSeparators->setText("");

    //  need to reconstruct the scene
    reconstruct_scene();
    m_scene->invalidate();

    update_rename_to_entry();

    update_rename_and_discard_area();
}

void MPSMainWindow::on_btnDefaultTokenSeparators_clicked()
{
    m_gui_obj.editSelectedTokenSeparators->setText(m_gui_obj.editDefaultSeparators->text());
    m_core_engine.update_selected_subtoken(
                m_core_engine.selected_subtoken()->text(),
                m_gui_obj.editSelectedTokenSeparators->text().toStdWString(),
                m_core_engine.selected_subtoken()->is_discard(),
                true    //  force the update
    );
    reconstruct_scene();

    update_rename_to_entry();

    update_rename_and_discard_area();
}

void MPSMainWindow::on_btnShiftTokenLeft_clicked()
{
    m_core_engine.shift_selected_subtoken(ELeft);
    reconstruct_scene();
    update_shift_buttons();

    update_rename_to_entry();

    update_rename_and_discard_area();
}

void MPSMainWindow::on_btnShiftTokenRight_clicked()
{
    m_core_engine.shift_selected_subtoken(ERight);
    reconstruct_scene();
    update_shift_buttons();

    update_rename_to_entry();

    update_rename_and_discard_area();
}

void MPSMainWindow::on_chkTokenDiscarded_stateChanged(int)
{
    m_core_engine.update_selected_subtoken(
                m_core_engine.selected_subtoken()->text(),
                m_core_engine.selected_subtoken()->separators(),
                !m_gui_obj.chkTokenDiscarded->isChecked()
    );

    update_rename_to_entry();

    m_scene->invalidate();
}

void MPSMainWindow::on_btnDiscardOne_clicked()
{
    //  get the index for the selected item
    int current_row = m_gui_obj.lstFiles->currentIndex().row();
    if (current_row == -1)
        return;

    //  force reselect & update
    std::wstring file_name = m_files_model->fileName(m_gui_obj.lstFiles->currentIndex()).toStdWString();
    m_core_engine.select_master_token (file_name);
    m_core_engine.update_selected_subtoken(m_core_engine.master_token()->text(), L"", false);

    update_rename_to_entry();

    //  other updates
    reconstruct_scene();
    update_selected_token_details();
    update_rename_and_discard_area();
}

void MPSMainWindow::on_btnDiscardAll_clicked()
{
    if (get_confirmation(KDiscardAllMessage) == QMessageBox::Cancel)
        return;    

    //  clear engine's internal renaming map
    m_core_engine.clear_files_map();

    if (m_gui_obj.lstFiles->currentIndex() != QModelIndex ()) {
        //  reselect current root token
        m_core_engine.select_master_token(
            m_files_model->fileName(m_gui_obj.lstFiles->currentIndex()).toStdWString()
        );
    }

    //  remove the separators from the root token
    if (m_core_engine.master_token())
        m_core_engine.update_selected_subtoken(m_core_engine.master_token()->text(), L"", false, true);

    reconstruct_scene();

    update_selected_token_details();
    update_rename_and_discard_area();

    //  rebuild the entire list of renames
    update_renames_list();
}

void MPSMainWindow::on_btnRenameOne_clicked()
{
    //  rename only selected, force map entry update
    bool result = m_core_engine.rename_one( QDir::toNativeSeparators(m_files_model->rootPath()).toStdWString(),
                                            m_core_engine.master_token()->text(),
                                            m_core_engine.current_rename_to(),
                                            true);

    //  print rename errors from engine's messages queue
    while (m_core_engine.has_messages()) {
        log_to_console(QString::fromStdWString(m_core_engine.pop_message()));
    }

    if (!result)
        return;

    //  deselect in engine and lists
    m_core_engine.select_master_token(L"");
    m_gui_obj.lstFiles->setCurrentIndex(QModelIndex());
    m_gui_obj.lstRenames->setCurrentRow(-1);

    //  reconstruct scene and update gui
    reconstruct_scene();
    update_rename_and_discard_area();
    update_selected_token_details();
    update_selected_token_transforms ();

    m_gui_obj.lstFiles->setFocus ();
}

void MPSMainWindow::on_btnRenameAll_clicked()
{
    if (get_confirmation(KRenameAllMessage) == QMessageBox::Cancel)
            return;

    m_core_engine.rename_all(QDir::toNativeSeparators(m_files_model->rootPath()).toStdWString());

    //  deselect in engine and lists
    m_core_engine.select_master_token(L"");
    m_gui_obj.lstFiles->setCurrentIndex(QModelIndex());
    m_gui_obj.lstRenames->setCurrentRow(-1);

    //  reconstruct scene and update gui
    reconstruct_scene();
    update_rename_and_discard_area();
    update_selected_token_details();
    update_selected_token_transforms ();

    //  rebuild the list of renames
    update_renames_list();

    //  print rename errors using engine's errors queue
    bool has_mess = m_core_engine.has_messages ();
    if (has_mess)
        log_to_console ("[START RENAME BATCH]");
    while (m_core_engine.has_messages()) {
        log_to_console(QString::fromStdWString(m_core_engine.pop_message()));
    }
    if (has_mess)
        log_to_console ("[END RENAME BATCH]");

    m_gui_obj.lstFiles->setFocus ();
}

void MPSMainWindow::on_btnUpcase_clicked()
{
    bool first_only = m_gui_obj.chkFirstOnly->isChecked();
    bool recursive = m_gui_obj.chkCaseRecursive->isChecked();

    m_core_engine.change_case(true, first_only, recursive);
    m_scene->invalidate();
    int prev_pos = m_gui_obj.editSelectedTokenText->cursorPosition();
    m_gui_obj.editSelectedTokenText->setText(QString::fromStdWString(m_core_engine.selected_subtoken()->text()));
    m_gui_obj.editSelectedTokenText->setCursorPosition(prev_pos);

    update_rename_to_entry();

    m_scene->clear();
    reconstruct_scene();

    update_rename_and_discard_area();
}

void MPSMainWindow::on_btnLowcase_clicked()
{
    bool first_only = m_gui_obj.chkFirstOnly->isChecked();
    bool recursive = m_gui_obj.chkCaseRecursive->isChecked();

    m_core_engine.change_case(false, first_only, recursive);
    m_scene->invalidate();
    int prev_pos = m_gui_obj.editSelectedTokenText->cursorPosition();
    m_gui_obj.editSelectedTokenText->setText(QString::fromStdWString(m_core_engine.selected_subtoken()->text()));
    m_gui_obj.editSelectedTokenText->setCursorPosition(prev_pos);

    update_rename_to_entry();

    m_scene->clear();
    reconstruct_scene();

    update_rename_and_discard_area();
}

void MPSMainWindow::on_chkEnableTransforms_stateChanged(int)
{
    bool enable = m_gui_obj.chkEnableTransforms->isChecked();
    m_core_engine.set_apply_transforms(enable);
    m_gui_obj.lstTransforms->setEnabled(enable);

    //  enable or disable the trans buttons
    m_gui_obj.btnAddTransform->setEnabled(enable);
    m_gui_obj.btnRemoveTransform->setEnabled(enable);
    m_gui_obj.btnEditTransform->setEnabled(enable);
    m_gui_obj.btnLoadTransChain->setEnabled (enable);
    m_gui_obj.btnClearTransforms->setEnabled(enable);
}

void MPSMainWindow::on_editDefaultSeparators_textEdited(const QString&)
{
    m_core_engine.set_default_separators(m_gui_obj.editDefaultSeparators->text().toStdWString());
}

void MPSMainWindow::on_btnEnableDisableTransform_clicked()
{
    int row = m_gui_obj.lstTransforms->currentRow();
    if (row == -1)
        return;

    //  update the transforms
    m_core_engine.toggle_transform_enabled_at(row);

    //  update the button
    MPSTransformInfo info = m_core_engine.get_transform_information_at(row);
    m_gui_obj.btnEnableDisableTransform->setText(QString (info.enabled ? KDisableTransformText : KEnableTransformText));

    //  update the line
    QFont font = m_gui_obj.lstTransforms->item(row)->font();
    font.setStrikeOut(!font.strikeOut());
    m_gui_obj.lstTransforms->item(row)->setFont(font);
}

void MPSMainWindow::on_btnMoveUpTransform_clicked()
{
    int row = m_gui_obj.lstTransforms->currentRow();
    if (row == -1)
        return;
    m_core_engine.move_up_transform_at(row);

    //  update list entry
    if (row > 0) {
        m_gui_obj.lstTransforms->item(row)->setText(QString::number(row + 1) + ". " + QString::fromStdWString(m_core_engine.get_transform_description_at(row)));
        m_gui_obj.lstTransforms->item(row - 1)->setText(QString::number(row) + ". " + QString::fromStdWString(m_core_engine.get_transform_description_at(row - 1)));
        m_gui_obj.lstTransforms->setCurrentRow(row - 1);
    }

    update_transforms_buttons();
}

void MPSMainWindow::on_btnMoveDownTransform_clicked()
{
    int row = m_gui_obj.lstTransforms->currentRow();
    if (row == -1)
        return;

    m_core_engine.move_down_transform_at(row);

    //  update list entry
    if (row >= 0 && row < m_gui_obj.lstTransforms->count() - 1) {
        m_gui_obj.lstTransforms->item(row)->setText(QString::number(row + 1) + ". " + QString::fromStdWString(m_core_engine.get_transform_description_at(row)));
        m_gui_obj.lstTransforms->item(row + 1)->setText(QString::number(row + 2) + ". " + QString::fromStdWString(m_core_engine.get_transform_description_at(row + 1)));
        m_gui_obj.lstTransforms->setCurrentRow(row + 1);
    }

    update_transforms_buttons();
}

void MPSMainWindow::on_lstTransforms_currentRowChanged(int)
{
    update_transforms_buttons();
}

void MPSMainWindow::on_btnAddRemLeft_toggled(bool checked)
{
    m_gui_obj.btnAddRemRight->setChecked(!checked);
}

void MPSMainWindow::on_btnAddRemRight_toggled(bool checked)
{
    m_gui_obj.btnAddRemLeft->setChecked(!checked);
}

void MPSMainWindow::on_btnAddDot_clicked()
{
    add_char_to_token('.');
}

void MPSMainWindow::on_btnAddDash_clicked()
{
    add_char_to_token('-');
}

void MPSMainWindow::on_btnAddParanthesis_clicked()
{
    if (m_gui_obj.btnAddRemLeft->isChecked()) {
        add_char_to_token('(');
    } else if (m_gui_obj.btnAddRemRight->isChecked()) {
        add_char_to_token(')');
    }
}

void MPSMainWindow::on_btnAddBracket_clicked()
{
    if (m_gui_obj.btnAddRemLeft->isChecked()) {
        add_char_to_token('[');
    } else if (m_gui_obj.btnAddRemRight->isChecked()) {
        add_char_to_token(']');
    }
}

void MPSMainWindow::on_btnClearConsole_clicked()
{
    m_gui_obj.editConsole->clear();
}

void MPSMainWindow::on_btnListRenames_clicked()
{
    bool map_has_entries = false;

    QString rootPath = m_files_model->rootPath();    
    QModelIndex modelIndex = m_files_model->index(rootPath);
    int colCount = m_files_model->columnCount(modelIndex);
    int rowCount = m_files_model->rowCount(modelIndex);
    QString fileName;   //  child fileName

    QModelIndex jx = m_gui_obj.tvFolders->currentIndex();
    QString rupa = m_folders_model->rootPath();
    int a = 3;
    /*
    QVector<QString> vec;
    for (int row = 0; row < rowCount; ++row)
    {
            //  coloana 0 are informatia necesara
        QModelIndex child = m_files_model->index(row, 0, modelIndex);
        fileName = m_files_model->fileName(child);
        vec.append(fileName);                    
    }
    */

    //  the file name we're looking for is on col 0/4 (file size is also there on one of the columns)
    int col = 0;
    modelIndex = m_files_model->index(m_files_model->rootPath());
    for (int idx = 0; idx < m_files_model->rowCount(modelIndex); ++idx) {
        //  must pass a parent to the index call
        QModelIndex child = m_files_model->index(idx, col, modelIndex);

        //  get the file name
        fileName = m_files_model->fileName(child);

        //  check the entry in engine's map
        std::wstring rename_to;
        if (m_core_engine.has_rename_to(fileName.toStdWString(), rename_to)) {
            QString qstr_rename_to = QString::fromStdWString(rename_to);

            if (fileName != qstr_rename_to) {
                QString msg = "Can rename \"";
                msg += fileName;
                msg += "\" to \"";
                msg += qstr_rename_to;
                msg += "\".";
                log_to_console(msg);
                if (!map_has_entries)
                    map_has_entries = true;
            } else if (m_gui_obj.chkFullList->isChecked()) {
                QString msg = "Identical entry for \"";
                msg += fileName;
                msg += "\".";
                log_to_console(msg);
                if (!map_has_entries)
                    map_has_entries = true;
            }
        }
    }
    if (!map_has_entries)
        log_to_console("No renames.");
}

void MPSMainWindow::on_btnInsertLeft_clicked()
{
    if (m_gui_obj.editTextToInsert->text ().isEmpty ())
        return;

    m_core_engine.insert_text (m_gui_obj.editTextToInsert->text ().toStdWString (), ELeft);

    m_scene->invalidate();
    update_rename_to_entry();
    m_scene->clear();
    reconstruct_scene();

    update_rename_and_discard_area();
}

void MPSMainWindow::on_btnInsertRight_clicked()
{
    if (m_gui_obj.editTextToInsert->text ().isEmpty ())
        return;

    m_core_engine.insert_text (m_gui_obj.editTextToInsert->text ().toStdWString (), ERight);

    m_scene->invalidate();
    update_rename_to_entry();
    m_scene->clear();
    reconstruct_scene();

    update_rename_and_discard_area();
}

void MPSMainWindow::on_btnClearInsertText_clicked()
{
    m_gui_obj.editTextToInsert->clear ();
    m_gui_obj.editTextToInsert->setFocus ();
}

void MPSMainWindow::on_btnLoadTransChain_clicked()
{
    if ((m_gui_obj.lstTransforms->count () > 0) &&
        get_confirmation(KLoadDefaultTransformsMessage) == QMessageBox::Cancel)
        return;

    m_core_engine.clear_transforms ();
    m_gui_obj.lstTransforms->clear();

    create_default_transforms ();
    for (int idx = 0; idx < m_core_engine.count_transforms(); ++idx) {
        QString item_text = QString::number(idx + 1) + ". " + QString::fromStdWString(m_core_engine.get_transform_description_at(idx));
        m_gui_obj.lstTransforms->addItem(item_text);
    }
}

/**  END event handlers **/

void MPSMainWindow::on_close(QCloseEvent *event)
{
    
    event->accept();
}