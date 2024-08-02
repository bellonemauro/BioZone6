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


void BioZone6_GUI::updateGUI() {

	if (isExceptionTriggered()) return;

	// all the following update functions are for GUI only, 
	// there is no time calculation
	// on the contrary, waste update has specific timing 
	updatePressureVacuum();
	updateSolutions();
	updatePPC1Leds();
	updateFlows();

	if (m_ppc1->isRunning()) {
		//m_update_GUI->start(); // TODO: check, this should be running anyway
	}

	if (m_pipette_active) { 
		updateDrawing(m_ppc1->getZoneSizePerc()); 
	}
	else {
		updateFlowControlPercentages();
		updateDrawing(m_ds_perc);
		//updateDrawing(ui->lcdNumber_dropletSize_percentage->value());
	}

	showSolutionsColor(m_simulationOnly || m_ppc1->isRunning());
}

void BioZone6_GUI::updatePressureVacuum()
{
	if (!m_simulationOnly) {

		// if we are here it means that we have no exception 

		// for all pressures/vacuum, 
		// get the sensor reading ( rounded to second decimal?)
		int sensor_reading = (int)(m_ppc1->getVswitchReading());
		// update status, label and bar
		m_pipette_status->v_switch_set_point = -m_ppc1->getVswitchSetPoint();
		ui->label_switchPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(-m_pipette_status->v_switch_set_point)) + " mbar    "));
		ui->progressBar_switch->setValue(-sensor_reading);

		sensor_reading = (int)(m_ppc1->getVrecircReading());
		m_pipette_status->v_recirc_set_point = -m_ppc1->getVrecircSetPoint();
		ui->label_recircPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(-m_pipette_status->v_recirc_set_point)) + " mbar    "));
		ui->progressBar_recirc->setValue(-sensor_reading);

		sensor_reading = (int)(m_ppc1->getPoffReading());
		m_pipette_status->poff_set_point = m_ppc1->getPoffSetPoint();
		ui->label_PoffPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(m_pipette_status->poff_set_point)) + " mbar    "));
		ui->progressBar_pressure_p_off->setValue(sensor_reading);

		sensor_reading = (int)(m_ppc1->getPonReading());
		m_pipette_status->pon_set_point = m_ppc1->getPonSetPoint();
		ui->label_PonPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(m_pipette_status->pon_set_point)) + " mbar    "));
		ui->progressBar_pressure_p_on->setValue(sensor_reading);

		// update droplet zone control zone size, flow speed and vacuum
		m_ds_perc = m_ppc1->getZoneSizePerc();
		m_fs_perc = m_ppc1->getFlowSpeedPerc();
		m_v_perc = m_ppc1->getVacuumPerc();

	}// end if m_simulation
}

void BioZone6_GUI::checkButtonWithoutSignals(QPushButton *_button, bool _enable)
{
	_button->blockSignals(true);
	_button->setChecked(_enable);
	_button->blockSignals(false);
}

void BioZone6_GUI::updateSolutions()
{
	if (!m_simulationOnly) {
		// check for open wells 
		if (m_ppc1->isWeel1Open())
		{
			m_pen_line.setColor(m_sol1_color);

			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol1);

			// calculate the middle point between the two widgets
			// to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution1->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution1->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

			// switch on the button for the solution 1
			checkButtonWithoutSignals(ui->pushButton_solution1, true);
			// all the other buttons have to be off
			checkButtonWithoutSignals(ui->pushButton_solution2, false);
			checkButtonWithoutSignals(ui->pushButton_solution3, false);
			checkButtonWithoutSignals(ui->pushButton_solution4, false);
			checkButtonWithoutSignals(ui->pushButton_solution5, false);
			checkButtonWithoutSignals(ui->pushButton_solution6, false);
		}
		if (m_ppc1->isWeel2Open()) {
			m_pen_line.setColor(m_sol2_color);
			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol2);

			// calculate the middle point between the two widgets
			// to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution2->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution2->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

			// switch on the button for the solution 2
			checkButtonWithoutSignals(ui->pushButton_solution2, true);
			// all the other buttons have to be off
			checkButtonWithoutSignals(ui->pushButton_solution1, false);
			checkButtonWithoutSignals(ui->pushButton_solution3, false);
			checkButtonWithoutSignals(ui->pushButton_solution4, false);
			checkButtonWithoutSignals(ui->pushButton_solution5, false);
			checkButtonWithoutSignals(ui->pushButton_solution6, false);
		}
		if (m_ppc1->isWeel3Open()) {
			m_pen_line.setColor(m_sol3_color);
			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol3);

			// calculate the middle point between the two widgets
			// to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution3->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution3->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

			// switch on the button for the solution 3
			checkButtonWithoutSignals(ui->pushButton_solution3, true);
			// all the other buttons have to be off
			checkButtonWithoutSignals(ui->pushButton_solution1, false);
			checkButtonWithoutSignals(ui->pushButton_solution2, false);
			checkButtonWithoutSignals(ui->pushButton_solution4, false);
			checkButtonWithoutSignals(ui->pushButton_solution5, false);
			checkButtonWithoutSignals(ui->pushButton_solution6, false);
		}
		if (m_ppc1->isWeel4Open()) {
			m_pen_line.setColor(m_sol4_color);
			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol4);

			// calculate the middle point between the two widgets
			// to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution4->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution4->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

			// switch on the button for the solution 4
			checkButtonWithoutSignals(ui->pushButton_solution4, true);
			// all the other buttons have to be off
			checkButtonWithoutSignals(ui->pushButton_solution1, false);
			checkButtonWithoutSignals(ui->pushButton_solution2, false);
			checkButtonWithoutSignals(ui->pushButton_solution3, false);
			checkButtonWithoutSignals(ui->pushButton_solution5, false);
			checkButtonWithoutSignals(ui->pushButton_solution6, false);
		}
		if (m_ppc1->isWeel5Open()) {
			m_pen_line.setColor(m_sol5_color);
			// move the arrow in the drawing to point on the solution 5
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol5);

			// calculate the middle point between the two widgets
			// to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution5->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution5->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

			// switch on the button for the solution 5
			checkButtonWithoutSignals(ui->pushButton_solution5, true);
			// all the other buttons have to be off
			checkButtonWithoutSignals(ui->pushButton_solution1, false);
			checkButtonWithoutSignals(ui->pushButton_solution2, false);
			checkButtonWithoutSignals(ui->pushButton_solution3, false);
			checkButtonWithoutSignals(ui->pushButton_solution4, false);
			checkButtonWithoutSignals(ui->pushButton_solution6, false);
		}
		if (m_ppc1->isWeel6Open()) {
			m_pen_line.setColor(m_sol6_color);
			// move the arrow in the drawing to point on the solution 6
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol6);

			// calculate the middle point between the two widgets
			// to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution6->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution6->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

			// switch on the button for the solution 5
			checkButtonWithoutSignals(ui->pushButton_solution6, true);
			// all the other buttons have to be off
			checkButtonWithoutSignals(ui->pushButton_solution1, false);
			checkButtonWithoutSignals(ui->pushButton_solution2, false);
			checkButtonWithoutSignals(ui->pushButton_solution3, false);
			checkButtonWithoutSignals(ui->pushButton_solution4, false);
			checkButtonWithoutSignals(ui->pushButton_solution5, false);
		}

		// if we have no open wells the droplet is removed from the drawing
		if (!m_ppc1->isWeel1Open() && !m_ppc1->isWeel2Open() &&
			!m_ppc1->isWeel3Open() && !m_ppc1->isWeel4Open() &&
			!m_ppc1->isWeel5Open() && !m_ppc1->isWeel6Open()) {
			m_pen_line.setColor(Qt::transparent);
			ui->widget_solutionArrow->setVisible(false);

			checkButtonWithoutSignals(ui->pushButton_solution1, false);
			checkButtonWithoutSignals(ui->pushButton_solution2, false);
			checkButtonWithoutSignals(ui->pushButton_solution3, false);
			checkButtonWithoutSignals(ui->pushButton_solution4, false);
			checkButtonWithoutSignals(ui->pushButton_solution5, false);
			checkButtonWithoutSignals(ui->pushButton_solution6, false);
		}

		// this should never happen, but the droplet would be removed anyway
		if (m_ppc1->isWeel1Open() && m_ppc1->isWeel2Open() &&
			m_ppc1->isWeel3Open() && m_ppc1->isWeel4Open() &&
			m_ppc1->isWeel5Open() && m_ppc1->isWeel6Open()) {
			m_pen_line.setColor(Qt::transparent);
			ui->widget_solutionArrow->setVisible(false);

			checkButtonWithoutSignals(ui->pushButton_solution1, false);
			checkButtonWithoutSignals(ui->pushButton_solution2, false);
			checkButtonWithoutSignals(ui->pushButton_solution3, false);
			checkButtonWithoutSignals(ui->pushButton_solution4, false);
			checkButtonWithoutSignals(ui->pushButton_solution5, false);
			checkButtonWithoutSignals(ui->pushButton_solution6, false);
		}
	}
}

