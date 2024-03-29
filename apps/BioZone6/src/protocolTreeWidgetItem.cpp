﻿/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolTreeWidgetItem.h"
#include <QSpinBox>


protocolTreeWidgetItem::protocolTreeWidgetItem(protocolTreeWidgetItem *_parent) :
	m_pr_params(new pr_params),
	m_cmd_idx_c(0), m_cmd_command_c (1), m_cmd_range_c (2),
	m_cmd_value_c (3), m_cmd_msg_c (4), m_cmd_level_c (5)
{

	//	std::cout << HERE << std::endl;

	QFont font;
	font.setBold(true);

	this->setText(editorParams::c_idx, "0");
	this->setText(editorParams::c_command, QString::number(0));
	this->setText(editorParams::c_range, this->getRangeColumn(0));
	this->setText(editorParams::c_value, ""); // 
	this->QTreeWidgetItem::setFont(editorParams::c_value, font);

	//this->setCheckState(m_cmd_msg_c, Qt::CheckState::Checked); // status message
	this->setText(m_cmd_msg_c, " "); // status message
	this->setFlags(this->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));


	m_tt_c_idx = tr("Index");
	m_tt_c_cmd = tr("Protocol command");
	m_tt_c_range = tr("Range of validity");
	m_tt_c_value = tr("Value to be applied");
	m_tt_c_msg = tr("Write here any message here");
	m_tt_cmd_alloff = tr("Switch off all pressure pumps and close all solution valves");
	m_tt_cmd_solution = tr("Open/close the solution X valve");
	m_tt_cmd_setPon = tr("Set the pressureON channel to a specific value");
	m_tt_cmd_setPoff = tr("Set the pressureOFF channel to a specific value");
	m_tt_cmd_setVrecirc = tr("Set the vacuum recirculation channel to a specific value");
	m_tt_cmd_setVswitch = tr("Set the vacuum switch channel to a specific value");
	m_tt_cmd_waitSync = tr("");
	m_tt_cmd_syncOut = tr("");
	m_tt_cmd_wait = tr("Set a waiting time in seconds");
	m_tt_cmd_showPopUp = tr("Show a progress pop up for the specified value and showing the status message as command");
	m_tt_cmd_ask = tr("Stop the protocol and ask a confirmation message");
	m_tt_cmd_pumpsOff = tr("Switch off all the pressure pumps");
	m_tt_cmd_loop = tr("Create a loop of commands");
	m_tt_cmd_comment = tr("Comment for the designer, no effect on the protocol running");
	m_tt_cmd_button = tr("Run push/stop protocol for button X");
	m_tt_cmd_ramp = tr("");
	m_tt_cmd_operational = tr("Pur the device in one of the operational modes, \n1 = Standard and Regular (SnR) \n2 = Large and Regular (LnR) \n3 = Standard and Slow (SnS) \n4 = Large and Slow (LnS)");
	m_tt_cmd_initialize = tr("Run initialize protocol");
	m_tt_cmd_standby = tr("Run standby protocol");
	m_tt_cmd_function = tr("");
	m_tt_cmd_smallAndSlow_button = tr("WRITE ME");
	m_tt_cmd_smallAndFast_button = tr("WRITE ME");
	m_tt_cmd_bigAndSlow_button = tr("WRITE ME");
	m_tt_cmd_bigAndFast_button = tr("WRITE ME");

	dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_idx, m_tt_c_idx);
	dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_c_cmd);
	dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_range, m_tt_c_range);
	dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_value, m_tt_c_value);
	dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_msg, m_tt_c_msg);

	if (_parent)
	{
		this->setParent(_parent);
	}
}


