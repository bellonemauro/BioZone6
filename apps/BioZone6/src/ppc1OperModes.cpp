﻿/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "BioZone6_GUI.h"


void BioZone6_GUI::setStandardAndSlow(bool _verbose )
{
	std::cout << HERE << std::endl;


	// if the button is unchecked, check it and uncheck all the others
	if (ui->pushButton_standardAndSlow->isChecked())
	{
		
		// reset this to 0 to avoid interference between modes
		ui->spinBox_PonOM->setValue(0);

		// uncheck the other buttons
		ui->pushButton_standardAndRegular->setChecked(false);
		ui->pushButton_largeAndSlow->setChecked(false);
		ui->pushButton_largeAndRegular->setChecked(false);


		// get the values 
		int new_p_on_default = m_pr_params->p_on_sAs;// 40;
		int new_p_off_default = m_pr_params->p_off_sAs;//8;
		int new_v_switch_default = m_pr_params->v_switch_sAs;//-90;
		int new_v_recirc_default = m_pr_params->v_recirc_sAs;//-45;
		//    <setPon value="40" message=" "/>
		//    <setPoff value = "8" message = " " / >
		//    <setVrecirc value = "-45" message = " " / >
		//	  <setVswitch value = "-90" message = " " / >

		// apply the new values 
		m_dialog_tools->setDefaultPressuresVacuums(new_p_on_default, new_p_off_default, 
			-new_v_recirc_default, -new_v_switch_default);

		// update button protocol files path 
		//m_button_protocol_path = m_protocol_path + 
		//	QString::fromStdString(m_ppc1->getTip()->tip_setting_path)+
		//	"/presetProtocols/internal/ButtonsPRTFiles/StandardAndSlow/";
		m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_SnS_path;

		if (_verbose)
		{
			if (getOnSolutionIndex() != 0)
			{
				QMessageBox::warning(this, m_str_warning,
					"A solution is ON, the solution will be OFF once the new setting is applied by pushing the operational button");
				//TODO: set the string 
			}
			else {
				QMessageBox::information(this, m_str_information,
					m_new_settings_applied);
			}
		}
	}
	else {
		ui->pushButton_standardAndSlow->setChecked(true);
	}
}

void BioZone6_GUI::setStandardAndRegular(bool _verbose )
{
	std::cout << HERE << std::endl;
	

	if (ui->pushButton_standardAndRegular->isChecked())
	{
		// reset this to 0 to avoid interference between modes
		ui->spinBox_PonOM->setValue(0);

		std::cout << HERE << "CHECKED" << std::endl;
		// uncheck the other buttons
		ui->pushButton_standardAndSlow->setChecked(false);
		ui->pushButton_largeAndSlow->setChecked(false);
		ui->pushButton_largeAndRegular->setChecked(false);

		// get the values 
		int new_p_on_default = m_pr_params->p_on_sAr;// 50;
		int new_p_off_default = m_pr_params->p_off_sAr;//11;
		int new_v_switch_default = m_pr_params->v_switch_sAr;//-115;
		int new_v_recirc_default = m_pr_params->v_recirc_sAr;//-75;
		//    <setPon value="50" message=" "/>
		//    <setPoff value = "11" message = " " / >
		//	<setVrecirc value = "-75" message = " " / >
		//	<setVswitch value = "-115" message = " " / >

		// apply the new values 
		m_dialog_tools->setDefaultPressuresVacuums(new_p_on_default, new_p_off_default,
			-new_v_recirc_default, -new_v_switch_default);
		
		// update button protocol files path 
		//m_button_protocol_path = m_protocol_path + 
		//	QString::fromStdString(m_ppc1->getTip()->tip_setting_path)+
		//	"/presetProtocols/internal/ButtonsPRTFiles/StandardAndRegular/";
		m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_SnR_path;// "/ButtonsPRTFiles/StandardAndRegular/";


		if (_verbose)
		{
			if (getOnSolutionIndex() != 0)
			{
				QMessageBox::warning(this, m_str_warning,
					"A solution is ON, the solution will be OFF once the new setting is applied by pushing the operational button");
				//TODO: set the string 
			}
			else {
				QMessageBox::information(this, m_str_information,
					m_new_settings_applied);
			}
		}
	}
	else {
		ui->pushButton_standardAndRegular->setChecked(true);
	}
}

void BioZone6_GUI::setLargeAndSlow(bool _verbose )
{
	std::cout << HERE << std::endl;
	

	if (ui->pushButton_largeAndSlow->isChecked())
	{
		// reset this to 0 to avoid interference between modes
		ui->spinBox_PonOM->setValue(0);

		// uncheck the other buttons
		ui->pushButton_standardAndSlow->setChecked(false);
		ui->pushButton_standardAndRegular->setChecked(false);
		ui->pushButton_largeAndRegular->setChecked(false);

		// get the values 
		int new_p_on_default = m_pr_params->p_on_lAs;// 40;
		int new_p_off_default = m_pr_params->p_off_lAs;//8;
		int new_v_switch_default = m_pr_params->v_switch_lAs;//-80;
		int new_v_recirc_default = m_pr_params->v_recirc_lAs;//45;
		//    <setPon value="40" message=" "/>
		//    <setPoff value = "8" message = " " / >
		//	<setVrecirc value = "-45" message = " " / >
		//	<setVswitch value = "-80" message = " " / >

		// apply the new values 
		m_dialog_tools->setDefaultPressuresVacuums(new_p_on_default, new_p_off_default,
			-new_v_recirc_default, -new_v_switch_default);

		// update button protocol files path 
		//m_button_protocol_path = m_protocol_path + 
		//	QString::fromStdString(m_ppc1->getTip()->tip_setting_path)+
		//	"/presetProtocols/internal/ButtonsPRTFiles/LargeAndSlow/";
		m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_LnS_path;// 

		if (_verbose)
		{
			if (getOnSolutionIndex() != 0)
			{
				QMessageBox::warning(this, m_str_warning,
					"A solution is ON, the solution will be OFF once the new setting is applied by pushing the operational button");
				//TODO: set the string 
			}
			else {
				QMessageBox::information(this, m_str_information,
					m_new_settings_applied);
			}
		}
	}
	else {
		ui->pushButton_largeAndSlow->setChecked(true);
	}
}

