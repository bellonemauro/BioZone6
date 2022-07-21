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

QString BioZone6_GUI::generateStyleSheet(const int _r, const int _g, const int _b)
{

	QString styleSheet(" QProgressBar{	border: 1px solid white;"
	                   " padding: 1px;"
	                   " color: rgb(255, 255, 255, 0); "
	                   " border-bottom-right-radius: 2px;"
	                   " border-bottom-left-radius: 2px;"
	                   " border-top-right-radius: 2px;"
	                   " border-top-left-radius: 2px;"
	                   " text-align:right;"
	                   " background-color: rgb(255, 255, 255, 0);"
	                   " width: 15px;}"
	                   " QProgressBar::chunk{"
	                   " background-color: rgb(");
	styleSheet.append(QString::number(_r));
	styleSheet.append(", ");
	styleSheet.append(QString::number(_g));
	styleSheet.append(", ");
	styleSheet.append(QString::number(_b));
	styleSheet.append("); ");
	styleSheet.append(" border-bottom-right-radius: 2px;"
	                  " border-bottom-left-radius: 2px;"
	                  " border-top-right-radius: 2px;"
	                  " border-top-left-radius: 2px;"
	                  " border: 0px solid white;"
	                  " height: 0.5px;}");

	return styleSheet;
}


void BioZone6_GUI::colSolution1Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol1_color.setRgb(_r, _g, _b);
	ui->progressBar_solution1->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor1(m_sol1_color);
}


void BioZone6_GUI::colSolution2Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol2_color.setRgb(_r, _g, _b);
	ui->progressBar_solution2->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor2(m_sol2_color);
}


void BioZone6_GUI::colSolution3Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol3_color.setRgb(_r, _g, _b);
	ui->progressBar_solution3->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor3(m_sol3_color);
}


void BioZone6_GUI::colSolution4Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol4_color.setRgb(_r, _g, _b);
	ui->progressBar_solution4->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor4(m_sol4_color);
}

void BioZone6_GUI::colSolution5Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol5_color.setRgb(_r, _g, _b);
	ui->progressBar_solution5->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor5(m_sol5_color);
}

void BioZone6_GUI::colSolution6Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol6_color.setRgb(_r, _g, _b);
	ui->progressBar_solution6->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor6(m_sol6_color);
}


void BioZone6_GUI::onPushButtonSolutionX(QPushButton *_button, int _idx)
{
	std::cout << HERE << std::endl;
	QString currentProtocolFileName = "";// m_button_protocol_path;
	if (!_button->isChecked()) {

		_button->setChecked(false);
		// stop pumping protocol

		currentProtocolFileName = m_operational_mode_protocol_path +
			"OFF_Button" + QString::number(_idx) +".prt";
		if (QFile::exists(currentProtocolFileName)) {
			//QMessageBox::information(this, m_str_warning,
			//	"FOUND" + tr("<br>%1")
			//	.arg(QDir::toNativeSeparators(currentProtocolFileName)));
			this->runProtocolFile(currentProtocolFileName);
		}
		else
		{
			QMessageBox::warning(this, m_str_warning,
				m_str_file_not_loaded + tr("<br>%1")
				.arg(QDir::toNativeSeparators(currentProtocolFileName)));
			_button->setChecked(true);
		}
	}
	else
	{
		// start pumping protocol
		currentProtocolFileName = m_operational_mode_protocol_path +
			"ON_Button" + QString::number(_idx) + ".prt";
		if (QFile::exists(currentProtocolFileName)) {
			//QMessageBox::warning(this, m_str_warning,
			//	"FOUND" + tr("<br>%1")
			//	.arg(QDir::toNativeSeparators(currentProtocolFileName)));
			this->runProtocolFile(currentProtocolFileName);
		}
		else
		{
			QMessageBox::warning(this, m_str_warning,
				m_str_file_not_loaded + tr("<br>%1")
				.arg(QDir::toNativeSeparators(currentProtocolFileName)));
			_button->setChecked(false);
		}
	}

}

void BioZone6_GUI::onPushButtonSolution1() 
{ 
	std::cout << HERE << std::endl;
    
	this->checkForOtherONbuttons(1);
	this->onPushButtonSolutionX(ui->pushButton_solution1, 1);
}

void BioZone6_GUI::onPushButtonSolution2()
{
	std::cout << HERE << std::endl;

	this->checkForOtherONbuttons(2);
	this->onPushButtonSolutionX(ui->pushButton_solution2, 2);
}

void BioZone6_GUI::onPushButtonSolution3()
{
	std::cout << HERE << std::endl;

	this->checkForOtherONbuttons(3);
	this->onPushButtonSolutionX(ui->pushButton_solution3, 3);
}

