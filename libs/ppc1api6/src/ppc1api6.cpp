/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | PPC1_6 API - 2021                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "fluicell/ppc1api6/ppc1api6.h"
#include <iomanip>


#ifdef VLD_MEMORY_CHECK
 #include <vld.h>
#endif

/*
 Thought about adding an external xml reader in the low level to be able to use the protocols in the api through a reader without the GUI
 // xmlreader.h // from here https://github.com/YLoveDBule/ShootGame/blob/6ede88734187b2c893ebf2d00919bc6a648b0142/Classes/XmlReader/XmlReader.h
 */

// this macro is used to print function name and useful debug information 
#define HERE std::string(__FUNCTION__ + std::string(" at line ") + std::to_string(__LINE__))

fluicell::PPC1api6::PPC1api6() :
	m_PPC1_data(new fluicell::PPC1api6dataStructures::PPC1api6_data),
	m_PPC1_status(new fluicell::PPC1api6dataStructures::PPC1api6_status),
	m_tip(new fluicell::PPC1api6dataStructures::tip),
	m_verbose(false),
	m_PPC1_serial(new serial::Serial()), // initialize serial port objects
	m_COMport("COM1"),
	m_baud_rate(115200),
	m_dataStreamPeriod(200),
	m_COM_timeout(250),
	m_wait_sync_timeout(60),
	m_excep_handler(false)
{
	// set default values for pressures and vacuums
	setDefaultPV();
	setSquareChannelMod();
	setPipeDiameter();
	setDynamicViscosity();
	
	// set default filter values
	m_filter_enabled = true;
	m_filter_size = 20;
	
	// initialize thread variables
	m_threadTerminationHandler = false; // it will be true when the thread starts
	m_isRunning = false;
	//std::string ss = this->getDeviceID();
}

void fluicell::PPC1api6::threadSerial() 
{
	try {
		std::mutex my_mutex;
		m_isRunning = true;
		while (!m_threadTerminationHandler)
		{
			if(my_mutex.try_lock())
			{
				std::string data;
				if (readData(data))
						m_PPC1_data->data_corrupted = !decodeDataLine(data, m_PPC1_data);
				this->updateFlows(*m_PPC1_data, *m_PPC1_status); 
				my_mutex.unlock();
			}
			else {
				logError(HERE, " impossible to lock ");
				my_mutex.unlock();
				m_threadTerminationHandler = true;
			}
		}
		m_isRunning = false;
	}
	catch (serial::IOException &e) 	{
		m_isRunning = false; 
		m_threadTerminationHandler = true;
		m_PPC1_serial->close(); 
		logError(HERE, " IOException " + std::string(e.what()));
		m_excep_handler = true;
		return;
	}
	catch (serial::SerialException &e) 	{
		m_isRunning = false; 
		m_threadTerminationHandler = true;
		m_PPC1_serial->close(); 
		logError(HERE, " SerialException " + std::string(e.what())); 
		m_excep_handler = true;
		return;
	}
	catch (std::exception &e) 	{
		m_isRunning = false; 
		m_threadTerminationHandler = true;
		m_PPC1_serial->close();
		logError(HERE, 
			std::string(" exception " + std::string(e.what())));
		m_excep_handler = true;
		return;
	}
	
}

void fluicell::PPC1api6::setTip(fluicell::PPC1api6dataStructures::tip::tipType _tip ) {
	
	switch (_tip)
	{
	case fluicell::PPC1api6dataStructures::tip::tipType::Standard:
	{
		m_tip->useStandardTip();
		break;
	}
	case fluicell::PPC1api6dataStructures::tip::tipType::Wide:
	{
		m_tip->useWideTip();
		break;
	}
	case fluicell::PPC1api6dataStructures::tip::tipType::UWZ:
	{
		m_tip->useUWZTip();
		break;
	}
	default:
		logError(HERE,
			std::string("Unknown setting of tip type " ));
		break;
	}

}

bool fluicell::PPC1api6::setChannelData(const std::string& _data, 
	fluicell::PPC1api6dataStructures::PPC1api6_data::channel* _channel) const
{
	std::vector<double> line;  // decoded line 
	if (decodeChannelLine(_data, line))  // decode the line 
	{   // and fill the right place in the data structure
		_channel->setChannelData(line.at(0), line.at(1),
			line.at(2), (int)line.at(3));
		return true;
	}
	
	logError(HERE, " Error in decoding line ");
	return false;
	
}