void BioZone6_GUI::setLargeAndRegular(bool _verbose )
{
	std::cout << HERE << std::endl;


	if (ui->pushButton_largeAndRegular->isChecked())
	{
		// reset this to 0 to avoid interference between modes
		ui->spinBox_PonOM->setValue(0);

		// uncheck the other buttons
		ui->pushButton_standardAndSlow->setChecked(false);
		ui->pushButton_standardAndRegular->setChecked(false);
		ui->pushButton_largeAndSlow->setChecked(false);
		
		// get the values 
		int new_p_on_default = m_pr_params->p_on_lAr;// 50;
		int new_p_off_default = m_pr_params->p_off_lAr;//11;
		int new_v_switch_default = m_pr_params->v_switch_lAr;//-105;
		int new_v_recirc_default = m_pr_params->v_recirc_lAr;//-60;
		//    <setPon value="50" message=" "/>
		//    <setPoff value = "11" message = " " / >
		//	<setVrecirc value = "-60" message = " " / >
		//	<setVswitch value = "-105" message = " " / >
				
		// apply the new values 
		m_dialog_tools->setDefaultPressuresVacuums(new_p_on_default, new_p_off_default,
			-new_v_recirc_default, -new_v_switch_default);
		
		// update button protocol files path 
		//m_button_protocol_path = m_protocol_path + 
		//	QString::fromStdString(m_ppc1->getTip()->tip_setting_path) +
		//	"/presetProtocols/internal/ButtonsPRTFiles/LargeAndRegular/";
		m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_LnR_path;// 

		if (_verbose)
		{
			if (getOnSolutionIndex() != 0)
			{
				QMessageBox::warning(this, m_str_warning,
					"A solution is ON, the solution will be OFF once the new setting is applied by pushing the operational button");
				//TODO: set the string 
			}
			else {
				QMessageBox::information(this, m_str_information,
					m_new_settings_applied);
			}
		}
	}
	else {
		ui->pushButton_largeAndRegular->setChecked(true);
	}
}



void BioZone6_GUI::onPlusPonOM()
{
	// lineEdit_PonOM + something
	std::cout << HERE << ui->spinBox_PonOM->value() << std::endl;
	int val = ui->spinBox_PonOM->value() + 1;
	ui->spinBox_PonOM->setValue(val);
	
	// this shoud actually modify the solution protocols

	ui->groupBox_PonOM->setEnabled(false);
	if (ui->pushButton_solution1->isChecked() ||
		ui->pushButton_solution2->isChecked() ||
		ui->pushButton_solution3->isChecked() ||
		ui->pushButton_solution4->isChecked() ||
		ui->pushButton_solution5->isChecked() ||
		ui->pushButton_solution6->isChecked())
	{
		double Pon_adapted = m_pipette_status->pon_set_point + 1;
		updatePonSetPoint(Pon_adapted);
		if(m_pipette_active) 
			QThread::msleep(150);
		double Vr_adapted = m_pipette_status->v_recirc_set_point - 1;
		updateVrecircSetPoint(Vr_adapted);
		if (m_pipette_active)
			QThread::msleep(150);
	}
	ui->groupBox_PonOM->setEnabled(true);
}


void BioZone6_GUI::onMinusPonOM()
{
	// lineEdit_PonOM - something
	std::cout << HERE << ui->spinBox_PonOM->value() << std::endl;
	int val = ui->spinBox_PonOM->value() - 1;
	ui->spinBox_PonOM->setValue(val);

	// this shoud actually modify the solution protocols
	ui->groupBox_PonOM->setEnabled(false);
	if (ui->pushButton_solution1->isChecked() ||
		ui->pushButton_solution2->isChecked() ||
		ui->pushButton_solution3->isChecked() ||
		ui->pushButton_solution4->isChecked() ||
		ui->pushButton_solution5->isChecked() ||
		ui->pushButton_solution6->isChecked())
	{
		double Pon_adapted = m_pipette_status->pon_set_point - 1;
		updatePonSetPoint(Pon_adapted);
		if (m_pipette_active) 
			QThread::msleep(150);
		double Vr_adapted = m_pipette_status->v_recirc_set_point + 1;
		updateVrecircSetPoint(Vr_adapted);
		if (m_pipette_active)
			QThread::msleep(150);
	}

	ui->groupBox_PonOM->setEnabled(true);
}

