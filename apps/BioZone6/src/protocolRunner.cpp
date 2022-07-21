/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolRunner.h"

BioZone6_protocolRunner::BioZone6_protocolRunner(QMainWindow *parent ) :
	m_ppc1(NULL),
	m_protocol(NULL),
	m_simulation_only(true),
	m_threadTerminationHandler(false),
	m_time_left_for_step(0),
	m_protocol_duration(0.0),
    m_time_elapsed(0.0)
{
	std::cout << HERE << std::endl;	
	m_parent = parent;
	initCustomStrings();
}


void BioZone6_protocolRunner::initCustomStrings()
{
	//setting custom strings to translate 
	m_str_success = tr("Success");
	m_str_failed = tr("Failed");
	m_str_stopped = tr("PROTOCOL STOPPED");
	m_str_not_connected = tr("PPC1 is NOT running, connect and try again");
}

void BioZone6_protocolRunner::switchLanguage(QString _translation_file)
{
	std::cout << HERE << std::endl;

	qApp->removeTranslator(&m_translator_runner);

	if (m_translator_runner.load(_translation_file)) {
		//m_translator_tool.translate("Labonatip_tool", "english");
		qApp->installTranslator(&m_translator_runner);

		//ui_tools->retranslateUi(this);

		initCustomStrings();

		std::cout << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_macroRunner::switchLanguage   installTranslator" << std::endl;
	}
}

