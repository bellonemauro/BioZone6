/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "BioZone6_GUI.h"

void BioZone6_GUI::newTip()
{
	std::cout << HERE << std::endl;

	setEnableMainWindow(false);

	//Ask: Place the pipette into the holder and tighten.THEN PRESS OK.
	this->askMessage(m_str_newtip_msg1);
	QApplication::setOverrideCursor(Qt::WaitCursor);   

	// reset wells and solutions
	emptyWells();
	refillSolution();

	//vf0
	closeAllValves();

	//D0
	updatePonSetPoint(0.0);

	//C0
	updatePoffSetPoint(0.0);

	//B0
	updateVswitchSetPoint(0.0);

	//A0
	updateVrecircSetPoint(0.0);

	//Wait 5 seconds
	if (!visualizeProgressMessage(5, m_str_initialization)) return;

	//D200
	updatePonSetPoint(200.0);

	//Wait 5 seconds
	if (!visualizeProgressMessage(5, m_str_newtip_msg2)) return;

	//vff
	if (m_pipette_active) {
		m_ppc1->openAllValves();
	}

	//Ask : wait until a droplet appears at the tip of the pipette and THEN PRESS OK.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	this->askMessage(m_str_newtip_msg3);
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	//Wait 40 seconds
	if (!visualizeProgressMessage(40, m_str_newtip_msg4)) return;

	//vf0
	if (m_pipette_active) {
		m_ppc1->closeAllValves();
	}

	//D0
	updatePonSetPoint(0.0);

	//Wait 10 seconds
	if (!visualizeProgressMessage(10, m_str_newtip_msg5)) return;

	//Ask : Remove the droplet using a lens tissue and put the pipette into solution.THEN PRESS OK.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	this->askMessage(m_str_newtip_msg6);
	this->askMessage(m_str_newtip_msg7);
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	//B - 200
	updateVswitchSetPoint(200.0);

	//A - 200
	updateVrecircSetPoint(200.0);

	//Wait 90 seconds
	if (!visualizeProgressMessage(90, m_str_newtip_msg8)) return;

	//C21
	updatePoffSetPoint(m_pr_params->p_off_default );

	//D190
	updatePonSetPoint(m_pr_params->p_on_default );

	//Wait 5 seconds
	if (!visualizeProgressMessage(5, m_str_newtip_msg9)) return;

	//B - 115
	updateVswitchSetPoint(-m_pr_params->v_switch_default);

	//A - 115
	updateVrecircSetPoint(-m_pr_params->v_recirc_default );

	//Ask: Pipette is ready for operation.PRESS OK TO START.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	this->askMessage(m_str_newtip_msg10);
	
	setEnableMainWindow(true);

}

void BioZone6_GUI::runProtocol()
{
	std::cout << HERE << std::endl;

	QString macro_path = m_current_protocol_file_name;
	QString msg = m_str_loaded_protocol_is;
	QStringList l = macro_path.split("/"); // the split is to show the name only (remove the path)
	QString name = l.last();
	msg.append(name);
	msg.append("<br>");
	msg.append(m_str_protocol_confirm);

	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, m_str_information, msg,
			QMessageBox::Cancel | QMessageBox::Ok,
			QMessageBox::Ok);

	if (resBtn == QMessageBox::Cancel)
	{
		m_macroRunner_thread->disconnect();

		m_macroRunner_thread->setSimulationFlag(m_simulationOnly);
		m_macroRunner_thread->killMacro(true);

		m_ppc1->setVerbose(m_pr_params->verboseOut);
		
		QString s = " Protocol execution stopped : ";
		s.append(m_current_protocol_file_name);
		int remaining_time_sec = m_protocol_duration - 0 * m_protocol_duration / 100;
		s.append(" ----- remaining time,  ");
		s.append(generateDurationString(remaining_time_sec));
		ui->progressBar_macroStatus->setValue(0);
		ui->label_macroStatus->setText(s);

		return;
	}

    std::cout << HERE << "  " << msg.toStdString() << std::endl;
    //the tree in the editor MUST be saved to be run
	QString tmp_file = QDir::tempPath();
	tmp_file.append("/tmp_biozone6_protocol.prt");
	this->saveXml(tmp_file, ui->treeWidget_macroTable);
	this->runProtocolFile(tmp_file);
}