bool protocolTreeWidgetItem::checkValidity( int _column)
{
	// get the command index 
	int idx = this->text(editorParams::c_command).toInt();

	// check validity for the element
	if (this->childCount() > 0)
	{
		// If we have children than the item IS a loop
		// so we force the item column to the loop function 
		if (protocolCommands::loop != idx)
		{
			this->setText(editorParams::c_command, QString::number(ppc1Cmd::loop));
		}
	}
    

	// get the number to be checked
	double number = this->text(editorParams::c_value).toDouble();// .toInt();
	switch (idx)
	{
	case protocolCommands::allOff: {
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_alloff);
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::solution1:
	case protocolCommands::solution2:
	case protocolCommands::solution3:
	case protocolCommands::solution4:
	case protocolCommands::solution5:
	case protocolCommands::solution6: {
		// check open valve : 0 = no valve, 1,2,3,4 valves 1,2,3,4
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_solution);
		if (number != 1) {
			this->setText(editorParams::c_value, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::setPon: {
		// if the number if lower than 0,
		// the value becomes automatically positive
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_setPon);
		if (number < 1) {
			number = -number;
			this->setText(editorParams::c_value, QString::number(int(number)));
		}
		else
		{
			this->setText(editorParams::c_value, QString::number(int(number)));
		}
		// if is not the range
		if (//number < m_pr_params->p_off_min || // not necessary as it min=0
			number > m_pr_params->p_on_max) {
			this->setText(editorParams::c_value, QString::number(m_pr_params->p_on_max));
			return false;
		}
		return true;
	}
	case protocolCommands::setPoff: {
		// if the number if lower than 0,
		// the value becomes automatically positive
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_setPoff);
		if (number < 1)
		{
			number = -number;
			this->setText(editorParams::c_value, QString::number(int(number)));
		}
		else
		{
			this->setText(editorParams::c_value, QString::number(int(number)));
		}
		// if is not the range
		if (//number < m_pr_params->p_off_min || // not necessary as it min=0
			number > m_pr_params->p_off_max) {
			this->setText(editorParams::c_value, QString::number(m_pr_params->p_off_max)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::setVrecirc: {
		// if the number if higher than 0,
		// the value becomes automatically negative
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_setVrecirc);
		if (number > -1)
		{
			number = -number;
			this->setText(editorParams::c_value, QString::number(number));
		}
		else
		{
			this->setText(editorParams::c_value, QString::number(int(number)));
		}
		// if is not the range
		if (number < m_pr_params->v_recirc_min) {  
			// || number > m_pr_params->v_recirc_max) { // not necessary as it max=0
			this->setText(editorParams::c_value, QString::number(m_pr_params->v_recirc_min)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::setVswitch: {
		// if the number if higher than 0,
		// the value becomes automatically negative
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_setVswitch);
		if (number > -1) {
			number = -number;
			this->setText(editorParams::c_value, QString::number(number));
		}
		else
		{
			this->setText(editorParams::c_value, QString::number(int(number)));
		}
		// if is not the range
		if (number < m_pr_params->v_switch_min){
			//number > m_pr_params->v_switch_max) {  // not necessary as it max=0
			this->setText(editorParams::c_value, QString::number(m_pr_params->v_switch_min)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::waitSync: {
		// if the value is not valid, reset to zero
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_waitSync);
		if (number != 1) {
			this->setText(editorParams::c_value, QString("0"));
			return false;
		}
		else
		{
			this->setText(editorParams::c_value, QString::number(int(number)));
		}
		return true;
	}
	case protocolCommands::syncOut: {
		// if the value is not valid, reset to zero
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_syncOut);
		if (number < MIN_PULSE_PERIOD) { // if is not the range
			this->setText(editorParams::c_value, QString("20"));
			return false;
		}
		else
		{
			this->setText(editorParams::c_value, QString::number(int(number)));
		}
		return true;
	}
	case protocolCommands::wait: {
		// if the value is not valid, reset to one
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_wait);
		if (number < 0) {
			this->setText(editorParams::c_value, QString("1"));
			return false;
		}
		else
		{
			this->setText(editorParams::c_value, QString::number(number));
		}
		return true;
	}
	case protocolCommands::showPopUp: {
		// if the value is not valid, reset to one
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_showPopUp);
		if (number < 1) {
			this->setText(editorParams::c_value, QString("1"));
			return false;
		}
		else
		{
			this->setText(editorParams::c_value, QString::number(int(number)));
		}
		return true;
	}
	case protocolCommands::ask: {
		// no need to check here
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_ask);
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::pumpsOff: {
		// no need to check here
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_pumpsOff);
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::loop: {
		// if is not the range minimum number is 1
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_loop);
		if (number < 1) {
			this->setText(editorParams::c_value, QString("1")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case protocolCommands::comment: {
		// no need to check here
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_comment);
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::operational: {
		// no need to check here
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_operational);
		//this->setText(editorParams::c_value, QString::number("")); // it removes whatever is there
		if (number < 1) {
			this->setText(editorParams::c_value, QString::number(1)); // if the value is not valid, reset to zero
			return false;
		}
		if (number > 4) {
			this->setText(editorParams::c_value, QString::number(1)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::initialize: {
		// no need to check here
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_initialize);
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::standby: {
		// no need to check here
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_standby);
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case 487878: //protocolCommands::function: 
	{
		// no need to check here
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_function);
		this->setText(editorParams::c_value, QString("1")); // it can only be 1
		return true;
	}
	case protocolCommands::SnR_ON_button:
	case protocolCommands::SnR_OFF_button: {
		// function
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_smallAndSlow_button);
		if (number < 1) {
			this->setText(editorParams::c_value, QString::number(1)); // if the value is not valid, reset to zero
			return false;
		}
		if (number > 6) {
			this->setText(editorParams::c_value, QString::number(1)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::LnR_ON_button:
	case protocolCommands::LnR_OFF_button: {
		// function
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_smallAndFast_button);
		if (number < 1) {
			this->setText(editorParams::c_value, QString::number(1)); // if the value is not valid, reset to zero
			return false;
		}
		if (number > 6) {
			this->setText(editorParams::c_value, QString::number(1)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::SnS_ON_button:
	case protocolCommands::SnS_OFF_button: {
		// function
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_bigAndSlow_button);
		if (number < 1) {
			this->setText(editorParams::c_value, QString::number(1)); // if the value is not valid, reset to zero
			return false;
		}
		if (number > 6) {
			this->setText(editorParams::c_value, QString::number(1)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::LnS_ON_button: 
	case protocolCommands::LnS_OFF_button: {
		// function
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_bigAndFast_button);
		if (number < 1) {
			this->setText(editorParams::c_value, QString::number(1)); // if the value is not valid, reset to zero
			return false;
		}
		if (number > 6) {
			this->setText(editorParams::c_value, QString::number(1)); // if the value is not valid, reset to zero
			return false;
		}
		return true; 
		
	}
	default: {
		// default function active if none of the previous

		break;
		}
	}
	
	return true;
}

QString protocolTreeWidgetItem::getRangeColumn( int _idx)
{

	ComboBoxDelegate cb;

	switch (_idx) {
	case protocolCommands::allOff://pCmd::setPoff: 
	{
		return QString("-");
	}
	case protocolCommands::solution1://pCmd::solution1: 
	case protocolCommands::solution2://pCmd::solution2:
	case protocolCommands::solution3://pCmd::solution3:
	case protocolCommands::solution4://pCmd::solution4:
	case protocolCommands::solution5://pCmd::solution5:
	case protocolCommands::solution6://pCmd::solution6:
	{ 
		// check open valve : 0 = no valve, 1,2,3,4,5,6 valves 1,2,3,4,5,6
		return QString("1/0 open/close");
	} 
	case protocolCommands::setPon://pCmd::setPon:
	case protocolCommands::setPoff://pCmd::setPoff:
	{ // check pressures
		return QString("(mbar) [" + QString::number(MIN_CHAN_C) +
			", " + QString::number(MAX_CHAN_C) + "] ");
	}
	case protocolCommands::setVrecirc:	
	case protocolCommands::setVswitch: { // check vacuums
		return QString("(mbar) [" + QString::number(MIN_CHAN_A) +
			", " + QString::number(MAX_CHAN_A) + "]");
	}
	case protocolCommands::waitSync: {
		// Wait sync"
		return QString("1/0 rise/fall");
	}
	case protocolCommands::syncOut: {
		// Sync out"
		return QString(">20");
	}
	case protocolCommands::wait: {
		// check Wait (s)
		return QString("(s) > 0");
	}
	case protocolCommands::showPopUp: {
		// check Wait (s)
		return QString("(s) > 0");
	}
	case protocolCommands::ask: {
		// ask 	
		return QString("-");
	}
	case protocolCommands::pumpsOff: {
		// pumps off
		return QString("-");
	}
	case protocolCommands::loop: {
		// loop	
		return QString(" > 0");
	}
	case protocolCommands::comment: {
		// Comment	
		return QString("-");
	}
	case protocolCommands::operational: {
		// function
		return QString("SnR, LnR, SnS, LnS");
	}
	case protocolCommands::initialize: {
		// function
		return QString("-");
	}
	case protocolCommands::standby: {
		// function
		return QString("-");
	}
	case protocolCommands::SnR_ON_button:
	case protocolCommands::SnR_OFF_button: 
	case protocolCommands::LnR_ON_button:
	case protocolCommands::LnR_OFF_button: 
	case protocolCommands::SnS_ON_button:
	case protocolCommands::SnS_OFF_button: 
	case protocolCommands::LnS_ON_button:
	case protocolCommands::LnS_OFF_button: {
		// function
		return QString("Button 1 to 6");
	}
	default: {
		// default function active if none of the previous
		return QString("Unknown");
	}
	}

}

void protocolTreeWidgetItem::setElements(int _cmd_ind, double _value, bool _show_msg, QString _msg)
{
	this->blockSignals(true);
	this->setText(m_cmd_idx_c, "0");
	this->setText(m_cmd_command_c, QString::number(_cmd_ind));
	this->setText(m_cmd_range_c, this->getRangeColumn(_cmd_ind));
	this->setText(m_cmd_value_c, QString::number(_value)); 
	//this->setCheckState(m_cmd_msg_c, Qt::CheckState(_show_msg)); // status message
	this->setText(m_cmd_msg_c, _msg); // status message
	this->setFlags(this->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));
	this->blockSignals(false);
}

void protocolTreeWidgetItem::setData(int column, int role, const QVariant & value)
{
	//cout << HERE 
	//	<< " previous " << this->text(column).toStdString() 
	//	<< " new value " << value.toString().toStdString() << endl;

	// this is to keep tracks of changes in the data items, 
	// it is then possible to overload a signal and 
	// bring the last value to the undo stack
	m_last_command = this->text(1).toInt();
	m_last_value = this->text(3).toDouble();
	m_last_msg = this->text(4);

	this->QTreeWidgetItem::setData(column, role, value);

}

protocolTreeWidgetItem * protocolTreeWidgetItem::clone()
{
		protocolTreeWidgetItem * clone = new protocolTreeWidgetItem();

		QFont font;
		font.setBold(true);

		clone->setText (m_cmd_idx_c, this->text(m_cmd_idx_c));
		clone->setText (m_cmd_command_c, this->text(m_cmd_command_c));
		clone->setText(m_cmd_range_c, this->text(m_cmd_range_c));
		clone->setText(m_cmd_value_c, this->text(m_cmd_value_c)); // 
		clone->QTreeWidgetItem::setFont(m_cmd_value_c, font);

		//clone->setCheckState(m_cmd_msg_c, this->checkState(m_cmd_msg_c)); // status message
		clone->setText(m_cmd_msg_c, this->text(m_cmd_msg_c)); // status message
		clone->setFlags(this->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));

		return clone;
}