void BioZone6_protocolRunner::simulateCommand(fluicell::PPC1api6dataStructures::command _cmd)
{

	int ist = _cmd.getInstruction();

	switch (ist)
	{
	case ppc1Cmd::setPon: { //setPon
		emit setPon(_cmd.getValue());
		msleep(50);
		return;
	}
	case ppc1Cmd::setPoff: {//setPoff
		emit setPoff(_cmd.getValue());
		msleep(50);
		return;

	}
	case ppc1Cmd::setVswitch: {//setVswitch
		emit setVs(-_cmd.getValue());
		msleep(50);
		return;

	}
	case ppc1Cmd::setVrecirc: {//setVrecirc
		emit setVr(-_cmd.getValue());
		msleep(50);
		return;

	}
	case ppc1Cmd::solution1: {//solution1
		emit solution1(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case ppc1Cmd::solution2: {//solution2
		emit solution2(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case ppc1Cmd::solution3: {//solution3
		emit solution3(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case ppc1Cmd::solution4: {//solution4
		emit solution4(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case ppc1Cmd::solution5: {//solution4
		emit solution5(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case ppc1Cmd::solution6: {//solution4
		emit solution6(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case ppc1Cmd::wait: {//sleep
		double val = _cmd.getValue();
		QString msg = QString::fromStdString(_cmd.getStatusMessage());
		simulateWait(val);
		return;
	}
	case ppc1Cmd::showPopUp: {//sleep
		int val = static_cast<int>(_cmd.getValue());
		QString msg = QString::fromStdString(_cmd.getStatusMessage());
		emit(sendWaitAsk(val, msg));
		return;
	}
	case ppc1Cmd::ask: {//ask_msg
		QString msg = QString::fromStdString(_cmd.getStatusMessage());
		emit sendAskMessage(msg); // send ask message event
		m_ask_ok = false;
		while (!m_ask_ok) {  // wait until the signal ok is pressed on the GUI
			msleep(500);
		}
		return;
	}
	case ppc1Cmd::allOff: {//allOff	
		emit closeAll();
		return;
	}
	case ppc1Cmd::pumpsOff: {//pumpsOff
		emit pumpOff();
		return;
	}
	case ppc1Cmd::waitSync: {//waitSync
		// nothing is really to be done in simulation
		return;
	}
	case ppc1Cmd::syncOut: {//syncOut 
		// nothing is really to be done in simulation
		return;
	}
	case ppc1Cmd::loop: {//loop
		// this is not to be done here
		return;
	}
	default: {
		std::cerr << HERE
			<< " fluicell::PPC1api6::runCommand(command _cmd) :::"
			<< " Command NOT recognized "
			<< std::endl;
		return;
	}
	}
}


void BioZone6_protocolRunner::simulateWait(double _sleep_for)
{
	// _sleep_for is the amount of time to sleep in

	if (_sleep_for < 1)
	{
		int time_to_sleep = _sleep_for * 1000;
		QThread::msleep(time_to_sleep);
		std::cout << "slept for " << time_to_sleep << " and it was " << _sleep_for << std::endl;
		return;
	}

	const qint64 kInterval = 1000;
	qint64 mtime = QDateTime::currentMSecsSinceEpoch();

	int number_of_seconds = int(_sleep_for);

	int number_of_milliseconds = int((_sleep_for - number_of_seconds) * 1000);
	for (int j = 0; j < number_of_seconds; j++) {
		// visualize step time left 
		m_time_left_for_step = _sleep_for - j;
		mtime += kInterval;
		qint64 sleepFor = mtime - QDateTime::currentMSecsSinceEpoch();
		if (sleepFor < 0) {
			sleepFor = kInterval - ((-sleepFor) % kInterval);
		}
		QThread::msleep(sleepFor);// (m_macro->at(i).Duration);					
		m_time_elapsed = m_time_elapsed + 1.0;
		double status = 100.0 * m_time_elapsed / m_protocol_duration;

		// send the time status to the GUI
		emit timeStatus(status);

		// stop the wait function in case of thread termination handler
		if (!m_threadTerminationHandler) {
			//QString result = m_str_stopped;
			emit resultReady(m_str_stopped);
			return;
		}
	}
	if (number_of_milliseconds > 0)
	{
		QThread::msleep(number_of_milliseconds);
		m_time_elapsed = m_time_elapsed + (number_of_milliseconds*0.001); // number of milliseconds in sec
		double status = 100.0 * m_time_elapsed / m_protocol_duration;
		emit timeStatus(status);// if we are here, we are done waiting
		std::cout << "slept for " << number_of_milliseconds << " and it was " << _sleep_for << std::endl;
	}
	return;

}


void BioZone6_protocolRunner::run() 
{
	std::cout << HERE << std::endl;
	
	QString result;
	m_threadTerminationHandler = true;

	//main thread
	try
	{
		// the ppc1api and protocol must be initialized 
		if (m_ppc1 && m_protocol)
		{
			std::cout << HERE  << " protocol size " << m_protocol->size() << std::endl;

			// compute the duration of the macro
			m_protocol_duration = m_ppc1->protocolDuration(*m_protocol);
			m_time_elapsed = 0.0;

			// for all the commands in the protocol
			for (size_t i = 0; i < m_protocol->size(); i++)
			{
				// if we get the terminationHandler the thread is stopped
				if (!m_threadTerminationHandler) {
					result = m_str_stopped; 
					emit resultReady(result);
					return;
				}

				if (m_simulation_only)
				{
					// in simulation we set the status message
					QString message = QString::fromStdString(m_protocol->at(i).getStatusMessage());
					message.append(" >>> command :  ");
					message.append(QString::fromStdString(m_protocol->at(i).getCommandAsString()));
					message.append(" value ");
					message.append(QString::number(m_protocol->at(i).getValue()));
					message.append(" status message ");
					message.append(QString::fromStdString(m_protocol->at(i).getStatusMessage()));
					emit sendStatusMessage(message);

					// the command is simulated
					simulateCommand(m_protocol->at(i));

				}// end simulation only
				else {
					// if we are not in simulation and the ppc1 is running 
					// the commands will be sent to the ppc1 api
					if (m_ppc1->isRunning()) {
						
						// at GUI level only ask_msg and wait are handled
						if (m_protocol->at(i).getInstruction() ==
							ppc1Cmd::ask) {
							QString msg = QString::fromStdString(m_protocol->at(i).getStatusMessage());

							emit sendAskMessage(msg); // send ask message event
							m_ask_ok = false;
							while (!m_ask_ok) {  // wait until the signal ok is pressed on the GUI
								msleep(500);
							}
						}

						// If the command is to wait, we do it here
						if (m_protocol->at(i).getInstruction() == ppc1Cmd::wait) 
						{	
							double val = m_protocol->at(i).getValue();
							simulateWait(val);
						}
						if (m_protocol->at(i).getInstruction() == ppc1Cmd::showPopUp)
						{
							int val = static_cast<int>(m_protocol->at(i).getValue());
							QString msg = QString::fromStdString(m_protocol->at(i).getStatusMessage());
							emit(sendWaitAsk(val, msg));
						}
						//TODO: the waitSync works properly in the ppc1api, however, when the command is run
						 //      the ppc1api stops waiting for the signal and the GUI looks freezing without any message
						//if (m_protocol->at(i).getInstruction() == // If the command is to wait, we do it here
						//	pCmd::waitSync) {

						//	emit sendAskMessage("wait sync will run now another message will appear when the sync signal is detected");
						//	if (!m_ppc1->runCommand(m_protocol->at(i))) // otherwise we run the actual command on the PPC1 
						//	{
						//		cerr << HERE 
						//			<< " ---- error --- MESSAGE:"
						//			<< " error in ppc1api PPC1api::runCommand" << endl;
						//	}
						//	emit sendAskMessage("sync arrived");
						//}
						else {
							if (!m_ppc1->runCommand(m_protocol->at(i))) // otherwise we run the actual command on the PPC1 
							{
								std::cerr << HERE << " ---- error --- MESSAGE:"
									<< " error in ppc1api PPC1api::runCommand" << std::endl;
							}
						}
					}
					else {
						std::cerr << HERE << "  ---- error --- MESSAGE:"
							 << " ppc1 is NOT running " << std::endl;

						result = m_str_not_connected; 
						emit resultReady(result);
						return;
					}
				}
			}//end for protocol
			simulateWait(0.2); // TODO: fix this number
			// TODO: this small wait time at the end of the protocol allows for pressure values to
			// be correclty applied and updated
		}
		else {
			std::cerr << HERE << "  ---- error --- MESSAGE: null pointer " << std::endl;
			result = m_str_failed; 

			emit resultReady(result);
			return;
		}
		emit timeStatus(100); 
		result = m_str_success; 
		emit resultReady(result);

		return;

	}
	catch(serial::IOException &e)
	{
		std::cerr << HERE << " IOException : " << e.what() << std::endl;
		//m_ppc1->disconnectCOM();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	catch (serial::PortNotOpenedException &e)
	{
		std::cerr << HERE << " PortNotOpenedException : " << e.what() << std::endl;
		//m_PPC1_serial->close();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	catch (serial::SerialException &e)
	{
		std::cerr << HERE << " SerialException : " << e.what() << std::endl;
		//m_PPC1_serial->close();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	catch (std::exception &e) {
		std::cerr << HERE << " Unhandled Exception: " << e.what() << std::endl;
		//m_PPC1_serial->close();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	
}