bool fluicell::PPC1api6::setFlagsData(const std::string& _data,
	fluicell::PPC1api6dataStructures::PPC1api6_data* _PPC1_data) const
{
	// string format:  i0|j0|k0|l0|e0|f0
		// char index   :  0123456789
	int value = toDigit(_data.at(1));
	if (value == 0 || value == 1) { // admitted values are only 0 and 1
		_PPC1_data->i = value;
	}
	else {
		logError(HERE,
			" Error in decoding line _PPC1api6_data->i string: " +
			_data + " value " + std::to_string(value));
		return false;
	}

	value = toDigit(_data.at(4));
	if (value == 0 || value == 1) { // admitted values are only 0 and 1
		_PPC1_data->j = value;
	}
	else {
		logError(HERE,
			" Error in decoding line _PPC1api6_data->j string: " +
			_data + " value " + std::to_string(value));
		return false;
	}

	value = toDigit(_data.at(7));
	if (value == 0 || value == 1) { // admitted values are only 0 and 1
		_PPC1_data->k = value;
	}
	else {
		logError(HERE,
			" Error in decoding line _PPC1api6_data->k string: " +
			_data + " value " + std::to_string(value));
		return false;
	}

	value = toDigit(_data.at(10));
	if (value == 0 || value == 1) { // admitted values are only 0 and 1
		_PPC1_data->l = value;
	}
	else {
		logError(HERE,
			" Error in decoding line _PPC1api6_data->l string: " +
			_data + " value " + std::to_string(value));
		return false;
	}

	value = toDigit(_data.at(13));
	if (value == 0 || value == 1) { // admitted values are only 0 and 1
		_PPC1_data->e = value;
	}
	else {
		logError(HERE,
			" Error in decoding line _PPC1api6_data->e string: " +
			_data + " value " + std::to_string(value));
		return false;
	}

	value = toDigit(_data.at(16));
	if (value == 0 || value == 1) { // admitted values are only 0 and 1
		_PPC1_data->f = value;
	}
	else {
		logError(HERE,
			" Error in decoding line _PPC1api6_data->f string: " +
			_data + " value " + std::to_string(value));
		return false;
	}

	return true;
}

bool fluicell::PPC1api6::decodeDataLine(const std::string &_data, 
	fluicell::PPC1api6dataStructures::PPC1api6_data *_PPC1_data) const
{
	//std::string ss = (const_cast<fluicell::PPC1api6*>(this))->getDeviceID();
	// check for empty data
	if (_data.empty())
	{
		logError(HERE, " Error in decoding line - Empty line ");
		return false;
	}

	// check for _PPC1_data initialized
	if (_PPC1_data == NULL)
	{
		logError(HERE, " Error in decoding line - _PPC1api6_data not initialized ");
		return false;
	}

	const char first_char = _data.at(0);
	if (first_char == 'A') 
		return setChannelData(_data, _PPC1_data->channel_A);

	if (first_char == 'B') 
		return setChannelData(_data, _PPC1_data->channel_B);

	if (first_char == 'C') 
		return setChannelData(_data, _PPC1_data->channel_C);

	if (first_char == 'D') 
		return setChannelData(_data, _PPC1_data->channel_D);

	if (first_char == 'i') {
		return setFlagsData(_data, _PPC1_data);
	}

	if (first_char == 'I') {
		// string format: IN1|OUT1 or IN0|OUT0
		// char index:    01234567
		int value = toDigit(_data.at(2));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->ppc1_IN = value;
		}
		else {
			logError(HERE, " Error in decoding line _PPC1api6_data->ppc1_IN " );
			return false;
		}
		value = toDigit(_data.at(7));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->ppc1_OUT = value;
		}
		else {
			logError(HERE, " Error in decoding line _PPC1api6_data->ppc1_OUT "); 
			return false;
		}
		return true;
	}

	if (first_char == 'P') {  // FALLING TTL signal detected
		// string format: P\n
		// char index:    01
		_PPC1_data->trigger_fall = true;
		_PPC1_data->trigger_rise = false;
		return true;
	} 

	if (first_char == 'R') {  //RISING TTL signal detected
		// string format: R\n
		// char index:    01
		_PPC1_data->trigger_fall = false;
		_PPC1_data->trigger_rise = true;
		return true;
	}

	return false;  // in case _data(0) is none of the expected value
}

bool fluicell::PPC1api6::decodeChannelLine(const std::string &_data, std::vector<double> &_line) const
{
	// check for empty data
	if (_data.empty())
	{
		logError(HERE, " Error in decoding line - Empty line "); 
		return false;
	}

	_line.clear();
	unsigned int byte_counter = 2;              // in the line 0 is letter and 1 is the separator e.g. A|
	while (byte_counter < _data.length())       // scan the whole string
	{
		std::string value;
		// extract line 
		// extract the value before the character "new line"
		while (_data.at(byte_counter) != m_separator)//*separator)
		{
			if (_data.at(byte_counter) == m_end_line) // if the char is the endline the function break
			{
				_line.push_back(std::stod(value));
				break;
			}

			// check the char for validity
			// this is to make sure that stod function get an actual number instead of a character 
			if (!isdigit(_data.at(byte_counter))) // if the char is not a digit
				if (_data.at(byte_counter) != m_minus) // if the char is not the minus sign
					if (_data.at(byte_counter) != m_decimal_separator) // if the char is not the decimal separator
					{
						return false;  // something is wrong with the string (not a number)
					}

			// validity check passed
			value.push_back(_data.at(byte_counter));
			byte_counter++;
			if (byte_counter >= _data.length()) 
				break;
		}
		byte_counter++;

		// check if the value is empty
		if (!value.empty()) {
			_line.push_back(stod(value)); 
			if (_line.size() > 3)
				return true; // we expect 4 values so we exit at the 4th
		}
		else {
			_line.push_back(0.0);
		}
	}

	// check for proper data size
	if (_line.size() < 3) {
		logError(HERE, " Error in decoding line - corrupted data line "); 
		return false;
	}

	return true;
}

