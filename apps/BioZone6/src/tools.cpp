﻿/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "tools.h"
#include <QCheckBox>
#include <QInputDialog>

BioZone6_tools::BioZone6_tools(QWidget* parent) :
	QMainWindow(parent),
	ui_tools(new Ui::BioZone6_tools),
	m_comSettings(new COMSettings()),
	m_solutionParams(new solutionsParams()),
	m_pr_params(new pr_params()),
	m_tip(new fluicell::PPC1api6dataStructures::tip()),
	m_expert(false),
	m_GUI_params(new GUIparams()),
	m_setting_file_name("./settings/settings.ini"),
	m_setting_folder_path("./settings"),
	m_setting_profile_standard_file_name("./Standard_tip_settings.ini"),
    m_setting_profile_wide_file_name("./Wide_tip_settings.ini"),
    m_setting_profile_UWZ_file_name("./UWZ_tip_settings.ini")
{
	ui_tools->setupUi(this );

	//ui_tools->pushButton_enableTipSetting->setEnabled(false); //TODO: remember to remove once the password is set

	//load settings from file
	loadSettings(m_setting_file_name);

	initCustomStrings();

	//make sure to start from the initial page
	ui_tools->actionGeneral->setChecked(true);
	ui_tools->stackedWidget->setCurrentIndex(0);

    // check serial settings
	ui_tools->comboBox_serialInfo->clear();

	// by default the standard and regulad settings are applied 
	int new_p_on_default = ui_tools->spinBox_sAr_Pon_def->value();// 50;
	int new_p_off_default = ui_tools->spinBox_sAr_Poff_def->value();// 11;
	int new_v_switch_default = ui_tools->spinBox_sAr_Vs_def->value();// -75;
	int new_v_recirc_default = ui_tools->spinBox_sAr_Vr_def->value();// -115;
	// apply the new values 
	this->setDefaultPressuresVacuums(new_p_on_default, new_p_off_default,
		-new_v_recirc_default, -new_v_switch_default);


	connect(ui_tools->comboBox_serialInfo,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		&BioZone6_tools::showPortInfo);

	
	// enumerate connected com ports
	enumerate();

	// connect GUI elements: communication tab
	connect(ui_tools->pushButton_enumerate,
		SIGNAL(clicked()), this, SLOT(enumerate()));

	connect(ui_tools->actionGeneral,
		SIGNAL(triggered()), this,
		SLOT(goToPage1()));

	connect(ui_tools->actionSolution,
		SIGNAL(triggered()), this,
		SLOT(goToPage2()));

	connect(ui_tools->actionPressure,
		SIGNAL(triggered()), this,
		SLOT(goToPage3()));

	connect(ui_tools->actionCommunication,
		SIGNAL(triggered()), this,
		SLOT(goToPage4()));

	connect(ui_tools->comboBox_profileSetting,
		SIGNAL(currentIndexChanged(int)), this, 
		SLOT(settingProfileChanged(int)));

	connect(ui_tools->pushButton_checkUpdates,
		SIGNAL(clicked()), this, 
		SLOT(checkForUpdates()));

	connect(ui_tools->pushButton_TTLtest,
		SIGNAL(clicked()), this,
		SLOT(testTTL()));

	// connect color solution settings
	connect(ui_tools->horizontalSlider_colorSol1,
		SIGNAL(valueChanged(int)), this,
		SLOT(colorSol1Changed(int)));

	connect(ui_tools->horizontalSlider_colorSol2,
		SIGNAL(valueChanged(int)), this,
		SLOT(colorSol2Changed(int)));

	connect(ui_tools->horizontalSlider_colorSol3,
		SIGNAL(valueChanged(int)), this,
		SLOT(colorSol3Changed(int)));

	connect(ui_tools->horizontalSlider_colorSol4,
		SIGNAL(valueChanged(int)), this,
		SLOT(colorSol4Changed(int)));

	connect(ui_tools->horizontalSlider_colorSol5,
		SIGNAL(valueChanged(int)), this,
		SLOT(colorSol5Changed(int)));

	connect(ui_tools->horizontalSlider_colorSol6,
		SIGNAL(valueChanged(int)), this,
		SLOT(colorSol6Changed(int)));

	connect(ui_tools->pushButton_emptyWaste,
		SIGNAL(clicked()), this, SLOT(emptyWastePressed()));

	
	connect(ui_tools->pushButton_refillSolution,
		SIGNAL(clicked()), this, SLOT(refillSolutionPressed()));

	connect(ui_tools->checkBox_disableTimer_s1,
		SIGNAL(stateChanged(int)), this,
        SLOT(setContinuousFlow_s1(int)));

	connect(ui_tools->checkBox_disableTimer_s2,
		SIGNAL(stateChanged(int)), this,
        SLOT(setContinuousFlow_s2(int)));

	connect(ui_tools->checkBox_disableTimer_s3,
		SIGNAL(stateChanged(int)), this,
        SLOT(setContinuousFlow_s3(int)));

	connect(ui_tools->checkBox_disableTimer_s4,
		SIGNAL(stateChanged(int)), this,
        SLOT(setContinuousFlow_s4(int)));

	connect(ui_tools->checkBox_disableTimer_s5,
		SIGNAL(stateChanged(int)), this,
		SLOT(setContinuousFlow_s5(int)));

	connect(ui_tools->checkBox_disableTimer_s6,
		SIGNAL(stateChanged(int)), this,
		SLOT(setContinuousFlow_s6(int)));

	connect(ui_tools->pushButton_toDefault,
		SIGNAL(clicked()), this, SLOT(resetToDefaultValues()));

    connect(ui_tools->checkBox_enableToolTips,
        SIGNAL(stateChanged(int)), this, SLOT(enableToolTip(int)));
	
	connect(ui_tools->checkBox_restrictOPmode,
		SIGNAL(stateChanged(int)), this, SLOT(enableIONoptix_checked(int)));

    connect(ui_tools->checkBox_enablePPC1filter,
        SIGNAL(stateChanged(int)), this, SLOT(enablePPC1filtering()));

	//connect(ui_tools->pushButton_enableTipSetting,
	//	SIGNAL(clicked()), this, SLOT(askPasswordToUnlock()));

	connect(ui_tools->comboBox_tipSelection,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		&BioZone6_tools::tipSelection);

	//connect(ui_tools->checkBox_modifyOperationalModeValues,
	//	SIGNAL(stateChanged(int)), this, SLOT(activateOperationaModeSettings(int)));
	
	//connect(ui_tools->pushButton_enableTipSetting,
	//	SIGNAL(clicked()), this, SLOT(activateOperationaModeSettings()));

    // connect tool window events Ok, Cancel, Apply
	connect(ui_tools->buttonBox->button(QDialogButtonBox::Ok), 
		SIGNAL(clicked()), this, SLOT(okPressed()));

	connect(ui_tools->buttonBox->button(QDialogButtonBox::Cancel), 
		SIGNAL(clicked()), this, SLOT(cancelPressed()));

	connect(ui_tools->buttonBox->button(QDialogButtonBox::Apply), 
		SIGNAL(clicked()), this, SLOT(applyPressed()));

}

void BioZone6_tools::goToPage1()
{
	ui_tools->actionGeneral->setChecked(true);
	ui_tools->actionSolution->setChecked(false);
	ui_tools->actionPressure->setChecked(false);
	ui_tools->actionCommunication->setChecked(false);
	ui_tools->stackedWidget->setCurrentIndex(0);

}

void BioZone6_tools::goToPage2()
{
	ui_tools->actionGeneral->setChecked(false);
	ui_tools->actionSolution->setChecked(true);
	ui_tools->actionPressure->setChecked(false);
	ui_tools->actionCommunication->setChecked(false);
	ui_tools->stackedWidget->setCurrentIndex(1);
}

void BioZone6_tools::goToPage3()
{
	ui_tools->actionGeneral->setChecked(false);
	ui_tools->actionSolution->setChecked(false);
	ui_tools->actionPressure->setChecked(true);
	ui_tools->actionCommunication->setChecked(false);
	ui_tools->stackedWidget->setCurrentIndex(2);
}

void BioZone6_tools::goToPage4()
{
	ui_tools->actionGeneral->setChecked(false);
	ui_tools->actionSolution->setChecked(false);
	ui_tools->actionPressure->setChecked(false);
	ui_tools->actionCommunication->setChecked(true);
	ui_tools->stackedWidget->setCurrentIndex(3);
}

void BioZone6_tools::setPreset1(int _p_on, int _p_off, int _v_switch, int _v_recirc)
{
		m_pr_params->p_on_preset1 = _p_on;
		m_pr_params->p_off_preset1 = _p_off;
		m_pr_params->v_switch_preset1 = _v_switch;
		m_pr_params->v_recirc_preset1 = _v_recirc;

		ui_tools->spinBox_p_on_preset1->setValue(_p_on);
		ui_tools->spinBox_p_off_preset1->setValue(_p_off);
		ui_tools->spinBox_v_switch_preset1->setValue(_v_switch);
		ui_tools->spinBox_v_recirc_preset1->setValue(_v_recirc);

		saveSettings(m_setting_file_name);

}

void BioZone6_tools::setPreset2(int _p_on, int _p_off, int _v_switch, int _v_recirc)
{
	m_pr_params->p_on_preset2 = _p_on;
	m_pr_params->p_off_preset2 = _p_off;
	m_pr_params->v_switch_preset2 = _v_switch;
	m_pr_params->v_recirc_preset2 = _v_recirc;

	ui_tools->spinBox_p_on_preset2->setValue(_p_on);
	ui_tools->spinBox_p_off_preset2->setValue(_p_off);
	ui_tools->spinBox_v_switch_preset2->setValue(_v_switch);
	ui_tools->spinBox_v_recirc_preset2->setValue(_v_recirc);

	saveSettings(m_setting_file_name);
}

void BioZone6_tools::setPreset3(int _p_on, int _p_off, int _v_switch, int _v_recirc)
{
	m_pr_params->p_on_preset3 = _p_on;
	m_pr_params->p_off_preset3 = _p_off;
	m_pr_params->v_switch_preset3 = _v_switch;
	m_pr_params->v_recirc_preset3 = _v_recirc;

	ui_tools->spinBox_p_on_preset3->setValue(_p_on);
	ui_tools->spinBox_p_off_preset3->setValue(_p_off);
	ui_tools->spinBox_v_switch_preset3->setValue(_v_switch);
	ui_tools->spinBox_v_recirc_preset3->setValue(_v_recirc);

	saveSettings(m_setting_file_name);
}

void BioZone6_tools::setCustomPreset(int _p_on, int _p_off, int _v_switch, int _v_recirc)
{
	m_pr_params->p_on_customPreset = _p_on;
	m_pr_params->p_off_customPreset = _p_off;
	m_pr_params->v_switch_customPreset = _v_switch;
	m_pr_params->v_recirc_customPreset = _v_recirc;

	//ui_tools->spinBox_p_on_preset3->setValue(_p_on);
	//ui_tools->spinBox_p_off_preset3->setValue(_p_off);
	//ui_tools->spinBox_v_switch_preset3->setValue(_v_switch);
	//ui_tools->spinBox_v_recirc_preset3->setValue(_v_recirc);

	//saveSettings(m_setting_file_name);
}