void BioZone6_GUI::onPushButtonSolution4()
{
	std::cout << HERE << std::endl;

	this->checkForOtherONbuttons(4);
	this->onPushButtonSolutionX(ui->pushButton_solution4, 4);
}

void BioZone6_GUI::onPushButtonSolution5()
{
	std::cout << HERE << std::endl;

	this->checkForOtherONbuttons(5);
	this->onPushButtonSolutionX(ui->pushButton_solution5, 5);
}

void BioZone6_GUI::onPushButtonSolution6()
{
	std::cout << HERE << std::endl;

	this->checkForOtherONbuttons(6);
	this->onPushButtonSolutionX(ui->pushButton_solution6, 6);
}

//TODO: a lot of code replication here 
void BioZone6_GUI::solution1(bool _enable)
{
	std::cout << HERE << std::endl;
	// we need to overlap this setting to avoid 
	//the solution to be stopped during the protocol runner
	m_solutionParams->continuous_flowing_sol1 = true;
	// this settings need to be restored at the end of the protocol

	
	// close all the valves
	this->closeAllValves();
	this->setEnableSolutionButtons(false);

	// if enable is false just turn off and exit
	if (!_enable)
	{
		ui->pushButton_solution1->setChecked(_enable);
		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		this->updateFlowControlPercentages();
		return;
	}

	// if _enable is true continue

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol1_color);
	m_flowing_solution = 1;

	// move the arrow in the drawing to point on the solution X
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol1);

	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution1->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution1->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	// set the solution release time
	double solution_release_time = m_solutionParams->pulse_duration_well1;
	m_time_multipilcator = (int)solution_release_time;

	ui->pushButton_solution1->setChecked(_enable);
	if (m_pipette_active)
	{
		m_ppc1->setValve_l(true);
	}

	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();
}



void BioZone6_GUI::solution2(bool _enable)
{
	std::cout << HERE << std::endl;
	// we need to overlap this setting to avoid 
	//the solution to be stopped during the protocol runner
	m_solutionParams->continuous_flowing_sol2 = true;
	// this settings need to be restored at the end of the protocol


	// close all the valves
	this->closeAllValves();
	this->setEnableSolutionButtons(false);

	// if enable is false just turn off and exit
	if (!_enable)
	{
		ui->pushButton_solution2->setChecked(_enable);
		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		this->updateFlowControlPercentages();
		return;
	}

	// if _enable is true continue

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol2_color);
	m_flowing_solution = 2;

	// move the arrow in the drawing to point on the solution X
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol2);

	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution2->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution2->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	// set the solution release time
	double solution_release_time = m_solutionParams->pulse_duration_well2;
	m_time_multipilcator = (int)solution_release_time;

	ui->pushButton_solution2->setChecked(_enable);
	if (m_pipette_active)
	{
		m_ppc1->setValve_k(true);
	}

	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();
}


void BioZone6_GUI::solution3(bool _enable)
{
	std::cout << HERE << std::endl;
	// we need to overlap this setting to avoid 
	//the solution to be stopped during the protocol runner
	m_solutionParams->continuous_flowing_sol3 = true;
	// this settings need to be restored at the end of the protocol


	// close all the valves
	this->closeAllValves();
	this->setEnableSolutionButtons(false);

	// if enable is false just turn off and exit
	if (!_enable)
	{
		ui->pushButton_solution3->setChecked(_enable); 
		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		this->updateFlowControlPercentages();
		return;
	}

	// if _enable is true continue

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol3_color);
	m_flowing_solution = 3;

	// move the arrow in the drawing to point on the solution X
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol3);

	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution3->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution3->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	// set the solution release time
	double solution_release_time = m_solutionParams->pulse_duration_well3;
	m_time_multipilcator = (int)solution_release_time;

	ui->pushButton_solution3->setChecked(_enable);
	if (m_pipette_active)
	{
		m_ppc1->setValve_j(true);
	}

	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();
}


void BioZone6_GUI::solution4(bool _enable)
{
	std::cout << HERE << std::endl;
	// we need to overlap this setting to avoid 
	//the solution to be stopped during the protocol runner
	m_solutionParams->continuous_flowing_sol4 = true;
	// this settings need to be restored at the end of the protocol


	// close all the valves
	this->closeAllValves();
	this->setEnableSolutionButtons(false);

	// if enable is false just turn off and exit
	if (!_enable)
	{
		ui->pushButton_solution4->setChecked(_enable); 
		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		this->updateFlowControlPercentages();
		return;
	}

	// if _enable is true continue

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol4_color);
	m_flowing_solution = 4;

	// move the arrow in the drawing to point on the solution X
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol4);

	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution4->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution4->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	// set the solution release time
	double solution_release_time = m_solutionParams->pulse_duration_well4;
	m_time_multipilcator = (int)solution_release_time;

	ui->pushButton_solution4->setChecked(_enable);
	if (m_pipette_active)
	{
		m_ppc1->setValve_i(true);
	}

	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();
}