void BioZone6_GUI::runProtocolFile(QString _protocol_path) {

	std::cout << HERE << " :: running protocol --> " << _protocol_path.toStdString() << std::endl;

	if (!m_macroRunner_thread->isRunning()) { 

		//if (!m_protocol) {
		//	QMessageBox::information(this, m_str_information,
		//		m_str_no_protocol_load_first);
		//	return;
		//} 
		//QApplication::setOverrideCursor(Qt::WaitCursor);
		
		// Reload the protocol in a virtual tree every time is executed to avoid 
		// modifications to the existing protocol in the editor
		QTreeWidget* virtual_tree = new QTreeWidget;
		this->openXml(_protocol_path, virtual_tree);
		addAllCommandsToPPC1Protocol(virtual_tree, m_protocol);

		//update the chart
		m_chart_view->updateChartProtocol(m_protocol);

		QApplication::restoreOverrideCursor();

		m_ppc1->setVerbose(false);

		std::cout << HERE << " :: running protocol --> protocol size " << m_protocol->size() << std::endl;

		m_macroRunner_thread->setProtocol(m_protocol);
		m_macroRunner_thread->setSimulationFlag(m_simulationOnly);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::resultReady, this,
			&BioZone6_GUI::protocolFinished);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::sendStatusMessage, this,
			&BioZone6_GUI::updateMacroStatusMessage);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::timeStatus, this,
			&BioZone6_GUI::updateMacroTimeStatus);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::sendAskMessage, this,
			&BioZone6_GUI::askMessage);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::setPon, this,
			&BioZone6_GUI::updatePonSetPoint);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::setPoff, this,
			&BioZone6_GUI::updatePoffSetPoint);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::setVs, this,
			&BioZone6_GUI::updateVswitchSetPoint);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::setVr, this,
			&BioZone6_GUI::updateVrecircSetPoint);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::solution1, this,
			&BioZone6_GUI::solution1);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::solution2, this,
			&BioZone6_GUI::solution2);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::solution3, this,
			&BioZone6_GUI::solution3);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::solution4, this,
			&BioZone6_GUI::solution4);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::solution5, this,
			&BioZone6_GUI::solution5);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::solution6, this,
			&BioZone6_GUI::solution6);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::closeAll, this,
			&BioZone6_GUI::closeAllValves);

		connect(m_macroRunner_thread,
			&BioZone6_protocolRunner::pumpOff, this,
			&BioZone6_GUI::pumpingOff);

		m_macroRunner_thread->start();

		ui->groupBox_operMode->setEnabled(false);
		ui->pushButton_operational->setEnabled(false);
		ui->pushButton_newTip->setEnabled(false);
		ui->pushButton_standby->setEnabled(false);
		ui->pushButton_stop->setEnabled(false);
		ui->toolBar_2->setEnabled(false);
		enableTab2(false);
		ui->tab_4->setEnabled(false);
		setEnableSolutionButtons(false);

		if (!ui->actionConnectDisconnect->isChecked()) { 
			ui->actionSimulation->setEnabled(true); 
		}
		else { 
			ui->actionSimulation->setEnabled(false); 
		}
		ui->actionReboot->setEnabled(false);
		ui->actionShudown->setEnabled(false);
		ui->label_runMacro->setText(m_str_label_stop_protocol);
	}
	else {
		m_macroRunner_thread->disconnect();

		m_macroRunner_thread->setSimulationFlag(m_simulationOnly);
		m_macroRunner_thread->killMacro(true);

		m_ppc1->setVerbose(m_pr_params->verboseOut);
		ui->groupBox_operMode->setEnabled(true);
		ui->pushButton_operational->setEnabled(true);
		ui->pushButton_newTip->setEnabled(true);
		ui->pushButton_standby->setEnabled(true);
		ui->toolBar_2->setEnabled(true);
		ui->pushButton_stop->setEnabled(true);
		enableTab2(true);
		ui->tab_4->setEnabled(true);
		setEnableSolutionButtons(true);
		ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
		if (!ui->actionConnectDisconnect->isChecked()) { 
			ui->actionSimulation->setEnabled(true); 
		}
		else { 
			ui->actionSimulation->setEnabled(false); 
		}

		ui->actionReboot->setEnabled(!m_simulationOnly);
		ui->actionShudown->setEnabled(!m_simulationOnly);
		ui->label_runMacro->setText(m_str_label_run_protocol);
	}
}


