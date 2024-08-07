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
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QAbstractAxis>
#include <QDesktopServices>


BioZone6_GUI::BioZone6_GUI(QMainWindow *parent) :
	QMainWindow(parent),
	ui(new Ui::BioZone6_GUI),
	m_pipette_active(false),
	m_ppc1 ( new fluicell::PPC1api6() ),
	m_g_spacer ( new QGroupBox()),
	m_a_spacer (new QAction()),
	m_protocol ( new std::vector<fluicell::PPC1api6dataStructures::command> ),
	m_protocol_duration(0.0),
	m_running_protocol_file_name(""),
	m_current_protocol_time_status(0.0),
	m_pen_line_width(7),
	l_x1(-16.0),
	l_y1(49.0),
	l_x2(65.0),
	l_y2(l_y1),
	m_language_idx(0),
	m_base_time_step(1000), 
	m_flowing_solution(0),
	m_sol1_color(QColor::fromRgb(255, 189, 0)), // this can be any random color
	m_sol2_color(QColor::fromRgb(255, 40, 0)),
	m_sol3_color(QColor::fromRgb(0, 158, 255)),
	m_sol4_color(QColor::fromRgb(130, 255, 0)),
	qerr(NULL),
	qout(NULL),
	m_setting_folder_string("/settings/"),
	m_ext_data_folder_string("/Ext_data/"),
    m_internal_protocol_string("/internal/"),
    m_preset_protocols_string("/presetProtocols/"),
	m_buttonPRTfiles_SnR_path("/ButtonsPRTFiles/StandardAndRegular/"),
	m_buttonPRTfiles_SnS_path("/ButtonsPRTFiles/StandardAndSlow/"),
	m_buttonPRTfiles_LnR_path("/ButtonsPRTFiles/LargeAndRegular/"),
	m_buttonPRTfiles_LnS_path("/ButtonsPRTFiles/LargeAndSlow/")
{

  // allows to use path alias
  //QDir::setSearchPaths("icons", QStringList(QDir::currentPath() + "/icons/"));
  
  // setup the user interface
  // this is required to initialize all the objects in the GUI from creator
  ui->setupUi (this);

  // as the automatic translation is not precise, we have set the set of strings
  // used in this application to allow translation
  initCustomStrings();

  m_biozone_updater = new BioZone6_updater();
  
  // hide this field for now as the calculation is wrong
  ui->textEdit_emptyTime_waste->hide();

  // initialize the tools as we need the settings  // TODO: waste of performances all these objects are created twice
  m_dialog_tools = new BioZone6_tools(this); 
  m_dialog_tools->setExtDataPath(m_ext_data_path);
  m_pipette_status = new pipetteStatus();
  m_comSettings = new COMSettings();
  m_solutionParams = new solutionsParams();
  m_pr_params = new pr_params();
  m_GUI_params = new GUIparams();
  *m_comSettings = m_dialog_tools->getComSettings();
  *m_solutionParams = m_dialog_tools->getSolutionsParams();
  *m_pr_params = m_dialog_tools->getPr_params();
  *m_GUI_params = m_dialog_tools->getGUIparams();

  //close the advanced dock page
  ui->dockWidget->close();  
  // put the tab widget to the chart page
  ui->tabWidget->setCurrentIndex(1);  
  ui->treeWidget_flowInfo->resizeColumnToContents(0);
  //ui->treeWidget_macroInfo->setColumnWidth(0, 200);
  ui->treeWidget_flowInfo->setHeaderHidden(false);

  // init the redirect buffer for messages
  qerr = new QDebugStream(std::cerr, ui->textEdit_qcerr);
  qout = new QDebugStream(std::cout, ui->textEdit_qcout);
  this->setRedirect(m_GUI_params->enableHistory);
  qerr->setVerbose(m_pr_params->verboseOut);
  qout->setVerbose(m_pr_params->verboseOut);

  // this removes the visualization settings (but it will be shown in debug)
#ifndef _DEBUG
  ui->tabWidget->removeTab(4);
  ui->tabWidget->removeTab(3);
  //ui->dockWidget->setMinimumWidth(180);
#endif

  // make sure that we start from the default window
  ui->stackedWidget_main->setCurrentIndex(0);
  ui->stackedWidget_indock->setCurrentIndex(0);

  // set the flows in the table
  ui->treeWidget_flowInfo->topLevelItem(12)->setText(1,
	  QString::number(m_pipette_status->rem_vol_well1));
  ui->treeWidget_flowInfo->topLevelItem(13)->setText(1,
	  QString::number(m_pipette_status->rem_vol_well2));
  ui->treeWidget_flowInfo->topLevelItem(14)->setText(1, 
	  QString::number(m_pipette_status->rem_vol_well3));
  ui->treeWidget_flowInfo->topLevelItem(15)->setText(1, 
	  QString::number(m_pipette_status->rem_vol_well4));
 
  // hide the warning label (it will be shown if there is a warning)
  ui->label_warning->hide();
  ui->label_warningIcon->hide();

  // move the arrow in the drawing to point at the solution 1
  ui->widget_solutionArrow->setVisible(false);
  ui->label_arrowSolution->setText(m_solutionParams->sol1);
   
  // set the scene for the graphic depiction of the solution flow
  m_scene_solution = new QGraphicsScene;
  {
	  // set the scene rectangle to avoid the graphic area to move
	  float s_x = 0.0;   // scene x-coordinate
	  float s_y = 0.0;   // y-coordinate
	  float s_w = 40.0;  // scene width
	  float s_h = 100.0; // height
	  m_scene_solution->setSceneRect(s_x, s_y, s_w, s_h);
  }
  m_pen_line.setColor(Qt::transparent);
  m_pen_line.setWidth(m_pen_line_width);
 
  // initialize PPC1api
  m_ppc1->setCOMport(m_comSettings->getName());
  m_ppc1->setBaudRate((int)m_comSettings->getBaudRate());
  m_ppc1->setVerbose(m_GUI_params->verboseOutput);
  m_ppc1->setFilterSize(m_pr_params->filterSize);
  m_ppc1->setTip(fluicell::PPC1api6dataStructures::tip::Standard);

  // init thread macroRunner 
  m_macroRunner_thread = new BioZone6_protocolRunner(this);
  m_macroRunner_thread->setDevice(m_ppc1);

  // status bar led
  status_bar_led = new QFled(ui->status_PPC1_led, QFled::ColorType::red);
  pon_bar_led = new QFled(ui->label_led_pon, QFled::ColorType::grey);
  poff_bar_led = new QFled(ui->label_led_poff, QFled::ColorType::grey);
  vr_bar_led = new QFled(ui->label_led_vr, QFled::ColorType::grey);
  vs_bar_led = new QFled(ui->label_led_vs, QFled::ColorType::grey);
  this->setStatusLed(false);

  ui->status_PPC1_label->setText(m_str_PPC1_status_discon);
  ui->label_macroStatus->setText(m_str_protocol_not_running);

  // init the timers 
  m_update_flowing_sliders = new QTimer();
  m_update_GUI = new QTimer();
  m_update_waste = new QTimer();
  m_waste_remainder = new QTimer();
  m_check_updates = new QTimer();
  m_timer_solution = 0;
  

  m_update_flowing_sliders->setInterval(m_base_time_step);
  m_update_GUI->setInterval(10);// (m_base_time_step);
  m_update_waste->setInterval(m_base_time_step);
  m_waste_remainder->setInterval(300 * m_base_time_step);
  m_check_updates->setInterval(5000);

  
  connect(m_update_flowing_sliders,
	  SIGNAL(timeout()), this,
	  SLOT(updateTimingSliders()));

  connect(m_update_GUI,
	  SIGNAL(timeout()), this,
	  SLOT(updateGUI()));
  m_update_GUI->start();

  connect(m_update_waste,
	  SIGNAL(timeout()), this,
	  SLOT(updateWaste()));
  m_update_waste->start();

  connect(m_waste_remainder,
	  SIGNAL(timeout()), this,
	  SLOT(emptyWasteRemainder()));

  connect(m_check_updates,
	  SIGNAL(timeout()), this,
	  SLOT(automaticCheckForUpdates()));
  m_check_updates->start();
  
  // reset the protocol table widget
  ui->treeWidget_macroTable->setColumnWidth(editorParams::c_idx, 70);
  ui->treeWidget_macroTable->setColumnWidth(editorParams::c_command, 240);
  ui->treeWidget_macroTable->setColumnWidth(editorParams::c_range, 160);
  ui->treeWidget_macroTable->setColumnWidth(editorParams::c_value, 100);

  ui->tabWidget_editor->setCurrentIndex(0);
  m_last_treeWidget_editor_idx = 0;
  new XmlSyntaxHighlighter(ui->textBrowser_XMLcode->document());
  ui->tabWidget_editor_advanced->setCurrentIndex(0);
   
  // set delegates
  m_combo_delegate = new ComboBoxDelegate();
  m_no_edit_delegate = new NoEditDelegate();
  m_no_edit_delegate2 = new NoEditDelegate();
  m_spinbox_delegate = new SpinBoxDelegate();
  ui->treeWidget_macroTable->setItemDelegateForColumn(
	  editorParams::c_idx, new NoEditDelegate(this));
  ui->treeWidget_macroTable->setItemDelegateForColumn(
	  editorParams::c_command, new ComboBoxDelegate(this));
  ui->treeWidget_macroTable->setItemDelegateForColumn(
	  editorParams::c_range, new NoEditDelegate(this));
  ui->treeWidget_macroTable->setItemDelegateForColumn(
	  editorParams::c_value, new SpinBoxDelegate(this));
 
  ui->treeWidget_macroTable->setContextMenuPolicy(
	  Qt::CustomContextMenu);
  ui->treeWidget_protocol_folder->setContextMenuPolicy(
	  Qt::CustomContextMenu);

  ui->treeWidget_params->resizeColumnToContents(0);
  ui->treeWidget_params->setHeaderHidden(false);

  // the undo/redo stack
  m_undo_stack = new QUndoStack(this);
  // the undo/redo stack can be visualized in a viewer (only for debug purposes)
  m_undo_view = new QUndoView(m_undo_stack);
  m_undo_view->setWindowTitle(tr("Command List"));
  m_undo_view->window()->setMinimumSize(300, 300);
  m_undo_view->setAttribute(Qt::WA_QuitOnClose, false);
#ifdef  _DEBUG
  this->showUndoStack();
#endif // ! _DEBUG
  
  //simulation button not-activated by default
  ui->actionSimulation->setChecked(false);
  m_simulationOnly = ui->actionSimulation->isChecked();
  ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
  ui->actionReboot->setEnabled(!m_simulationOnly);
  ui->actionShudown->setEnabled(!m_simulationOnly);
  ui->actionReboot->setEnabled(false);
  ui->actionShudown->setEnabled(false);
  ui->groupBox_action->setEnabled(false);
  ui->groupBox_operMode->setEnabled(false);
  ui->groupBox_3->setEnabled(false);
  ui->groupBox_PonOM->setEnabled(false);
  enableTab2(false);

  //init the chart view
  m_chart_view = new protocolChart(m_pipette_status);
  m_chartView = m_chart_view->getChartView();
  ui->gridLayout_12->addWidget(m_chartView);


  // set the user name into the GUI
  QString s;
  s.append(m_str_user);
  s.append(" ");
  s.append(m_dialog_tools->getUserName());
  ui->label_user->setText(s);

//  m_labonatip_chart_view->setGUIchart();

  // refill solutions and waste according to the loaded settings
  refillSolution();
  emptyWells();

  // set translation
  QString translation_file = "./languages/eng.qm";
  if (!m_translator.load(translation_file))
	  std::cout << HERE << " translation not loaded" << std::endl;
  else
	  std::cout << HERE << " Translation loaded " << std::endl;

  qApp->installTranslator(&m_translator);
  this->switchLanguage(m_GUI_params->language);

  ui->textEdit_emptyTime->hide();


  //speech synthesis
  m_speech = new QTextToSpeech(this);

  // set a few shortcuts
  ui->pushButton_solution1->setShortcut(
	  QApplication::translate("BioZone6_GUI", "F1", Q_NULLPTR));
  ui->pushButton_solution2->setShortcut(
	  QApplication::translate("BioZone6_GUI", "F2", Q_NULLPTR));
  ui->pushButton_solution3->setShortcut(
	  QApplication::translate("BioZone6_GUI", "F3", Q_NULLPTR));
  ui->pushButton_solution4->setShortcut(
	  QApplication::translate("BioZone6_GUI", "F4", Q_NULLPTR));
  ui->pushButton_solution5->setShortcut(
	  QApplication::translate("BioZone6_GUI", "F5", Q_NULLPTR)); 
  ui->pushButton_solution6->setShortcut(
	  QApplication::translate("BioZone6_GUI", "F6", Q_NULLPTR)); 
  ui->pushButton_undo->setShortcut(
	  QApplication::translate("BioZone6_GUI", "Ctrl+Z", Q_NULLPTR));
  ui->pushButton_redo->setShortcut(
	  QApplication::translate("BioZone6_GUI", "Ctrl+Y", Q_NULLPTR));
  ui->pushButton_removeMacroCommand->setShortcut(
	  QApplication::translate("BioZone6_GUI", "Del", Q_NULLPTR));

  // install the event filter on -everything- in the app
  qApp->installEventFilter(this);

  toolApply(); // this is to be sure that the settings are brought into the app at startup

  QColor c = QColor::fromRgba(0xAAAAAAAA);
  this->colSolution1Changed(c.red(), c.green(), c.blue());
  this->colSolution2Changed(c.red(), c.green(), c.blue());
  this->colSolution3Changed(c.red(), c.green(), c.blue());
  this->colSolution4Changed(c.red(), c.green(), c.blue());
  this->colSolution5Changed(c.red(), c.green(), c.blue());
  this->colSolution6Changed(c.red(), c.green(), c.blue());

  // all the connects to signal/slots are in this function
  initConnects();

  if (m_GUI_params->restrictOPmode == true || m_GUI_params->useIONoptixLogo == true)
  {
	  ui->actionAbout->setIcon(QPixmap(":/icons/IONoptix_fluicell_logo.png"));
  }


  std::cout << HERE << m_dialog_tools->getUserName().toStdString() << std::endl;

  ui->pushButton_standardAndRegular->setChecked(true);

}