bool BioZone6_GUI::isExceptionTriggered() // return true if the exception is triggered
{
	if (!m_simulationOnly) {
		// check exceptions, TODO: this is not the best way to do it !!!
		//if (m_ppc1->isConnected() && m_ppc1->isExceptionHappened()) { // this was there before, why ? the exception can happen connected or not
		if (m_ppc1->isExceptionHappened()) {
			// if there is an exception, a message is out
			QMessageBox::information(this, m_str_warning,
				m_str_lost_connection + "<br>" + m_str_swapping_to_simulation);

			// stop updating the GUI
			m_update_GUI->stop();

			// go back to the simulation mode
			ui->actionConnectDisconnect->setEnabled(false);
			ui->actionConnectDisconnect->setChecked(false);
			ui->actionConnectDisconnect->setText(m_str_connect);
			ui->actionReboot->setEnabled(false);
			ui->actionShudown->setEnabled(false);

			// disconnect from the PPC1
			m_ppc1->disconnectCOM();
			m_ppc1->stop();
			QThread::msleep(500);

			// verify that we are actually disconnected
			if (m_ppc1->isConnected())
				m_ppc1->disconnectCOM();
			QThread::msleep(500);

			// enable the simulation buttons
			ui->actionSimulation->setEnabled(true);
			ui->actionSimulation->setChecked(true);
			
			//this->setStatusLed(false);
			status_bar_led->setColor(QFled::ColorType::red);
			//ui->status_PPC1_led->setPixmap(*led_red);
			ui->status_PPC1_label->setText(m_str_PPC1_status_discon);

			// end
			return true;
		}
	}
	return false;
}


void BioZone6_GUI::updatePPC1Leds()
{
	// update leds according to the PPC1 pressure/vacuum status, 
	// for each channel the state of the PPC1 is connected to the led
	// ok = 0 = green, 
	// fault = 1 = red
	// ok = 0 with reading far from the set point 10%+-3mbar = orange
	if (!m_simulationOnly) {

		if (m_ppc1->isRunning())
		{
			// the first is the communication state in the main GUI
			if (m_ppc1->getCommunicationState() == true) {
				status_bar_led->setColor(QFled::ColorType::green);
				ui->status_PPC1_label->setText(m_str_PPC1_status_con);
			}
			else
			{
				status_bar_led->setColor(QFled::ColorType::red);
				ui->status_PPC1_label->setText(m_str_PPC1_status_unstable_con); 
			}

			// update LED for Pon
			if (m_ppc1->getPonState() == 0) {
				
				pon_bar_led->setColor(QFled::ColorType::green);
				if (std::abs(m_ppc1->getPonReading() - m_ppc1->getPonSetPoint()) >
					0.1 * m_ppc1->getPonSetPoint() + 3)
				{
					pon_bar_led->setColor(QFled::ColorType::orange);
				}
			}
			else {
				pon_bar_led->setColor(QFled::ColorType::red);
			}

			// update LED for Poff
			if (m_ppc1->getPoffState() == 0) {
				poff_bar_led->setColor(QFled::ColorType::green);
				if (std::abs(m_ppc1->getPoffReading() - m_ppc1->getPoffSetPoint()) >
					0.1*m_ppc1->getPoffSetPoint() + 3)
				{
					poff_bar_led->setColor(QFled::ColorType::orange);
				}
			}
			else {
				poff_bar_led->setColor(QFled::ColorType::red);
			}

			// update LED for Vswitch
			if (m_ppc1->getVswitchState() == 0) {
				vs_bar_led->setColor(QFled::ColorType::green);
				if (std::abs(m_ppc1->getVswitchReading() - m_ppc1->getVswitchSetPoint()) >
					-0.1*m_ppc1->getVswitchSetPoint() + 3)
				{
					vs_bar_led->setColor(QFled::ColorType::orange);
				}
			}
			else {
				vs_bar_led->setColor(QFled::ColorType::red);
			}

			// update LED for Vrecirc
			if (m_ppc1->getVrecircState() == 0) {
				vr_bar_led->setColor(QFled::ColorType::green);
				double as = m_ppc1->getVrecircReading();
				double ad = m_ppc1->getVrecircSetPoint();
				if (std::abs(m_ppc1->getVrecircReading() - m_ppc1->getVrecircSetPoint()) >
					-0.1*m_ppc1->getVrecircSetPoint() + 3)
				{
					vr_bar_led->setColor(QFled::ColorType::orange);
				}
			}
			else {
				vr_bar_led->setColor(QFled::ColorType::red);
			}
		}// end if m_ppc1->isRunning()
		else
		{
			status_bar_led->setColor(QFled::ColorType::red);
			pon_bar_led->setColor(QFled::ColorType::grey);
			poff_bar_led->setColor(QFled::ColorType::grey);
			vs_bar_led->setColor(QFled::ColorType::grey);
			vr_bar_led->setColor(QFled::ColorType::grey);

		}
	}//end if m_simulation_only
	else
	{
		pon_bar_led->setColor(QFled::ColorType::green);
		poff_bar_led->setColor(QFled::ColorType::green);
		vs_bar_led->setColor(QFled::ColorType::green);
		vr_bar_led->setColor(QFled::ColorType::green);
	}
}