void BioZone6_tools::okPressed() {

	checkHistory ();

	this->applyPressed();
	emit ok();
	this->close();
}

void BioZone6_tools::cancelPressed() {

	loadSettings(m_setting_file_name);
	emit cancel();
	this->close();
}


void BioZone6_tools::applyPressed() {

	getCOMsettingsFromGUI();
	getSolutionSettingsFromGUI();
	getGUIsettingsFromGUI();
	getPRsettingsFromGUI();
	getTipSettingsFromGUI();

	saveSettings(m_setting_file_name);

	emit apply();
}

void BioZone6_tools::checkHistory () {
    int folder_size = calculateFolderSize(m_GUI_params->outFilePath);

    //TODO: translate strings
    if (folder_size > 1000000) {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, m_str_warning,
            tr("It looks you have many files in the history folder <br>") + 
			m_GUI_params->outFilePath +
            tr("<br> Do you want to clean the history? <br>") + 
			tr("Yes = clean, NO = abort operation, "),
            QMessageBox::No | QMessageBox::Yes,
            QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
			QMessageBox mb = QMessageBox(QMessageBox::Question,
				m_str_information, m_str_operation_cancelled, QMessageBox::Ok);
			mb.exec();
        }
        else {
            QDir dir(m_GUI_params->outFilePath);
            dir.setNameFilters(QStringList() << "*.txt");
            dir.setFilter(QDir::Files);
            foreach(QString dirFile, dir.entryList())
            {
                dir.remove(dirFile);
            }
			QMessageBox mb = QMessageBox(QMessageBox::Question,
				m_str_information, m_str_history_cleaned, QMessageBox::Ok);
			mb.exec();
        }

    }
}

int BioZone6_tools::calculateFolderSize(const QString _dirPath)
{
	long int sizex = 0;
	QFileInfo str_info(_dirPath);
	if (str_info.isDir())
	{
		QDir dir(_dirPath);
		
		QStringList ext_list;
		dir.setFilter(QDir::Files | QDir::Dirs | QDir::Hidden | QDir::NoSymLinks);
		QFileInfoList list = dir.entryInfoList();

		for (int i = 0; i < list.size(); ++i)
		{
			QFileInfo fileInfo = list.at(i);
			if ((fileInfo.fileName() != ".") && (fileInfo.fileName() != ".."))
			{
				//sizex += this->calculateFolderSize(fileInfo.filePath());			
				sizex += (fileInfo.isDir()) ? this->calculateFolderSize(fileInfo.filePath()) : fileInfo.size();
				QApplication::processEvents();
			}
		}
	}
	std::cout << HERE << " folder  " << _dirPath.toStdString()
		<< " size = " << sizex << std::endl;
	return sizex;
}


void BioZone6_tools::refillSolutionPressed() {
	std::cout << HERE << std::endl;

	m_solutionParams->vol_well1 = ui_tools->spinBox_vol_sol1->value();
	m_solutionParams->vol_well2 = ui_tools->spinBox_vol_sol2->value();
	m_solutionParams->vol_well3 = ui_tools->spinBox_vol_sol3->value();
	m_solutionParams->vol_well4 = ui_tools->spinBox_vol_sol4->value();
	m_solutionParams->vol_well5 = ui_tools->spinBox_vol_sol5->value();
	m_solutionParams->vol_well6 = ui_tools->spinBox_vol_sol6->value();

	emit refillSolution();
}

void BioZone6_tools::emptyWastePressed() {
	std::cout << HERE << std::endl;

	m_solutionParams->vol_well7 = MAX_WASTE_VOLUME; 
	m_solutionParams->vol_well8 = MAX_WASTE_VOLUME; 

	emit emptyWaste();
}


void BioZone6_tools::enumerate()
{
	ui_tools->comboBox_serialInfo->clear();
	// set serial port description for the first found port
	QString description;
	QString manufacturer;
	QString serialNumber;

	// try to get device information
	std::vector<serial::PortInfo> devices = serial::list_ports();
	std::vector<fluicell::PPC1api6dataStructures::serialDeviceInfo> devs;
	for (unsigned int i = 0; i < devices.size(); i++) // for all the connected devices extract information
	{
		fluicell::PPC1api6dataStructures::serialDeviceInfo dev;
		dev.port = devices.at(i).port;
		dev.description = devices.at(i).description;
		dev.hardware_ID = devices.at(i).hardware_id;
		devs.push_back(dev);
		ui_tools->comboBox_serialInfo->addItem(QString::fromStdString(dev.port));
	}
}


void BioZone6_tools::setDefaultPressuresVacuums(int _p_on_default, int _p_off_default, int _v_recirc_default, int _v_switch_default)
{
	std::cout << HERE 
		<< "  _p_on_default = "  << _p_on_default
		<< "  _p_off_default = " << _p_off_default
		<< "  _v_recirc_default = " << _v_recirc_default
		<< "  _v_switch_default = " << _v_switch_default
		<< std::endl;

	m_pr_params->p_on_default = _p_on_default;
	m_pr_params->p_off_default = _p_off_default;
	m_pr_params->v_recirc_default = _v_recirc_default;
	m_pr_params->v_switch_default = _v_switch_default;

	ui_tools->spinBox_p_on_default->setValue(_p_on_default);
	ui_tools->spinBox_p_off_default->setValue(_p_off_default);
	ui_tools->spinBox_v_recirc_default->setValue(-_v_recirc_default);
	ui_tools->spinBox_v_switch_default->setValue(-_v_switch_default);
	
	this->applyPressed();
}

void BioZone6_tools::setDefaultPressuresVacuums_sAr(int _new_p_on, int _new_p_off,
	int _new_v_recirc, int _new_v_switch) 
{
	std::cout << HERE
		<< "  _p_on_default = " << _new_p_on
		<< "  _p_off_default = " << _new_p_off
		<< "  _v_recirc_default = " << _new_v_recirc
		<< "  _v_switch_default = " << _new_v_switch
		<< std::endl;

	m_pr_params->p_on_sAr = _new_p_on;
	m_pr_params->p_off_sAr = _new_p_off;
	m_pr_params->v_recirc_sAr = _new_v_recirc;
	m_pr_params->v_switch_sAr = _new_v_switch;

	ui_tools->spinBox_sAr_Pon_def->setValue(_new_p_on);
	ui_tools->spinBox_sAr_Poff_def->setValue(_new_p_off);
	ui_tools->spinBox_sAr_Vr_def->setValue(_new_v_recirc);
	ui_tools->spinBox_sAr_Vs_def->setValue(_new_v_switch);

	this->applyPressed();
}

void BioZone6_tools::setDefaultPressuresVacuums_lAr(int _new_p_on, int _new_p_off,
		int _new_v_recirc, int _new_v_switch) {
	std::cout << HERE
		<< "  _p_on_default = " << _new_p_on
		<< "  _p_off_default = " << _new_p_off
		<< "  _v_recirc_default = " << _new_v_recirc
		<< "  _v_switch_default = " << _new_v_switch
		<< std::endl;

	m_pr_params->p_on_lAr = _new_p_on;
	m_pr_params->p_off_lAr = _new_p_off;
	m_pr_params->v_recirc_lAr = _new_v_recirc;
	m_pr_params->v_switch_lAr = _new_v_switch;

	ui_tools->spinBox_lAr_Pon_def->setValue(_new_p_on);
	ui_tools->spinBox_lAr_Poff_def->setValue(_new_p_off);
	ui_tools->spinBox_lAr_Vr_def->setValue(_new_v_recirc);
	ui_tools->spinBox_lAr_Vs_def->setValue(_new_v_switch);

	this->applyPressed();
}

void BioZone6_tools::setDefaultPressuresVacuums_sAs(int _new_p_on, int _new_p_off,
		int _new_v_recirc, int _new_v_switch) {
	std::cout << HERE
		<< "  _p_on_default = " << _new_p_on
		<< "  _p_off_default = " << _new_p_off
		<< "  _v_recirc_default = " << _new_v_recirc
		<< "  _v_switch_default = " << _new_v_switch
		<< std::endl;

	m_pr_params->p_on_sAs = _new_p_on;
	m_pr_params->p_off_sAs = _new_p_off;
	m_pr_params->v_recirc_sAs = _new_v_recirc;
	m_pr_params->v_switch_sAs = _new_v_switch;

	ui_tools->spinBox_sAs_Pon_def->setValue(_new_p_on);
	ui_tools->spinBox_sAs_Poff_def->setValue(_new_p_off);
	ui_tools->spinBox_sAs_Vr_def->setValue(_new_v_recirc);
	ui_tools->spinBox_sAs_Vs_def->setValue(_new_v_switch);

	this->applyPressed();
}

void BioZone6_tools::setDefaultPressuresVacuums_lAs(int _new_p_on, int _new_p_off,
		int _new_v_recirc, int _new_v_switch) {
	std::cout << HERE
		<< "  _p_on_default = " << _new_p_on
		<< "  _p_off_default = " << _new_p_off
		<< "  _v_recirc_default = " << _new_v_recirc
		<< "  _v_switch_default = " << _new_v_switch
		<< std::endl;

	m_pr_params->p_on_lAs = _new_p_on;
	m_pr_params->p_off_lAs = _new_p_off;
	m_pr_params->v_recirc_lAs = _new_v_recirc;
	m_pr_params->v_switch_lAs = _new_v_switch;

	ui_tools->spinBox_lAs_Pon_def->setValue(_new_p_on);
	ui_tools->spinBox_lAs_Poff_def->setValue(_new_p_off);
	ui_tools->spinBox_lAs_Vr_def->setValue(_new_v_recirc);
	ui_tools->spinBox_lAs_Vs_def->setValue(_new_v_switch);

	this->applyPressed();
}


void BioZone6_tools::showPortInfo(int idx)
{
	if (idx == -1)
		return;

	QStringList list = ui_tools->comboBox_serialInfo->itemData(idx).toStringList();
	ui_tools->descriptionLabel->setText(
		tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr("N/A")));
	ui_tools->manufacturerLabel->setText(
		tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr("N/A")));
	ui_tools->serialNumberLabel->setText(
		tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr("N/A")));
	ui_tools->locationLabel->setText(
		tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr("N/A")));
	ui_tools->vidLabel->setText(
		tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr("N/A")));
	ui_tools->pidLabel->setText(
		tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr("N/A")));
}

void BioZone6_tools::enableToolTip(int _inx)
{
	m_GUI_params->enableToolTips = ui_tools->checkBox_enableToolTips->isChecked();
}


void BioZone6_tools::enableIONoptix_checked(int _inx)
{
	if (ui_tools->checkBox_restrictOPmode->isChecked())
		ui_tools->comboBox_tipSelection->setCurrentIndex(2);
	else
		ui_tools->comboBox_tipSelection->setCurrentIndex(0);

}

void BioZone6_tools::settingProfileChanged(int _idx)
{
	QMessageBox msgBox;
	msgBox.setText(m_str_override_setting_profile);
	msgBox.setWindowTitle(m_str_information);
	msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
	msgBox.setDefaultButton(QMessageBox::Yes);
	msgBox.setIcon(QMessageBox::Question);
	int resBtn = msgBox.exec();

	if (resBtn != QMessageBox::Yes) {
		
		return;
	}

	switch (_idx) {
	case 0:
		loadSettings(m_setting_folder_path + m_setting_profile_standard_file_name);
		break;
	case 1:
		loadSettings(m_setting_folder_path + m_setting_profile_wide_file_name);
		break;
	case 2:
		loadSettings(m_setting_folder_path + m_setting_profile_UWZ_file_name);
		break;
	default:
		std::cout << "Error" << std::endl;
		break;
	}

}