void BioZone6_GUI::automaticCheckForUpdates()
{
	m_check_updates->stop();

	connect(m_biozone_updater,
		&BioZone6_updater::updateAvailable, this,
		&BioZone6_GUI::handleUpdateAvailable);

	if (m_GUI_params->automaticUpdates_idx != 0)// 0 = no updates, 1 = notify all updates, 2 = notify release only
	{
		// check for updates on startup
		if (m_GUI_params->automaticUpdates_idx == 1) {
			m_biozone_updater->setNotifyExperimental(true);
		}
		else {
			m_biozone_updater->setNotifyExperimental(false);
		}
		m_biozone_updater->setVersion(m_version);
		m_biozone_updater->isUpdateAvailable();
		//checkForUpdates();
	}
}

void BioZone6_GUI::handleUpdateAvailable()
{

	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, m_str_information, 
			m_str_update_information, 
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes) {
		checkForUpdates();
	}
}


void BioZone6_GUI::askMessage(const QString &_message) 
{
	// if the speech is active, the message will be read
	if (m_GUI_params->speechActive)  m_speech->say(_message);
	
	//QMessageBox::information(this, m_str_ask_msg, _message, m_str_ok);
	QMessageBox mb = QMessageBox(QMessageBox::Information,
		m_str_ask_msg, _message, QMessageBox::Ok);
	mb.exec();
	// an event is sent upon dialog close
	m_macroRunner_thread->askOkEvent(true);
	
	std::cout << HERE << " " << _message.toStdString() << std::endl;
}


void BioZone6_GUI::pumpingOff() {

	std::cout << HERE << std::endl;

	if (m_pipette_active) {
		m_ppc1->pumpingOff();
	}

	updatePonSetPoint(0.0);
	updatePoffSetPoint(0.0);
	updateVrecircSetPoint(0.0);
	updateVswitchSetPoint(0.0);

	//this will stop the solution flow 
	m_timer_solution = std::numeric_limits<int>::max();
	updateDrawing(-1);
}


void BioZone6_GUI::closeAllValves() {

	std::cout << HERE << std::endl;

	if (m_pipette_active) {
		if (m_ppc1->isConnected()) m_ppc1->closeAllValves();
	}

	//this will stop the solution flow 
	m_timer_solution = std::numeric_limits<int>::max();
	updateDrawing(-1);
}


void BioZone6_GUI::setEnableSolutionButtons(bool _enable ) {
	ui->pushButton_solution1->setEnabled(_enable);
	ui->pushButton_solution2->setEnabled(_enable);
	ui->pushButton_solution3->setEnabled(_enable);
	ui->pushButton_solution4->setEnabled(_enable);
	ui->pushButton_solution5->setEnabled(_enable);
	ui->pushButton_solution6->setEnabled(_enable);
}