void BioZone6_GUI::updateFlows()
{
	//shorter variables inside the function just for convenience
	double v_s = m_pipette_status->v_switch_set_point;
	double v_r = m_pipette_status->v_recirc_set_point;
	double p_on = m_pipette_status->pon_set_point;
	double p_off = m_pipette_status->poff_set_point;

	// if we are not in simulation, we just get the numbers from the PPC1api
	// otherwise the flows will be calculated according to the current values
	if (!m_simulationOnly)
	{ 
		const fluicell::PPC1api6dataStructures::PPC1api6_status *status = m_ppc1->getPipetteStatus();
		m_pipette_status->outflow_on = status->outflow_on;
		m_pipette_status->outflow_off = status->outflow_off;
		m_pipette_status->outflow_tot = status->outflow_tot;
		m_pipette_status->inflow_recirculation = status->inflow_recirculation;
		m_pipette_status->inflow_switch = status->inflow_switch;
		m_pipette_status->in_out_ratio_on = status->in_out_ratio_on;
		m_pipette_status->in_out_ratio_off = status->in_out_ratio_off;
		m_pipette_status->in_out_ratio_tot = status->in_out_ratio_tot;
		m_pipette_status->flow_well1 = status->flow_rate_1;
		m_pipette_status->flow_well2 = status->flow_rate_2;
		m_pipette_status->flow_well3 = status->flow_rate_3;
		m_pipette_status->flow_well4 = status->flow_rate_4;
		m_pipette_status->flow_well5 = status->flow_rate_5;
		m_pipette_status->flow_well6 = status->flow_rate_6;
		m_pipette_status->flow_well7 = status->flow_rate_7;
		m_pipette_status->flow_well8 = status->flow_rate_8;
	}
	else {
		// calculate inflow
		m_pipette_status->delta_pressure = 100.0 * v_r;
		m_pipette_status->inflow_recirculation = 
			2.0 * m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, m_ppc1->getLengthToTip());

		m_pipette_status->delta_pressure = 100.0 * (v_r + 
			2.0 * p_off * (1 - m_ppc1->getLengthToZone() / m_ppc1->getLengthToTip()));
		m_pipette_status->inflow_switch = 
			2.0 * m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, m_ppc1->getLengthToTip());

		m_pipette_status->delta_pressure = 100.0 * 2.0 * p_off;
		m_pipette_status->solution_usage_off = 
			m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, 2 * m_ppc1->getLengthToZone());

		m_pipette_status->delta_pressure = 100.0 * p_on;
		m_pipette_status->solution_usage_on = 
			m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, m_ppc1->getLengthToTip());

		m_pipette_status->delta_pressure = 100.0 * (p_on + p_off * 3.0 - v_s * 2.0);   // TODO magic numbers
		m_pipette_status->outflow_on = 
			m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, m_ppc1->getLengthToTip());

		m_pipette_status->delta_pressure = 100.0 * (p_off * 4.0 - v_s * 2.0);
		m_pipette_status->outflow_off = 2.0 * m_ppc1->getFlowSimple(
			m_pipette_status->delta_pressure, 2 * m_ppc1->getLengthToZone());

		// this is to avoid NAN on the ratio
        if (m_pipette_status->inflow_recirculation == 0) 
			m_pipette_status->in_out_ratio_on = 0;
		else
			m_pipette_status->in_out_ratio_on =
			m_pipette_status->outflow_on / m_pipette_status->inflow_recirculation;

		// this is to avoid NAN on the ratio
        if (m_pipette_status->inflow_recirculation == 0)
			m_pipette_status->in_out_ratio_off = 0;
		else
			m_pipette_status->in_out_ratio_off =
			m_pipette_status->outflow_off / m_pipette_status->inflow_recirculation;

		// flow when solution is off 
		if (ui->pushButton_solution1->isChecked() ||
			ui->pushButton_solution2->isChecked() ||
			ui->pushButton_solution3->isChecked() ||
			ui->pushButton_solution4->isChecked() ||
			ui->pushButton_solution5->isChecked() ||
			ui->pushButton_solution6->isChecked() 
			) //TODO: fix here for 6 channels
		{
		
			m_pipette_status->in_out_ratio_tot = m_pipette_status->in_out_ratio_on;
			m_pipette_status->outflow_tot = m_pipette_status->outflow_on;

			if (ui->pushButton_solution1->isChecked()) 
				m_pipette_status->flow_well1 = m_pipette_status->solution_usage_on;
			else
				m_pipette_status->flow_well1 = m_pipette_status->solution_usage_off;

			if (ui->pushButton_solution2->isChecked()) 
				m_pipette_status->flow_well2 = m_pipette_status->solution_usage_on;
			else
				m_pipette_status->flow_well2 = m_pipette_status->solution_usage_off;

			if (ui->pushButton_solution3->isChecked())
				m_pipette_status->flow_well3 = m_pipette_status->solution_usage_on;
			else
				m_pipette_status->flow_well3 = m_pipette_status->solution_usage_off;

			if (ui->pushButton_solution4->isChecked())
				m_pipette_status->flow_well4 = m_pipette_status->solution_usage_on;
			else
				m_pipette_status->flow_well4 = m_pipette_status->solution_usage_off; 
			
			if (ui->pushButton_solution5->isChecked())
				m_pipette_status->flow_well5 = m_pipette_status->solution_usage_on;
			else
				m_pipette_status->flow_well5 = m_pipette_status->solution_usage_off; 
			
			if (ui->pushButton_solution6->isChecked())
				m_pipette_status->flow_well6 = m_pipette_status->solution_usage_on;
			else 
				m_pipette_status->flow_well6 = m_pipette_status->solution_usage_off;

		}
		else // flow when solution is on 
		{
			//m_pipette_status->delta_pressure = 100.0 * (p_off * 4.0 - v_s * 2.0);
			//m_pipette_status->outflow = 2.0 * m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, 2 * LENGTH_TO_ZONE);

			m_pipette_status->in_out_ratio_tot = m_pipette_status->in_out_ratio_off;
			m_pipette_status->outflow_tot = m_pipette_status->outflow_off;

			m_pipette_status->flow_well1 = m_pipette_status->solution_usage_off;
			m_pipette_status->flow_well2 = m_pipette_status->solution_usage_off;
			m_pipette_status->flow_well3 = m_pipette_status->solution_usage_off;
			m_pipette_status->flow_well4 = m_pipette_status->solution_usage_off;
			m_pipette_status->flow_well5 = m_pipette_status->solution_usage_off;
			m_pipette_status->flow_well6 = m_pipette_status->solution_usage_off;
			

		}
		//m_pipette_status->flow_well5 = m_pipette_status->inflow_switch / 2.0;
		//m_pipette_status->flow_well6 = m_pipette_status->inflow_switch / 2.0;
		//m_pipette_status->flow_well7 = m_pipette_status->inflow_recirculation / 2.0;
		m_pipette_status->flow_well7 = m_pipette_status->inflow_switch / 2.0;
		m_pipette_status->flow_well8 = m_pipette_status->inflow_recirculation / 2.0;
	}

	// update the tree widget