void fluicell::PPC1api6::updateFlows(const fluicell::PPC1api6dataStructures::PPC1api6_data &_PPC1_data,
	fluicell::PPC1api6dataStructures::PPC1api6_status &_PPC1_status) const
{
	// calculate inflow
	double delta_pressure = 100.0 * (-_PPC1_data.channel_A->sensor_reading);//   v_r;

	_PPC1_status.inflow_recirculation = 2.0 * 
		this->getFlowSimple(delta_pressure, m_tip->length_to_tip);

	delta_pressure = 100.0 * (-_PPC1_data.channel_A->sensor_reading +
		2.0 * _PPC1_data.channel_C->sensor_reading * ( 1 - m_tip->length_to_tip / m_tip->length_to_zone) );
	_PPC1_status.inflow_switch = 2.0 * this->getFlowSimple(delta_pressure, m_tip->length_to_tip);

	delta_pressure = 100.0 * 2.0 * _PPC1_data.channel_C->sensor_reading;
	_PPC1_status.solution_usage_off = this->getFlowSimple(delta_pressure, 2.0 * m_tip->length_to_zone);

	delta_pressure = 100.0 * _PPC1_data.channel_D->sensor_reading;
	_PPC1_status.solution_usage_on = this->getFlowSimple(delta_pressure, m_tip->length_to_tip);

	delta_pressure = 100.0 * (_PPC1_data.channel_D->sensor_reading +
		(_PPC1_data.channel_C->sensor_reading * 3.0) -
		(-_PPC1_data.channel_B->sensor_reading * 2.0));
	_PPC1_status.outflow_on = this->getFlowSimple(delta_pressure, m_tip->length_to_tip);

	delta_pressure = 100.0 * ((_PPC1_data.channel_C->sensor_reading * 4.0) -
		(-_PPC1_data.channel_B->sensor_reading * 2.0));
	_PPC1_status.outflow_off = 2.0 * this->getFlowSimple(delta_pressure, 2.0 * m_tip->length_to_zone);

	_PPC1_status.in_out_ratio_on = _PPC1_status.outflow_on / _PPC1_status.inflow_recirculation;
	_PPC1_status.in_out_ratio_off = _PPC1_status.outflow_off / _PPC1_status.inflow_recirculation;

	if (_PPC1_data.e || _PPC1_data.f || 
		_PPC1_data.i || _PPC1_data.j ||
		_PPC1_data.k || _PPC1_data.l ) // if one of the solutions is on
	{
		delta_pressure = 100.0 * (_PPC1_data.channel_D->sensor_reading +
			(_PPC1_data.channel_C->sensor_reading * 3.0) -
			(-_PPC1_data.channel_B->sensor_reading * 2.0));

		_PPC1_status.outflow_tot = _PPC1_status.outflow_on;
		_PPC1_status.in_out_ratio_tot = _PPC1_status.in_out_ratio_on;

		_PPC1_status.flow_rate_1 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_2 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_3 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_4 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_5 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_6 = _PPC1_status.solution_usage_off;

		if (_PPC1_data.l) _PPC1_status.flow_rate_1 = _PPC1_status.solution_usage_on;
		if (_PPC1_data.k) _PPC1_status.flow_rate_2 = _PPC1_status.solution_usage_on;
		if (_PPC1_data.j) _PPC1_status.flow_rate_3 = _PPC1_status.solution_usage_on;
		if (_PPC1_data.i) _PPC1_status.flow_rate_4 = _PPC1_status.solution_usage_on;
		if (_PPC1_data.e) _PPC1_status.flow_rate_5 = _PPC1_status.solution_usage_on;
		if (_PPC1_data.f) _PPC1_status.flow_rate_6 = _PPC1_status.solution_usage_on;
	}
	else {

		_PPC1_status.outflow_tot = _PPC1_status.outflow_off;
		_PPC1_status.in_out_ratio_tot = _PPC1_status.in_out_ratio_off;

		_PPC1_status.flow_rate_1 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_2 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_3 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_4 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_5 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_6 = _PPC1_status.solution_usage_off;
	}

	_PPC1_status.flow_rate_7 = _PPC1_status.inflow_switch; // / 2.0;
	_PPC1_status.flow_rate_8 = _PPC1_status.inflow_recirculation; // / 2.0;

}