void BioZone6_tools::checkForUpdates()
{
	emit checkUpdatesNow();
}

void BioZone6_tools::testTTL()
{
	std::cout << HERE 
		<< "  TTLsignal = " << ui_tools->checkBox_TTLtest->isChecked()
		<< std::endl;

	emit TTLsignal(ui_tools->checkBox_TTLtest->isChecked());

}


void BioZone6_tools::colorSol1Changed(int _value)
{
	float v = _value / 16777216.0;

	uint32_t color = giveRainbowColor(v);

	int red = color & 0x0000FF;
	int green = (color >> 8) & 0x0000FF;
	int blue = (color >> 16) & 0x0000FF;

	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Base, QColor::fromRgb(red, green, blue));
	ui_tools->lineEdit_sol1_name->setPalette(*palette);
	m_solutionParams->sol1_color = QColor::fromRgb(red, green, blue);
	emit colSol1Changed(red, green, blue);

}

void BioZone6_tools::colorSol2Changed(int _value)
{
	float v = _value / 16777216.0;

	uint32_t color = giveRainbowColor(v);

	int red = color & 0x0000FF;
	int green = (color >> 8) & 0x0000FF;
	int blue = (color >> 16) & 0x0000FF;

	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Base, QColor::fromRgb(red, green, blue));
	ui_tools->lineEdit_sol2_name->setPalette(*palette);
	m_solutionParams->sol2_color = QColor::fromRgb(red, green, blue);
	emit colSol2Changed(red, green, blue);
}

void BioZone6_tools::colorSol3Changed(int _value)
{
	float v = _value / 16777216.0;

	uint32_t color = giveRainbowColor(v);

	int red = color & 0x0000FF;
	int green = (color >> 8) & 0x0000FF;
	int blue = (color >> 16) & 0x0000FF;

	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Base, QColor::fromRgb(red, green, blue));
	//palette.setColor(QPalette::Text, Qt::white);
	ui_tools->lineEdit_sol3_name->setPalette(*palette);
	m_solutionParams->sol3_color = QColor::fromRgb(red, green, blue);
	emit colSol3Changed(red, green, blue);
}

void BioZone6_tools::colorSol4Changed(int _value)
{
	float v = _value / 16777216.0;

	uint32_t color = giveRainbowColor(v);
	
	int red = color & 0x0000FF;
	int green = (color >> 8 ) & 0x0000FF;
	int blue = (color >> 16) & 0x0000FF;

	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Base, QColor::fromRgb(red, green, blue));
	//palette.setColor(QPalette::Text, Qt::white);
	ui_tools->lineEdit_sol4_name->setPalette(*palette);
	m_solutionParams->sol4_color = QColor::fromRgb(red, green, blue);
	emit colSol4Changed(red, green, blue);

}

void BioZone6_tools::colorSol5Changed(int _value)
{
	float v = _value / 16777216.0;

	uint32_t color = giveRainbowColor(v);

	int red = color & 0x0000FF;
	int green = (color >> 8) & 0x0000FF;
	int blue = (color >> 16) & 0x0000FF;

	QPalette* palette = new QPalette();
	palette->setColor(QPalette::Base, QColor::fromRgb(red, green, blue));
	//palette.setColor(QPalette::Text, Qt::white);
	ui_tools->lineEdit_sol5_name->setPalette(*palette);
	m_solutionParams->sol5_color = QColor::fromRgb(red, green, blue);
	emit colSol5Changed(red, green, blue);

}

void BioZone6_tools::colorSol6Changed(int _value)
{
	float v = _value / 16777216.0;

	uint32_t color = giveRainbowColor(v);

	int red = color & 0x0000FF;
	int green = (color >> 8) & 0x0000FF;
	int blue = (color >> 16) & 0x0000FF;

	QPalette* palette = new QPalette();
	palette->setColor(QPalette::Base, QColor::fromRgb(red, green, blue));
	//palette.setColor(QPalette::Text, Qt::white);
	ui_tools->lineEdit_sol6_name->setPalette(*palette);
	m_solutionParams->sol6_color = QColor::fromRgb(red, green, blue);
	emit colSol6Changed(red, green, blue);

}
void BioZone6_tools::getCOMsettingsFromGUI()
{
	m_comSettings->setName (ui_tools->comboBox_serialInfo->currentText().toStdString());
	m_comSettings->setBaudRate ( ui_tools->comboBox_baudRate->currentText().toInt());
	m_comSettings->setDataBits(static_cast<serial::bytesize_t>(
		ui_tools->comboBox_dataBit->currentIndex()));
	m_comSettings->setFlowControl(static_cast<serial::flowcontrol_t>(
		ui_tools->comboBox_flowControl->currentIndex())); 
	m_comSettings->setParity(static_cast<serial::parity_t>(
		ui_tools->comboBox_parity->currentIndex()));
	m_comSettings->setStopBits(static_cast<serial::stopbits_t>(
		ui_tools->comboBox_stopBit->currentIndex()));
}

void BioZone6_tools::getSolutionSettingsFromGUI()
{
	m_solutionParams->vol_well1 = ui_tools->spinBox_vol_sol1->value();
	m_solutionParams->vol_well2 = ui_tools->spinBox_vol_sol2->value();
	m_solutionParams->vol_well3 = ui_tools->spinBox_vol_sol3->value();
	m_solutionParams->vol_well4 = ui_tools->spinBox_vol_sol4->value();
	m_solutionParams->vol_well5 = ui_tools->spinBox_vol_sol5->value();
	m_solutionParams->vol_well6 = ui_tools->spinBox_vol_sol6->value();
	m_solutionParams->vol_well7 = MAX_WASTE_VOLUME; 
	m_solutionParams->vol_well8 = MAX_WASTE_VOLUME; 

	m_solutionParams->sol1 = ui_tools->lineEdit_sol1_name->text();
	m_solutionParams->sol2 = ui_tools->lineEdit_sol2_name->text();
	m_solutionParams->sol3 = ui_tools->lineEdit_sol3_name->text();
	m_solutionParams->sol4 = ui_tools->lineEdit_sol4_name->text();
	m_solutionParams->sol5 = ui_tools->lineEdit_sol5_name->text();
	m_solutionParams->sol6 = ui_tools->lineEdit_sol6_name->text();

	m_solutionParams->pulse_duration_well1 = ui_tools->doubleSpinBox_pulse_sol1->value();
	m_solutionParams->pulse_duration_well2 = ui_tools->doubleSpinBox_pulse_sol2->value();
	m_solutionParams->pulse_duration_well3 = ui_tools->doubleSpinBox_pulse_sol3->value();
	m_solutionParams->pulse_duration_well4 = ui_tools->doubleSpinBox_pulse_sol4->value();
	m_solutionParams->pulse_duration_well5 = ui_tools->doubleSpinBox_pulse_sol5->value();
	m_solutionParams->pulse_duration_well6 = ui_tools->doubleSpinBox_pulse_sol6->value();

	m_solutionParams->continuous_flowing_sol1 = ui_tools->checkBox_disableTimer_s1->isChecked();
	m_solutionParams->continuous_flowing_sol2 = ui_tools->checkBox_disableTimer_s2->isChecked();
	m_solutionParams->continuous_flowing_sol3 = ui_tools->checkBox_disableTimer_s3->isChecked();
	m_solutionParams->continuous_flowing_sol4 = ui_tools->checkBox_disableTimer_s4->isChecked();
	m_solutionParams->continuous_flowing_sol5 = ui_tools->checkBox_disableTimer_s5->isChecked();
	m_solutionParams->continuous_flowing_sol6 = ui_tools->checkBox_disableTimer_s6->isChecked();

	m_pr_params->base_ds_increment = ui_tools->spinBox_ds_increment->value();
	m_pr_params->base_fs_increment = ui_tools->spinBox_fs_increment->value();
	m_pr_params->base_v_increment = ui_tools->spinBox_v_increment->value();

}

void BioZone6_tools::getGUIsettingsFromGUI()
{
//	m_GUI_params->showTextToolBar = Qt::ToolButtonStyle(ui_tools->comboBox_toolButtonStyle->currentIndex());
	m_GUI_params->automaticUpdates_idx = ui_tools->comboBox_automaticUpdates->currentIndex(); 
	m_GUI_params->enableToolTips = ui_tools->checkBox_enableToolTips->isChecked();
	m_GUI_params->verboseOutput = ui_tools->checkBox_verboseOut->isChecked();
	m_GUI_params->enableHistory = ui_tools->checkBox_EnableHistory->isChecked();
	m_GUI_params->dumpHistoryToFile = ui_tools->checkBox_dumpToFile->isChecked();
	m_GUI_params->speechActive = ui_tools->checkBox_enableSynthesis->isChecked();
	m_GUI_params->outFilePath = ui_tools->lineEdit_msg_out_file_path->text();
	m_GUI_params->setLanguage(ui_tools->comboBox_language->currentIndex());
	m_GUI_params->restrictOPmode = ui_tools->checkBox_restrictOPmode->isChecked();
	m_GUI_params->useIONoptixLogo = ui_tools->checkBox_enableIonOptixLogo->isChecked();

}

void BioZone6_tools::getPRsettingsFromGUI()
{

	m_pr_params->p_on_default = ui_tools->spinBox_p_on_default->value();
	m_pr_params->p_off_default = ui_tools->spinBox_p_off_default->value();
	m_pr_params->v_recirc_default = ui_tools->spinBox_v_recirc_default->value();
	m_pr_params->v_switch_default = ui_tools->spinBox_v_switch_default->value();
	m_pr_params->verboseOut = ui_tools->checkBox_enablePPC1verboseOut->isChecked();
	m_pr_params->useDefValSetPoint = ui_tools->checkBox_useSetPoint->isChecked(); 
	m_pr_params->enableFilter = ui_tools->checkBox_enablePPC1filter->isChecked();
	m_pr_params->filterSize = ui_tools->spinBox_PPC1filterSize->value();
	m_pr_params->waitSyncTimeout = ui_tools->spinBox_PPC1_sync_timeout->value();
	
	m_pr_params->p_on_sAs = ui_tools->spinBox_sAs_Pon_def->value();
	m_pr_params->p_off_sAs = ui_tools->spinBox_sAs_Poff_def->value();
	m_pr_params->v_recirc_sAs = ui_tools->spinBox_sAs_Vr_def->value();
	m_pr_params->v_switch_sAs = ui_tools->spinBox_sAs_Vs_def->value();

	m_pr_params->p_on_sAr = ui_tools->spinBox_sAr_Pon_def->value();
	m_pr_params->p_off_sAr = ui_tools->spinBox_sAr_Poff_def->value();
	m_pr_params->v_recirc_sAr = ui_tools->spinBox_sAr_Vr_def->value();
	m_pr_params->v_switch_sAr = ui_tools->spinBox_sAr_Vs_def->value();

	m_pr_params->p_on_lAs = ui_tools->spinBox_lAs_Pon_def->value();
	m_pr_params->p_off_lAs = ui_tools->spinBox_lAs_Poff_def->value();
	m_pr_params->v_recirc_lAs = ui_tools->spinBox_lAs_Vr_def->value();
	m_pr_params->v_switch_lAs = ui_tools->spinBox_lAs_Vs_def->value();

	m_pr_params->p_on_lAr = ui_tools->spinBox_lAr_Pon_def->value();
	m_pr_params->p_off_lAr = ui_tools->spinBox_lAr_Poff_def->value();
	m_pr_params->v_recirc_lAr = ui_tools->spinBox_lAr_Vr_def->value();
	m_pr_params->v_switch_lAr = ui_tools->spinBox_lAr_Vs_def->value();

}