#pragma message("TODO: this was commented on 04122020 as the calculation is wrong, uncomment when the calculation is available")
	ui->treeWidget_flowInfo->topLevelItem(0)->setText(1,
		QString::number(m_pipette_status->outflow_tot, 'g', 3));
	ui->treeWidget_flowInfo->topLevelItem(1)->setText(1,
		QString::number(m_pipette_status->inflow_recirculation, 'g', 4));
	ui->treeWidget_flowInfo->topLevelItem(2)->setText(1,
		QString::number(m_pipette_status->inflow_switch, 'g', 4));
	ui->treeWidget_flowInfo->topLevelItem(3)->setText(1,
		QString::number(m_pipette_status->in_out_ratio_tot, 'g', 2));
	ui->treeWidget_flowInfo->topLevelItem(4)->setText(1,
		QString::number(m_pipette_status->flow_well1, 'g', 2));
	ui->treeWidget_flowInfo->topLevelItem(5)->setText(1,
		QString::number(m_pipette_status->flow_well2, 'g', 2));
	ui->treeWidget_flowInfo->topLevelItem(6)->setText(1,
		QString::number(m_pipette_status->flow_well3, 'g', 2));
	ui->treeWidget_flowInfo->topLevelItem(7)->setText(1,
		QString::number(m_pipette_status->flow_well4, 'g', 2));
	ui->treeWidget_flowInfo->topLevelItem(8)->setText(1,
		QString::number(m_pipette_status->flow_well5, 'g', 2));
	ui->treeWidget_flowInfo->topLevelItem(9)->setText(1,
		QString::number(m_pipette_status->flow_well6, 'g', 2));
	ui->treeWidget_flowInfo->topLevelItem(10)->setText(1,
		QString::number(m_pipette_status->flow_well7, 'g', 2));
	ui->treeWidget_flowInfo->topLevelItem(11)->setText(1,
		QString::number(m_pipette_status->flow_well8, 'g', 2));

#pragma message("TODO: this was commented on 24062024 as the calculation is wrong when using IonOptix this just overrides the update flow")
	if (m_GUI_params->useIONoptixLogo == true && 
		m_ppc1->getTip()->type == fluicell::PPC1api6dataStructures::tip::UWZ )
	{
		QString NA = "NA";
		ui->treeWidget_flowInfo->topLevelItem(0)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(1)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(2)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(3)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(4)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(5)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(6)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(7)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(8)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(9)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(10)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(11)->setText(1, NA);
	}

	return;
}