bool fluicell::PPC1api6::connectCOM() 
{
	try {

		m_PPC1_serial->setPort(m_COMport);
		m_PPC1_serial->setBaudrate(m_baud_rate);
		m_PPC1_serial->setFlowcontrol(serial::flowcontrol_none);
		m_PPC1_serial->setParity(serial::parity_none);

		if (!checkVIDPID(m_COMport)) {
			logError(HERE, " no match VID/PID device "); 
			return false;
		}
		logStatus(HERE, " VID/PID match ");
		
		// "Is the port open?";
		if (m_PPC1_serial->isOpen())
			std::this_thread::sleep_for(std::chrono::microseconds(100));  //--> do nothing, wait
		else {
			m_PPC1_serial->open();
            std::this_thread::sleep_for(std::chrono::microseconds(100));  
		}

		// if the first attempt to open the port fails then the connection fails
		if (!m_PPC1_serial->isOpen()) {
			logError(HERE, "FAILED - Serial port not open ");
			return false;
		}
		m_excep_handler = false; //only on connection verified we reset the exception handler
		return true; // open connection verified 
	}
	catch (serial::IOException &e)
	{
		logError(HERE, " IOException " + std::string(e.what()));
		m_PPC1_serial->close(); 
		//throw e;
		m_excep_handler = true;
		return false;
	}
	catch (serial::PortNotOpenedException &e)
	{
		logError(HERE, " PortNotOpenedException " + std::string(e.what()));
		m_PPC1_serial->close();
		//throw e;
		m_excep_handler = true;
		return false;
	}
	catch (serial::SerialException &e)
	{
		logError(HERE, " SerialException " + std::string(e.what()));
		m_PPC1_serial->close(); 
		//throw e;
		m_excep_handler = true;
		return false;
	}
	catch (std::exception &e) 
	{
		logError(HERE, " Unhandled Exception " + std::string(e.what()));
		m_PPC1_serial->close(); 
		//throw e;  // TODO: this crashes
		m_excep_handler = true;
		return false;
	}
}

void fluicell::PPC1api6::disconnectCOM()
{
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->close();
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		m_excep_handler = false;
	}
}

void fluicell::PPC1api6::pumpingOff() const
{
	if (m_PPC1_serial->isOpen()) {
		setVacuumChannelA(0.0);   
		setVacuumChannelB(0.0);   
		setPressureChannelC(0.0); 
		setPressureChannelD(0.0); 
		closeAllValves();
	}
}

bool fluicell::PPC1api6::openAllValves() const
{
	if (m_PPC1_serial->isOpen()) 
		return setValvesState(0xFF);
	else return false;
}

bool fluicell::PPC1api6::closeAllValves() const
{
	if (m_PPC1_serial->isOpen()) 
		return setValvesState(0x30);
	else return false;
}

void fluicell::PPC1api6::reboot() const
{
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->write("!\n");
	}
}