void BioZone6_tools::getTipSettingsFromGUI()
{
	if (ui_tools->doubleSpinBox_lengthToTip->isEnabled())
		m_tip->length_to_tip = ui_tools->doubleSpinBox_lengthToTip->value();

	if (ui_tools->doubleSpinBox_lengthToZone->isEnabled())
		m_tip->length_to_zone = ui_tools->doubleSpinBox_lengthToZone->value();

	this->tipSelection(ui_tools->comboBox_tipSelection->currentIndex());
}

void BioZone6_tools::activateOperationaModeSettings(int _enable)
{
	std::cout << HERE << std::endl;

	bool enable = false;
	if (ui_tools->pushButton_enableTipSetting->isChecked()==true)// && askPasswordToUnlock() == true)
	{
		enable = true;
		ui_tools->pushButton_enableTipSetting->setChecked(Qt::CheckState::Checked);
	}
	else
		ui_tools->pushButton_enableTipSetting->setChecked(Qt::CheckState::Unchecked);
	
	//ui_tools->spinBox_p_on_default->setEnabled(enable);
	//ui_tools->spinBox_p_off_default->setEnabled(enable);
	//ui_tools->spinBox_v_switch_default->setEnabled(enable);
	//ui_tools->spinBox_v_recirc_default->setEnabled(enable);

	// enable all the fiels
	ui_tools->spinBox_sAs_Pon_def->setEnabled(enable);
	ui_tools->spinBox_sAs_Poff_def->setEnabled(enable);
	ui_tools->spinBox_sAs_Vs_def->setEnabled(enable);
	ui_tools->spinBox_sAs_Vr_def->setEnabled(enable);

	ui_tools->spinBox_sAr_Pon_def->setEnabled(enable);
	ui_tools->spinBox_sAr_Poff_def->setEnabled(enable);
	ui_tools->spinBox_sAr_Vs_def->setEnabled(enable);
	ui_tools->spinBox_sAr_Vr_def->setEnabled(enable);

	ui_tools->spinBox_lAs_Pon_def->setEnabled(enable);
	ui_tools->spinBox_lAs_Poff_def->setEnabled(enable);
	ui_tools->spinBox_lAs_Vs_def->setEnabled(enable);
	ui_tools->spinBox_lAs_Vr_def->setEnabled(enable);

	ui_tools->spinBox_lAr_Pon_def->setEnabled(enable);
	ui_tools->spinBox_lAr_Poff_def->setEnabled(enable);
	ui_tools->spinBox_lAr_Vs_def->setEnabled(enable);
	ui_tools->spinBox_lAr_Vr_def->setEnabled(enable);

	ui_tools->doubleSpinBox_lengthToTip->setEnabled(enable);
	ui_tools->doubleSpinBox_lengthToZone->setEnabled(enable);
	ui_tools->comboBox_tipSelection->setEnabled(enable);
	ui_tools->checkBox_enableIonOptixLogo->setEnabled(enable);


}

