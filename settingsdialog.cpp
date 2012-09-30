/*
 *      settingsdialog.cpp
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This file is part of gpg4usb.
 *
 *      Gpg4usb is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      Gpg4usb is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with gpg4usb.  If not, see <http://www.gnu.org/licenses/>
 */

#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    tabWidget = new QTabWidget;
    generalTab = new GeneralTab;
    appearanceTab = new AppearanceTab;
    mimeTab = new MimeTab;
    keyserverTab = new KeyserverTab;
    advancedTab = new AdvancedTab;
    gpgPathsTab = new GpgPathsTab;

    tabWidget->addTab(generalTab, tr("General"));
    tabWidget->addTab(appearanceTab, tr("Appearance"));
    tabWidget->addTab(mimeTab, tr("PGP/Mime"));
    tabWidget->addTab(keyserverTab, tr("Keyserver"));
    tabWidget->addTab(gpgPathsTab, tr("Gpg paths"));
    tabWidget->addTab(advancedTab, tr("Advanced"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Settings"));

    exec(); 
}

void SettingsDialog::slotAccept()
{
    generalTab->applySettings();
    mimeTab->applySettings();
    appearanceTab->applySettings();
    keyserverTab->applySettings();
    advancedTab->applySettings();
    gpgPathsTab->applySettings();
    close();
}

// http://www.informit.com/articles/article.aspx?p=1405555&seqNum=3
// http://developer.qt.nokia.com/wiki/How_to_create_a_multi_language_application
QHash<QString, QString> SettingsDialog::listLanguages()
{
    QHash<QString, QString> languages;

    languages.insert("", tr("System Default"));

    QString appPath = qApp->applicationDirPath();
    QDir qmDir = QDir(appPath + "/ts/");
    QStringList fileNames =
            qmDir.entryList(QStringList("gpg4usb_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i) {
        QString locale = fileNames[i];
        locale.truncate(locale.lastIndexOf('.'));
        locale.remove(0, locale.indexOf('_') + 1);

        // this works in qt 4.8
        QLocale qloc(locale);
        #if QT_VERSION < 0x040800
                        QString language =  QLocale::languageToString(qloc.language()) +" (" + locale + ")"; //+ " (" + QLocale::languageToString(qloc.language()) + ")";
        #else
                        QString language =  qloc.nativeLanguageName() +" (" + locale + ")"; //+ " (" + QLocale::languageToString(qloc.language()) + ")";
        #endif
        languages.insert(locale, language);
    }
    return languages;
}



GeneralTab::GeneralTab(QWidget *parent)
    : QWidget(parent)
{

    /*****************************************
     * remember Password-Box
     *****************************************/
    QGroupBox *rememberPasswordBox = new QGroupBox(tr("Remember Password"));
    QHBoxLayout *rememberPasswordBoxLayout = new QHBoxLayout();
    rememberPasswordCheckBox = new QCheckBox(tr("Remember password until closing gpg4usb"), this);
    rememberPasswordBoxLayout->addWidget(rememberPasswordCheckBox);
    rememberPasswordBox->setLayout(rememberPasswordBoxLayout);

    /*****************************************
     * Save-Checked-Keys-Box
     *****************************************/
    QGroupBox *saveCheckedKeysBox = new QGroupBox(tr("Save Checked Keys"));
    QHBoxLayout *saveCheckedKeysBoxLayout = new QHBoxLayout();
    saveCheckedKeysCheckBox = new QCheckBox(tr("Save checked private keys on exit and restore them on next start."), this);
    saveCheckedKeysBoxLayout->addWidget(saveCheckedKeysCheckBox);
    saveCheckedKeysBox->setLayout(saveCheckedKeysBoxLayout);

    /*****************************************
     * Key-Impport-Confirmation Box
     *****************************************/
    QGroupBox *importConfirmationBox = new QGroupBox(tr("Confirm drag'n'drop key import"));
    QHBoxLayout *importConfirmationBoxLayout = new QHBoxLayout();
    importConfirmationCheckBox= new QCheckBox(tr("Import files dropped on the keylist without confirmation."), this);
    importConfirmationBoxLayout->addWidget(importConfirmationCheckBox);
    importConfirmationBox->setLayout(importConfirmationBoxLayout);

    /*****************************************
     * Language Select Box
     *****************************************/
    QGroupBox *langBox = new QGroupBox(tr("Language"));
    QVBoxLayout *langBoxLayout = new QVBoxLayout();
    langSelectBox = new QComboBox;
    lang = SettingsDialog::listLanguages();

    foreach(QString l , lang) {
        langSelectBox->addItem(l);
    }

    langBoxLayout->addWidget(langSelectBox);
    langBoxLayout->addWidget(new QLabel(tr("<b>NOTE: </b> Gpg4usb will restart automatically if you change the language!")));
    langBox->setLayout(langBoxLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(rememberPasswordBox);
    mainLayout->addWidget(saveCheckedKeysBox);
    mainLayout->addWidget(importConfirmationBox);
    mainLayout->addWidget(langBox);
    setSettings();
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

/**********************************
 * Read the settings from config
 * and set the buttons and checkboxes
 * appropriately
 **********************************/
void GeneralTab::setSettings()
{
    QSettings settings;
    // Keysaving
    if (settings.value("keys/keySave").toBool()) {
        saveCheckedKeysCheckBox->setCheckState(Qt::Checked);
    }

    // Remember Password
    if (settings.value("general/rememberPassword").toBool()) {
        rememberPasswordCheckBox->setCheckState(Qt::Checked);
    }

    // Language setting
    QString langKey = settings.value("int/lang").toString();
    QString langValue = lang.value(langKey);
    if (langKey != "") {
        langSelectBox->setCurrentIndex(langSelectBox->findText(langValue));
    }
    // Ask for confirmation to import, if keyfiles are dropped on keylist
    if (settings.value("general/confirmImportKeys",Qt::Checked).toBool()){
        importConfirmationCheckBox->setCheckState(Qt::Checked);
    }
}

/***********************************
  * get the values of the buttons and
  * write them to settings-file
  *************************************/
void GeneralTab::applySettings()
{
    QSettings settings;
    settings.setValue("keys/keySave", saveCheckedKeysCheckBox->isChecked());
    // TODO: clear passwordCache instantly on unset rememberPassword
    settings.setValue("general/rememberPassword", rememberPasswordCheckBox->isChecked());
    settings.setValue("int/lang", lang.key(langSelectBox->currentText()));
    settings.setValue("general/confirmImportKeys", importConfirmationCheckBox->isChecked());
}

MimeTab::MimeTab(QWidget *parent)
    : QWidget(parent)
{
    /*****************************************
     * MIME-Parsing-Box
     *****************************************/
    QGroupBox *mimeQPBox = new QGroupBox(tr("Decode quoted printable"));
    QVBoxLayout  *mimeQPBoxLayout = new QVBoxLayout();
    mimeQPCheckBox = new QCheckBox(tr("Try to recognize quoted printable."), this);
    mimeQPBoxLayout->addWidget(mimeQPCheckBox);
    mimeQPBox->setLayout(mimeQPBoxLayout);

    QGroupBox *mimeParseBox = new QGroupBox(tr("Parse PGP/MIME (Experimental)"));
    QVBoxLayout  *mimeParseBoxLayout = new QVBoxLayout();
    mimeParseCheckBox = new QCheckBox(tr("Try to split attachments from PGP-MIME ecrypted messages."), this);
    mimeParseBoxLayout->addWidget(mimeParseCheckBox);
    mimeParseBox->setLayout(mimeParseBoxLayout);

    QGroupBox *mimeOpenAttachmentBox = new QGroupBox(tr("Open with external application (Experimental)"));
    QVBoxLayout  *mimeOpenAttachmentBoxLayout = new QVBoxLayout();
    QLabel *mimeOpenAttachmentText = new QLabel(tr("Open attachments with default application for the filetype.<br> "
                                                   "There are at least two possible problems with this behaviour:"
                                                   "<ol><li>File needs to be saved unencrypted to attachments folder.<br> "
                                                   "Its your job to clean this folder.</li>"
                                                   "<li>The external application may have its own temp files.</li></ol>"));

    //mimeOpenAttachmentBox->setDisabled(true);
    mimeOpenAttachmentCheckBox = new QCheckBox(tr("Enable opening with external applications."), this);

    mimeOpenAttachmentBoxLayout->addWidget(mimeOpenAttachmentText);
    mimeOpenAttachmentBoxLayout->addWidget(mimeOpenAttachmentCheckBox);
    mimeOpenAttachmentBox->setLayout(mimeOpenAttachmentBoxLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(mimeParseBox);
    mainLayout->addWidget(mimeOpenAttachmentBox);
    mainLayout->addWidget(mimeQPBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
    setSettings();
}

/**********************************
 * Read the settings from config
 * and set the buttons and checkboxes
 * appropriately
 **********************************/
void MimeTab::setSettings()
{
    QSettings settings;

    // MIME-Parsing
    if (settings.value("mime/parsemime").toBool()) mimeParseCheckBox->setCheckState(Qt::Checked);

    // Qouted Printable
    if (settings.value("mime/parseQP",true).toBool()) mimeQPCheckBox->setCheckState(Qt::Checked);

    // Open Attachments with external app
    if (settings.value("mime/openAttachment").toBool()) mimeOpenAttachmentCheckBox->setCheckState(Qt::Checked);
}


/***********************************
  * get the values of the buttons and
  * write them to settings-file
  *************************************/
void MimeTab::applySettings()
{
    QSettings settings;
    settings.setValue("mime/parsemime" , mimeParseCheckBox->isChecked());
    settings.setValue("mime/parseQP" , mimeQPCheckBox->isChecked());
    settings.setValue("mime/openAttachment" , mimeOpenAttachmentCheckBox->isChecked());

}

AppearanceTab::AppearanceTab(QWidget *parent)
    : QWidget(parent)
{
    /*****************************************
      * Icon-Size-Box
      *****************************************/
    QGroupBox *iconSizeBox = new QGroupBox(tr("Iconsize"));
    iconSizeGroup = new QButtonGroup();
    iconSizeSmall = new QRadioButton(tr("small"));
    iconSizeMedium = new QRadioButton(tr("medium"));
    iconSizeLarge = new QRadioButton(tr("large"));

    iconSizeGroup->addButton(iconSizeSmall, 1);
    iconSizeGroup->addButton(iconSizeMedium, 2);
    iconSizeGroup->addButton(iconSizeLarge, 3);

    QHBoxLayout *iconSizeBoxLayout = new QHBoxLayout();
    iconSizeBoxLayout->addWidget(iconSizeSmall);
    iconSizeBoxLayout->addWidget(iconSizeMedium);
    iconSizeBoxLayout->addWidget(iconSizeLarge);

    iconSizeBox->setLayout(iconSizeBoxLayout);

    /*****************************************
      * Icon-Style-Box
      *****************************************/
    QGroupBox *iconStyleBox = new QGroupBox(tr("Iconstyle"));
    iconStyleGroup = new QButtonGroup();
    iconTextButton = new QRadioButton(tr("just text"));
    iconIconsButton = new QRadioButton(tr("just icons"));
    iconAllButton = new QRadioButton(tr("text and icons"));

    iconStyleGroup->addButton(iconTextButton, 1);
    iconStyleGroup->addButton(iconIconsButton, 2);
    iconStyleGroup->addButton(iconAllButton, 3);

    QHBoxLayout *iconStyleBoxLayout = new QHBoxLayout();
    iconStyleBoxLayout->addWidget(iconTextButton);
    iconStyleBoxLayout->addWidget(iconIconsButton);
    iconStyleBoxLayout->addWidget(iconAllButton);

    iconStyleBox->setLayout(iconStyleBoxLayout);

    /*****************************************
      * Window-Size-Box
      *****************************************/
    QGroupBox *windowSizeBox = new QGroupBox(tr("Windowstate"));
    QHBoxLayout *windowSizeBoxLayout = new QHBoxLayout();
    windowSizeCheckBox = new QCheckBox(tr("Save window size and position on exit."), this);
    windowSizeBoxLayout->addWidget(windowSizeCheckBox);
    windowSizeBox->setLayout(windowSizeBoxLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(iconSizeBox);
    mainLayout->addWidget(iconStyleBox);
    mainLayout->addWidget(windowSizeBox);
    mainLayout->addStretch(1);
    setSettings();
    setLayout(mainLayout);
}

/**********************************
 * Read the settings from config
 * and set the buttons and checkboxes
 * appropriately
 **********************************/
void AppearanceTab::setSettings()
{
    QSettings settings;

    //Iconsize
    QSize iconSize = settings.value("toolbar/iconsize", QSize(24, 24)).toSize();
    switch (iconSize.height()) {
    case 12: iconSizeSmall->setChecked(true);
        break;
    case 24:iconSizeMedium->setChecked(true);
        break;
    case 32:iconSizeLarge->setChecked(true);
        break;
    }
    // Iconstyle
    Qt::ToolButtonStyle iconStyle = static_cast<Qt::ToolButtonStyle>(settings.value("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon).toUInt());
    switch (iconStyle) {
    case Qt::ToolButtonTextOnly: iconTextButton->setChecked(true);
        break;
    case Qt::ToolButtonIconOnly:iconIconsButton->setChecked(true);
        break;
    case Qt::ToolButtonTextUnderIcon:iconAllButton->setChecked(true);
        break;
    default:
        break;
    }

    // Window Save and Position
    if (settings.value("window/windowSave").toBool()) windowSizeCheckBox->setCheckState(Qt::Checked);

}

/***********************************
  * get the values of the buttons and
  * write them to settings-file
  *************************************/
void AppearanceTab::applySettings()
{
    QSettings settings;
    switch (iconSizeGroup->checkedId()) {
    case 1: settings.setValue("toolbar/iconsize", QSize(12, 12));
        break;
    case 2:settings.setValue("toolbar/iconsize", QSize(24, 24));
        break;
    case 3:settings.setValue("toolbar/iconsize", QSize(32, 32));
        break;
    }

    switch (iconStyleGroup->checkedId()) {
    case 1: settings.setValue("toolbar/iconstyle", Qt::ToolButtonTextOnly);
        break;
    case 2:settings.setValue("toolbar/iconstyle", Qt::ToolButtonIconOnly);
        break;
    case 3:settings.setValue("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon);
        break;
    }
    settings.setValue("window/windowSave", windowSizeCheckBox->isChecked());
}

KeyserverTab::KeyserverTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *keyServerBox = new QWidget(this);
    QHBoxLayout *keyServerBoxLayout = new QHBoxLayout(keyServerBox);

    comboBox = new QComboBox;
    comboBox->setEditable(false);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // button for removing currently chosen keyserver
    QIcon ico(":button_trash.png");
    QPushButton *removeKeyServerButton = new QPushButton(QIcon(ico.pixmap(24, 24)), "");
    removeKeyServerButton->setToolTip(tr("remove currently selected server from list"));
    connect(removeKeyServerButton,SIGNAL(clicked()), this, SLOT(removeKeyServer()));

    QIcon ico2(":button_plus.png");
    QPushButton *addKeyServerButton = new QPushButton(QIcon(ico2.pixmap(24, 24)), "");
    addKeyServerButton->setToolTip(tr("Add new keyserver to list"));
    connect(addKeyServerButton,SIGNAL(clicked()), this, SLOT(addKeyServer()));

    keyServerBoxLayout->addWidget(comboBox);
    keyServerBoxLayout->addWidget(removeKeyServerButton);
    keyServerBoxLayout->addWidget(addKeyServerButton);
    keyServerBoxLayout->addStretch(1);

    /*
     * add everything to the mainlayout
     */
    mainLayout->addWidget(new QLabel(tr("Default keyserver:")));
    mainLayout->addWidget(keyServerBox);
    mainLayout->addStretch(1);

    // Read keylist from ini-file and fill it into combobox
    setSettings();
    connect(comboBox, SIGNAL(editTextChanged(QString)), this, SLOT(editTextChangedAction()));
}

/**********************************
 * Read the settings from config
 * and set the buttons and checkboxes
 * appropriately
 **********************************/
void KeyserverTab::setSettings()
{
    QSettings settings;
    QString defKeyserver = settings.value("keyserver/defaultKeyServer").toString();
    comboBox->addItems(settings.value("keyserver/keyServerList").toStringList());
    comboBox->setCurrentIndex(comboBox->findText(defKeyserver));
}

/***********************************
  * get the values of the buttons and
  * write them to settings-file
  *************************************/
void KeyserverTab::applySettings()
{
    QSettings settings;
    settings.setValue("keyserver/defaultKeyServer",comboBox->currentText());
    QStringList *keyServerList = new QStringList();
    for(int i=0; i < comboBox->count(); i++) {
        keyServerList->append(comboBox->itemText(i));
    }
    settings.setValue("keyserver/keyServerList", *keyServerList);
}

/***********************************
  * add the keyserver from the
  * qlineedit to the keyserverlist
  *************************************/
void KeyserverTab::addKeyServer()
{
    comboBox->addItem("http://");
    comboBox->setEditable(true);
    comboBox->setCurrentIndex(comboBox->count()-1);
    comboBox->setFocus();
}

void KeyserverTab::editTextChangedAction()
{
    comboBox->setItemText(comboBox->currentIndex(),comboBox->currentText());
}

void KeyserverTab::removeKeyServer()
{
    comboBox->removeItem(comboBox->currentIndex());
}

AdvancedTab::AdvancedTab(QWidget *parent)
    : QWidget(parent)
{
    /*****************************************
     * Steganography Box
     *****************************************/
    QGroupBox *steganoBox = new QGroupBox(tr("Show Steganography Options [Advanced]"));
    QHBoxLayout *steganoBoxLayout = new QHBoxLayout();
    steganoCheckBox= new QCheckBox(tr("Show Steganographic Options."), this);
    steganoBoxLayout->addWidget(steganoCheckBox);
    steganoBox->setLayout(steganoBoxLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(steganoBox);
    setSettings();
    mainLayout->addStretch(1);
    setLayout(mainLayout);

}

void AdvancedTab::setSettings()
{
    QSettings settings;
    if (settings.value("advanced/steganography").toBool()){
        steganoCheckBox->setCheckState(Qt::Checked);
    }
}

void AdvancedTab::applySettings()
{
    QSettings settings;
    settings.setValue("advanced/steganography", steganoCheckBox->isChecked());
}

GpgPathsTab::GpgPathsTab(QWidget *parent)
    : QWidget(parent)
{
    setSettings();

    /*****************************************
     * Keydb Box
     *****************************************/
    QGroupBox *keydbBox = new QGroupBox(tr("Relative path to keydb"));
    QGridLayout *keydbBoxLayout = new QGridLayout();

    // Label containing the current keydbpath relative to default keydb path
    keydbLabel = new QLabel(accKeydbPath,this);

    QPushButton *keydbButton = new QPushButton("Change keydb path",this);
    connect(keydbButton, SIGNAL(clicked()), this, SLOT(chooseKeydbDir()));
    QPushButton *keydbDefaultButton = new QPushButton("Set keydb to default path",this);
    connect(keydbDefaultButton, SIGNAL(clicked()), this, SLOT(setKeydbPathToDefault()));

    keydbBox->setLayout(keydbBoxLayout);
    keydbBoxLayout->addWidget(new QLabel(tr("Current keydb path: ")),1,1);
    keydbBoxLayout->addWidget(keydbLabel,1,2);
    keydbBoxLayout->addWidget(keydbButton,1,3);
    keydbBoxLayout->addWidget(keydbDefaultButton,2,3);
    keydbBoxLayout->addWidget(new QLabel(tr("<b>NOTE: </b> Gpg4usb will restart automatically if you change the keydb path!")),3,1,1,3);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(keydbBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

QString GpgPathsTab::getRelativePath(const QString dir1,const QString dir2)
{
    QDir dir(dir1);
    QString s;

    s = dir.relativeFilePath(dir2);
    qDebug() << "relative path: " << s;
    if (s.isEmpty()) {
        s = ".";
    }
    return s;
}

void GpgPathsTab::setKeydbPathToDefault()
{
    accKeydbPath = ".";
    keydbLabel->setText(".");
}

QString GpgPathsTab::chooseKeydbDir()
{
    QString dir = QFileDialog::getExistingDirectory(this,tr ("Choose keydb directory"),accKeydbPath,QFileDialog::ShowDirsOnly);

    accKeydbPath = getRelativePath(defKeydbPath, dir);
    keydbLabel->setText(accKeydbPath);
    return "";
}

void GpgPathsTab::setSettings()
{
    defKeydbPath = qApp->applicationDirPath() + "/keydb";

    QSettings settings;
    accKeydbPath = settings.value("gpgpaths/keydbpath").toString();
    if (accKeydbPath.isEmpty()) {
        accKeydbPath = ".";
    }
}

void GpgPathsTab::applySettings()
{
    QSettings settings;
    settings.setValue("gpgpaths/keydbpath",accKeydbPath);
}
