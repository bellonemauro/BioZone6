﻿/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef BioZone6_TOOLS_H_
#define BioZone6_TOOLS_H_

// standard libraries
#include <iostream>
#include <string>

#include "ui_tools.h"
#include <QMainWindow>
#include <QSettings>
#include <QTranslator>
#include <QDateTime>
#include <QMessageBox>
#include <QDir>

#include <serial/serial.h>

#include <dataStructures.h>


/** \brief BioZone6_tools class for settings
*
*   Tools open a new window to allow the user to choose settings. 
*   There are 4 main subsection:
*      - General settings : 
*      - Solution settings :
*      - Pressure and vacuum settings :
*      - Communication settings :
*
*   For each of this section a data structure is defined in the 
*   header dataStructure.h
*
*/
class BioZone6_tools : public  QMainWindow
{
	Q_OBJECT

	/** Create signals to be passed to the main app,
	*   the signals allows the tools class to send information to the main class for specific events
	*   the main class implements a connect to a slot to handle the emitted signals
	* 
	*/
	signals :
		void ok();    //!< signal generated when ok is pressed
		void apply(); //!< signal generated when apply is pressed
		void cancel(); //!< signal generated when cancel is pressed
		void emptyWaste(); //!< signal generated when empty wells is pressed
		void refillSolution(); //!< signal generated when empty wells is pressed
		void TTLsignal(bool _state); //!< signal generated test TTL is pressed
		void colSol1Changed(const int _r, const int _g, const int _b); //!< signal generated when the solution color is changed
		void colSol2Changed(const int _r, const int _g, const int _b); //!< signal generated when the solution color is changed
		void colSol3Changed(const int _r, const int _g, const int _b); //!< signal generated when the solution color is changed
		void colSol4Changed(const int _r, const int _g, const int _b); //!< signal generated when the solution color is changed
		void colSol5Changed(const int _r, const int _g, const int _b); //!< signal generated when the solution color is changed
		void colSol6Changed(const int _r, const int _g, const int _b); //!< signal generated when the solution color is changed
		void checkUpdatesNow(); //!< signal generated when the updates button is pressed

public:

	explicit BioZone6_tools(QWidget *parent = 0); //!< Explicit ctor

	~BioZone6_tools(); //!< dtor

	/** \brief Get communication settings
	*
	*   Expose to the main class the communication settings
	*
	*   \return COMSettings
	*/
	COMSettings getComSettings() { return *m_comSettings; }

	/** \brief Get solution settings
	*
	*   Expose to the main class the solution settings
	*
	*   \return solutionsParams
	*/
	solutionsParams getSolutionsParams() { return *m_solutionParams; }

	/** \brief Get pressure and vacuum settings
	*
	*   Expose to the main class the pressure and vacuum settings
	*
	*   \return pr_params
	*/
	pr_params getPr_params() { return *m_pr_params; }

	/** \brief Get GUI settings
	*
	*   Expose to the main class the GUI settings
	*
	*   \return GUIparams
	*/
	GUIparams getGUIparams() { return *m_GUI_params; }

	void setFirstRun(bool _firstRun) { m_GUI_params->isFirstRun = _firstRun; }

	fluicell::PPC1api6dataStructures::tip::tipType getTipType()
	{
		return m_tip->type;//   ui_tools->comboBox_tipSelection->currentIndex();
	}

	fluicell::PPC1api6dataStructures::tip getTip() { return *m_tip; }
	bool isExpertMode() { return m_expert; }

	void switchLanguage(QString _translation_file);

	void updateDevices() { this->enumerate(); 
	this->applyPressed();
	}

	void setSettingsFolderPath(QString _folder) {
		m_setting_folder_path = _folder;
	}

	bool setLoadSettingsFileName(QString _filename) { 
		m_setting_file_name = _filename; 
		return loadSettings(m_setting_file_name);
	}

	void setExtDataPath(QString _filename) {
		m_GUI_params->outFilePath = _filename;
		ui_tools->lineEdit_msg_out_file_path->setText(_filename);
	}

	bool setFileNameAndSaveSettings(QString _filename) {
		m_setting_file_name = _filename;
		return saveSettings(_filename);
	}