bool BioZone6_tools::loadSettings(QString _path)
{

	if (_path.isEmpty())
	{
		std::cout << HERE 
			<< "Warning  ::  Setting file not found \n please set the setting file " << std::endl;
		//_path = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(),  // dialog to open files
		//	"Ini file (*.ini);; Data file (*.dat);; Binary File (*.bin);; All Files(*.*)", 0);
	}

	std::cout << HERE 
		 << "  the path is : " << _path.toStdString() << std::endl;

	m_settings = new QSettings(_path, QSettings::IniFormat);

	// read default group
	QString user = m_settings->value("default/user", "No profile selected").toString();
	ui_tools->lineEdit_userName->setText(user);

	QString comment = m_settings->value("default/comment", "Initial run").toString();
	ui_tools->lineEdit_comment->setText(comment);

	QString language = m_settings->value("default/language", "English").toString();
	ui_tools->comboBox_language->setCurrentIndex(parseLanguageString(language));
	m_GUI_params->setLanguage(ui_tools->comboBox_language->currentIndex());
	
	// read com group
	//ComName
	QString comPort = m_settings->value("COM/ComName", "COM1").toString();
	m_comSettings->setName ( comPort.toStdString());

	//BaudRate
	int baudRate = m_settings->value("COM/BaudRate", "115200").toInt();
	m_comSettings->setBaudRate(baudRate);
	ui_tools->comboBox_baudRate->setCurrentIndex(7);  // baudrate forced value 115200 

	//DataBits
	int dataBits = m_settings->value("COM/DataBits", "8").toInt();
	switch (dataBits) {
	case 5:
		m_comSettings->setDataBits (serial::fivebits);
		ui_tools->comboBox_dataBit->setCurrentIndex(0);
		break;
	case 6:
		m_comSettings->setDataBits(serial::sixbits);
		ui_tools->comboBox_dataBit->setCurrentIndex(1);
		break;
	case 7:
		m_comSettings->setDataBits(serial::sevenbits);
		ui_tools->comboBox_dataBit->setCurrentIndex(2);
		break;
	case 8:
		m_comSettings->setDataBits(serial::eightbits);
		ui_tools->comboBox_dataBit->setCurrentIndex(3);

		break;
	default:
		std::cerr << HERE 
			 << " Error data bit cannot be read, using default value 8" << std::endl;
		m_comSettings->setDataBits(serial::eightbits);
		ui_tools->comboBox_dataBit->setCurrentIndex(3);
		break;
	}

	//Parity = NoParity
	QString parity = m_settings->value("COM/Parity", "NoParity").toString();
	ui_tools->comboBox_parity->setCurrentIndex(
		ui_tools->comboBox_parity->findText(parity));
	m_comSettings->setParity(static_cast<serial::parity_t>(
		ui_tools->comboBox_parity->currentIndex()));


	//StopBits = 1
	QString stopBits = m_settings->value("COM/StopBits", "OneStop").toString();
	ui_tools->comboBox_stopBit->setCurrentIndex(
		ui_tools->comboBox_stopBit->findText(stopBits));
	m_comSettings->setStopBits(static_cast<serial::stopbits_t>(
		ui_tools->comboBox_stopBit->currentIndex()));


	//FlowControl = noFlow
	QString flowControl = m_settings->value("COM/FlowControl", "noFlow").toString();
	ui_tools->comboBox_flowControl->setCurrentIndex(
		ui_tools->comboBox_flowControl->findText(flowControl));
	m_comSettings->setFlowControl(static_cast<serial::flowcontrol_t>(
		ui_tools->comboBox_flowControl->currentIndex()));

	//read GUI params
	bool is_first_run = m_settings->value("GUI/isFirstRun", "1").toBool();
	m_GUI_params->isFirstRun = is_first_run;
	
	bool enable_tool_tips = m_settings->value("GUI/EnableToolTips", "0").toBool();
	ui_tools->checkBox_enableToolTips->setChecked(enable_tool_tips);
	m_GUI_params->enableToolTips = enable_tool_tips;
	
	QString profileSettingFile = m_settings->value("GUI/SettingProfile", "Standard").toString();
	ui_tools->comboBox_profileSetting->blockSignals(true);
	ui_tools->comboBox_profileSetting->setCurrentIndex(
		ui_tools->comboBox_profileSetting->findText(profileSettingFile));
	ui_tools->comboBox_profileSetting->blockSignals(false);

	bool verb_out = m_settings->value("GUI/VerboseOutput", "0").toBool();
	ui_tools->checkBox_verboseOut->setChecked(verb_out);
	m_GUI_params->verboseOutput = verb_out;

	bool en_history = m_settings->value("GUI/EnableHistory", "1").toBool();
	ui_tools->checkBox_EnableHistory->setChecked(en_history);
	m_GUI_params->enableHistory = en_history;

	bool dump_to_file = m_settings->value("GUI/DumpHistoryToFile", "1").toBool();
	ui_tools->checkBox_dumpToFile->setChecked(dump_to_file);
	m_GUI_params->dumpHistoryToFile = dump_to_file;

	bool speech_active = m_settings->value("GUI/SpeechActive", "0").toBool();
	ui_tools->checkBox_enableSynthesis->setChecked(speech_active);
	m_GUI_params->speechActive = speech_active;

	bool enable_IONoptix = m_settings->value("GUI/restrictOPmode", "0").toBool();
	ui_tools->checkBox_restrictOPmode->setChecked(enable_IONoptix);
	m_GUI_params->restrictOPmode = enable_IONoptix;

	bool enable_IONoptixLogo = m_settings->value("GUI/enableIONoptixLogo", "0").toBool();
	ui_tools->checkBox_enableIonOptixLogo->setChecked(enable_IONoptixLogo);
	m_GUI_params->useIONoptixLogo = enable_IONoptixLogo;
		
	QString out_file_path = m_settings->value("GUI/OutFilePath", "./Ext_data/").toString();
	m_GUI_params->outFilePath = out_file_path; 

	int automaticUpdates_index = m_settings->value("GUI/AutomaticUpdates", "2").toInt();
	ui_tools->comboBox_automaticUpdates->setCurrentIndex(automaticUpdates_index);
	m_GUI_params->automaticUpdates_idx = automaticUpdates_index;

	// read pr_limits group
    bool ok = false;
    int p_on_max = m_settings->value("pr_limits/p_on_max", "450").toInt(&ok);
    if (!ok) {
		std::cerr << HERE 
			<< " p_on_max corrupted in setting file, using default value " << std::endl;
		p_on_max = 450;
	}
	ui_tools->spinBox_p_on_max->setValue(p_on_max);
	m_pr_params->p_on_max = p_on_max;

	int p_on_min = m_settings->value("pr_limits/p_on_min", "0").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " p_on_min corrupted in setting file, using default value " << std::endl;
		p_on_min = 0;
	}
	ui_tools->spinBox_p_on_min->setValue(p_on_min);
	m_pr_params->p_on_min = p_on_min;

	//int p_on_default = m_settings->value("pr_limits/p_on_default", "190").toInt(&ok);
	int p_on_default = m_settings->value("pv_standardAndRegular/p_on", "55").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " p_on_default corrupted in setting file, using default value " << std::endl;
		p_on_default = 55;
	}
	ui_tools->spinBox_p_on_default->setValue(p_on_default);
	m_pr_params->p_on_default = p_on_default;

	int p_off_max = m_settings->value("pr_limits/p_off_max", "450").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " p_off_max corrupted in setting file, using default value " << std::endl;
		p_off_max = 450;
	}
	ui_tools->spinBox_p_off_max->setValue(p_off_max);
	m_pr_params->p_off_max = p_off_max;

	int p_off_min = m_settings->value("pr_limits/p_off_min", "0").toInt(&ok);
	if (!ok) {
		std::cerr << HERE 
			<< " p_off_min corrupted in setting file, using default value " << std::endl;
		p_off_min = 0;
	}
	ui_tools->spinBox_p_off_min->setValue(p_off_min);
	m_pr_params->p_off_min = p_off_min;

	//int p_off_default = m_settings->value("pr_limits/p_off_default", "21").toInt(&ok);
	int p_off_default = m_settings->value("pv_standardAndRegular/p_off", "12").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " p_off_default corrupted in setting file, using default value " << std::endl;
		p_off_default = 12;
	}
	ui_tools->spinBox_p_off_default->setValue(p_off_default);
	m_pr_params->p_off_default = p_off_default;

	int v_switch_max = m_settings->value("pr_limits/v_switch_max", "0").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " v_switch_max corrupted in setting file, using default value " << std::endl;
		v_switch_max = 0;
	}
	ui_tools->spinBox_v_switch_max->setValue(v_switch_max);
	m_pr_params->v_switch_max = v_switch_max;

	int v_switch_min = m_settings->value("pr_limits/v_switch_min", "-300").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " v_switch_min corrupted in setting file, using default value " << std::endl;
		v_switch_min = -300;
	}
	ui_tools->spinBox_v_switch_min->setValue(v_switch_min);
	m_pr_params->v_switch_min = v_switch_min;

	//int v_switch_default = m_settings->value("pr_limits/v_switch_default", "-115").toInt(&ok);
	int v_switch_default = m_settings->value("pv_standardAndRegular/v_switch", "-100").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " v_switch_default corrupted in setting file, using default value " << std::endl;
		v_switch_default = -100;
	}
	ui_tools->spinBox_v_switch_default->setValue(v_switch_default);
	m_pr_params->v_switch_default = v_switch_default;

	int v_recirc_max = m_settings->value("pr_limits/v_recirc_max", "0").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " v_recirc_max corrupted in setting file, using default value " << std::endl;
		v_recirc_max = 0;
	}
	ui_tools->spinBox_v_recirc_max->setValue(v_recirc_max);
	m_pr_params->v_recirc_max = v_recirc_max;

	int v_recirc_min = m_settings->value("pr_limits/v_recirc_min", "-300").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " v_recirc_min corrupted in setting file, using default value " << std::endl;
		v_recirc_min = -300;
	}
	ui_tools->spinBox_v_recirc_min->setValue(v_recirc_min);
	m_pr_params->v_recirc_min = v_recirc_min;

	//int v_recirc_default = m_settings->value("pr_limits/v_recirc_default", "-115").toInt(&ok);
	int v_recirc_default = m_settings->value("pv_standardAndRegular/v_recirc", "-85").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " v_recirc_default corrupted in setting file, using default value " << std::endl;
		v_recirc_default = -85;
	}
	ui_tools->spinBox_v_recirc_default->setValue(v_recirc_default);
	m_pr_params->v_recirc_default = v_recirc_default;

	int base_ds_increment = m_settings->value("pr_limits/base_ds_increment", "10").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " base_ds_increment corrupted in setting file, using default value " << std::endl;
		base_ds_increment = 10;
	}
	ui_tools->spinBox_ds_increment->setValue(base_ds_increment);
	m_pr_params->base_ds_increment = base_ds_increment;

	int base_fs_increment = m_settings->value("pr_limits/base_fs_increment", "10").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " base_fs_increment corrupted in setting file, using default value " << std::endl;
		base_fs_increment = 10;
	}
	ui_tools->spinBox_fs_increment->setValue(base_fs_increment);
	m_pr_params->base_fs_increment = base_fs_increment;

	int base_v_increment = m_settings->value("pr_limits/base_v_increment", "5").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " base_v_increment corrupted in setting file, using default value " << std::endl;
		base_v_increment = 5;
	}
	ui_tools->spinBox_v_increment->setValue(base_v_increment);
	m_pr_params->base_v_increment = base_v_increment;

	int p_on_sAs = m_settings->value("pv_standardAndSlow/p_on", "120").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_standardAndSlow/p_on corrupted in setting file, using default value " << std::endl;
		p_on_sAs = 120;
	}
	ui_tools->spinBox_sAs_Pon_def->setValue(p_on_sAs);
	m_pr_params->p_on_sAs = p_on_sAs;

	int p_off_sAs = m_settings->value("pv_standardAndSlow/p_off", "7").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_standardAndSlow/p_off corrupted in setting file, using default value " << std::endl;
		p_off_sAs = 7;
	}
	ui_tools->spinBox_sAs_Poff_def->setValue(p_off_sAs);
	m_pr_params->p_off_sAs = p_off_sAs;

	int v_switch_sAs = m_settings->value("pv_standardAndSlow/v_switch", "-70").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_standardAndSlow/v_switch corrupted in setting file, using default value " << std::endl;
		v_switch_sAs = -70;
	}
	ui_tools->spinBox_sAs_Vs_def->setValue(v_switch_sAs);
	m_pr_params->v_switch_sAs = v_switch_sAs;

	int v_recirc_sAs = m_settings->value("pv_standardAndSlow/v_recirc", "-60").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_standardAndSlow/v_recirc corrupted in setting file, using default value " << std::endl;
		v_recirc_sAs = -60;
	}
	ui_tools->spinBox_sAs_Vr_def->setValue(v_recirc_sAs);
	m_pr_params->v_recirc_sAs = v_recirc_sAs;


	int p_on_sAr = m_settings->value("pv_standardAndRegular/p_on", "160").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_standardAndRegular/p_on corrupted in setting file, using default value " << std::endl;
		p_on_sAr = 160;
	}
	ui_tools->spinBox_sAr_Pon_def->setValue(p_on_sAr);
	m_pr_params->p_on_sAr = p_on_sAr;

	int p_off_sAr = m_settings->value("pv_standardAndRegular/p_off", "12").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_standardAndRegular/p_off corrupted in setting file, using default value " << std::endl;
		p_off_sAr = 12;
	}
	ui_tools->spinBox_sAr_Poff_def->setValue(p_off_sAr);
	m_pr_params->p_off_sAr = p_off_sAr;

	int v_switch_sAr = m_settings->value("pv_standardAndRegular/v_switch", "-100").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_standardAndRegular/v_switch corrupted in setting file, using default value " << std::endl;
		v_switch_sAr = -100;
	}
	ui_tools->spinBox_sAr_Vs_def->setValue(v_switch_sAr);
	m_pr_params->v_switch_sAr = v_switch_sAr;

	int v_recirc_sAr = m_settings->value("pv_standardAndRegular/v_recirc", "-85").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_standardAndRegular/v_recirc corrupted in setting file, using default value " << std::endl;
		v_recirc_sAr = -85;
	}
	ui_tools->spinBox_sAr_Vr_def->setValue(v_recirc_sAr);
	m_pr_params->v_recirc_sAr = v_recirc_sAr;

	int p_on_lAs = m_settings->value("pv_largeAndSlow/p_on", "140").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_largeAndSlow/p_on corrupted in setting file, using default value " << std::endl;
		p_on_lAs = 140;
	}
	ui_tools->spinBox_lAs_Pon_def->setValue(p_on_lAs);
	m_pr_params->p_on_lAs = p_on_lAs;

	int p_off_lAs = m_settings->value("pv_largeAndSlow/p_off", "8").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_largeAndSlow/p_off corrupted in setting file, using default value " << std::endl;
		p_off_lAs = 8;
	}
	ui_tools->spinBox_lAs_Poff_def->setValue(p_off_lAs);
	m_pr_params->p_off_lAs = p_off_lAs;

	int v_switch_lAs = m_settings->value("pv_largeAndSlow/v_switch", "-72").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_largeAndSlow/v_switch corrupted in setting file, using default value " << std::endl;
		v_switch_lAs = -72;
	}
	ui_tools->spinBox_lAs_Vs_def->setValue(v_switch_lAs);
	m_pr_params->v_switch_lAs = v_switch_lAs;

	int v_recirc_lAs = m_settings->value("pv_largeAndSlow/v_recirc", "-68").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_largeAndSlow/v_recirc corrupted in setting file, using default value " << std::endl;
		v_recirc_lAs = -68;
	}
	ui_tools->spinBox_lAs_Vr_def->setValue(v_recirc_lAs);
	m_pr_params->v_recirc_lAs = v_recirc_lAs;

	int p_on_lAr = m_settings->value("pv_largeAndRegular/p_on", "170").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_largeAndRegular/p_on corrupted in setting file, using default value " << std::endl;
		p_on_lAr = 170;
	}
	ui_tools->spinBox_lAr_Pon_def->setValue(p_on_lAr);
	m_pr_params->p_on_lAr = p_on_lAr;

	int p_off_lAr = m_settings->value("pv_largeAndRegular/p_off", "10").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_largeAndRegular/p_off corrupted in setting file, using default value " << std::endl;
		p_off_lAr = 10;
	}
	ui_tools->spinBox_lAr_Poff_def->setValue(p_off_lAr);
	m_pr_params->p_off_lAr = p_off_lAr;

	int v_switch_lAr = m_settings->value("pv_largeAndRegular/v_switch", "-90").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_largeAndRegular/v_switch corrupted in setting file, using default value " << std::endl;
		v_switch_lAr = -90;
	}
	ui_tools->spinBox_lAr_Vs_def->setValue(v_switch_lAr);
	m_pr_params->v_switch_lAr = v_switch_lAr;

	int v_recirc_lAr = m_settings->value("pv_largeAndRegular/v_recirc", "-85").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_largeAndRegular/v_recirc corrupted in setting file, using default value " << std::endl;
		v_recirc_lAr = -85;
	}
	ui_tools->spinBox_lAr_Vr_def->setValue(v_recirc_lAr);
	m_pr_params->v_recirc_lAr = v_recirc_lAr;

	int p_on_p1 = m_settings->value("pv_preset1/p_on", "55").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset1/p_on corrupted in setting file, using default value " << std::endl;
		p_on_p1 = 55;
	}
	ui_tools->spinBox_p_on_preset1->setValue(p_on_p1);
	m_pr_params->p_on_preset1 = p_on_p1; 

	int p_off_p1 = m_settings->value("pv_preset1/p_off", "12").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset1/p_off corrupted in setting file, using default value " << std::endl;
		p_off_p1 = 12;
	}
	ui_tools->spinBox_p_off_preset1->setValue(p_off_p1);
	m_pr_params->p_off_preset1 = p_off_p1;

	int v_switch_p1 = m_settings->value("pv_preset1/v_switch", "-100").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset1/v_switch corrupted in setting file, using default value " << std::endl;
		v_switch_p1 = -100;
	}
	ui_tools->spinBox_v_switch_preset1->setValue(v_switch_p1);
	m_pr_params->v_switch_preset1 = v_switch_p1;

	int v_recirc_p1 = m_settings->value("pv_preset1/v_recirc", "-85").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset1/v_recirc corrupted in setting file, using default value " << std::endl;
		v_recirc_p1 = -85;
	}
	ui_tools->spinBox_v_recirc_preset1->setValue(v_recirc_p1);
	m_pr_params->v_recirc_preset1 = v_recirc_p1;

	int p_on_p2 = m_settings->value("pv_preset2/p_on", "110").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset2/p_on corrupted in setting file, using default value " << std::endl;
		p_on_p2 = 110;
	}
	ui_tools->spinBox_p_on_preset2->setValue(p_on_p2);
	m_pr_params->p_on_preset2 = p_on_p2;

	int p_off_p2 = m_settings->value("pv_preset2/p_off", "24").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset2/p_off corrupted in setting file, using default value " << std::endl;
		p_off_p2 = 24;
	}
	ui_tools->spinBox_p_off_preset2->setValue(p_off_p2);
	m_pr_params->p_off_preset2 = p_off_p2;

	int v_switch_p2 = m_settings->value("pv_preset2/v_switch", "-200").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset2/v_switch corrupted in setting file, using default value " << std::endl;
		v_switch_p2 = -200;
	}
	ui_tools->spinBox_v_switch_preset2->setValue(v_switch_p2);
	m_pr_params->v_switch_preset2 = v_switch_p2;

	int v_recirc_p2 = m_settings->value("pv_preset2/v_recirc", "-170").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset2/v_recirc corrupted in setting file, using default value " << std::endl;
		v_recirc_p2 = -170;
	}
	ui_tools->spinBox_v_recirc_preset2->setValue(v_recirc_p2);
	m_pr_params->v_recirc_preset2 = v_recirc_p2;

	int p_on_p3 = m_settings->value("pv_preset3/p_on", "160").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset3/p_on corrupted in setting file, using default value " << std::endl;
		p_on_p3 = 160;
	}
	ui_tools->spinBox_p_on_preset3->setValue(p_on_p3);
	m_pr_params->p_on_preset3 = p_on_p3;

	int p_off_p3 = m_settings->value("pv_preset3/p_off", "14").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset3/p_off corrupted in setting file, using default value " << std::endl;
		p_off_p3 = 14;
	}
	ui_tools->spinBox_p_off_preset3->setValue(p_off_p3);
	m_pr_params->p_off_preset3 = p_off_p3;

	int v_switch_p3 = m_settings->value("pv_preset3/v_switch", "-110").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset3/v_switch corrupted in setting file, using default value " << std::endl;
		v_switch_p3 = -110;
	}
	ui_tools->spinBox_v_switch_preset3->setValue(v_switch_p3);
	m_pr_params->v_switch_preset3 = v_switch_p3;

	int v_recirc_p3 = m_settings->value("pv_preset3/v_recirc", "-90").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< "pv_preset3/v_recirc corrupted in setting file, using default value " << std::endl;
		v_recirc_p3 = -90;
	}
	ui_tools->spinBox_v_recirc_preset3->setValue(v_recirc_p1);
	m_pr_params->v_recirc_preset3 = v_recirc_p3;


	int tip_type = m_settings->value("tip/tip_type", "0").toInt(&ok);
	if (!ok)
	{
		std::cerr << HERE
			<< "tip/tip_type corrupted in setting file, using default value " << std::endl;
		tip_type = 0;
	}
	ui_tools->comboBox_tipSelection->setCurrentIndex(tip_type);

	//m_pr_params->-----= ddddd;
	//
	
	double length_to_zone = m_settings->value("tip/length_to_zone", "0.065").toDouble(&ok);
	if (!ok)
	{
		std::cerr << HERE
			<< "tip/length_to_zone corrupted in setting file, using default value " << std::endl;
		length_to_zone = DEFAULT_LENGTH_TO_ZONE_STANDARD;
	}
	ui_tools->doubleSpinBox_lengthToZone->setValue(length_to_zone);

	double length_to_tip = m_settings->value("tip/length_to_tip", "0.062").toDouble(&ok);
	if (!ok)
	{
		std::cerr << HERE
			<< "tip/length_to_zone corrupted in setting file, using default value " << std::endl;
		length_to_tip = DEFAULT_LENGTH_TO_TIP_STANDARD;
	}
	ui_tools->doubleSpinBox_lengthToTip->setValue(length_to_tip);


	bool verbose_out = m_settings->value("PPC1/VerboseOut", "1").toBool();
	ui_tools->checkBox_enablePPC1verboseOut->setChecked(verbose_out);
	m_pr_params->verboseOut = verbose_out;

	bool use_def_v_set_p = m_settings->value("PPC1/UseDefValSetPoint", "1").toBool();
	ui_tools->checkBox_useSetPoint->setChecked(use_def_v_set_p); 
	m_pr_params->useDefValSetPoint = use_def_v_set_p;

	bool enable_filter = m_settings->value("PPC1/EnableFilter", "1").toBool();
	ui_tools->checkBox_enablePPC1filter->setChecked(enable_filter);
	m_pr_params->enableFilter = enable_filter;
	ui_tools->spinBox_PPC1filterSize->setEnabled(enable_filter);

	int filter_size = m_settings->value("PPC1/FilterSize", "20").toInt(&ok);
	if (!ok) {
		std::cerr << HERE << " filter size is not valid " << std::endl;
		filter_size = 10;
	}
	ui_tools->spinBox_PPC1filterSize->setValue(filter_size);
	m_pr_params->filterSize = filter_size;

	int wait_sync_timeout = m_settings->value("PPC1/WaitSyncTimeout", "60").toInt(&ok);
	if (!ok) {
		std::cerr << HERE << " wait sync timeout is not valid " << std::endl;
		wait_sync_timeout = 10;
	}
	ui_tools->spinBox_PPC1_sync_timeout->setValue(wait_sync_timeout);
	m_pr_params->waitSyncTimeout = wait_sync_timeout;


	//Read solution volumes block
	int vol_sol1 = m_settings->value("solutions/volWell1", "25").toInt(&ok);
	if (!ok) {
		std::cerr << HERE 
			<< " volume of solution 1 corrupted in setting file, using default value " << std::endl;
	}
	ui_tools->spinBox_vol_sol1->setValue(vol_sol1);
	m_solutionParams->vol_well1 = vol_sol1;

	int vol_sol2 = m_settings->value("solutions/volWell2", "25").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " volume of solution 2 corrupted in setting file, using default value " << std::endl;
	}
	ui_tools->spinBox_vol_sol2->setValue(vol_sol2);
	m_solutionParams->vol_well2 = vol_sol2;

	int vol_sol3 = m_settings->value("solutions/volWell3", "25").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " volume of solution 3 corrupted in setting file, using default value " << std::endl;
	}
	ui_tools->spinBox_vol_sol3->setValue(vol_sol3);
	m_solutionParams->vol_well3 = vol_sol3;


	int vol_sol4 = m_settings->value("solutions/volWell4", "25").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " volume of solution 4 corrupted in setting file, using default value " << std::endl;
	}
	ui_tools->spinBox_vol_sol4->setValue(vol_sol4);
	m_solutionParams->vol_well4 = vol_sol4;

	int vol_sol5 = m_settings->value("solutions/volWell5", "25").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " volume of solution 5 corrupted in setting file, using default value " << std::endl;
	}
	ui_tools->spinBox_vol_sol5->setValue(vol_sol5);
	m_solutionParams->vol_well5 = vol_sol5;

	int vol_sol6 = m_settings->value("solutions/volWell6", "25").toInt(&ok);
	if (!ok) {
		std::cerr << HERE
			<< " volume of solution 6 corrupted in setting file, using default value " << std::endl;
	}
	ui_tools->spinBox_vol_sol6->setValue(vol_sol6);
	m_solutionParams->vol_well6 = vol_sol6;

	//Read solution names block
	QString solname1 = m_settings->value("solutions/sol1", "no name").toString();
	ui_tools->lineEdit_sol1_name->setText(solname1);
	m_solutionParams->sol1 = solname1; 

	QString solname2 = m_settings->value("solutions/sol2", "no name").toString();
	ui_tools->lineEdit_sol2_name->setText(solname2);
	m_solutionParams->sol2 = solname2; 

	QString solname3 = m_settings->value("solutions/sol3", "no name").toString();
	ui_tools->lineEdit_sol3_name->setText(solname3);
	m_solutionParams->sol3 = solname3; 

	QString solname4 = m_settings->value("solutions/sol4", "no name").toString();
	ui_tools->lineEdit_sol4_name->setText(solname4);
	m_solutionParams->sol4 = solname4; 

	QString solname5 = m_settings->value("solutions/sol5", "no name").toString();
	ui_tools->lineEdit_sol5_name->setText(solname5);
	m_solutionParams->sol5 = solname5;

	QString solname6 = m_settings->value("solutions/sol6", "no name").toString();
	ui_tools->lineEdit_sol6_name->setText(solname6);
	m_solutionParams->sol6 = solname6;

	int sol1colSlider = m_settings->value("solutions/sol1colSlider", "3522620").toInt();
	ui_tools->horizontalSlider_colorSol1->setValue(sol1colSlider);
	colorSol1Changed(sol1colSlider);

		
	int sol2colSlider = m_settings->value("solutions/sol2colSlider", "5164400").toInt();
	ui_tools->horizontalSlider_colorSol2->setValue(sol2colSlider);
	colorSol2Changed(sol2colSlider);

	int sol3colSlider = m_settings->value("solutions/sol3colSlider", "12926220").toInt();
	ui_tools->horizontalSlider_colorSol3->setValue(sol3colSlider);
	colorSol3Changed(sol3colSlider);

	int sol4colSlider = m_settings->value("solutions/sol4colSlider", "1432930").toInt();
	ui_tools->horizontalSlider_colorSol4->setValue(sol4colSlider);
	colorSol4Changed(sol4colSlider);
	
	int sol5colSlider = m_settings->value("solutions/sol5colSlider", "5164400").toInt();
	ui_tools->horizontalSlider_colorSol5->setValue(sol5colSlider);
	colorSol5Changed(sol5colSlider);
	
	int sol6colSlider = m_settings->value("solutions/sol6colSlider", "3522620").toInt();
	ui_tools->horizontalSlider_colorSol6->setValue(sol6colSlider);
	colorSol6Changed(sol6colSlider);
	



	double pulseDuration1 = m_settings->value("solutions/pulseDuration1", "5.0").toDouble();
	ui_tools->doubleSpinBox_pulse_sol1->setValue(pulseDuration1);
	m_solutionParams->pulse_duration_well1 = pulseDuration1; 

	double pulseDuration2 = m_settings->value("solutions/pulseDuration2", "5.0").toDouble();
	ui_tools->doubleSpinBox_pulse_sol2->setValue(pulseDuration2);
	m_solutionParams->pulse_duration_well2 = pulseDuration2;
	
	double pulseDuration3 = m_settings->value("solutions/pulseDuration3", "5.0").toDouble();
	ui_tools->doubleSpinBox_pulse_sol3->setValue(pulseDuration3);
	m_solutionParams->pulse_duration_well3 = pulseDuration3;
	
	double pulseDuration4 = m_settings->value("solutions/pulseDuration4", "5.0").toDouble();
	ui_tools->doubleSpinBox_pulse_sol4->setValue(pulseDuration4);
	m_solutionParams->pulse_duration_well4 = pulseDuration4;
	
	double pulseDuration5 = m_settings->value("solutions/pulseDuration5", "10.0").toDouble();
	ui_tools->doubleSpinBox_pulse_sol5->setValue(pulseDuration5);
	m_solutionParams->pulse_duration_well5 = pulseDuration5;
	
	double pulseDuration6 = m_settings->value("solutions/pulseDuration6", "10.0").toDouble();
	ui_tools->doubleSpinBox_pulse_sol6->setValue(pulseDuration6);
	m_solutionParams->pulse_duration_well6 = pulseDuration6;
	
	bool disableTimer_s1 = m_settings->value("solutions/continuousFlowingWell1", "1").toBool();
	ui_tools->checkBox_disableTimer_s1->setChecked(disableTimer_s1);
	ui_tools->doubleSpinBox_pulse_sol1->setEnabled(!disableTimer_s1);
	m_solutionParams->continuous_flowing_sol1 = disableTimer_s1;

	bool disableTimer_s2 = m_settings->value("solutions/continuousFlowingWell2", "1").toBool();
	ui_tools->checkBox_disableTimer_s2->setChecked(disableTimer_s2);
	ui_tools->doubleSpinBox_pulse_sol2->setEnabled(!disableTimer_s2);
	m_solutionParams->continuous_flowing_sol2 = disableTimer_s2;

	bool disableTimer_s3 = m_settings->value("solutions/continuousFlowingWell3", "1").toBool();
	ui_tools->checkBox_disableTimer_s3->setChecked(disableTimer_s3);
	ui_tools->doubleSpinBox_pulse_sol3->setEnabled(!disableTimer_s3);
	m_solutionParams->continuous_flowing_sol3 = disableTimer_s3;

	bool disableTimer_s4 = m_settings->value("solutions/continuousFlowingWell4", "1").toBool();
	ui_tools->checkBox_disableTimer_s4->setChecked(disableTimer_s4);
	ui_tools->doubleSpinBox_pulse_sol4->setEnabled(!disableTimer_s4);
	m_solutionParams->continuous_flowing_sol4 = disableTimer_s4;

	bool disableTimer_s5 = m_settings->value("solutions/continuousFlowingWell5", "1").toBool();
	ui_tools->checkBox_disableTimer_s5->setChecked(disableTimer_s5);
	ui_tools->doubleSpinBox_pulse_sol5->setEnabled(!disableTimer_s5);
	m_solutionParams->continuous_flowing_sol5 = disableTimer_s5;

	bool disableTimer_s6 = m_settings->value("solutions/continuousFlowingWell6", "1").toBool();
	ui_tools->checkBox_disableTimer_s6->setChecked(disableTimer_s6);
	ui_tools->doubleSpinBox_pulse_sol6->setEnabled(!disableTimer_s6);
	m_solutionParams->continuous_flowing_sol6 = disableTimer_s6;
	// continuous flowing

	this->applyPressed();
	return true;
}