void BioZone6_GUI::protocolFinished(const QString &_result) {

	std::cout << HERE << std::endl;

	// TODO: this was removed to allow solutions to run protocols properly without
	//       annoying success messages, this can be achieved anyway by adding 
	//       an ask command as last command of a protocol
	//QMessageBox::information(this, m_str_information, _result);

	// restore settings that have been overlapped during the protocol running
	this->toolApply();

	// restore GUI 
	ui->label_runMacro->setText(m_str_label_run_protocol);
	ui->groupBox_operMode->setEnabled(true);
	ui->pushButton_operational->setEnabled(true);
	ui->pushButton_newTip->setEnabled(true);
	ui->pushButton_stop->setEnabled(true);
	ui->pushButton_standby->setEnabled(true);
	ui->toolBar_2->setEnabled(true);
	enableTab2(true);
	ui->tab_4->setEnabled(true);
	setEnableSolutionButtons(true);
	ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
	if (!ui->actionConnectDisconnect->isChecked()) { 
		ui->actionSimulation->setEnabled(true);
	}
	else { 
		ui->actionSimulation->setEnabled(false); 
	}

	ui->actionReboot->setEnabled(!m_simulationOnly);
	ui->actionShudown->setEnabled(!m_simulationOnly);

	if (!m_simulationOnly) {
		updateVrecircSetPoint(-m_ppc1->getVrecircSetPoint());
		updateVswitchSetPoint(-m_ppc1->getVswitchSetPoint());
		updatePoffSetPoint(m_ppc1->getPoffSetPoint());
		updatePonSetPoint(m_ppc1->getPonSetPoint());
	}

	// update the slider for the GUI
	ui->horizontalSlider_recirculation->blockSignals(true);
	ui->horizontalSlider_recirculation->setValue(m_pipette_status->v_recirc_set_point);
	ui->horizontalSlider_recirculation->blockSignals(false);
	ui->horizontalSlider_switch->blockSignals(true);
	ui->horizontalSlider_switch->setValue(m_pipette_status->v_switch_set_point);
	ui->horizontalSlider_switch->blockSignals(false);
	ui->horizontalSlider_p_off->blockSignals(true);
	ui->horizontalSlider_p_off->setValue(m_pipette_status->poff_set_point);
	ui->horizontalSlider_p_off->blockSignals(false);
	ui->horizontalSlider_p_on->blockSignals(true);
	ui->horizontalSlider_p_on->setValue(m_pipette_status->pon_set_point);
	ui->horizontalSlider_p_on->blockSignals(false);

	//disconnect protocol runner events
	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::resultReady, this,
		&BioZone6_GUI::protocolFinished);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::sendStatusMessage, this,
		&BioZone6_GUI::updateMacroStatusMessage);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::timeStatus, this,
		&BioZone6_GUI::updateMacroTimeStatus);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::sendAskMessage, this,
		&BioZone6_GUI::askMessage);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::setPon, this,
		&BioZone6_GUI::updatePonSetPoint);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::setPoff, this,
		&BioZone6_GUI::updatePoffSetPoint);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::setVs, this,
		&BioZone6_GUI::updateVswitchSetPoint);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::setVr, this,
		&BioZone6_GUI::updateVrecircSetPoint);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::solution1, this,
		&BioZone6_GUI::solution1);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::solution2, this,
		&BioZone6_GUI::solution2);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::solution3, this,
		&BioZone6_GUI::solution3);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::solution4, this,
		&BioZone6_GUI::solution4);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::solution5, this,
		&BioZone6_GUI::solution5);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::solution6, this,
		&BioZone6_GUI::solution6);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::closeAll, this,
		&BioZone6_GUI::closeAllValves);

	disconnect(m_macroRunner_thread,
		&BioZone6_protocolRunner::pumpOff, this,
		&BioZone6_GUI::pumpingOff);

	addAllCommandsToPPC1Protocol(ui->treeWidget_macroTable, m_protocol);

	m_chart_view->updateChartProtocol(m_protocol);
}