void BioZone6_GUI::solution5(bool _enable)
{
	std::cout << HERE << std::endl;
	// we need to overlap this setting to avoid 
	//the solution to be stopped during the protocol runner
	m_solutionParams->continuous_flowing_sol5 = true;
	// this settings need to be restored at the end of the protocol


	// close all the valves
	this->closeAllValves();
	this->setEnableSolutionButtons(false);

	// if enable is false just turn off and exit
	if (!_enable)
	{
		ui->pushButton_solution5->setChecked(_enable); 
		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		this->updateFlowControlPercentages();
		return;
	}

	// if _enable is true continue

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol5_color);
	m_flowing_solution = 5;

	// move the arrow in the drawing to point on the solution X
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol5);

	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution5->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution5->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	// set the solution release time
	double solution_release_time = m_solutionParams->pulse_duration_well5;
	m_time_multipilcator = (int)solution_release_time;

	ui->pushButton_solution5->setChecked(_enable);
	if (m_pipette_active)
	{
		m_ppc1->setValve_e(true);
	}

	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();
}

void BioZone6_GUI::solution6(bool _enable) 
{
	std::cout << HERE << std::endl;
	// we need to overlap this setting to avoid 
	//the solution to be stopped during the protocol runner
	m_solutionParams->continuous_flowing_sol6 = true;
	// this settings need to be restored at the end of the protocol
	
	
	// close all the valves
	this->closeAllValves();
	this->setEnableSolutionButtons(false);

	// if enable is false just turn off and exit
	if (!_enable)
	{
		ui->pushButton_solution6->setChecked(_enable); 
		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		this->updateFlowControlPercentages();
		return;
	}

	// if _enable is true continue
	
	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol6_color);
	m_flowing_solution = 6;

	// move the arrow in the drawing to point on the solution X
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol6);

	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution6->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution6->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	// set the solution release time
	double solution_release_time = m_solutionParams->pulse_duration_well6; 
	m_time_multipilcator = (int)solution_release_time;

	ui->pushButton_solution6->setChecked(_enable);
	if (m_pipette_active)
	{
		m_ppc1->setValve_f(true);		
	}

	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();
}


int BioZone6_GUI::getOnSolutionIndex()
{
	// This function assumes that no solutions can be ON 
	// at the same time otherwise the first will be returned 
	if (ui->pushButton_solution1->isChecked())
		return 1;
	if (ui->pushButton_solution2->isChecked())
		return 2;
	if (ui->pushButton_solution3->isChecked())
		return 3;
	if (ui->pushButton_solution4->isChecked())
		return 4;
	if (ui->pushButton_solution5->isChecked())
		return 5;
	if(ui->pushButton_solution6->isChecked())
		return 6;
	return 0;

}

void BioZone6_GUI::checkForOtherONbuttons(int _this_button_idx)
{

	if (ui->pushButton_solution1->isChecked() && _this_button_idx != 1)
	{
		ui->pushButton_solution1->setChecked(false);
		this->onPushButtonSolutionX(ui->pushButton_solution1, 1);
	}
	if (ui->pushButton_solution2->isChecked() && _this_button_idx != 2)
	{
		ui->pushButton_solution2->setChecked(false);
		this->onPushButtonSolutionX(ui->pushButton_solution2, 2);
	}
	if (ui->pushButton_solution3->isChecked() && _this_button_idx != 3)
	{
		ui->pushButton_solution3->setChecked(false);
		this->onPushButtonSolutionX(ui->pushButton_solution3, 3);
	}
	if (ui->pushButton_solution4->isChecked() && _this_button_idx != 4)
	{
		ui->pushButton_solution4->setChecked(false);
		this->onPushButtonSolutionX(ui->pushButton_solution4, 4);
	}
	if (ui->pushButton_solution5->isChecked() && _this_button_idx != 5)
	{
		ui->pushButton_solution5->setChecked(false);
		this->onPushButtonSolutionX(ui->pushButton_solution5, 5);
	}
	if (ui->pushButton_solution6->isChecked() && _this_button_idx != 6)
	{
		ui->pushButton_solution6->setChecked(false);
		this->onPushButtonSolutionX(ui->pushButton_solution6, 6);
	}
}