bool BioZone6_tools::saveSettings(QString _file_name)
{

	//m_settings->setPath(QSettings::IniFormat, QSettings::UserScope, _file_name);
	QSettings *settings = new QSettings(_file_name, QSettings::IniFormat);
	// [default]
	// user = 
	settings->setValue("default/user", ui_tools->lineEdit_userName->text());
	// comment = 
	settings->setValue("default/comment", ui_tools->lineEdit_comment->text());
	// language = 
	settings->setValue("default/language", ui_tools->comboBox_language->currentText());
	
	// [COM]
	// ComName = COM_
	settings->setValue("COM/ComName", QString::fromStdString(m_comSettings->getName())); 
	// BaudRate = 115200
	settings->setValue("COM/BaudRate", ui_tools->comboBox_baudRate->currentText()); 
	// DataBits = 8
	settings->setValue("COM/DataBits", ui_tools->comboBox_dataBit->currentText());
	// Parity = NoParity
	settings->setValue("COM/Parity", ui_tools->comboBox_parity->currentText());
	// StopBits = 1
	settings->setValue("COM/StopBits", ui_tools->comboBox_stopBit->currentText());
	// FlowControl = noFlow
	settings->setValue("COM/FlowControl", ui_tools->comboBox_flowControl->currentText());

	// [GUI]
//	settings->setValue("GUI/ToolButtonStyle", ui_tools->comboBox_toolButtonStyle->currentIndex());
	settings->setValue("GUI/isFirstRun", int(0));
	settings->setValue("GUI/EnableToolTips", int(ui_tools->checkBox_enableToolTips->isChecked()));
	settings->setValue("GUI/VerboseOutput", int(ui_tools->checkBox_verboseOut->isChecked()));
	settings->setValue("GUI/EnableHistory", int(ui_tools->checkBox_EnableHistory->isChecked()));
	settings->setValue("GUI/DumpHistoryToFile", int(ui_tools->checkBox_dumpToFile->isChecked())); 
	settings->setValue("GUI/SpeechActive", int(ui_tools->checkBox_enableSynthesis->isChecked()));
	settings->setValue("GUI/OutFilePath", QString(ui_tools->lineEdit_msg_out_file_path->text()));
	settings->setValue("GUI/restrictOPmode", int(ui_tools->checkBox_restrictOPmode->isChecked()));
	settings->setValue("GUI/enableIONoptixLogo", int(ui_tools->checkBox_enableIonOptixLogo->isChecked()));
	settings->setValue("GUI/SettingProfile", ui_tools->comboBox_profileSetting->currentText());
	

	// automatic updates =
	settings->setValue("GUI/AutomaticUpdates", int(ui_tools->comboBox_automaticUpdates->currentIndex()));

	// [pr_limits]
	// p_on_max = 
	settings->setValue("pr_limits/p_on_max", ui_tools->spinBox_p_on_max->value());
	// p_on_min =
	settings->setValue("pr_limits/p_on_min", ui_tools->spinBox_p_on_min->value());
	// p_on_default = 
	settings->setValue("pr_limits/p_on_default", ui_tools->spinBox_p_on_default->value());
	// p_off_max = 
	settings->setValue("pr_limits/p_off_max", ui_tools->spinBox_p_off_max->value());
	// p_off_min = 
	settings->setValue("pr_limits/p_off_min", ui_tools->spinBox_p_off_min->value());
	// p_off_default = 
	settings->setValue("pr_limits/p_off_default", ui_tools->spinBox_p_off_default->value());
	// v_switch_max = 
	settings->setValue("pr_limits/v_switch_max", ui_tools->spinBox_v_switch_max->value());
	// v_switch_min = 
	settings->setValue("pr_limits/v_switch_min", ui_tools->spinBox_v_switch_min->value());
	// v_switch_default = 
	settings->setValue("pr_limits/v_switch_default", ui_tools->spinBox_v_switch_default->value());
	// v_recirc_max = 
	settings->setValue("pr_limits/v_recirc_max", ui_tools->spinBox_v_recirc_max->value());
	// v_recirc_min = 
	settings->setValue("pr_limits/v_recirc_min", ui_tools->spinBox_v_recirc_min->value());
	// v_recirc_default = 
	settings->setValue("pr_limits/v_recirc_default", ui_tools->spinBox_v_recirc_default->value());
	// base_ds_increment = 
	settings->setValue("pr_limits/base_ds_increment", ui_tools->spinBox_ds_increment->value());
    // base_fs_increment =
	settings->setValue("pr_limits/base_fs_increment", ui_tools->spinBox_fs_increment->value());
    // base_v_increment =
	settings->setValue("pr_limits/base_v_increment", ui_tools->spinBox_v_increment->value());

	settings->setValue("pv_standardAndSlow/p_on", ui_tools->spinBox_sAs_Pon_def->value());
	settings->setValue("pv_standardAndSlow/p_off", ui_tools->spinBox_sAs_Poff_def->value());
	settings->setValue("pv_standardAndSlow/v_switch", ui_tools->spinBox_sAs_Vs_def->value());
	settings->setValue("pv_standardAndSlow/v_recirc", ui_tools->spinBox_sAs_Vr_def->value());

	settings->setValue("pv_standardAndRegular/p_on", ui_tools->spinBox_sAr_Pon_def->value());
	settings->setValue("pv_standardAndRegular/p_off", ui_tools->spinBox_sAr_Poff_def->value());
	settings->setValue("pv_standardAndRegular/v_switch", ui_tools->spinBox_sAr_Vs_def->value());
	settings->setValue("pv_standardAndRegular/v_recirc", ui_tools->spinBox_sAr_Vr_def->value());

	settings->setValue("pv_largeAndSlow/p_on", ui_tools->spinBox_lAs_Pon_def->value());
	settings->setValue("pv_largeAndSlow/p_off", ui_tools->spinBox_lAs_Poff_def->value());
	settings->setValue("pv_largeAndSlow/v_switch", ui_tools->spinBox_lAs_Vs_def->value());
	settings->setValue("pv_largeAndSlow/v_recirc", ui_tools->spinBox_lAs_Vr_def->value());

	settings->setValue("pv_largeAndRegular/p_on", ui_tools->spinBox_lAr_Pon_def->value());
	settings->setValue("pv_largeAndRegular/p_off", ui_tools->spinBox_lAr_Poff_def->value());
	settings->setValue("pv_largeAndRegular/v_switch", ui_tools->spinBox_lAr_Vs_def->value());
	settings->setValue("pv_largeAndRegular/v_recirc", ui_tools->spinBox_lAr_Vr_def->value());


	settings->setValue("pv_preset1/p_on", ui_tools->spinBox_p_on_preset1->value());
	settings->setValue("pv_preset1/p_off", ui_tools->spinBox_p_off_preset1->value());
	settings->setValue("pv_preset1/v_switch", ui_tools->spinBox_v_switch_preset1->value());
	settings->setValue("pv_preset1/v_recirc", ui_tools->spinBox_v_recirc_preset1->value());

	settings->setValue("pv_preset2/p_on", ui_tools->spinBox_p_on_preset2->value());
	settings->setValue("pv_preset2/p_off", ui_tools->spinBox_p_off_preset2->value());
	settings->setValue("pv_preset2/v_switch", ui_tools->spinBox_v_switch_preset2->value());
	settings->setValue("pv_preset2/v_recirc", ui_tools->spinBox_v_recirc_preset2->value());

	settings->setValue("pv_preset3/p_on", ui_tools->spinBox_p_on_preset3->value());
	settings->setValue("pv_preset3/p_off", ui_tools->spinBox_p_off_preset3->value());
	settings->setValue("pv_preset3/v_switch", ui_tools->spinBox_v_switch_preset3->value());
	settings->setValue("pv_preset3/v_recirc", ui_tools->spinBox_v_recirc_preset3->value());

	settings->setValue("PPC1/VerboseOut", int(ui_tools->checkBox_enablePPC1verboseOut->isChecked()));
	settings->setValue("PPC1/UseDefValSetPoint", int(ui_tools->checkBox_useSetPoint->isChecked())); 
	settings->setValue("PPC1/EnableFilter", int(ui_tools->checkBox_enablePPC1filter->isChecked()));
	settings->setValue("PPC1/FilterSize", int(ui_tools->spinBox_PPC1filterSize->value()));
	settings->setValue("PPC1/WaitSyncTimeout", int(ui_tools->spinBox_PPC1_sync_timeout->value()));

	// [Well volumes]
	// well 1
	settings->setValue("solutions/volWell1", ui_tools->spinBox_vol_sol1->value());
	// well 2
	settings->setValue("solutions/volWell2", ui_tools->spinBox_vol_sol2->value());
	// well 3
	settings->setValue("solutions/volWell3", ui_tools->spinBox_vol_sol3->value());
	// well 4
	settings->setValue("solutions/volWell4", ui_tools->spinBox_vol_sol4->value());
	// well 5
	settings->setValue("solutions/volWell5", ui_tools->spinBox_vol_sol5->value());
	// well 6
	settings->setValue("solutions/volWell6", ui_tools->spinBox_vol_sol6->value());

	// [tip settings]
	settings->setValue("tip/tip_type", ui_tools->comboBox_tipSelection->currentIndex());
	settings->setValue("tip/length_to_zone", ui_tools->doubleSpinBox_lengthToZone->value());
	settings->setValue("tip/length_to_tip", ui_tools->doubleSpinBox_lengthToTip->value());


	// [solutionNames]
	// solution1
	settings->setValue("solutions/sol1", ui_tools->lineEdit_sol1_name->text());
	// solution2
	settings->setValue("solutions/sol2", ui_tools->lineEdit_sol2_name->text());
	// solution3 
	settings->setValue("solutions/sol3", ui_tools->lineEdit_sol3_name->text());
	// solution4
	settings->setValue("solutions/sol4", ui_tools->lineEdit_sol4_name->text());
	// solution5 
	settings->setValue("solutions/sol5", ui_tools->lineEdit_sol5_name->text());
	// solution6
	settings->setValue("solutions/sol6", ui_tools->lineEdit_sol6_name->text());
	// sol1colSlider
	settings->setValue("solutions/sol1colSlider", ui_tools->horizontalSlider_colorSol1->value());
	// sol2colSlider
	settings->setValue("solutions/sol2colSlider", ui_tools->horizontalSlider_colorSol2->value());
	// sol3colSlider
	settings->setValue("solutions/sol3colSlider", ui_tools->horizontalSlider_colorSol3->value());
	// sol4colSlider
	settings->setValue("solutions/sol4colSlider", ui_tools->horizontalSlider_colorSol4->value());
	// sol5colSlider
	settings->setValue("solutions/sol5colSlider", ui_tools->horizontalSlider_colorSol5->value());
	// sol6colSlider
	settings->setValue("solutions/sol6colSlider", ui_tools->horizontalSlider_colorSol6->value());

	// pulse time solution 1
	settings->setValue("solutions/pulseDuration1", ui_tools->doubleSpinBox_pulse_sol1->value());
	// pulse time solution 2
	settings->setValue("solutions/pulseDuration2", ui_tools->doubleSpinBox_pulse_sol2->value());
	// pulse time solution 3
	settings->setValue("solutions/pulseDuration3", ui_tools->doubleSpinBox_pulse_sol3->value());
	// pulse time solution 4
	settings->setValue("solutions/pulseDuration4", ui_tools->doubleSpinBox_pulse_sol4->value());
	// pulse time solution 5
	settings->setValue("solutions/pulseDuration5", ui_tools->doubleSpinBox_pulse_sol5->value());
	// pulse time solution 6
	settings->setValue("solutions/pulseDuration6", ui_tools->doubleSpinBox_pulse_sol6->value());
	// continuous flowing sol 1
	settings->setValue("solutions/continuousFlowingWell1", int(ui_tools->checkBox_disableTimer_s1->isChecked()));
	// continuous flowing sol 2
	settings->setValue("solutions/continuousFlowingWell2", int(ui_tools->checkBox_disableTimer_s2->isChecked()));
	// continuous flowing sol 3
	settings->setValue("solutions/continuousFlowingWell3", int(ui_tools->checkBox_disableTimer_s3->isChecked()));
	// continuous flowing sol 4
	settings->setValue("solutions/continuousFlowingWell4", int(ui_tools->checkBox_disableTimer_s4->isChecked()));
	// continuous flowing sol 5
	settings->setValue("solutions/continuousFlowingWell5", int(ui_tools->checkBox_disableTimer_s5->isChecked()));
	// continuous flowing sol 6
	settings->setValue("solutions/continuousFlowingWell6", int(ui_tools->checkBox_disableTimer_s6->isChecked()));

	settings->sync();
	
	return true;
}