void BioZone6_GUI::operationalMode() {

	std::cout << HERE << std::endl;

	QApplication::setOverrideCursor(Qt::WaitCursor);   
	setEnableMainWindow(false);

	//vf0
	closeAllValves();
	
	updateVrecircSetPoint(-m_pr_params->v_recirc_default );// update the set point
	updateVswitchSetPoint(-m_pr_params->v_switch_default );// update the set point
	if (!visualizeProgressMessage(5, m_str_waiting)) return;
	updatePoffSetPoint(m_pr_params->p_off_default );// update the set point
	updatePonSetPoint(m_pr_params->p_on_default);// update the set point

	setEnableMainWindow(true);
	QApplication::restoreOverrideCursor();
}


void BioZone6_GUI::stopFlow()
{
	closeAllValves();
	updatePoffSetPoint(0.0);
	updatePonSetPoint(0.0);
	if (!visualizeProgressMessage(3, m_str_stop_1)) return;
	updateVswitchSetPoint(0.0);
	updateVrecircSetPoint(0.0);
	if (!visualizeProgressMessage(3, m_str_stop_2)) return;

	return;
}


void BioZone6_GUI::standby()
{
	std::cout << HERE << std::endl;

	setEnableMainWindow(false);

	//Set new standby values according to the tip selection
	double pon = 0;
	double poff = 0;
	double vs = 0;
	double vr = 0;
	if (m_ppc1->getTipType() == 0)
	{
		pon = 45.0;
		poff = 11.0;
		vs = 50.0;
		vr = 50.0;
	}
	if (m_ppc1->getTipType() == 1){
		pon = 45.0;
		poff = 11.0;
		vs = 55.0;
		vr = 55.0;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);  

	//vf0
	closeAllValves();
	
	updatePonSetPoint(pon); 
	updatePoffSetPoint(poff);

	if (!visualizeProgressMessage(5, m_str_standby_operation)) return;

	updateVswitchSetPoint(vs);
	updateVrecircSetPoint(vr);

	setEnableMainWindow(true);
	QApplication::restoreOverrideCursor(); 
}


void BioZone6_GUI::setStandardAndSlow()
{
	std::cout << HERE << std::endl;
	
	// if the button is unchecked, check it and uncheck all the others
	if (ui->pushButton_standardAndSlow->isChecked())
	{
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

		QMessageBox::information(this, m_str_information,
			m_new_settings_applied);
	}
	else {
		ui->pushButton_standardAndSlow->setChecked(true);
	}
}

void BioZone6_GUI::setStandardAndRegular()
{
	std::cout << HERE << std::endl;

	if (ui->pushButton_standardAndRegular->isChecked())
	{
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

		QMessageBox::information(this, m_str_information,
			m_new_settings_applied);
	}
	else {
		ui->pushButton_standardAndRegular->setChecked(true);
	}
}

void BioZone6_GUI::setLargeAndSlow()
{
	std::cout << HERE << std::endl;

	if (ui->pushButton_largeAndSlow->isChecked())
	{
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

		QMessageBox::information(this, m_str_information,
			m_new_settings_applied);
	}
	else {
		ui->pushButton_largeAndSlow->setChecked(true);
	}
}

void BioZone6_GUI::setLargeAndRegular()
{
	std::cout << HERE << std::endl;

	if (ui->pushButton_largeAndRegular->isChecked())
	{
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

		QMessageBox::information(this, m_str_information,
			m_new_settings_applied);
	}
	else {
		ui->pushButton_largeAndRegular->setChecked(true);
	}
}