void BioZone6_GUI::updateDrawing(int _value) {


#pragma message ("this overwrites the settings about the droplet size, it should be solved once we have the precise calculation ")
	if (ui->pushButton_standardAndRegular->isChecked() == true)
		_value = 85;
	if (ui->pushButton_largeAndRegular->isChecked() == true)
		_value = 190;
	if (ui->pushButton_standardAndSlow->isChecked() == true)
		_value = 85;
	if (ui->pushButton_largeAndSlow->isChecked() == true)
		_value = 190;

	QColor sol1_color;
	QColor sol2_color;
	QColor sol3_color;
	QColor sol4_color;
	QColor sol5_color;
	QColor sol6_color;
	if (ui->groupBox_3->isEnabled() == false)
	{
		sol1_color = QColor::fromRgba(0xFFFFFFFF);
		sol2_color = QColor::fromRgba(0xFFFFFFFF);
		sol3_color = QColor::fromRgba(0xFFFFFFFF);
		sol4_color = QColor::fromRgba(0xFFFFFFFF);
		sol5_color = QColor::fromRgba(0xFFFFFFFF);
		sol6_color = QColor::fromRgba(0xFFFFFFFF);


	}
	else
	{
		sol1_color = m_sol1_color;
		sol2_color = m_sol2_color;
		sol3_color = m_sol3_color;
		sol4_color = m_sol4_color;
		sol5_color = m_sol5_color;
		sol6_color = m_sol6_color;
	}



	if (_value <= 0) { // _value = -1 cleans the scene and make the flow disappear 

		m_scene_solution->clear();
		ui->graphicsView->update();
		ui->graphicsView->show();
		return;
	}

	if (_value >= MAX_ZONE_SIZE_PERC) {
		_value = MAX_ZONE_SIZE_PERC;
	}

	//clean the scene
	m_scene_solution->clear();

	// draw the circle 
	QBrush brush(m_pen_line.color(), Qt::SolidPattern);

	QPen border_pen;
	border_pen.setColor(Qt::transparent);
	border_pen.setWidth(2);

	double droplet_modifier = (10.0 - _value / 10.0);

	// this is an attempt to make the droplet to look a little bit more realistic
	QPainterPath droplet;
	droplet.arcMoveTo((qreal)ui->doubleSpinBox_d_x->value() + droplet_modifier, (qreal)ui->doubleSpinBox_d_y->value(),
		(qreal)ui->doubleSpinBox_d_w->value() - droplet_modifier, (qreal)ui->doubleSpinBox_d_h->value(), (qreal)ui->doubleSpinBox_d_a->value());
	
	droplet.arcTo((qreal)ui->doubleSpinBox_d2x->value() + droplet_modifier, (qreal)ui->doubleSpinBox_d2y->value(),
		(qreal)ui->doubleSpinBox_d2w->value() - droplet_modifier, (qreal)ui->doubleSpinBox_d2h->value(),
		(qreal)ui->doubleSpinBox_d2a->value(), (qreal)ui->doubleSpinBox_d2l->value());


	droplet.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(droplet, border_pen, brush);

	//all this function is rather GUI fix stuff and the numbers can be set using an hidden visualization tool
	QPainterPath circle;
	circle.arcMoveTo((qreal)ui->doubleSpinBox_c_x->value(), (qreal)ui->doubleSpinBox_c_y->value(),
		(qreal)ui->doubleSpinBox_c_w->value(), (qreal)ui->doubleSpinBox_c_h->value(),
		(qreal)ui->doubleSpinBox_c_a->value());  

	circle.arcTo((qreal)ui->doubleSpinBox_c2x->value(), (qreal)ui->doubleSpinBox_c2y->value(),
		(qreal)ui->doubleSpinBox_c2w->value(), (qreal)ui->doubleSpinBox_c2h->value(),
		(qreal)ui->doubleSpinBox_c2a->value(), (qreal)ui->doubleSpinBox_c2l->value());

	circle.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(circle, border_pen, brush);

	int border_pen_pipe_width = 5; 
	QBrush brush_pipes(Qt::transparent, Qt::NoBrush);
	QPen border_pen_pipe1;
	border_pen_pipe1.setColor(sol4_color);
	border_pen_pipe1.setWidth(border_pen_pipe_width);
	QPainterPath path_pipe1;
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);

	path_pipe1.arcMoveTo((qreal)ui->doubleSpinBox_p1_x->value(), (qreal)ui->doubleSpinBox_p1_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p1_w->value(), (qreal)ui->doubleSpinBox_p1_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p1_a->value());

	path_pipe1.arcTo((qreal)ui->doubleSpinBox_p12x->value(), (qreal)ui->doubleSpinBox_p12y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p12w->value(), (qreal)ui->doubleSpinBox_p12h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p12a->value(), (qreal)ui->doubleSpinBox_p12l->value()); //qreal startAngle, qreal arcLength


	path_pipe1.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(path_pipe1, border_pen_pipe1, brush_pipes);

	QPen border_pen_pipe2;
	border_pen_pipe2.setColor(sol2_color);
	border_pen_pipe2.setWidth(border_pen_pipe_width);
	QPainterPath path_pipe2;
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);


	path_pipe2.arcMoveTo((qreal)ui->doubleSpinBox_p2_x->value(), (qreal)ui->doubleSpinBox_p2_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p2_w->value(), (qreal)ui->doubleSpinBox_p2_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p2_a->value());
	path_pipe2.arcTo((qreal)ui->doubleSpinBox_p22x->value(), (qreal)ui->doubleSpinBox_p22y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p22w->value(), (qreal)ui->doubleSpinBox_p22h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p22a->value(), (qreal)ui->doubleSpinBox_p22l->value()); //qreal startAngle, qreal arcLength

	path_pipe2.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(path_pipe2, border_pen_pipe2, brush_pipes);

	
	QPen border_pen_pipe3;
	border_pen_pipe3.setColor(sol1_color); 
	border_pen_pipe3.setWidth(border_pen_pipe_width);
	QPainterPath path_pipe3; 
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);
	path_pipe3.arcMoveTo((qreal)ui->doubleSpinBox_p3_x->value(), (qreal)ui->doubleSpinBox_p3_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p3_w->value(), (qreal)ui->doubleSpinBox_p3_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p3_a->value());
	path_pipe3.arcTo((qreal)ui->doubleSpinBox_p32x->value(), (qreal)ui->doubleSpinBox_p32y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p32w->value(), (qreal)ui->doubleSpinBox_p32h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p32a->value(), (qreal)ui->doubleSpinBox_p32l->value()); //qreal startAngle, qreal arcLength
	path_pipe3.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(path_pipe3, border_pen_pipe3, brush_pipes);

	
	QPen border_pen_pipe4;
	border_pen_pipe4.setColor(sol3_color);
	border_pen_pipe4.setWidth(border_pen_pipe_width);
	QPainterPath path_pipe4;
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);
	path_pipe4.arcMoveTo((qreal)ui->doubleSpinBox_p4_x->value(), (qreal)ui->doubleSpinBox_p4_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p4_w->value(), (qreal)ui->doubleSpinBox_p4_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p4_a->value());

	path_pipe4.arcTo((qreal)ui->doubleSpinBox_p42x->value(), (qreal)ui->doubleSpinBox_p42y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p42w->value(), (qreal)ui->doubleSpinBox_p42h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p42a->value(), (qreal)ui->doubleSpinBox_p42l->value()); //qreal startAngle, qreal arcLength

	path_pipe4.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(path_pipe4, border_pen_pipe4, brush_pipes);

	QPen border_pen_pipe5;
	border_pen_pipe5.setColor(sol5_color);
	border_pen_pipe5.setWidth(border_pen_pipe_width);
	QPainterPath path_pipe5;
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);
	path_pipe5.arcMoveTo((qreal)ui->doubleSpinBox_p5_x->value(), (qreal)ui->doubleSpinBox_p5_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p5_w->value(), (qreal)ui->doubleSpinBox_p5_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p5_a->value());

	path_pipe5.arcTo((qreal)ui->doubleSpinBox_p52x->value(), (qreal)ui->doubleSpinBox_p52y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p52w->value(), (qreal)ui->doubleSpinBox_p52h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p52a->value(), (qreal)ui->doubleSpinBox_p52l->value()); //qreal startAngle, qreal arcLength

	path_pipe5.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(path_pipe5, border_pen_pipe5, brush_pipes);

	QPen border_pen_pipe6;
	border_pen_pipe6.setColor(sol6_color);
	border_pen_pipe6.setWidth(border_pen_pipe_width);
	QPainterPath path_pipe6;
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);
	path_pipe6.arcMoveTo((qreal)ui->doubleSpinBox_p6_x->value(), (qreal)ui->doubleSpinBox_p6_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p6_w->value(), (qreal)ui->doubleSpinBox_p6_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p6_a->value());

	path_pipe6.arcTo((qreal)ui->doubleSpinBox_p62x->value(), (qreal)ui->doubleSpinBox_p62y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p62w->value(), (qreal)ui->doubleSpinBox_p62h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p62a->value(), (qreal)ui->doubleSpinBox_p62l->value()); //qreal startAngle, qreal arcLength

	path_pipe6.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(path_pipe6, border_pen_pipe6, brush_pipes);

	// draw a line from the injector to the solution release point 
	m_scene_solution->addLine(l_x1, l_y1, l_x2, l_y2, m_pen_line);
	
	ui->graphicsView->setScene(m_scene_solution);
	ui->graphicsView->show();

	return;
}