void BioZone6_GUI::setRedirect(bool _enable)
{
	// qerr and cout must be already initialized
	// the pointer are set to NULL by default in ctor
	if (qerr == NULL) 
		return;
	if (qout == NULL)
		return;

	// errors will go always to GUI 
	qerr->redirectOutInGUI(true);

#ifndef _DEBUG
	// if we are not in debug 
	//we redirect normal messages to the GUI according to the settings
	qout->redirectOutInGUI(_enable);
	// and normal messages will not go to the terminal
	qout->copyOutToTerminal(false);
	// errors will not go the the terminal (we have no terminal window)
	qerr->copyOutToTerminal(true);
#else
	// otherwise, we are in debug hence 
	// all messages will go to the terminal
	qerr->copyOutToTerminal(true);
	qout->copyOutToTerminal(true);

	// out messages will go to GUI according to the settings
	qout->redirectOutInGUI(_enable);
#endif
}

void BioZone6_GUI::switchLanguage(int _value )
{
	std::cout << HERE << std::endl;
	
	if (_value == m_language_idx) 
		return; // no translation needed

	// this will re-init custom strings to English for re-translation
	qApp->removeTranslator(&m_translator);
	QString translation_file;
	m_language_idx = _value;
	switch (_value)
	{ 
	case GUIparams::Chinese: 
	{
		translation_file = ":/languages/cn.qm";	
		break;
	}
	case GUIparams::English:
	{
		translation_file = ":/languages/eng.qm";
		break;
	}
	case GUIparams::Italiano:
	{
		translation_file = ":/languages/ita.qm";
		break;
	}
	case GUIparams::Svenska:
	{
		translation_file = ":/languages/sve.qm";
		break;
	}
	default: 
		translation_file = ":/languages/eng.qm"; 
		qApp->removeTranslator(&m_translator);
		break;
	}

	if (m_translator.load(translation_file)) {

		// translate app
		qApp->installTranslator(&m_translator);

		initCustomStrings();
	
		// translate other dialogs and objects
		m_dialog_tools->switchLanguage(translation_file);
		m_macroRunner_thread->switchLanguage(translation_file);
		m_biozone_updater->switchLanguage(translation_file);
	}
	else std::cout << HERE << " translation not loaded " << std::endl;

}


void BioZone6_GUI::changeEvent(QEvent* _event)
{
	if (0 != _event) {
		switch (_event->type()) {
			// this event is send if a translator is loaded
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		case QEvent::Resize:
			this->resizeToolbar();
			break;
		case QEvent::WindowStateChange:
			this->resizeToolbar();
			break;
		break;
		}
	}

	QMainWindow::changeEvent(_event);
}

bool BioZone6_GUI::eventFilter(QObject *_obj, QEvent *_event)
{
	// activate/deactivate tool tips http://www.qtcentre.org/threads/11056-enable-all-Tooltips
	// http://doc.qt.io/qt-4.8/qobject.html#installEventFilter

	if (_event->type() == QEvent::ToolTip) {
		
		if (!m_GUI_params->enableToolTips) { 
			return true; //this filter the event
		}
		// standard event processing
		return QObject::eventFilter(_obj, _event);
	}
	// standard event processing
	return QObject::eventFilter(_obj, _event);

}

void BioZone6_GUI::setStatusLed( bool _connect ) 
{
	if (ui->actionConnectDisconnect->isChecked()) {
		ui->status_PPC1_led->clear();
		if (_connect) {
			//ui->status_PPC1_led->setPixmap(*led_green);
			status_bar_led->setColor(QFled::ColorType::green);
		}
		else {
			//ui->status_PPC1_led->setPixmap(*led_red);
			status_bar_led->setColor(QFled::ColorType::red);
		}
	}
}

