/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef BIOZONE_CONSOLE_H_
#define BIOZONE_CONSOLE_H_

#include "ui_BioZone_console.h"

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QtCore/QDebug>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


class BioZone_console;

/** This console has been implemented to try serial communication between
*   the Fluicell device and the BioZone application
* \note
*/
class BioZone_console : public  QDialog
{
	Q_OBJECT

		signals :
	void getData(const QByteArray &data);

public:
	explicit BioZone_console(QWidget *parent = nullptr);

	~BioZone_console();

	void setLocalEchoEnabled(bool set) { localEchoEnabled = set; }

	void exit();

	private slots:

	void connectCOM();

	void disconnectCOM();

	void showPortInfo(int idx);

	void handleError(QSerialPort::SerialPortError error);

	void readData();

	void sendData();

	void emumeratePorts();

protected: //TODO: maybe the events are not necessary
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;

private:
	bool localEchoEnabled;
	QSerialPort *serial;

protected:
	Ui::BioZone_console *ui_console;    //--> the user interface
};

#endif /* BIOZONE_CONSOLE_H_ */