// this is updated every second
// this does not work with PPC1api (only works in simulation)
void BioZone6_GUI::updateWaste() 
{
	// NB:  here there is a calculation of the volume as follows:
	//      remaining volume = current_volume (mL) - delta_t * flow_rate (nL/s)
	//      there is an assumption of this to run every second hence delta_t = 1  
	//      this should be changed to fit the real values of timers
	// when we switch from simulation to real device, wells should be reset ?
	//m_update_waste->start();

	if (m_ds_perc < 10) return;

	double waste_remaining_time_in_sec = 0;

	//updateWells();
	int max = MAX_VOLUME_IN_WELL;  // just to have a shorter name in formulas

								   // update wells when the solution is flowing
	if (ui->pushButton_solution1->isChecked()) {
		m_pipette_status->rem_vol_well1 = m_pipette_status->rem_vol_well1 -
			0.001 * m_pipette_status->flow_well1; // 0.001 is to transform in mL/s

		if (m_pipette_status->rem_vol_well1 < 0) {
			//stopSolutionFlow(); //automatic stop of solution flow on solution ended
			
			// deprecated on 29012021
			// this generates replicating messages so there should be a better strategy to handle ending solutions
			//QMessageBox::information(this, m_str_warning,
			//	m_str_solution_ended);

			// deprecated 06072018
			//QMessageBox::StandardButton resBtn =
			//	QMessageBox::question(this, m_str_warning,
			//		QString(m_str_solution_ended),
			//		QMessageBox::No | QMessageBox::Yes,
			//		QMessageBox::Yes);
			//if (resBtn == QMessageBox::Yes) {
			//	refillSolution();
			//}
		}

		double perc = 100.0 * m_pipette_status->rem_vol_well1 / max;
		ui->progressBar_solution1->setValue(int(perc));
	}
	if (ui->pushButton_solution2->isChecked()) {
		m_pipette_status->rem_vol_well2 = m_pipette_status->rem_vol_well2 -
			0.001 * m_pipette_status->flow_well2; // 0.001 is to transform in mL/s

		if (m_pipette_status->rem_vol_well2 < 0) {
			//stopSolutionFlow(); //automatic stop of solution flow on solution ended
			//QMessageBox::information(this, m_str_warning,
			//	m_str_solution_ended);
		}

		double perc = 100.0 * m_pipette_status->rem_vol_well2 / max;
		ui->progressBar_solution2->setValue(int(perc));
	}
	if (ui->pushButton_solution3->isChecked()) {
		m_pipette_status->rem_vol_well3 = m_pipette_status->rem_vol_well3 -
			0.001 * m_pipette_status->flow_well3; // 0.001 is to transform in mL/s

		if (m_pipette_status->rem_vol_well3 < 0) {
			//stopSolutionFlow();  //automatic stop of solution flow on solution ended
			//QMessageBox::information(this, m_str_warning,
			//	m_str_solution_ended);

		}

		double perc = 100.0 * m_pipette_status->rem_vol_well3 / max;
		ui->progressBar_solution3->setValue(int(perc));
	}
	if (ui->pushButton_solution4->isChecked()) {
		m_pipette_status->rem_vol_well4 = m_pipette_status->rem_vol_well4 -
			0.001 * m_pipette_status->flow_well4; // 0.001 is to transform in mL/s

		if (m_pipette_status->rem_vol_well4 < 0) {
			//stopSolutionFlow();  //automatic stop of solution flow on solution ended
			//QMessageBox::information(this, m_str_warning,
			//	m_str_solution_ended);
		}

		double perc = 100.0 * m_pipette_status->rem_vol_well4 / max;
		ui->progressBar_solution4->setValue(int(perc));
	}
	if (ui->pushButton_solution5->isChecked()) {
		m_pipette_status->rem_vol_well5 = m_pipette_status->rem_vol_well5 -
			0.001 * m_pipette_status->flow_well5; // 0.001 is to transform in mL/s

		if (m_pipette_status->rem_vol_well5 < 0) {
			//stopSolutionFlow();  //automatic stop of solution flow on solution ended
			//QMessageBox::information(this, m_str_warning,
			//	m_str_solution_ended);
		}

		double perc = 100.0 * m_pipette_status->rem_vol_well5 / max;
		ui->progressBar_solution6->setValue(int(perc));
	}
	if (ui->pushButton_solution6->isChecked()) {
		m_pipette_status->rem_vol_well6 = m_pipette_status->rem_vol_well6 -
			0.001 * m_pipette_status->flow_well6; // 0.001 is to transform in mL/s

		if (m_pipette_status->rem_vol_well6 < 0) {
			//stopSolutionFlow();  //automatic stop of solution flow on solution ended
			//QMessageBox::information(this, m_str_warning,
			//	m_str_solution_ended);
			
		}

		double perc = 100.0 * m_pipette_status->rem_vol_well6 / max;
		ui->progressBar_solution6->setValue(int(perc));
	}

#pragma message("TODO: this was done on 13122021 as a dirty fix to the calculation")
		m_pipette_status->rem_vol_well7 = m_pipette_status->rem_vol_well7 +
			0.001 * m_pipette_status->flow_well7; // 12 is the flow in nL/s that is an estimation of the non-active (passive) flow
		m_pipette_status->rem_vol_well8 = m_pipette_status->rem_vol_well8 +
			0.001 * m_pipette_status->flow_well8;  //12

	// show the warning label
	if (m_pipette_status->rem_vol_well7 > MAX_WASTE_WARNING_VOLUME ||
		m_pipette_status->rem_vol_well8 > MAX_WASTE_WARNING_VOLUME) {
		ui->label_warningIcon->show();
		ui->label_warning->setText(m_str_warning_waste_full);
		ui->label_warning->show();
	}

	// only the minimum of the remaining solution is shown and important
	std::vector<double> v1;
	v1.push_back(m_solutionParams->vol_well7 - m_pipette_status->rem_vol_well7);
	v1.push_back(m_solutionParams->vol_well8 - m_pipette_status->rem_vol_well8);

	auto min = std::min_element(v1.begin(), v1.end());
	int min_index = std::distance(v1.begin(), min);


	switch (min_index)
	{
	case 0: {
		if (m_pipette_status->flow_well7 > 0) {
			waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well7 -
				m_pipette_status->rem_vol_well7) / m_pipette_status->flow_well7;
		}
		else {
			waste_remaining_time_in_sec = 0;
		}
		break;
	}
	case 1: {
		if (m_pipette_status->flow_well8 > 0) {
			waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well8 -
				m_pipette_status->rem_vol_well8) / m_pipette_status->flow_well8;
		}
		else {
			waste_remaining_time_in_sec = 0;
		}
		break;
	}
	default: {
		std::cerr << HERE << " error --- no max found " << std::endl;
	}
	}
#pragma message("TODO: this was commented on 04122020 as the calculation is wrong, uncomment when the calculation is available")
	int v = m_pipette_status->rem_vol_well1 * 10; // this is to set a single decimal digit
	ui->treeWidget_flowInfo->topLevelItem(12)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well2 * 10;
	ui->treeWidget_flowInfo->topLevelItem(13)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well3 * 10;
	ui->treeWidget_flowInfo->topLevelItem(14)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well4 * 10;
	ui->treeWidget_flowInfo->topLevelItem(15)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well5 * 10;
	ui->treeWidget_flowInfo->topLevelItem(16)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well6 * 10;
	ui->treeWidget_flowInfo->topLevelItem(17)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well7 * 10;
	ui->treeWidget_flowInfo->topLevelItem(18)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well8 * 10;
	ui->treeWidget_flowInfo->topLevelItem(19)->setText(1, QString::number(v / 10.0));