	void setDefaultPressuresVacuums(int _p_on_default, int _p_off_default, 
		                            int _v_recirc_default, int _v_switch_default);

	void setDefaultPressuresVacuums_sAr(int _new_p_on, int _new_p_off,
		int _new_v_recirc, int _new_v_switch);

	void setDefaultPressuresVacuums_lAr(int _new_p_on, int _new_p_off,
		int _new_v_recirc, int _new_v_switch);

	void setDefaultPressuresVacuums_sAs(int _new_p_on, int _new_p_off,
		int _new_v_recirc, int _new_v_switch);

	void setDefaultPressuresVacuums_lAs(int _new_p_on, int _new_p_off,
		int _new_v_recirc, int _new_v_switch);

	/** Load an ini setting file
	* in the GUI initialization it takes a default value ./settings/setting.ini
	* \note
	*/
	bool loadSettings(QString _path = QString("./settings/setting.ini"));

	/** Save the setting file
	*
	* \note
	*/
	bool saveSettings(QString _file_name = QString("./settings/setting.ini"));

	/** \brief Set values of preset group 1
	*
	* \note
	*/
	void setPreset1(int _p_on, int _p_off, int _v_switch, int _v_recirc);

	/** \brief Set values of preset group 2
	*
	* \note
	*/
	void setPreset2(int _p_on, int _p_off, int _v_switch, int _v_recirc);

	/** \brief Set values of preset group 3
	*
	* \note
	*/
	void setPreset3(int _p_on, int _p_off, int _v_switch, int _v_recirc);

	/** \brief Set values of a custom preset mode
	*
	* \note
	*/
	void setCustomPreset(int _p_on, int _p_off, int _v_switch, int _v_recirc);


	QString getUserName() { return ui_tools->lineEdit_userName->text(); }

private slots:

	void goToPage1();
	void goToPage2();
	void goToPage3();
	void goToPage4();

	/** Show information for the serial port communication
	*
	* \note
	*/
	void showPortInfo(int idx);

	void enableToolTip(int _inx);

	void enableIONoptix_checked(int _inx);

	void settingProfileChanged(int _idx);

	void checkForUpdates();

	void testTTL();
	/** Color solution 1 changed
	*
	* \note
	*/
	void colorSol1Changed(int _value);

	/** Color solution 2 changed
	*
	* \note
	*/
	void colorSol2Changed(int _value);

	/** Color solution 3 changed
	*
	* \note
	*/
	void colorSol3Changed(int _value);

	/** Color solution 4 changed
	*
	* \note
	*/
	void colorSol4Changed(int _value);

	/** Color solution 5 changed
	*
	* \note
	*/
	void colorSol5Changed(int _value);

	/** Color solution 6 changed
	*
	* \note
	*/
	void colorSol6Changed(int _value);

	/** \brief Enable/Disable the timer for an infinite solution flow
	*
	* \note
	*/
    void setContinuousFlow_s1(int _state) {
		  ui_tools->doubleSpinBox_pulse_sol1->setEnabled(!_state); 
	}

	/** \brief Enable/Disable the timer for an infinite solution flow
	*
	* \note
	*/
    void setContinuousFlow_s2(int _state) {
		ui_tools->doubleSpinBox_pulse_sol2->setEnabled(!_state); 
	}

	/** \brief Enable/Disable the timer for an infinite solution flow
	*
	* \note
	*/
    void setContinuousFlow_s3(int _state) {
		ui_tools->doubleSpinBox_pulse_sol3->setEnabled(!_state); 
	}

	/** \brief Enable/Disable the timer for an infinite solution flow
	*
	* \note
	*/
    void setContinuousFlow_s4(int _state) {
		ui_tools->doubleSpinBox_pulse_sol4->setEnabled(!_state); 
	}


	/** \brief Enable/Disable the timer for an infinite solution flow
	*
	* \note
	*/
	void setContinuousFlow_s5(int _state) {
		ui_tools->doubleSpinBox_pulse_sol5->setEnabled(!_state);
	}