void BioZone6_GUI::initConnects()
{
	//windows toolbar
	connect(ui->actionTools, 
		SIGNAL(triggered()), this, 
		SLOT(showToolsDialog()));
	
	connect(ui->actionEditor,
		SIGNAL(triggered()), this,
		SLOT(showProtocolEditor()));

	connect(ui->actionLoad_profile, 
		SIGNAL(triggered()), this, 
		SLOT(loadPressed()));

	connect(ui->actionSave_profile,
		SIGNAL(triggered()), this, 
		SLOT(savePressed()));

	connect(ui->actionAbout,
		SIGNAL(triggered()), this, 
		SLOT(about()));

	connect(ui->actionAdvanced, 
		SIGNAL(triggered()), this, 
		SLOT(closeOpenDockTools()));

	connect(ui->actionConnectDisconnect, 
		SIGNAL(triggered(bool)), this,
		SLOT(disCon(bool)));

	connect(ui->actionSimulation,
		SIGNAL(triggered()), this, 
		SLOT(simulationOnly()));

	connect(ui->actionReboot, 
		SIGNAL(triggered()), this, 
		SLOT(reboot()));
	
	connect(ui->actionShudown,
		SIGNAL(triggered()), this, 
		SLOT(shutdown()));

//	connect(ui->actionWhatsthis, 
//		SIGNAL(triggered()), this, 
//		SLOT(ewst()));

	connect(ui->pushButton_p_on_down, 
		SIGNAL(clicked()), this, 
		SLOT(pressurePonDown()));

	connect(ui->pushButton_p_on_up, 
		SIGNAL(clicked()), this, 
		SLOT(pressurePonUp()));

	connect(ui->pushButton_p_off_down, 
		SIGNAL(clicked()), this, 
		SLOT(pressurePoffDown()));

	connect(ui->pushButton_p_off_up, 
		SIGNAL(clicked()), this, 
		SLOT(pressurePoffUp()));

	connect(ui->pushButton_switchDown, 
		SIGNAL(clicked()), this, 
		SLOT(pressButtonPressed_switchDown()));

	connect(ui->pushButton_switchUp, 
		SIGNAL(clicked()), this, 
		SLOT(pressButtonPressed_switchUp()));

	connect(ui->pushButton_recirculationDown, 
		SIGNAL(clicked()), this, 
		SLOT(recirculationDown()));

	connect(ui->pushButton_recirculationUp, 
		SIGNAL(clicked()), this, 
		SLOT(recirculationUp()));

	connect(ui->pushButton_set_preset1,
		SIGNAL(clicked()), this,
		SLOT(setPreset1()));

	connect(ui->pushButton_set_preset2,
		SIGNAL(clicked()), this,
		SLOT(setPreset2()));

	connect(ui->pushButton_set_preset3,
		SIGNAL(clicked()), this,
		SLOT(setPreset3()));

	connect(ui->pushButton_reset_preset1,
		SIGNAL(clicked()), this,
		SLOT(resetPreset1()));

	connect(ui->pushButton_reset_preset2,
		SIGNAL(clicked()), this,
		SLOT(resetPreset2()));

	connect(ui->pushButton_reset_preset3,
		SIGNAL(clicked()), this,
		SLOT(resetPreset3()));

	connect(ui->pushButton_solution1, 
		SIGNAL(clicked()), this, 
		SLOT(onPushButtonSolution1()));

	connect(ui->pushButton_solution2, 
		SIGNAL(clicked()), this, 
		SLOT(onPushButtonSolution2()));

	connect(ui->pushButton_solution3, 
		SIGNAL(clicked()), this, 
		SLOT(onPushButtonSolution3()));

	connect(ui->pushButton_solution4, 
		SIGNAL(clicked()), this, 
		SLOT(onPushButtonSolution4()));

	connect(ui->pushButton_solution5,
		SIGNAL(clicked()), this,
		SLOT(onPushButtonSolution5()));

	connect(ui->pushButton_solution6,
		SIGNAL(clicked()), this,
		SLOT(onPushButtonSolution6()));

	connect(ui->pushButton_standby, 
		SIGNAL(clicked()), this, 
		SLOT(standby()));

	connect(ui->pushButton_standardAndSlow,
		SIGNAL(clicked()), this,
		SLOT(setStandardAndSlow()));

	connect(ui->pushButton_largeAndSlow,
		SIGNAL(clicked()), this,
		SLOT(setLargeAndSlow()));

	connect(ui->pushButton_standardAndRegular,
		SIGNAL(clicked()), this,
		SLOT(setStandardAndRegular())); 

	connect(ui->pushButton_largeAndRegular,
		SIGNAL(clicked()), this,
		SLOT(setLargeAndRegular()));

	connect(ui->pushButton_plusPonOM,
		SIGNAL(clicked()), this,
		SLOT(onPlusPonOM()));
	
	connect(ui->pushButton_minusPonOM,
		SIGNAL(clicked()), this,
		SLOT(onMinusPonOM()));

	connect(ui->pushButton_stop, 
		SIGNAL(clicked()), this, 
		SLOT(stopFlow()));

	connect(ui->pushButton_operational, 
		SIGNAL(clicked()), this, 
		SLOT(operationalMode()));

	connect(ui->pushButton_runMacro, 
		SIGNAL(clicked()), this, 
		SLOT(runProtocol()));

	connect(ui->pushButton_newTip, 
		SIGNAL(clicked()), this, 
		SLOT(newTip()));

	connect(ui->pushButton_cleanHistory,
		SIGNAL(clicked()), this,
		SLOT(cleanHistory()));

	connect(ui->pushButton_setFlowCalcParams,
		SIGNAL(clicked()), this,
		SLOT(setFlowParams()));

	// connect sliders
	connect(ui->horizontalSlider_p_on, 
		SIGNAL(valueChanged(int)), this, 
		SLOT(sliderPonChanged(int)));

	connect(ui->horizontalSlider_p_off, 
		SIGNAL(valueChanged(int)), this, 
		SLOT(sliderPoffChanged(int)));

	connect(ui->horizontalSlider_recirculation, 
		SIGNAL(valueChanged(int)), this, 
		SLOT(sliderRecircChanged(int)));

	connect(ui->horizontalSlider_switch, 
		SIGNAL(valueChanged(int)), this, 
		SLOT(sliderSwitchChanged(int)));
	
	connect(m_biozone_updater,
		SIGNAL(exit()), this,
		SLOT(closeSoftware())); 

	connect(m_dialog_tools,
		SIGNAL(emptyWaste()), this,
		SLOT(emptyWells()));

	connect(m_dialog_tools,
		SIGNAL(refillSolution()), this,
		SLOT(refillSolution()));

	connect(m_dialog_tools, 
		SIGNAL(apply()), this, 
		SLOT(toolApply()));

	connect(m_dialog_tools,
		SIGNAL(ok()), this,
		SLOT(toolOk()));
	
	connect(m_dialog_tools,
		SIGNAL(checkUpdatesNow()), this,
		SLOT(checkForUpdates()));

	connect(m_dialog_tools,
		&BioZone6_tools::TTLsignal, this,
		&BioZone6_GUI::testTTL);

	connect(m_dialog_tools,
		&BioZone6_tools::colSol1Changed, this,
		&BioZone6_GUI::colSolution1Changed);

	connect(m_dialog_tools,
		&BioZone6_tools::colSol2Changed, this,
		&BioZone6_GUI::colSolution2Changed);

	connect(m_dialog_tools,
		&BioZone6_tools::colSol3Changed, this,
		&BioZone6_GUI::colSolution3Changed);

	connect(m_dialog_tools,
		&BioZone6_tools::colSol4Changed, this,
		&BioZone6_GUI::colSolution4Changed);

	connect(m_dialog_tools,
		&BioZone6_tools::colSol5Changed, this,
		&BioZone6_GUI::colSolution5Changed);

	connect(m_dialog_tools,
		&BioZone6_tools::colSol6Changed, this,
		&BioZone6_GUI::colSolution6Changed);

	connect(ui->treeWidget_protocol_folder,
		SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		this, SLOT(onProtocolClicked(QTreeWidgetItem*, int)));

	//connect(ui->pushButton_openFolder,
	//	SIGNAL(clicked()), this, SLOT(openProtocolFolder())); //TODO: openProtocolFolder MUST BE SET BACK BEFORE ACTIVATING IT

	connect(ui->treeWidget_protocol_folder,
		SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(protocolsMenu(const QPoint&)));

	connect(ui->tabWidget_editor,
		SIGNAL(currentChanged(int)),
		this, SLOT(onTabEditorChanged(int)));

	//ui->pushButton_undo->setEnabled(false); // TODO: disabled for now as it does not work
	connect(ui->pushButton_undo,
		SIGNAL(clicked()), this, SLOT(undo()));

	//ui->pushButton_redo->setEnabled(false); // TODO: disabled for now as it does not work
	connect(ui->pushButton_redo,
		SIGNAL(clicked()), this, SLOT(redo()));

	connect(ui->treeWidget_macroTable,
		SIGNAL(itemChanged(QTreeWidgetItem *, int)), this,
		SLOT(itemChanged(QTreeWidgetItem *, int)));

	connect(ui->pushButton_addMacroCommand,
		SIGNAL(clicked()), this, SLOT(addCommand()));


	connect(ui->pushButton_removeMacroCommand,
		SIGNAL(clicked()), this, SLOT(removeCommand()));

	connect(ui->pushButton_moveDown,
		SIGNAL(clicked()), this, SLOT(moveDown()));

	connect(ui->pushButton_moveUp,
		SIGNAL(clicked()), this, SLOT(moveUp()));

	//connect(ui->pushButton_plusIndent,
	//	SIGNAL(clicked()), this, SLOT(plusIndent()));

	connect(ui->pushButton_duplicateLine,
		SIGNAL(clicked()), this, SLOT(duplicateItem()));

	connect(ui->pushButton_clearCommands,
		SIGNAL(clicked()), this, SLOT(clearAllCommandsRequest()));

	connect(ui->pushButton_loop,
		SIGNAL(clicked()), this, SLOT(createNewLoop()));

}

void BioZone6_GUI::setFlowParams()
{

	double lenght_to_tip = ui->doubleSpinBox_lTp->value();
	double lenght_to_zone = ui->doubleSpinBox_lTz->value();
	double pipe_diameter = ui->doubleSpinBox_pd->value();
	double dynamic_viscosity = ui->doubleSpinBox_dv->value();
	double square_channel_mod = ui->doubleSpinBox_scm->value();

	m_ppc1->setLengthToTip(lenght_to_tip);
	m_ppc1->setLengthToZone(lenght_to_zone);
	m_ppc1->setPipeDiameter(pipe_diameter);
	m_ppc1->setDynamicViscosity(dynamic_viscosity);
	m_ppc1->setSquareChannelMod(square_channel_mod);

}

void BioZone6_GUI::testTTL(bool _state) {

	std::cout << HERE << "  " << _state << std::endl;

	if (m_ppc1->isConnected())
	{
		m_ppc1->setTTLstate(_state);
	}
	else
	{
		QMessageBox::warning(this, m_str_warning, m_str_TTL_failed);
	}
}

