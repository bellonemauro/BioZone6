﻿/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef BioZone6_protocolRunner_H_
#define BioZone6_protocolRunner_H_

// standard libraries
#include <iostream>
#include <string>

// Qt
#include <QMainWindow>
#include <QDateTime>
#include <QTranslator>
#include <QApplication>
#include <QProgressDialog>
#include <QMessageBox>

// QT threads
#include <qthread.h>
#include "dataStructures.h"

// PPC1api 
#include <fluicell/ppc1api6/ppc1api6.h>

class BioZone6_protocolRunner : public  QThread
{
	Q_OBJECT

public:

	explicit BioZone6_protocolRunner(QMainWindow *parent = nullptr);
	
	void run() Q_DECL_OVERRIDE;

	void switchLanguage(QString _translation_file);

	void setDevice(fluicell::PPC1api6 *_ppc1) { m_ppc1 = _ppc1; }
	
	void setProtocol(std::vector<fluicell::PPC1api6dataStructures::command> *_protocol) { m_protocol = _protocol; };

	void killMacro(bool _kill) {
		m_ppc1->resetSycnSignals(true);  // makes sure that the waitSync command stops
		m_threadTerminationHandler = !_kill; }

	void setSimulationFlag(bool _sim_flag){ m_simulation_only = _sim_flag; }

	void askOkEvent(bool _ask_ok) { m_ask_ok = _ask_ok; }

	int getTimeLeftForStep() { return m_time_left_for_step; }

// interactions between protocol runner and main GUI is done using signals
signals:
	void resultReady(const QString &_s);                //!< emit a string when the result is ready
	void sendStatusMessage(const QString &_message);    //!< send a status message
	void sendAskMessage(const QString& _message);       //!< send a message to ask to the user 
	void sendWaitAsk(int _sec, const QString &_message);       //!< send a message to ask to the user 
	void timeStatus(const double &_time);               //!< send the time status

	void setPon(const double &_v);               //!< setPon
	void setPoff(const double &_v);               //!< setPoff
	void setVs(const double &_v);               //!< setVs
	void setVr(const double &_v);               //!< setVr
	void solution1(const bool &_v);               //!< solution1
	void solution2(const bool &_v);               //!< solution2
	void solution3(const bool &_v);               //!< solution3
	void solution4(const bool& _v);               //!< solution4
	void solution5(const bool& _v);               //!< solution5
	void solution6(const bool &_v);               //!< solution6

	void closeAll();
	void pumpOff();

	void setDropletSizeSIG(const double &_perc);
	void changeDropletSizeSIG(const double &_perc);
	void setFlowSpeedSIG(const double &_perc);
	void changeFlowSpeedSIG(const double &_perc);
	void setVacuumSIG(const double &_perc);
	void changeVacuumSIG(const double &_perc);

private: 
	
	void initCustomStrings();
	void simulateCommand(fluicell::PPC1api6dataStructures::command _cmd);
	void simulateWait(double _sleep_for); // _sleep_for in sec
	

	fluicell::PPC1api6 *m_ppc1;                            //!< pointer to the device to run the protocol 
	std::vector<fluicell::PPC1api6dataStructures::command> *m_protocol;  //!< protocol to run
	bool m_simulation_only;                               //!< true if simulation, false use the PPC1
	bool m_threadTerminationHandler;                      //!< true to terminate the macro
	bool m_ask_ok;                                        //!< false when a message dialog is out, true to continue
	int m_time_left_for_step;                             //!< time left for the current step
	double m_protocol_duration;
	double m_time_elapsed;

    // custom strings for translations
	QString m_str_success;
	QString m_str_failed;
	QString m_str_stopped;
	QString m_str_not_connected;

	QMainWindow* m_parent;
	QTranslator m_translator_runner;
};

#endif /* BioZone6_protocolRunner_H_ */