	/** \brief Enable/Disable the timer for an infinite solution flow
	*
	* \note
	*/
	void setContinuousFlow_s6(int _state) {
		ui_tools->doubleSpinBox_pulse_sol6->setEnabled(!_state);
	}
	/** emit ok signal, save the setting, send the current macro to the main
	*   and close the window
	* \note
	*/
	void okPressed();

	/** Emit cancel signal and close the window
	* \note
	*/
	void cancelPressed();

	/** emit apply signal, save the setting, and send the current macro to the main
	*   
	*/
	void applyPressed();

	/** emit empty waste signal on button pressed
	*
	*/
	void emptyWastePressed();

	/** emit empty refill solution signal on button pressed
	*
	*/
	void refillSolutionPressed();

	/** Enumerate serial ports
	*
	*/
	void enumerate();

	/** on enable check box clicked, it enable/disable the filtering
	*
	*/
	void enablePPC1filtering() {
		ui_tools->spinBox_PPC1filterSize->setEnabled(
			ui_tools->checkBox_enablePPC1filter->isChecked());
	}

	/** on check box clicked, it enable/disable the operational modes setting fields
	*
	*/
	void activateOperationaModeSettings(int _enable = false);

	

	/** \brief ask a password to unlock protected tip settings
	*
	*/
	//void enableTipSetting();



	void tipSelection(int _idx);

	/** \brief Reset all the values to default values
	*
	*/
	void resetToDefaultValues();



private:

	QSettings *m_settings; //!<  data member containing the setting information

	/** \brief Initialize all the custom strings
	*
	*/
	void initCustomStrings();

	/** \brief Parse the language string
	*
	*   The string _language is parced as an index
	*/
	int parseLanguageString(QString _language);

	/** \brief Get the serial port setting from the tool dialog
	*
	*/
	void getCOMsettingsFromGUI();

	/** \brief Get the solution setting from the tool dialog
	*
	*/
	void getSolutionSettingsFromGUI();

	/** \brief Get the GUI setting from the tool dialog
	*
	*/
	void getGUIsettingsFromGUI();

	/** \brief Get the pressure and vacuum setting from the tool dialog
	*
	*/
	void getPRsettingsFromGUI();

	void getTipSettingsFromGUI();

	/** \brief ask a password
	*
	* \return true if the password is correct
	*/
	bool askPasswordToUnlock();

	/** \brief From _position to 3 RGB bytes 
	*
	*/
	uint32_t giveRainbowColor(float _position);

	/** \brief lock protected settings
	*
	*/
	//void unlockProtectedSettings(bool _lock);



    /** \brief Check the size of the history and ask to clean it if necessary
    *
    */
    void checkHistory ();


	/** \brief Calculate the size of a folder, used for cleaning history
	*
	*/
	int calculateFolderSize(const QString _wantedDirPath);

	QString m_setting_file_name;   //!<  setting file name for loading and saving
	QString m_setting_folder_path;   //!<  setting folder name for loading and saving
	QString m_setting_profile_standard_file_name;   //!<  setting file name for loading and saving
	QString m_setting_profile_wide_file_name;   //!<  setting file name for loading and saving
	QString m_setting_profile_UWZ_file_name;   //!<  setting file name for loading and saving

	QTranslator m_translator_tool; //!<  translation object

	COMSettings *m_comSettings;    //!<  serial port setting structure
	solutionsParams *m_solutionParams;  //!<  solution setting structure
	pr_params *m_pr_params;       //!<  pressure and vacuum setting structure
	GUIparams *m_GUI_params;      //!<  GUI setting structure

	fluicell::PPC1api6dataStructures::tip *m_tip;
	bool m_expert;             //!< expert mode, set to true upon correct password

	// translatable strings
	QString m_str_warning;
	QString m_str_factory_reset;
	QString m_str_areyousure;
	QString m_str_override_setting_profile;
	QString m_str_information; 
	QString m_str_ok;
	QString m_str_operation_cancelled;
	QString m_str_history_cleaned;
	QString m_ask_password;
	QString m_wrong_password;
	QString m_correct_password;

protected:
	Ui::BioZone6_tools *ui_tools;    //!<  the user interface
};


#endif /* BioZone6_TOOLS_H_ */