#pragma message("TODO: this was done on 31072024 as the calculation is wrong, so we just overpose NA to any other value")
	if (m_GUI_params->useIONoptixLogo == true &&
		m_ppc1->getTip()->type == fluicell::PPC1api6dataStructures::tip::UWZ)
	{
		QString NA = "NA";
		ui->treeWidget_flowInfo->topLevelItem(12)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(13)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(14)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(15)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(16)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(17)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(18)->setText(1, NA);
		ui->treeWidget_flowInfo->topLevelItem(19)->setText(1, NA);
	}

	double value = 100.0 * m_pipette_status->rem_vol_well7 / m_solutionParams->vol_well7;
	ui->progressBar_switch7->setValue(value);

	value = 100.0 * m_pipette_status->rem_vol_well8 / m_solutionParams->vol_well8;
	ui->progressBar_recirc8->setValue(value);


	if (waste_remaining_time_in_sec < 0) {

		waste_remaining_time_in_sec = 0; //this is to avoid to show negative values 

		// if we have 0 remaining time but the remainder is active, we keep going
		if (m_waste_remainder->isActive()) return;


		// otherwise, show a message and start a remainder timer
		QMessageBox::information(this, m_str_warning,
			m_str_waste_full);

		m_waste_remainder->start();

		// deprecated 07072018
		//QMessageBox::StandardButton resBtn =
		//	QMessageBox::question(this, m_str_warning,
		//		QString(m_str_waste_full),
		//		QMessageBox::No | QMessageBox::Yes,
		//		QMessageBox::Yes);
		//if (resBtn == QMessageBox::Yes) {
		//	emptyWells();
		//}

		return;
	}

	// build the string for the waste label
	QString s;
	s.append(m_str_pulse_waste + " ");
	s.append(QString::number(min_index + 5));
	s.append(" " + m_str_pulse_full_in + ": ");
	int remaining_hours = floor(waste_remaining_time_in_sec / 3600); // 3600 sec in a hour
	int remaining_mins = floor(((int)waste_remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
	int remaining_secs = waste_remaining_time_in_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
	s.append(QString::number(remaining_hours));
	//s.append(" h ");
	//s.append(QString::number(remaining_mins));
	//s.append(" min ");
	s.append(" ");
	s.append(m_str_h);
	s.append(",   ");
	s.append(QString::number(remaining_mins));
	s.append(" ");
	s.append(m_str_min);
	//s.append(QString::number(remaining_secs));
	//s.append(" sec ");

	ui->textEdit_emptyTime_waste->setText(s);
}

void BioZone6_GUI::emptyWasteRemainder()
{
	QMessageBox::information(this, m_str_warning,
		m_str_waste_full);

	m_waste_remainder->start();
}

void BioZone6_GUI::updateMacroStatusMessage(const QString &_message) {

	// this is not used for now, the protocol runner sends a message here, 
	// but this is not currently passed to the GUI
    QString s = " PROTOCOL RUNNING : <<<  ";
	s.append(m_current_protocol_file_name);
    s.append(" >>> remaining time = "); 

    s.append(_message);

	std::cout << HERE << s.toStdString() << std::endl;
}


void BioZone6_GUI::updateWaitAsk(const int _sec, const QString& _message) {

	std::cout << HERE << " wait " << _sec << " seconds, with message " << _message.toStdString() << std::endl;
	if (m_shitty_multiple_call_detector == 0) {
		m_shitty_multiple_call_detector = 1;

		double duration = m_ppc1->protocolDuration(*m_protocol);
		int remaining_time_sec = duration - m_current_protocol_time_status * duration / 100;
		int message_duration = _sec;
		if (_sec > remaining_time_sec) {
			message_duration = remaining_time_sec; //this avoids the pop up to be shown longer than the protocol duration
		}

		if (!visualizeProgressMessage(message_duration, _message)) {
			this->runProtocol(); //this will actually stop the protocol (if the process is running)
			m_shitty_multiple_call_detector = 0;
			return;
		}
		m_shitty_multiple_call_detector = 0;
		return;
	}
	else
	{
		std::cout << HERE << " multiple shitty call "  << std::endl;

	}
}

void BioZone6_GUI::updateMacroTimeStatus(const double &_status) 
{
	// update the vertical line for the time status on the chart
	m_chart_view->updateChartTime(_status);
	m_current_protocol_time_status = _status;
    QString s = m_str_update_time_macro_msg1;
	QFileInfo fi(m_current_protocol_file_name);
	s.append(fi.fileName());
	
	s.append(m_str_update_time_macro_msg2);

	//int remaining_time_sec = m_protocol_duration - _status * m_protocol_duration / 100;
	double duration = m_ppc1->protocolDuration(*m_protocol);
	int remaining_time_sec = duration - m_current_protocol_time_status * duration / 100;
    int remaining_hours = floor(remaining_time_sec / 3600); // 3600 sec in a hour
    int remaining_mins = floor((remaining_time_sec % 3600) / 60); // 60 minutes in a hour
    int remaining_secs = remaining_time_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
    
	s.append(QString::number(remaining_hours));
    //s.append(" h,   ");
    //s.append(QString::number(remaining_mins));
    //s.append(" min,   ");
    //s.append(QString::number(remaining_secs));
    //s.append(" sec   ");
	s.append(" ");
	s.append(m_str_h);
	s.append(",   ");
	s.append(QString::number(remaining_mins));
	s.append(" ");
	s.append(m_str_min);
	s.append(",   ");
	s.append(QString::number(remaining_secs));
	s.append(" ");
	s.append(m_str_sec);
	s.append("   ");
    ui->progressBar_macroStatus->setValue(_status);
    ui->label_macroStatus->setText(s);

    s.clear();
    s.append(QString::number(m_macroRunner_thread->getTimeLeftForStep()));
    s.append(" s");
    ui->label_duration->setText(s);


    // update the slider for the GUI
    ui->horizontalSlider_recirculation->blockSignals(true);
    ui->horizontalSlider_recirculation->setValue(m_ppc1->getVrecircSetPoint());
    ui->horizontalSlider_recirculation->blockSignals(false);

    ui->horizontalSlider_switch->blockSignals(true);
    ui->horizontalSlider_switch->setValue(m_ppc1->getVswitchSetPoint());
    ui->horizontalSlider_switch->blockSignals(false);

    ui->horizontalSlider_p_off->blockSignals(true);
    ui->horizontalSlider_p_off->setValue(m_ppc1->getPoffSetPoint());
    ui->horizontalSlider_p_off->blockSignals(false);

    ui->horizontalSlider_p_on->blockSignals(true);
    ui->horizontalSlider_p_on->setValue(m_ppc1->getPonSetPoint());
    ui->horizontalSlider_p_on->blockSignals(false);

	double currentTime = _status * duration / 100.0 ;
	updateFlowControlPercentages();
}


void BioZone6_GUI::updateTimingSliders()
{
	QProgressBar* _bar;
	QPushButton* _button;
	double status;
	bool continuous_flowing = false;
	double release_time = 0.0;

	switch (m_flowing_solution)
	{
	case 1: {
		_bar = ui->progressBar_solution1;
		_button = ui->pushButton_solution1;
		status = m_pipette_status->rem_vol_well1;
		continuous_flowing = m_solutionParams->continuous_flowing_sol1;
		release_time = m_solutionParams->pulse_duration_well1;
		break;
	}
	case 2: {
		_bar = ui->progressBar_solution2;
		_button = ui->pushButton_solution2;
		status = m_pipette_status->rem_vol_well2;
		continuous_flowing = m_solutionParams->continuous_flowing_sol2;
		release_time = m_solutionParams->pulse_duration_well2;
		break;
	}
	case 3: {
		_bar = ui->progressBar_solution3;
		_button = ui->pushButton_solution3;
		status = m_pipette_status->rem_vol_well3;
		continuous_flowing = m_solutionParams->continuous_flowing_sol3;
		release_time = m_solutionParams->pulse_duration_well3;
		break;
	}
	case 4: {
		_bar = ui->progressBar_solution4;
		_button = ui->pushButton_solution4;
		status = m_pipette_status->rem_vol_well4;
		continuous_flowing = m_solutionParams->continuous_flowing_sol4;
		release_time = m_solutionParams->pulse_duration_well4;
		break;
	}
	case 5: {
		_bar = ui->progressBar_solution5;
		_button = ui->pushButton_solution5;
		status = m_pipette_status->rem_vol_well5;
		continuous_flowing = m_solutionParams->continuous_flowing_sol5;
		release_time = m_solutionParams->pulse_duration_well5;
		break;
	}
	case 6: {
		_bar = ui->progressBar_solution6;
		_button = ui->pushButton_solution6;
		status = m_pipette_status->rem_vol_well6;
		continuous_flowing = m_solutionParams->continuous_flowing_sol6;
		release_time = m_solutionParams->pulse_duration_well6;
		break;
	}

	default: {
		std::cerr << HERE
			<< " error --- no valid m_flowing_solution value " << std::endl;
		m_update_flowing_sliders->stop();  // stop the timer to make sure the function is not recalled if the solutions are not flowing
		return;
	}
	}

	if (m_timer_solution < m_time_multipilcator) {
		m_update_flowing_sliders->start();
		//int status = int(100 * m_timer_solution / m_time_multipilcator);
		//_bar->setValue(100 - status); 
		QString s;
		if (!continuous_flowing) {
			s.append(m_str_pulse_remaining_time + "\n");
			//s.append(QString::number(m_flowing_solution));
			//s.append(" empty in \n");
			int remaining_time_in_sec = (m_time_multipilcator - m_timer_solution);
			int remaining_hours = floor(remaining_time_in_sec / 3600); // 3600 sec in a hour
			int remaining_mins = floor((remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
			int remaining_secs = remaining_time_in_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
			s.append(QString::number(remaining_hours));
			s.append(" h ");
			s.append(QString::number(remaining_mins));
			s.append(" min ");
			s.append(QString::number(remaining_secs));
			s.append(" sec ");
			ui->textEdit_emptyTime->show();
			ui->textEdit_emptyTime->setText(s);
			m_timer_solution++;

			if (m_pipette_active)
				updateDrawing(m_ppc1->getZoneSizePerc());
			else
				updateDrawing(m_ds_perc);
			//updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

		// show the warning label
			if (status < MIN_WARNING_VOLUME) {
				ui->label_warningIcon->show();
				ui->label_warning->setText(m_str_warning_solution_end);
				ui->label_warning->show();
			}
			return;
		}
		else
		{
			if (!m_macroRunner_thread->isRunning())
				s.append(m_str_pulse_continuous_flowing);
			ui->textEdit_emptyTime->show();
			ui->textEdit_emptyTime->setText(s);

			// show the warning label
			if (status < MIN_WARNING_VOLUME) {
				ui->label_warningIcon->show();
				ui->label_warning->setText(m_str_warning_solution_end);
				ui->label_warning->show();
			}
			return;
		}
	}
	else  // here we are ending the release process of the solution
	{
		double solution_release_time = release_time; // m_dialog_tools->getSolutionTime();
		m_time_multipilcator = (int)solution_release_time;
		double rest = solution_release_time - m_time_multipilcator;
		QThread::msleep(rest * 1000);
		// TODO: here we wait the remaining time for the last digit
		//       however, this is a shitty solution and it must be
		//       changed to a proper timer and interrupt architecture

		m_update_flowing_sliders->stop();
		m_timer_solution = 0;

		if (m_pipette_active)
		{
			//m_ppc1->closeAllValves(); //automatic shutdown of pumps when the solution ends
		}
		if (!m_macroRunner_thread->isRunning())
			setEnableSolutionButtons(true);
		_button->setChecked(false);
		this->onPushButtonSolutionX(_button, m_flowing_solution);

		ui->widget_solutionArrow->setVisible(false);

		m_pen_line.setColor(Qt::transparent);
		updateDrawing(m_ds_perc);// (-1); // remove the droplet from the drawing
		//updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

		ui->label_warningIcon->hide();
		ui->label_warning->hide();
		ui->textEdit_emptyTime->hide();
		return;
	}
}

void BioZone6_GUI::updateFlowControlPercentages()
{
	updateFlows();
	if (m_simulationOnly) {

		// calculate droplet size percentage
		m_ds_perc = 100.0 * (m_pipette_status->in_out_ratio_on + 0.21) / 0.31;
		if (m_ds_perc < 0 || m_ds_perc > 1000) {
			//ui->lcdNumber_dropletSize_percentage->display(display_e);
		}
		else {
			//ui->lcdNumber_dropletSize_percentage->display(m_ds_perc);
		}

		// calculate flow speed percentage
		double ponp = 100.0 * m_pipette_status->pon_set_point / m_pr_params->p_on_default;

		//MB: mod to consider pon only in the calculation of the speed
		m_fs_perc = ponp;
		//ui->lcdNumber_flowspeed_percentage->display(m_fs_perc);

		//calculate vacuum percentage
		m_v_perc = 100.0 * m_pipette_status->v_recirc_set_point / (-m_pr_params->v_recirc_default);
		//ui->lcdNumber_vacuum_percentage->display(m_v_perc);
	}
}


void BioZone6_GUI::showSolutionsColor(bool _enable)
{
	if (_enable)
	{
		QColor c1 = m_solutionParams->sol1_color;
		this->colSolution1Changed(c1.red(), c1.green(), c1.blue());
		QColor c2 = m_solutionParams->sol2_color;
		this->colSolution2Changed(c2.red(), c2.green(), c2.blue());
		QColor c3 = m_solutionParams->sol3_color;
		this->colSolution3Changed(c3.red(), c3.green(), c3.blue());
		QColor c4 = m_solutionParams->sol4_color;
		this->colSolution4Changed(c4.red(), c4.green(), c4.blue());
		QColor c5 = m_solutionParams->sol5_color;
		this->colSolution5Changed(c5.red(), c5.green(), c5.blue());
		QColor c6 = m_solutionParams->sol6_color;
		this->colSolution6Changed(c6.red(), c6.green(), c6.blue());
	}
	else
	{
		QColor c = QColor::fromRgba(0xAAAAAAAA);
		this->colSolution1Changed(c.red(), c.green(), c.blue());
		this->colSolution2Changed(c.red(), c.green(), c.blue());
		this->colSolution3Changed(c.red(), c.green(), c.blue());
		this->colSolution4Changed(c.red(), c.green(), c.blue());
		this->colSolution5Changed(c.red(), c.green(), c.blue());
		this->colSolution6Changed(c.red(), c.green(), c.blue());
	}
}