bool fluicell::PPC1api6::setVacuumChannelA(const double _value) const
{

	if (_value >= MIN_CHAN_A && _value <= MAX_CHAN_A)
	{
		std::string ss;
		ss.append("A");
		ss.append(std::to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		logError(HERE, " out of range " );
		sendData("A0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api6::setVacuumChannelB(const double _value) const
{
	if (_value >= MIN_CHAN_B && _value <= MAX_CHAN_B)
	{
		std::string ss;
		ss.append("B");
		ss.append(std::to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		logError(HERE, " out of range ");
		sendData("B0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api6::setPressureChannelC(const double _value) const
{
	if (_value >= MIN_CHAN_C && _value <= MAX_CHAN_C)
	{
		std::string ss;
		ss.append("C");
		ss.append(std::to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		logError(HERE, " out of range ");
		sendData("C0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api6::setPressureChannelD(const double _value) const
{
	if (_value >= MIN_CHAN_D && _value <= MAX_CHAN_D)
	{
		std::string ss;
		ss.append("D");
		ss.append(std::to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else {
		logError(HERE, " out of range ");
		sendData("D0.0\n");  // send 0
		return false;
	}

	return false;
}

bool fluicell::PPC1api6::setValve_l(const bool _value) const
{
	if (_value) 
		return sendData("l1\n");   // close
	else 
		return sendData("l0\n");  // open
}

bool fluicell::PPC1api6::setValve_k(const bool _value)  const 
{
	if (_value) 
		return sendData("k1\n");   // close
	else 
		return sendData("k0\n");  // open
}

bool fluicell::PPC1api6::setValve_j(const bool _value)  const 
{
	if (_value) 
		return sendData("j1\n");   // close
	else 
		return sendData("j0\n");  // open
}

bool fluicell::PPC1api6::setValve_i(const bool _value) const
{
	if (_value) 
		return sendData("i1\n");   // close
	else 
		return sendData("i0\n");  // open
}

bool fluicell::PPC1api6::setValve_e(const bool _value) const
{
	if (_value)
		return sendData("e1\n");   // close
	else
		return sendData("e0\n");  // open
}

bool fluicell::PPC1api6::setValve_f(const bool _value) const
{
	if (_value)
		return sendData("f1\n");   // close
	else
		return sendData("f0\n");  // open
}

bool fluicell::PPC1api6::setValvesState(const int _value) const
{
	std::stringstream value;
	value << std::setfill('0') 
		  << std::setw(2) // we expect only one byte so 2 is the number of allowed hex digits
		  << std::hex << _value;
	std::string msg;
	msg.append("v");
	msg.append(value.str());
	msg.append("\n");
	return sendData(msg);
}

bool fluicell::PPC1api6::setTTLstate(const bool _value) const
{
	if (_value == 1) {
		if (sendData("o1\n"))   // high
		{
			m_PPC1_data->TTL_out_trigger = true;
			return true;
		}
	}
	// otherwise set to low
	if (sendData("o0\n"))  // low
	{
		m_PPC1_data->TTL_out_trigger = false;
		return true;
	}
	
	return false;
}

bool fluicell::PPC1api6::setPulsePeriod(const int _value) const
{
	if (_value >= MIN_PULSE_PERIOD )
	{
		std::string ss;
		ss.append("p");
		ss.append(std::to_string(_value));
		ss.append("\n");
		return sendData(ss);
	}

	logError(HERE, " out of range ");
	return false;
}

bool fluicell::PPC1api6::setRuntimeTimeout(const int _value) const
{

	if (_value < 1)
	{
		std::string ss;
		ss.append("z");
		ss.append(std::to_string(_value));
		ss.append("\n");
		return sendData(ss);
	}

	logError(HERE, " out of range ");
	return false;
}


double fluicell::PPC1api6::getZoneSizePerc() const
{
	double in_out_ratio_on = 0;

	bool use_sensor_reading = false;
	if (use_sensor_reading) {
		in_out_ratio_on = m_PPC1_status->in_out_ratio_on;
	}
	else
	{
		// calculate the outflow_on based on set value instead of the sensor reading
		double outflow_on;
		double delta_pressure = 100.0 * (m_PPC1_data->channel_D->set_point +
			(m_PPC1_data->channel_C->set_point * 3.0) -
			(-m_PPC1_data->channel_B->set_point * 2.0));
	    outflow_on = this->getFlowSimple(delta_pressure, m_tip->length_to_tip);

		// calculate inflow_recirculation based on set value instead of the sensor reading
		double inflow_recirculation;
		delta_pressure = 100.0 * (-m_PPC1_data->channel_A->set_point);//   v_r;
		inflow_recirculation = 2.0 * this->getFlowSimple(delta_pressure, m_tip->length_to_tip);
		
		in_out_ratio_on = outflow_on / inflow_recirculation;
	}
	double ds = 100.0 *(in_out_ratio_on + 0.21) / 0.31;
	return ds;
}


double fluicell::PPC1api6::getFlowSpeedPerc() const
{
	bool use_sensor_reading = false;
	double p1 = 0;
	double p2 = 0;
	double p3 = 0;
	double p4 = 0;
	if (use_sensor_reading) {

		p1 = std::abs(100.0 * m_PPC1_data->channel_A->sensor_reading / m_default_v_recirc);
		p2 = std::abs(100.0 * m_PPC1_data->channel_B->sensor_reading / m_default_v_switch);
		p3 = std::abs(100.0 * m_PPC1_data->channel_C->sensor_reading / m_default_poff);
		p4 = std::abs(100.0 * m_PPC1_data->channel_D->sensor_reading / m_default_pon);
	}
	else
	{
		p1 = std::abs(100.0 * m_PPC1_data->channel_A->set_point / m_default_v_recirc);
		p2 = std::abs(100.0 * m_PPC1_data->channel_B->set_point / m_default_v_switch);
		p3 = std::abs(100.0 * m_PPC1_data->channel_C->set_point / m_default_poff);
		p4 = std::abs(100.0 * m_PPC1_data->channel_D->set_point / m_default_pon);
	}
	double mean_percentage = (p1 + p2 + p3 + p4) / 4.0; // average 4 values
	return mean_percentage;
}


double fluicell::PPC1api6::getVacuumPerc() const
{
	bool use_sensor_reading = false;
	double value_A = 0;
	if (use_sensor_reading) {
		value_A = m_PPC1_data->channel_A->sensor_reading;
	}
	else {
		value_A = m_PPC1_data->channel_A->set_point;
	}

	double p1 = std::abs(100.0 * value_A / m_default_v_recirc);
	return p1;
}

double fluicell::PPC1api6::getFlow(double _square_channel_mod, 
	double _pipe_diameter, double _delta_pressure, 
	double _dynamic_viscosity, double _pipe_length) const
{
	double num = M_PI * std::pow((_square_channel_mod * _pipe_diameter), 4) * _delta_pressure;
	double den = 128.0 * _dynamic_viscosity * _pipe_length;
	
	// flow in transformation to nL/s
	double flow = (num / den) * 1000.0 * 1000000000.0; // per channel 

	return flow;
}

bool fluicell::PPC1api6::runCommand(fluicell::PPC1api6dataStructures::command _cmd) const
{
	if (!_cmd.checkValidity())  {
		std::string msg = " check validity failed ";
		msg.append(_cmd.getCommandAsString());
		msg.append(" value ");
		msg.append(std::to_string(_cmd.getValue()));
		logError(HERE, msg );
		return false;
	}

	logStatus(HERE, " running the command " + _cmd.getCommandAsString() +
	  " value " + std::to_string(_cmd.getValue()));

	switch (_cmd.getInstruction()) {
	case fluicell::PPC1api6dataStructures::command::instructions::wait: {//sleep
		//TODO: this is not safe as one can stop the macro without breaking the wait function
		//however, wait function is handled at GUI level not at API level
		//std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(_cmd.getValue())));
		// This could solve the issue of waiting and stopping 
		// (it has to wait for 1 second to finish though)
		// moreover, this introduces uncertainty in the time line
		// TO BE TESTED
		/*
		int number_of_seconds = static_cast<int>(_cmd.getValue());
		int count = 0;
		if (!m_threadTerminationHandler && count<number_of_seconds)
		{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		count++;
		}*/
		return true;
	}
	case fluicell::PPC1api6dataStructures::command::instructions::allOff: {//allOff	
		return closeAllValves();
	}
	case fluicell::PPC1api6dataStructures::command::instructions::solution1: {//solution1
		if (!closeAllValves())return false;
		bool valve_status = static_cast<bool>(_cmd.getValue());
		return setValve_l(valve_status);
	}
	case fluicell::PPC1api6dataStructures::command::instructions::solution2: {//solution2
		if (!closeAllValves())return false;
		bool valve_status = static_cast<bool>(_cmd.getValue());
		return setValve_k(valve_status);
	}
	case fluicell::PPC1api6dataStructures::command::instructions::solution3: {//solution3
		if (!closeAllValves())return false;
		bool valve_status = static_cast<bool>(_cmd.getValue());
		return setValve_j(valve_status);
	}
	case fluicell::PPC1api6dataStructures::command::instructions::solution4: {//solution4
		if (!closeAllValves())return false;
		bool valve_status = static_cast<bool>(_cmd.getValue());
		return setValve_i(valve_status);
	}
	case fluicell::PPC1api6dataStructures::command::instructions::solution5: {//solution5
		if (!closeAllValves())return false;
		bool valve_status = static_cast<bool>(_cmd.getValue());
		return setValve_e(valve_status);
	}
	case fluicell::PPC1api6dataStructures::command::instructions::solution6: {//solution6
		if (!closeAllValves())return false;
		bool valve_status = static_cast<bool>(_cmd.getValue());
		return setValve_f(valve_status);
	}
	case fluicell::PPC1api6dataStructures::command::instructions::setPon: { //setPon
		return setPressureChannelD(_cmd.getValue());
	}
	case fluicell::PPC1api6dataStructures::command::instructions::setPoff: {//setPoff
		return setPressureChannelC(_cmd.getValue());
	}
	case fluicell::PPC1api6dataStructures::command::instructions::setVrecirc: {//setVrecirc
		return setVacuumChannelA(_cmd.getValue());
	}
	case fluicell::PPC1api6dataStructures::command::instructions::setVswitch: {//setVswitch
		return setVacuumChannelB(_cmd.getValue());
	}
	case fluicell::PPC1api6dataStructures::command::instructions::ask: {//ask_msg
		logStatus(HERE, " ask_msg NOT implemented at the API level ");
		return true;
	}
	case fluicell::PPC1api6dataStructures::command::instructions::pumpsOff: {//pumpsOff
		pumpingOff();
		return true;
	}
	case fluicell::PPC1api6dataStructures::command::instructions::waitSync: {//waitSync 
		// waitsync(front type : can be : RISE or FALL), 
		// protocol stops until trigger signal is received
		bool state;
		if (_cmd.getValue() == 0) state = false;
		else state = true;
		// reset the sync signals and then wait for the correct state to come
		resetSycnSignals(false);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		clock_t begin = clock();
		while (!syncSignalArrived(state))
		{
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			clock_t end = clock();
			double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
			if (elapsed_secs > m_wait_sync_timeout) // break if timeout
			{
				logError(HERE, " waitSync timeout ");
				return false;
			}
		}
		return true;

	}
	case fluicell::PPC1api6dataStructures::command::instructions::syncOut: {//syncOut //TODO
		// syncout(int: pulse length in ms) if negative then default state is 1
		// and pulse is 0, if positive, then pulse is 1 and default is 0
		int v = static_cast<int>(_cmd.getValue());
		logStatus(HERE, " syncOut test value " + v);
		int current_ppc1out_status = m_PPC1_data->ppc1_OUT;
		bool success = setPulsePeriod(v);
		std::this_thread::sleep_for(std::chrono::milliseconds(v));
		//TODO : this function is unsafe, in case the protocol is stop during this function, 
		//       the stop will not work, it has to wait for the wait to end
		/*
		clock_t begin = clock();
		while (current_ppc1out_status == m_PPC1_data->ppc1_OUT)
		{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
		while (current_ppc1out_status != m_PPC1_data->ppc1_OUT)
		{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;*/
		return success;
	}
	case fluicell::PPC1api6dataStructures::command::instructions::loop: {//loop
		logStatus(HERE, " loop NOT implemented at the API level " );
		return true;
	}
	case fluicell::PPC1api6dataStructures::command::instructions::operational:
	case fluicell::PPC1api6dataStructures::command::instructions::standby:
	case fluicell::PPC1api6dataStructures::command::instructions::SnR_ON_button:
	case fluicell::PPC1api6dataStructures::command::instructions::SnR_OFF_button:
	case fluicell::PPC1api6dataStructures::command::instructions::LnR_ON_button:
	case fluicell::PPC1api6dataStructures::command::instructions::LnR_OFF_button:
	case fluicell::PPC1api6dataStructures::command::instructions::SnS_ON_button:
	case fluicell::PPC1api6dataStructures::command::instructions::SnS_OFF_button:
	case fluicell::PPC1api6dataStructures::command::instructions::LnS_ON_button:
	case fluicell::PPC1api6dataStructures::command::instructions::LnS_OFF_button:
	case fluicell::PPC1api6dataStructures::command::instructions::initialize:
	case fluicell::PPC1api6dataStructures::command::instructions::showPopUp:
	{	
		
		logStatus(HERE, _cmd.getCommandAsString() + ": this command NOT implemented at the API level ");
		return true;

	}
	default:{
		logError(HERE, " Command NOT recognized ");
		return false;
	}
	}
	return false;
}

bool fluicell::PPC1api6::setDataStreamPeriod(const int _value) {
	if (_value >=  MIN_STREAM_PERIOD && _value <=  MAX_STREAM_PERIOD )
	{
		m_dataStreamPeriod = _value;
		std::string ss;
		ss.append("u");
		ss.append(std::to_string(_value));
		ss.append("\n");
		return sendData(ss);
	}

	logError(HERE, " out of range ");
	sendData("u200\n");  // send default value
	return false;
}

bool fluicell::PPC1api6::setDefaultPV(double _default_pon, double _default_poff, 
	double _default_v_recirc, double _default_v_switch)
{
	logStatus(HERE, 
		" _default_pon " + std::to_string(_default_pon) +
		" _default_poff " + std::to_string(_default_poff) +
		" _default_v_recirc " + std::to_string(_default_v_recirc) +
		" _default_v_switch " + std::to_string(_default_v_switch) );

if (_default_pon > MIN_CHAN_D && _default_pon < MAX_CHAN_D) {
		m_default_pon = _default_pon;
	}
	else {
		logError(HERE, " default pon out of range ");
		return false;
	}
	if (_default_poff > MIN_CHAN_C && _default_poff < MAX_CHAN_C) {
		m_default_poff = _default_poff;
	}
	else {
		logError(HERE, " default poff out of range ");
		return false;
	}
	if (_default_v_recirc > MIN_CHAN_A && _default_v_recirc < MAX_CHAN_A) {
		m_default_v_recirc = _default_v_recirc;
	}
	else {
		logError(HERE, " default v recirculation out of range ");
		return false;
	}
	if (_default_v_switch > MIN_CHAN_B && _default_v_switch < MAX_CHAN_B) {
		m_default_v_switch = _default_v_switch;
	}
	else {
		logError(HERE, " default v switch out of range ");
		return false;
	}

	return true;
}

std::string fluicell::PPC1api6::getDeviceID() 
{
	std::string serialNumber; //device serial number

	if (!m_PPC1_serial->isOpen()) {
		logError(HERE, " cannot return the device serial number, device not connected ");
		return "";
	}

	// stop the stream to be able to get the value
	if (!setDataStreamPeriod(0))
	{
		logError(HERE, " cannot set the data stream period to 0 ");
	}

	// send the character to get the device serial number
	sendData("#\n"); // this character is not properly sent maybe a serial lib bug?
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	readData(serialNumber);
	logStatus(HERE, " the serial number is : " + serialNumber);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	// restore the data stream to the default value
	setDataStreamPeriod(200);// (m_dataStreamPeriod);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	logStatus(HERE, " set data stream to : " + m_dataStreamPeriod);

	return serialNumber;
}

void fluicell::PPC1api6::setFilterEnabled(bool _enable)
{
	logStatus(HERE, " new filter size value " + _enable);
	m_filter_enabled = _enable;

	m_PPC1_data->enableFilter(_enable);
}

void fluicell::PPC1api6::setFilterSize(int _size)
{
	logStatus(HERE, " new filter size value " + _size);

	if (_size < 1)
	{
		logError(HERE, " negative value on set size " + std::to_string(_size) );
		return;
	}
	
	m_PPC1_data->setFilterSize(_size); 
}

bool fluicell::PPC1api6::sendData(const std::string &_data) const
{
	if (m_PPC1_serial->isOpen()) {
		logStatus(HERE, " sending the string " + _data );

		if (m_PPC1_serial->write(_data) > 0) {
			return true;
		}
		logError(HERE, " cannot write data --- readline ");
	}
	return false;
}

bool fluicell::PPC1api6::readData(std::string &_out_data)
{
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->flush();   // make sure that the buffer is clean
		if (m_PPC1_serial->readline(_out_data, 65536, "\n") > 0) {
			return true;
		}
		logError(HERE, " cannot read data --- readline ");
		return false;
	} // if the port is not open we cannot read data
	logError(HERE, " cannot read data --- port not open");
	return false;
}

bool fluicell::PPC1api6::checkVIDPID(const std::string &_port) const
{
	// try to get device information
	std::vector<serial::PortInfo> devices = serial::list_ports();
	std::vector<fluicell::PPC1api6dataStructures::serialDeviceInfo> devs;
	for (unsigned int i = 0; i < devices.size(); i++) // for all the connected devices extract information
	{
		fluicell::PPC1api6dataStructures::serialDeviceInfo dev;
		dev.port = devices.at(i).port;
		dev.description = devices.at(i).description;
		dev.VID = "N/A";
		dev.PID = "N/A";
		std::string hw_info = devices.at(i).hardware_id;
		std::string v = "VID"; 
		std::string p = "PID";
		// the fluicell PPC1 device expected string is USB\VID_16D0&PID_083A&REV_0200
		if (hw_info.length() < 1) {
			logError(HERE, " hardware info string length not correct ");
			return false; 
		}

		for (unsigned int j = 0; j < hw_info.size() - 2; j++)
		{
			// extract 3 characters looking for the strings VID or PID
			const std::string s = hw_info.substr(j, 3);
			if (s.compare(v) == 0 && hw_info.size() >= j + 4) { 
				// extract the 4 characters after VID_
				std::string vid = hw_info.substr(j + 4, 4); 
				dev.VID = vid;
			}
			if (s.compare(p) == 0 && hw_info.size() >= j+4 ) {
				// extract the 4 characters after PID_
				std::string pid = hw_info.substr(j + 4, 4);
				dev.PID = pid;
			}
		}
		devs.push_back(dev);
	}
	
	for (const auto& dev : devs)
		if (dev.port == _port && dev.VID == PPC1_VID && dev.PID == PPC1_PID)
			return true; 
	
	return false; 
	// TODO: test the previous cycle and remove the following 

	for (unsigned int i = 0; i < devs.size(); i++) // for all the connected devices 
		if (devs.at(i).port.compare(_port) == 0) // look for the device connected on _port
			if (devs.at(i).VID.compare(PPC1_VID) == 0) // check VID
				if (devs.at(i).PID.compare(PPC1_PID) == 0) // check PID
					return true; // if all success return true
	return false; // if only one on previous fails, return false VID/PID do not match
}

void fluicell::PPC1api6::logError(const std::string& _caller, const std::string& _message) const
{
	std::cerr << currentDateTime() << "  " << _caller << ": "
		<< " ---- error --- MESSAGE:" << _message << std::endl;
}

void fluicell::PPC1api6::logStatus(const std::string& _caller, const std::string& _message) const
{
	if (m_verbose)
		std::cout << currentDateTime() << "  " << _caller << ": "
			<< _message << std::endl;
}

std::string fluicell::PPC1api6::currentDateTime() const
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

double fluicell::PPC1api6::protocolDuration(
	std::vector<fluicell::PPC1api6dataStructures::command> &_protocol) const
{
	// compute the duration of the protocol
	double duration = 0.0;
	for (size_t i = 0; i < _protocol.size(); i++) {
		if (_protocol.at(i).getInstruction() ==
			fluicell::PPC1api6dataStructures::command::wait)
			duration += _protocol.at(i).getValue();
	}
	return duration;
}

fluicell::PPC1api6::~PPC1api6()
{
	// make sure the thread and the communications are properly closed
	if (m_threadTerminationHandler) {
		//this->stop();
	}
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->close();
	}

	// free memory
	delete m_PPC1_data;
	delete m_PPC1_status;
	delete m_PPC1_serial;
}