//void BioZone6_tools::enableTipSetting()
//{
//	return unlockProtectedSettings(askPasswordToUnlock());
//}

bool BioZone6_tools::askPasswordToUnlock()
{
	//ask password
	//if correct 
	bool ok;
	QString text = QInputDialog::getText(0, m_str_warning,
		m_ask_password, QLineEdit::Password,
		"", &ok);
	if (ok && !text.isEmpty()) {
		QString password = text;
		QString password_check = "FluicellGrowth2018";
		if (!password.compare(password_check))
		{
			QMessageBox::information(this, m_str_information, m_correct_password);
			m_expert = true;
			return true;
		}
		else
		{
			QMessageBox::warning(this, m_str_information, m_wrong_password);
			m_expert = false;
			return false;
		}
	}
	return false;
}

void BioZone6_tools::tipSelection(int _idx)
{
	if (_idx == 0)
	{   //tip prime
		ui_tools->doubleSpinBox_lengthToTip->setValue(DEFAULT_LENGTH_TO_TIP_STANDARD);
		ui_tools->doubleSpinBox_lengthToZone->setValue(DEFAULT_LENGTH_TO_ZONE_STANDARD);
		m_tip->useStandardTip();

		//TODO SET THE DEFAULT VALUES
		//Standard
		//Pon(mbar) 190
		//ui_tools->spinBox_p_on_default->setValue(190);
		
		//Poff(mbar) 22
		//ui_tools->spinBox_p_off_default->setValue(22);
		
		//Vswitch(mbar) - 115
		//ui_tools->spinBox_v_switch_default->setValue(-115);
		
		//Vrecirc(mbar) - 115
		//ui_tools->spinBox_v_recirc_default->setValue(-115);
	}
	if (_idx == 1)
	{   // tip flex
		ui_tools->doubleSpinBox_lengthToTip->setValue(DEFAULT_LENGTH_TO_TIP_WIDE);
		ui_tools->doubleSpinBox_lengthToZone->setValue(DEFAULT_LENGTH_TO_ZONE_WIDE);
		m_tip->useWideTip();
		//TODO: add save of the tip type
		// TODO SET THE DEFAULT VALUES
		//Standard
		//Pon(mbar) 215
		//ui_tools->spinBox_p_on_default->setValue(215);
		
		//Poff(mbar) 30
		//ui_tools->spinBox_p_off_default->setValue(30);
		
		//Vswitch(mbar) - 135
		//ui_tools->spinBox_v_switch_default->setValue(-135);
		
		//Vrecirc(mbar) - 135
		//ui_tools->spinBox_v_recirc_default->setValue(-135);
		//Standby
		//Pon(mbar) 0
		//Poff(mbar) 11
		//Vswitch(mbar) - 55
		//Vrecirc(mbar) - 55

	}
	if (_idx == 2)
	{   // tip flex
		ui_tools->doubleSpinBox_lengthToTip->setValue(DEFAULT_LENGTH_TO_TIP_ION);
		ui_tools->doubleSpinBox_lengthToZone->setValue(DEFAULT_LENGTH_TO_ZONE_ION);
		m_tip->useUWZTip();
		//TODO: add save of the tip type
		// TODO SET THE DEFAULT VALUES
		//Standard
		//Pon(mbar) 215
		//ui_tools->spinBox_p_on_default->setValue(215);
		
		//Poff(mbar) 30
		//ui_tools->spinBox_p_off_default->setValue(30);
		
		//Vswitch(mbar) - 135
		//ui_tools->spinBox_v_switch_default->setValue(-135);
		
		//Vrecirc(mbar) - 135
		//ui_tools->spinBox_v_recirc_default->setValue(-135);
		//Standby
		//Pon(mbar) 0
		//Poff(mbar) 11
		//Vswitch(mbar) - 55
		//Vrecirc(mbar) - 55

	}

}