void BioZone6_GUI::initCustomStrings()
{
	//setting custom strings to translate 
	m_str_areyousure = tr("Are you sure?");
	m_str_waiting = tr("Waiting ...");
	m_str_advanced = tr("Advanced");
	m_str_basic = tr("Basic");
	m_str_operation_cancelled = tr("Operation cancelled");
	m_str_no_file_loaded = tr("No file loaded");
	m_str_xml_file_in_old_format = tr("This protocol file is saved using a deprecated format, \nplease check that it was read correctly and save the protocol to adapt to the new format");
	m_str_information = tr("Information");
	m_str_warning = tr("Warning");
	m_str_error = tr("Error");
	m_str_cancel = tr("Cancel");
	m_str_ok = tr("Ok");
	m_str_h = tr("h");
	m_str_min = tr("min");
	m_str_s = tr("s");
	m_str_sec = tr("sec");
	m_str_save = tr("Save");
	m_str_save_profile = tr("Save profile");
	m_str_load = tr("Load");
	m_str_load_profile = tr("Load profile");
	m_str_commander = tr("Commander");
	m_str_editor = tr("Editor");
	m_str_PPC1_status_con = tr("PPC1 STATUS: Connected  ");
	m_str_PPC1_status_unstable_con = tr("PPC1 STATUS: UNSTABLE Connection  ");
	m_str_PPC1_status_discon = tr("PPC1 STATUS: NOT Connected  ");
	m_str_protocol_running = tr("Cancel");
	m_str_protocol_not_running = tr("No protocol running  ");
	m_str_connect = tr("Connect");
	m_str_disconnect = tr("Disconnect");
	m_str_save_profile = tr("Save profile");
	m_str_load_profile = tr("Load profile");
	m_str_cannot_save_profile = tr("Cannot save the file");
	m_str_cannot_load_profile = tr("Cannot load the file");
	m_str_warning_simulation_only = tr("The program is in simulation mode");
	m_str_cannot_connect_ppc1 = tr("Unable to connect to PPC1");
	m_str_cannot_connect_ppc1_twice = tr("Unable to connect to PPC1 twice");
	m_str_cannot_connect_ppc1_check_cables = tr("Please, check cables and settings and press Ok");
	m_str_question_find_device = tr("Automatically identify the connected device?");
	m_str_ppc1_connected_but_not_running = tr("PPC1 is connected but not running");
	m_str_question_stop_ppc1 = tr("This will stop the PPC1");
	m_str_unable_stop_ppc1 = tr("Unable to stop and disconnect PPC1");
	m_str_shutdown_pressed = tr("Shutdown pressed, this will take 30 seconds, press Ok to continue, cancel to abort");
	m_str_shutdown_pressed_p_off = tr("The pressure is off, waiting for the vacuum");
	m_str_shutdown_pressed_v_off = tr("Vacuum off, stopping the flow in the device");
	m_str_rebooting = tr("Rebooting ...");
	m_str_reconnecting = tr("Reconnecting ...");
	m_str_initialization = tr("Initialization");
	m_str_newtip_msg1 = tr("Place the pipette into the holder and tighten. THEN PRESS OK");
	m_str_newtip_msg2 = tr("Pressurize the system");
	m_str_newtip_msg3 = tr("Wait until a droplet appears at the tip of the pipette and THEN PRESS OK");
	m_str_newtip_msg4 = tr("Purging the liquid channels");
	m_str_newtip_msg5 = tr("Still purging the liquid channels");
	m_str_newtip_msg6 = tr("Remove the droplet using a lens tissue. THEN PRESS OK");
	m_str_newtip_msg7 = tr("Put the pipette into solution. THEN PRESS OK");
	m_str_newtip_msg8 = tr("Purging the vacuum channels");
	m_str_newtip_msg9 = tr("Establishing operational pressures");
	m_str_newtip_msg10 = tr("Pipette is ready for operation. PRESS OK TO START");
	m_str_stop_1 = tr("Pumps are off, depressuring ...");
	m_str_stop_2 = tr("Depressuring ...");
	m_str_standby_operation = tr("Standby operation progress");
	m_str_label_run_protocol = tr("Run protocol");
	m_str_label_stop_protocol = tr("Stop protocol");
	m_str_no_protocol_load_first = tr("No protocol loaded, load one first");
	m_str_loaded_protocol_is = tr("The protocol loaded is : ");
	m_str_protocol_confirm = tr("Press Ok to run the protocol, or press Cancel to load a new one");
	m_str_progress_msg1 = tr("This operation will take ");
	m_str_progress_msg2 = tr(" seconds");
	m_str_ask_msg = tr("Ask message command");
	m_str_editor_apply_msg1 = tr(" No protocol loaded : ");
	m_str_editor_apply_msg2 = tr(" Protocol loaded : ");
	m_str_cleaning_history_msg1 = tr("This will remove all the files in the history folder");
	m_str_cleaning_history_msg2 = tr("History cleaned");
	m_str_update_time_macro_msg1 = tr(" PROTOCOL RUNNING : ");
	m_str_update_time_macro_msg2 = tr(" ----- remaining time,  ");
	m_str_pulse_remaining_time = tr("Pulse time remaining:");
	m_str_pulse_continuous_flowing = tr("Continuous flowing");
	m_str_pulse_waste = tr("Waste");
	m_str_pulse_full_in = tr("full in");
	m_str_operation_cannot_be_done = tr("Operation cannot be done");
	m_str_out_of_bound = tr("Please, check for out of bound values");
	m_str_user = tr("User :");
	m_str_protocol_running_stop = tr("A protocol is running, stop the protocol first");
	m_str_lost_connection = tr("Lost connection with PPC1");
	m_str_swapping_to_simulation = tr("swapping to simulation mode");
	m_str_warning_solution_end = tr("Warning: the solution is running out");
	m_str_warning_waste_full = tr("Warning: the waste is full");
	m_str_save_protocol = tr("Save profile");
	m_str_select_folder = tr("Select folder");
	m_str_file_not_saved = tr("File not saved");
	m_str_file_not_loaded = tr("File not loaded");
	m_str_protocol_duration = tr("Protocol duration : ");
	m_str_remove_file = tr("This action will remove the file, are you sure?");
	m_str_current_prot_name = tr("The current protocol file name is");
	m_str_question_override = tr("Do you want to overwrite?");
	m_str_override_guide = tr("Yes = overwrite, NO = save As, Cancel = do nothing");
	m_str_add_protocol_bottom = tr("Do you want to add to the bottom of the protocol?");
	m_str_add_protocol_bottom_guide = tr("Click NO to clean the workspace and load a new protocol");
	m_str_clear_commands = tr("This will clear all items in the current protocol");
	m_str_solution_ended = tr("Solution ended, the flow was stopped");
	m_str_waste_full = tr("A waste well is full and needs to be emptied. Click Ok to continue");// ("The waste is full, click yes to empty");
	m_str_TTL_failed = tr("PPC1 not connected, TTL test cannot run");
	m_str_update_information = tr("A new update is available, do you want to download it now?");
	m_ask_password = tr("This is for expert users only, a password is required");
	m_wrong_password = tr("Wrong password, file not saved");
	m_correct_password = tr("Correct password, file saved");
	m_new_settings_applied = tr("New settings applied, <br> please click operational to activate the pumps");
}