void BioZone6_tools::resetToDefaultValues()
{
	QString msg;
	msg.append(m_str_factory_reset);
	msg.append("\n");
	msg.append(m_str_areyousure);

	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, m_str_warning, msg,
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		return;
	}
	else {
		loadSettings("");
	}
}


void BioZone6_tools::initCustomStrings()
{

	m_str_warning = tr("Warning");
	m_str_factory_reset = tr("This will reset user defined settings and parameters to the factory default values");
	m_str_areyousure = tr("Are you sure?");
	m_str_override_setting_profile = tr("This action will overwrite your current setting to load the selected profile, are you sure?");
	m_str_information = tr("Information");
	m_str_ok = tr("Ok");
	m_str_operation_cancelled = tr("Operation cancelled");
	m_str_history_cleaned = tr("History cleaned");
	m_ask_password = tr("This is for expert users only, a password is required");
	m_wrong_password = tr("Wrong password");
	m_correct_password = tr("Correct password");
}

int BioZone6_tools::parseLanguageString(QString _language)
{
	if (_language == "Chinese") 
	{
		return GUIparams::Chinese;
	}
	if (_language == "English")
	{
		return GUIparams::English;
	}
	if (_language == "Italiano")
	{
		return GUIparams::Italiano;
	}
	if (_language == "Svenska")
	{
		return GUIparams::Svenska;
	}
	return 0;
}



uint32_t BioZone6_tools::giveRainbowColor(float _position)
{

	// this function gives 1D linear RGB color gradient
	// color is proportional to position
	// position  <0;1>
	// position means position of color in color gradient

	if (_position>1) _position = 1;//position-int(position);
								// if position > 1 then we have repetition of colors
								// it maybe useful
	uint8_t R = 0;// byte
	uint8_t G = 0;// byte
	uint8_t B = 0;// byte
	int nmax = 6;// number of color bars
	float m = nmax* _position;
	int n = int(m); // integer of m
	float f = m - n;  // fraction of m
	uint8_t t = int(f * 255);


	switch (n) {
	case 0:
	{
		R = 0;
		G = 255;
		B = t;
		break;
	}

	case 1:
	{
		R = 0;
		G = 255 - t;
		B = 255;
		break;
	}
	case 2:
	{
		R = t;
		G = 0;
		B = 255;
		break;
	}
	case 3:
	{
		R = 255;
		G = 0;
		B = 255 - t;
		break;
	}
	case 4:
	{
		R = 255;
		G = t;
		B = 0;
		break;
	}
	case 5: {
		R = 255 - t;
		G = 255;
		B = 0;
		break;
	}
	case 6:
	{
		R = 0;
		G = 255;
		B = 0;
		break;
	}

	}; // case


	return (R << 16) | (G << 8) | B;
}

//void BioZone6_tools::unlockProtectedSettings(bool _lock)
//{
//	ui_tools->doubleSpinBox_lengthToTip->setEnabled(_lock);
//	ui_tools->doubleSpinBox_lengthToZone->setEnabled(_lock);
//	ui_tools->checkBox_restrictOPmode->setEnabled(_lock);
//	//ui_tools->comboBox_tipSelection->setEnabled(_lock);
//}



void BioZone6_tools::switchLanguage(QString _translation_file)
{
	std::cout << HERE << std::endl;
	
	qApp->removeTranslator(&m_translator_tool);

	if (m_translator_tool.load(_translation_file)) {
		//m_translator_tool.translate("Labonatip_tool", "english");
		qApp->installTranslator(&m_translator_tool);

		ui_tools->retranslateUi(this); 

		initCustomStrings();

		std::cout << HERE << " installTranslator" << std::endl;
	}

}


BioZone6_tools::~BioZone6_tools() {

	delete m_comSettings;
	delete m_solutionParams;
	delete m_pr_params;
	delete m_GUI_params;
	delete m_settings;

	delete ui_tools;
}