// the appScaling is still to be done
void BioZone6_GUI::appScaling(int _dpiX, int _dpiY)
{
	QSize toolbar_icon_size = ui->toolBar->iconSize();
	toolbar_icon_size.scale(toolbar_icon_size*_dpiX/100, Qt::KeepAspectRatioByExpanding);
	ui->toolBar->setIconSize(toolbar_icon_size);

	toolbar_icon_size = ui->toolBar_2->iconSize();
	toolbar_icon_size.scale(toolbar_icon_size*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->toolBar_2->setIconSize(toolbar_icon_size);

	toolbar_icon_size = ui->toolBar_3->iconSize();
	toolbar_icon_size.scale(toolbar_icon_size*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->toolBar_3->setIconSize(toolbar_icon_size);

	QSize gr_b_action = ui->groupBox_action->minimumSize();
	gr_b_action.scale(gr_b_action*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->groupBox_action->setMinimumSize(gr_b_action);

	QSize gr_5 = ui->groupBox_5->minimumSize();
	gr_5.scale(gr_5*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->groupBox_5->setMinimumSize(gr_5);

	//QSize gr_chart = ui->groupBox_chart->minimumSize();
	//gr_chart.scale(gr_chart*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	//ui->groupBox_chart->setMinimumSize(gr_chart);

	QSize action_button_size = ui->pushButton_newTip->minimumSize();
	action_button_size.scale(action_button_size*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->pushButton_newTip->setMinimumSize(action_button_size);
	ui->pushButton_runMacro->setMinimumSize(action_button_size);
	ui->pushButton_operational->setMinimumSize(action_button_size);
	ui->pushButton_stop->setMinimumSize(action_button_size);
	ui->pushButton_standby->setMinimumSize(action_button_size);

	QSize action_button_icon_size = ui->pushButton_newTip->iconSize();
	action_button_icon_size.scale(action_button_icon_size*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->pushButton_newTip->setIconSize(action_button_icon_size);
	ui->pushButton_runMacro->setIconSize(action_button_icon_size);
	ui->pushButton_operational->setIconSize(action_button_icon_size);
	ui->pushButton_stop->setIconSize(action_button_icon_size);
	ui->pushButton_standby->setIconSize(action_button_icon_size);

	
}

void BioZone6_GUI::emptyWells()
{
	std::cout << HERE << std::endl;

	// empty the wells 
	m_pipette_status->rem_vol_well7 = 0.0;
	m_pipette_status->rem_vol_well8 = 0.0;

	// remove the warnings
	ui->label_warningIcon->hide();
	ui->label_warning->hide();

	// if the waste remainder is active, stop it
	if (m_waste_remainder->isActive()) 
		m_waste_remainder->stop();	
}

void BioZone6_GUI::refillSolution()
{
	std::cout << HERE << std::endl;

	// get the latest settings from the tools
	*m_solutionParams = m_dialog_tools->getSolutionsParams();

	// reset the wells
	m_pipette_status->rem_vol_well1 = m_solutionParams->vol_well1;
	m_pipette_status->rem_vol_well2 = m_solutionParams->vol_well2;
	m_pipette_status->rem_vol_well3 = m_solutionParams->vol_well3;
	m_pipette_status->rem_vol_well4 = m_solutionParams->vol_well4;
	m_pipette_status->rem_vol_well5 = m_solutionParams->vol_well5;
	m_pipette_status->rem_vol_well6 = m_solutionParams->vol_well6;

	// remove the warnings
	ui->label_warningIcon->hide();
	ui->label_warning->hide();

	// update wells volume. 
	// The max volume is constant, 
	// hence the visualization of the percentage is calculated according to the max volume
	m_pipette_status->rem_vol_well1 = m_pipette_status->rem_vol_well1 - 
		0.001 * m_pipette_status->flow_well1;
	double perc = 100.0 * m_pipette_status->rem_vol_well1 / MAX_VOLUME_IN_WELL;
	ui->progressBar_solution1->setValue(int(perc));

	m_pipette_status->rem_vol_well2 = m_pipette_status->rem_vol_well2 -
		0.001 * m_pipette_status->flow_well2;
	perc = 100.0 * m_pipette_status->rem_vol_well2 / MAX_VOLUME_IN_WELL;
	ui->progressBar_solution2->setValue(int(perc));

	m_pipette_status->rem_vol_well3 = m_pipette_status->rem_vol_well3 -
		0.001 * m_pipette_status->flow_well3;
	perc = 100.0 * m_pipette_status->rem_vol_well3 / MAX_VOLUME_IN_WELL;
	ui->progressBar_solution3->setValue(int(perc));

	m_pipette_status->rem_vol_well4 = m_pipette_status->rem_vol_well4 -
		0.001 * m_pipette_status->flow_well4;
	perc = 100.0 * m_pipette_status->rem_vol_well4 / MAX_VOLUME_IN_WELL;
	ui->progressBar_solution4->setValue(int(perc));

	m_pipette_status->rem_vol_well5 = m_pipette_status->rem_vol_well5 -
		0.001 * m_pipette_status->flow_well5;
	perc = 100.0 * m_pipette_status->rem_vol_well5 / MAX_VOLUME_IN_WELL;
	ui->progressBar_solution5->setValue(int(perc));

	m_pipette_status->rem_vol_well6 = m_pipette_status->rem_vol_well6 -
		0.001 * m_pipette_status->flow_well6;
	perc = 100.0 * m_pipette_status->rem_vol_well6 / MAX_VOLUME_IN_WELL;
	ui->progressBar_solution6->setValue(int(perc));

}

void BioZone6_GUI::toolApply()
{
	std::cout << HERE << std::endl;

	*m_comSettings = m_dialog_tools->getComSettings();
	*m_solutionParams = m_dialog_tools->getSolutionsParams();
	*m_pr_params = m_dialog_tools->getPr_params();
	*m_GUI_params = m_dialog_tools->getGUIparams();
	m_ppc1->setTip(m_dialog_tools->getTipType());

	//if (m_dialog_tools->isExpertMode()) 
	{
		fluicell::PPC1api6dataStructures::tip my_tip = m_dialog_tools->getTip();
		m_ppc1->setTipParameters(my_tip.length_to_tip, my_tip.length_to_zone);
	}

	//setUserFilesPath(m_base_biozone_path);

	m_ppc1->setCOMport(m_comSettings->getName());
	m_ppc1->setBaudRate((int)m_comSettings->getBaudRate());
	m_ppc1->setFilterEnabled(m_pr_params->enableFilter);
	m_ppc1->setFilterSize(m_pr_params->filterSize);
	m_ppc1->setVerbose(m_pr_params->verboseOut);
	qerr->setVerbose(m_pr_params->verboseOut);
	qout->setVerbose(m_pr_params->verboseOut);
	m_ext_data_path = m_GUI_params->outFilePath;
	this->setRedirect(m_GUI_params->enableHistory);

	ui->treeWidget_params->topLevelItem(0)->setText(1, m_solutionParams->sol1);
	ui->treeWidget_params->topLevelItem(1)->setText(1, m_solutionParams->sol2);
	ui->treeWidget_params->topLevelItem(2)->setText(1, m_solutionParams->sol3);
	ui->treeWidget_params->topLevelItem(3)->setText(1, m_solutionParams->sol4);
	ui->treeWidget_params->topLevelItem(4)->setText(1, m_solutionParams->sol5);
	ui->treeWidget_params->topLevelItem(5)->setText(1, m_solutionParams->sol6);

	ui->treeWidget_params->topLevelItem(6)->setText(1, QString::number(m_pr_params->p_on_default));
	ui->treeWidget_params->topLevelItem(7)->setText(1, QString::number(m_pr_params->p_off_default));
	ui->treeWidget_params->topLevelItem(8)->setText(1, QString::number(m_pr_params->v_recirc_default));
	ui->treeWidget_params->topLevelItem(9)->setText(1, QString::number(m_pr_params->v_switch_default));

	this->switchLanguage(m_GUI_params->language);

	QString s;
	s.append(m_str_user);
	s.append(" ");
	s.append(m_dialog_tools->getUserName());
	ui->label_user->setText(s);

	// TODO: code replication, fix this
	m_tip_selection_string = QString::fromStdString(m_ppc1->getTip()->tip_setting_path);  // /tips/Standard/   	
	m_ext_data_path = m_base_biozone_path + m_ext_data_folder_string;

	m_protocols_path = m_base_biozone_path + m_tip_selection_string + m_preset_protocols_string;
	m_internal_protocol_path = m_protocols_path + m_internal_protocol_string;
	m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_SnR_path;

	if (ui->pushButton_standardAndRegular->isChecked()) {
		m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_SnR_path;
		//this->setStandardAndRegular();
	}
	else 
		if (ui->pushButton_standardAndSlow->isChecked()) {
			m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_SnS_path;
			//this->setStandardAndSlow();
		}
		else 
			if (ui->pushButton_largeAndRegular->isChecked()) {
				m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_LnR_path;
				//this->setLargeAndRegular();
			}
			else 
				if (ui->pushButton_largeAndSlow->isChecked()) {
					m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_LnS_path;
					//this->setLargeAndSlow();
				}

	
	if (m_GUI_params->restrictOPmode == true)
	{
		ui->actionAbout->setIcon(QPixmap(":/icons/IONoptix_fluicell_logo.png"));
		ui->groupBox_operMode->setEnabled(false);
		ui->pushButton_standardAndRegular->setChecked(true);
	}
	else
	{
		ui->actionAbout->setIcon(QPixmap(":/icons/fluicell_logo_BIG.png"));

		if(ui->actionSimulation->isChecked() || ui->actionConnectDisconnect->isChecked())
			ui->groupBox_operMode->setEnabled(true);
	}


	if (m_GUI_params->useIONoptixLogo == true)
		ui->actionAbout->setIcon(QPixmap(":/icons/IONoptix_fluicell_logo.png"));
	else
		ui->actionAbout->setIcon(QPixmap(":/icons/fluicell_logo_BIG.png"));

}

void BioZone6_GUI::toolOk()
{
	bool verbose = false; 
	if (ui->pushButton_standardAndRegular->isChecked())
	{
		this->setStandardAndRegular(verbose);
	}
	if (ui->pushButton_standardAndSlow->isChecked())
	{
		this->setStandardAndSlow(verbose);
	}
	if (ui->pushButton_largeAndRegular->isChecked())
	{
		this->setLargeAndRegular(verbose);
	}
	if (ui->pushButton_largeAndSlow->isChecked())
	{
		this->setLargeAndSlow(verbose);
	}

}

void BioZone6_GUI::setEnableMainWindow(bool _enable) {

	ui->centralwidget->setEnabled(_enable);
	ui->dockWidget->setEnabled(_enable);
	ui->toolBar->setEnabled(_enable);
	ui->toolBar_2->setEnabled(_enable);
	ui->toolBar_3->setEnabled(_enable);
}

bool BioZone6_GUI::visualizeProgressMessage(int _seconds, QString _message)
{
	std::cout << HERE << "   " << _message.toStdString() << std::endl;

	QString msg = _message;
	msg.append("<br>");
	msg.append(m_str_progress_msg1);
	msg.append(QString::number(_seconds));
	msg.append(m_str_progress_msg2);

	//if (m_GUI_params->speechActive)  m_speech->say(_message);

	//this will make the windows more reactive
	// so the loop will take x(sec) / acceleration_time
	int updates_per_second = 5; 

	QProgressDialog *PD = new QProgressDialog(msg, m_str_cancel, 0, 
		updates_per_second*_seconds, this);
	PD->setMinimumWidth(350);   // here there is a warning that the geometry cannot be set, forget about it!
	PD->setMinimumHeight(150);
	PD->setMaximumWidth(700);
	PD->setMaximumHeight(300);
	PD->setValue(0);
	PD->setMinimumDuration(0); // Change the Minimum Duration before displaying from 4 sec. to 0 sec. 
	PD->show(); // Make sure that the dialog is displayed immediately
	PD->setValue(1); 
	PD->setWindowModality(Qt::WindowModal);
	//PD->setCancelButtonText(m_str_cancel);// (QApplication::translate("BioZone6_GUI", "Cancel", Q_NULLPTR));

	// To have more precise timing I have implemented the code 
    // available here https://stackoverflow.com/questions/21232520/precise-interval-in-qthread
	// the currentMSecsSinceEpoch function allows to keep track of the time absolutely not relatively
	int time_step = 1000 / updates_per_second; // 1000 ms = 1 sec
	qint64 current_time = QDateTime::currentMSecsSinceEpoch();
	for (int i = 0; i < updates_per_second*_seconds; i++) {
		PD->setValue(i);
		//int elapsed_time = tt.elapsed();
		//QThread::msleep(wait_time - elapsed_time);
		//tt.restart();
		if (PD->wasCanceled()) // the operation cannot be cancelled
		{
			QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
			QMessageBox::information(this, m_str_warning, m_str_operation_cancelled );
			setEnableMainWindow(true);
			delete PD;
			return false;
		}
		current_time += time_step;
		qint64 sleep_for = current_time - QDateTime::currentMSecsSinceEpoch();
		if (sleep_for < 0) {
			// We got prompted for too long - for all we know, the system could
			// have even gotten suspended (lid close on a laptop).
			// Note: We should avoid the implementation-defined behavior of 
			// modulus (%) for negative values.
			sleep_for = time_step - ((-sleep_for) % time_step);
		}
		// use the appropriate API on given platform for sleep
		QThread::msleep(sleep_for);
	}
	PD->cancel();
	delete PD;
	return true;
}

void BioZone6_GUI::ewst() {

	std::cout << HERE << std::endl;
	QWhatsThis::enterWhatsThisMode();

}

void BioZone6_GUI::cleanHistory()
{
	std::cout << HERE << std::endl;

	// question, will you delete history?
	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, m_str_warning,
			QString(m_str_cleaning_history_msg1 + "\n" + m_str_areyousure),
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
	//if is not yes, the operation is cancelled
		//QMessageBox::information(this, m_str_information, m_str_operation_cancelled, m_str_ok);
		QMessageBox mb = QMessageBox(QMessageBox::Information,
			m_str_information, m_str_operation_cancelled, QMessageBox::Ok);
		mb.exec();
		return;
	}

	// otherwise, delete files
	//read history folder
	QDir dir(m_ext_data_path);
	// list all the file names
	dir.setNameFilters(QStringList() << "*.txt");
	dir.setFilter(QDir::Files);
	// remove the files one by one
	foreach(QString dirFile, dir.entryList())
	{
		dir.remove(dirFile);
	}
	// confirm message
	//QMessageBox::information(this, m_str_information, m_str_cleaning_history_msg2, m_str_ok);
	QMessageBox mb = QMessageBox(QMessageBox::Information,
		m_str_information, m_str_cleaning_history_msg2, QMessageBox::Ok);
	mb.exec();

}
 
void BioZone6_GUI::about() {

	std::cout << HERE << std::endl;

	QMessageBox messageBox;
	QString msg_title = "About Fluicell BioZone6 ";
	QString msg_content = tr("BioZone6 is part of the <br> Fluicell Lab-on-a-tip technology family,<br>"
		"Copyrighted Sweden 2024.<br><br>"
		"BioZone6®, Fluicell®, Lab-on-a-tip® <br>are all registered trademarks of Fluicell AB, Sweden <br> <br>"
		"Flöjelbergsgatan 8C<br>"
		"SE-431 37 Mölndal, Sweden<br>"
		"Tel: +46 76 208 3354 <br>"
		"e-mail: info@fluicell.com <br><br>"
		"Developer: Mauro Bellone <br>"
		"Version: ");
	msg_content.append(m_version);
	messageBox.setText(msg_content);
	messageBox.setWindowTitle(msg_title);
	QPixmap pic(":/icons/fluicell_iconBIG.png");
	pic = pic.scaled(50, 50, Qt::KeepAspectRatio);
	messageBox.setIconPixmap(pic);
	messageBox.setWindowIcon(QPixmap(":/icons/fluicell_iconBIG.ico"));
	messageBox.setFixedSize(200, 300);

	QAbstractButton* pButtonQG = messageBox.addButton(tr("Open quick guide"), QMessageBox::YesRole);
	messageBox.addButton(m_str_ok, QMessageBox::NoRole);

	messageBox.exec();

	if (messageBox.clickedButton() == pButtonQG) {
		//Execute command
		QString fileName = QDir::currentPath() + "./guide/Biozone6 - Setup guide - FBZ001.pdf";
		QDesktopServices::openUrl(QUrl("file:///" + fileName));
	}

	return;
}

void BioZone6_GUI::checkForUpdates() {

	m_biozone_updater->setVersion(m_version);
	m_biozone_updater->setParent(this);
	m_biozone_updater->setWindowFlags(Qt::Window);
	m_biozone_updater->show();
}

void BioZone6_GUI::enableTab2(bool _enable)
{
	ui->pushButton_p_on_down->setEnabled(_enable);
	ui->horizontalSlider_p_on->setEnabled(_enable);
	ui->pushButton_p_on_up->setEnabled(_enable);
		
	ui->pushButton_p_off_down->setEnabled(_enable);
	ui->horizontalSlider_p_off->setEnabled(_enable);
	ui->pushButton_p_off_up->setEnabled(_enable);

	ui->pushButton_switchDown->setEnabled(_enable);
	ui->horizontalSlider_switch->setEnabled(_enable);
	ui->pushButton_switchUp->setEnabled(_enable);

	ui->pushButton_recirculationDown->setEnabled(_enable);
	ui->horizontalSlider_recirculation->setEnabled(_enable);
	ui->pushButton_recirculationUp->setEnabled(_enable);

	ui->pushButton_set_preset1->setEnabled(_enable);
	ui->pushButton_reset_preset1->setEnabled(_enable);

	ui->pushButton_set_preset2->setEnabled(_enable);
	ui->pushButton_reset_preset2->setEnabled(_enable);

	ui->pushButton_set_preset3->setEnabled(_enable);
	ui->pushButton_reset_preset3->setEnabled(_enable);
}



void BioZone6_GUI::closeEvent(QCloseEvent *event) {

	std::cout << HERE << std::endl;


	QMessageBox msgBox;
	msgBox.setText(m_str_areyousure);
	msgBox.setWindowTitle(m_str_information);
	msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
	msgBox.setDefaultButton(QMessageBox::Yes);
	msgBox.setIcon(QMessageBox::Question);
	int resBtn = msgBox.exec();

	//QMessageBox::StandardButton resBtn	=
	//	QMessageBox::question(this, m_str_information, m_str_areyousure,
	//	QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
	//	QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		event->ignore();
		return; 
	}

	if (m_macroRunner_thread->isRunning()) {
		//this->runProtocol(); // this will stop the protocol if running
			
		QMessageBox mb = QMessageBox(QMessageBox::Question, 
			m_str_information, m_str_protocol_running_stop, QMessageBox::Ok);
		mb.exec();

		event->ignore();
		return;
	}

	// dump log file
	if (m_GUI_params->dumpHistoryToFile)
	{
		// save log data, messages from the console ect. 
		dumpLogs();
	}

	if (m_ppc1->isConnected()) {
			
		m_ppc1->stop();
		m_ppc1->disconnectCOM(); //if is active, disconnect
	}

	// this is to make sure that the buttons are unchecked on close event
	// thus the default values are back to normal and the setting file is properly written
	ui->pushButton_standardAndSlow->setChecked(false);
	this->setStandardAndSlow();
	ui->pushButton_standardAndRegular->setChecked(false);
	this->setStandardAndRegular();
	ui->pushButton_largeAndRegular->setChecked(false);
	this->setLargeAndRegular();
	ui->pushButton_largeAndSlow->setChecked(false);
	this->setLargeAndSlow();

	delete m_dialog_tools;
	event->accept();
	
}

void BioZone6_GUI::dumpLogs()
{
	std::cout << HERE 
		<< " - version " << m_version.toStdString() << std::endl;

	// save log data, messages from the console ect. 
	QString cout_file_name = m_ext_data_path;
	cout_file_name.append("/Cout_");
	cout_file_name.append(QDate::currentDate().toString());
	cout_file_name.append("_");
	cout_file_name.append(QString::number(QTime::currentTime().hour()));
	cout_file_name.append("_");
	cout_file_name.append(QString::number(QTime::currentTime().minute()));
	cout_file_name.append("_");
	cout_file_name.append(QString::number(QTime::currentTime().second()));
	cout_file_name.append(".txt");

	QFile coutfile;
	coutfile.setFileName(cout_file_name);
	coutfile.open(QIODevice::Append | QIODevice::Text);
	QTextStream c_out(&coutfile);
    c_out << ui->textEdit_qcout->toPlainText() << Qt::endl;

	// save log data, messages from the console ect. 
	QString Err_file_name = m_ext_data_path;
	Err_file_name.append("/Err_");
	Err_file_name.append(QDate::currentDate().toString());
	Err_file_name.append("_");
	Err_file_name.append(QString::number(QTime::currentTime().hour()));
	Err_file_name.append("_");
	Err_file_name.append(QString::number(QTime::currentTime().minute()));
	Err_file_name.append("_");
	Err_file_name.append(QString::number(QTime::currentTime().second()));
	Err_file_name.append(".txt");

	QFile cerrfile;
	cerrfile.setFileName(Err_file_name);
	cerrfile.open(QIODevice::Append | QIODevice::Text);
	QTextStream c_err(&cerrfile);
    c_err << ui->textEdit_qcerr->toPlainText() << Qt::endl;

}

void BioZone6_GUI::setUserSettingPath(QString _path)
{
	m_settings_path = _path;
	m_dialog_tools->setLoadSettingsFileName(m_settings_path);
	if (m_dialog_tools->getGUIparams().isFirstRun)
	{
		m_dialog_tools->setFirstRun(false);
		QMessageBox::StandardButton resBtn =
			QMessageBox::question(this, m_str_warning,
				QString("BioZone is running for the first time, would you like to load the configuration for ionOptix?"),
				 QMessageBox::No | QMessageBox::Yes,
				QMessageBox::Yes);
		if (resBtn == QMessageBox::Yes) {
			QString fullIonOptixSettingFilePath = m_base_biozone_path + m_setting_folder_string + "settings_IonOptix.ini";
			m_dialog_tools->setLoadSettingsFileName(fullIonOptixSettingFilePath);
		}
		m_dialog_tools->saveSettings(m_settings_path);
	}
}

void BioZone6_GUI::setUserFilesPath(QString _path)
{
	_path = QDir::cleanPath(_path);

	// here we expect to know where the user has placed its files, 
	// by default is /user/documents/BioZone6 so we set that folder
	// then we have different settings for each tip, 
	//   /user/documents/BioZone6/tips/__TIP_TYPE___/presetProtocols/___OPERATIONAL_MODE____/internal
	//   /user/documents/BioZone6/tips/__TIP_TYPE___/settings/
	m_base_biozone_path = _path;
	m_tip_selection_string = QString::fromStdString(m_ppc1->getTip()->tip_setting_path);  // /tips/Standard/   	
	//m_settings_path = _path + m_tip_selection_string + m_settings_string;
	//m_settings_path = _path + m_settings_string;
	

	m_ext_data_path = m_base_biozone_path + m_ext_data_folder_string;

	m_protocols_path = m_base_biozone_path + m_tip_selection_string + m_preset_protocols_string;
	m_internal_protocol_path = m_protocols_path + m_internal_protocol_string;
	m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_SnR_path;

	if(ui->pushButton_standardAndRegular->isChecked())
		m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_SnR_path;
	if (ui->pushButton_standardAndSlow->isChecked())
		m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_SnS_path;
	if (ui->pushButton_largeAndRegular->isChecked())
		m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_LnR_path;
	if (ui->pushButton_largeAndSlow->isChecked())
		m_operational_mode_protocol_path = m_internal_protocol_path + m_buttonPRTfiles_LnS_path;

	this->readProtocolFolder(m_protocols_path);  // look for files in the folder
	m_dialog_tools->setSettingsFolderPath(m_base_biozone_path + m_setting_folder_string);
}


void BioZone6_GUI::setVersion(std::string _version) {
	// set the version in the main window
	m_version = QString::fromStdString(_version);
	this->setWindowTitle(QString("BioZone v.") + m_version);
}

void BioZone6_GUI::closeSoftware()
{
	if (m_macroRunner_thread->isRunning()) {
		//this->runProtocol(); // this will stop the macro if running
		//QMessageBox::question(this, m_str_information, m_str_protocol_running_stop, m_str_ok);
		QMessageBox mb = QMessageBox(QMessageBox::Question,
			m_str_information, m_str_protocol_running_stop, QMessageBox::Ok);
		mb.exec();

		return;
	}
	// dump log file
	if (m_GUI_params->dumpHistoryToFile)
	{
		// save log data, messages from the console ect. 
		dumpLogs();
	}
	if (m_ppc1->isConnected()) {

		m_ppc1->stop();
		m_ppc1->disconnectCOM(); //if is active, disconnect
	}

	delete m_dialog_tools;
	qApp->quit();
}


bool BioZone6_GUI::saveXml()
{
	QString file_name =
		QFileDialog::getSaveFileName(this, tr("Save Protocol File"),
			QDir::currentPath(),
			tr("XBEL Files (*.prt *.xml)"));
	if (!file_name.isEmpty())
		return saveXml(file_name, ui->treeWidget_macroTable);

	return false;
}

bool BioZone6_GUI::saveXml(QString _filename, QTreeWidget* _widget)
{
	QFile file(_filename);
	if (_filename.contains("stopSolution", Qt::CaseSensitive) ||
		_filename.contains("pumpSolution", Qt::CaseSensitive))
	{
		// Ask for the password
		bool ok;
		QString text = QInputDialog::getText(0, m_str_warning,
			m_ask_password, QLineEdit::Password,
			"", &ok);
		if (ok && !text.isEmpty()) {
			QString password = text;
			QString password_check = "FluicellGrowth2018";
			if (!password.compare(password_check))
			{
				QMessageBox::information(this, m_str_information, m_correct_password);
				// continue out of the if statement
			}
			else
			{
				QMessageBox::warning(this, m_str_information, m_wrong_password);
				return false;
			}
		}
	}

	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, m_str_warning,
			m_str_file_not_saved + tr("<br>%1:\n%2.")
			.arg(QDir::toNativeSeparators(_filename),
				file.errorString()));
		return false;
	}
	XmlProtocolWriter writer(_widget);
	if (writer.writeFile(&file))
	{
		return true;
	}

	return false;
}

bool BioZone6_GUI::openXml()  
{
	QString fileName =
		QFileDialog::getOpenFileName(this, tr("Open  File"),
			QDir::currentPath(),
			tr("prt Files (*.prt *.xml)"));
	if (fileName.isEmpty())
		return false;

	//ui->treeWidget_macroTable->clear();

	return openXml(fileName, ui->treeWidget_macroTable);
}


bool BioZone6_GUI::openXml(QString _filename, QTreeWidget* _widget)
{

	XmlProtocolReader reader(_widget);
	//if (!reader.read(&file, dynamic_cast<protocolTreeWidgetItem*> (
	//	ui->treeWidget_macroTable->currentItem()))) 
	QFile file(_filename);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		std::cerr << HERE << " impossible to open the file " << std::endl;
		return false;
	}
	
	if (!reader.read(&file, 0))// dynamic_cast<protocolTreeWidgetItem*>(_widget->currentItem())))
	{
		QMessageBox::warning(this, m_str_warning,
			m_str_no_file_loaded + tr("<br>Is this a xml file protocol? <br>%1:\n%2.")
			.arg(QDir::toNativeSeparators(_filename),
				reader.errorString()));
		return true;
	}

	
	if (reader.isOldProtocolAttribute())
	{
	QMessageBox::warning(this, m_str_warning,
		m_str_xml_file_in_old_format);
	}

	return false;
}



BioZone6_GUI::~BioZone6_GUI()
{
  delete qout;
  delete qerr;
  delete m_comSettings;
  delete m_pr_params;
  delete m_GUI_params;
  delete m_pipette_status;
  delete m_protocol; 
  delete m_ppc1;
  delete m_macroRunner_thread;
  delete m_update_flowing_sliders;
  delete m_update_GUI;
  delete m_update_waste;
  delete m_scene_solution;
  delete status_bar_led;
  delete pon_bar_led;
  delete poff_bar_led;
  delete vs_bar_led;
  delete vr_bar_led;
  delete m_g_spacer;
  delete m_a_spacer; 
  delete m_chart_view;

  delete m_combo_delegate;
  delete m_no_edit_delegate;
  delete m_no_edit_delegate2;
  delete m_spinbox_delegate;
  delete m_biozone_updater;
  delete ui;
  qApp->quit();
}
