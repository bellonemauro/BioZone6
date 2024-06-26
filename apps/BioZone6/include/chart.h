﻿/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_chart_H_
#define Labonatip_chart_H_

// Qt
#include <QDateTime>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include "dataStructures.h"

// PPC1api test
#include <fluicell/ppc1api6/ppc1api6.h>

class protocolChart 
{
	// define a type for Fluicell protocol
	typedef std::vector<fluicell::PPC1api6dataStructures::command> f_protocol;

public:

	explicit protocolChart(const pipetteStatus* _ppc1_status);  //!>  Ctor
	
	~protocolChart();           //!> Dtor

	/** \brief Get the chart view 
	*
	*  The chart is created in this class and attached 
	*  using a QChartView in the main GUI, this function
	*  allow the GUI to access the constantly updated chart
	*
	*  \return: QtCharts::QChartView the chart view
	*/
	QChartView* getChartView() {
		return m_chartView;
	}

	/** \brief Set solution 1 color
	*
	*  This set of color allow the dynamic change
	*  of colors in the chart
	*
	*  \note: changes private data members
	*/
	void setSolutionColor1(QColor _col_sol) {
		*m_col_sol1 = _col_sol; 
		m_pen_s1->setColor(*m_col_sol1);
		m_area_solution1->setPen(*m_pen_s1);
		m_area_solution1->setColor(*m_col_sol1);
	
	}

	/** \brief Set solution 2 color
	*
	*  This set of color allow the dynamic change
	*  of colors in the chart
	*
	*  \note: changes private data members
	*/
	void setSolutionColor2(QColor _col_sol) {
		*m_col_sol2 = _col_sol;
		m_pen_s2->setColor(*m_col_sol2);
		m_area_solution2->setPen(*m_pen_s2);
		m_area_solution2->setColor(*m_col_sol2);
		}

	/** \brief Set solution 3 color
	*
	*  This set of color allow the dynamic change
	*  of colors in the chart
	*
	*  \note: changes private data members
	*/
	void setSolutionColor3(QColor _col_sol) { 
		*m_col_sol3 = _col_sol;
		m_pen_s3->setColor(*m_col_sol3); 
		m_area_solution3->setPen(*m_pen_s3);
		m_area_solution3->setColor(*m_col_sol3);
	}

	/** \brief Set solution 4 color
	*
	*  This set of color allow the dynamic change 
	*  of colors in the chart
	*
	*  \note: changes private data members
	*/
	void setSolutionColor4(QColor _col_sol) { 
		*m_col_sol4 = _col_sol; 
		m_pen_s4->setColor(*m_col_sol4); 
		m_area_solution4->setPen(*m_pen_s4);
		m_area_solution4->setColor(*m_col_sol4);
	}

	/** \brief Set solution 4 color
	*
	*  This set of color allow the dynamic change
	*  of colors in the chart
	*
	*  \note: changes private data members
	*/
	void setSolutionColor5(QColor _col_sol) {
		*m_col_sol5 = _col_sol;
		m_pen_s5->setColor(*m_col_sol5);
		m_area_solution5->setPen(*m_pen_s5);
		m_area_solution5->setColor(*m_col_sol5);
	}

	/** \brief Set solution 4 color
	*
	*  This set of color allow the dynamic change
	*  of colors in the chart
	*
	*  \note: changes private data members
	*/
	void setSolutionColor6(QColor _col_sol) {
		*m_col_sol6 = _col_sol;
		m_pen_s6->setColor(*m_col_sol6);
		m_area_solution6->setPen(*m_pen_s6);
		m_area_solution6->setColor(*m_col_sol6);
	}

	/** \brief Update the chart time line during the execution of a protocol
	*
	*/
	void updateChartTime(int _time_value);

	/** \brief Update the chart when a protocol is loaded
	*
	*   Interpret all the values in a protocol, including the 
	*   time line, to build an interactive chart based on the loaded protocol
	*
	*/
	void updateChartProtocol(f_protocol *_protocol);


private:
	
	/** \brief Append a new point to Pon serie
	*
	*   Append a new point to Pon serie
	*
	*
	*  \note: changes private data member m_series_pon
	*/
	void appendPonPoint(double _current_time, double _value);

	/** \brief Append a new point to Pof series
	*
	*   Append a new point to Poff series
	*
	*
	*  \note: changes private data member m_series_poff
	*/
	void appendPoffPoint(double _current_time, double _value);

	/** \brief Append a new point to Vrecirc series
	*
	*   Append a new point to Vrecirc series
	*
	*
	*  \note: changes private data member 
	*/
	void appendVrPoint(double _current_time, double _value);

	/** \brief Append a new point to Vswitch series
	*
	*   Append a new point to Vswitch series
	*
	*
	*  \note: changes private data member
	*/
	void appendVsPoint(double _current_time, double _value);

	/** \brief Append a new point to solutions series
	*
	*   Append a new point to solution series
	*
	*/
	void appendSolutionPoint(QLineSeries *_serie, double _current_time, double _value);
	
	const pipetteStatus* m_ppc1_status;  //!< pointer to the ppc1 status member in the main gui class

	// data members to build the chart, 
	// in this case private pointers to the data are preferred to have simple updates
	// the chart is made using series of points for each line and areas for solutions
	QLineSeries *m_series_Pon;         //!> Pon series 
	QLineSeries *m_series_Poff;        //!> Poff series 
	QLineSeries *m_series_V_switch;    //!> Vswitch series 
	QLineSeries *m_series_V_recirc;    //!> Vrecirc series 
	QLineSeries *m_series_solution1;   //!> Solution 1 series 
	QLineSeries *m_series_solution2;   //!> Solution 2 series 
	QLineSeries *m_series_solution3;   //!> Solution 3 series 
	QLineSeries *m_series_solution4;   //!> Solution 4 series //TODO: add here the other solutions
	QLineSeries* m_series_solution5;   //!> Solution 3 series
	QLineSeries* m_series_solution6;   //!> Solution 3 series
	QAreaSeries *m_area_solution1;     //!> Solution 1 area 
	QAreaSeries *m_area_solution2;     //!> Solution 2 area 
	QAreaSeries *m_area_solution3;     //!> Solution 3 area 
	QAreaSeries* m_area_solution4;     //!> Solution 4 area 
	QAreaSeries* m_area_solution5;     //!> Solution 4 area 
	QAreaSeries *m_area_solution6;     //!> Solution 4 area 
	QPen *m_pen_s1;        //!> Pen for solution 1, used to allow dynamic color change 
	QPen *m_pen_s2;        //!> Pen for solution 2, used to allow dynamic color change
	QPen *m_pen_s3;        //!> Pen for solution 3, used to allow dynamic color change
	QPen* m_pen_s4;        //!> Pen for solution 4, used to allow dynamic color change
	QPen* m_pen_s5;        //!> Pen for solution 4, used to allow dynamic color change   //TODO: the chart is not updated with the new solutions
	QPen *m_pen_s6;        //!> Pen for solution 4, used to allow dynamic color change
	QColor *m_col_sol1;    //!> Color for solution 1, used to allow dynamic color change
	QColor *m_col_sol2;    //!> Color for solution 2, used to allow dynamic color change
	QColor *m_col_sol3;    //!> Color for solution 3, used to allow dynamic color change
	QColor* m_col_sol4;    //!> Color for solution 4, used to allow dynamic color change
	QColor* m_col_sol5;    //!> Color for solution 4, used to allow dynamic color change  //TODO: the chart is not updated with the new solutions
	QColor *m_col_sol6;    //!> Color for solution 4, used to allow dynamic color change
	QLineSeries *m_series_solution;
	QLineSeries *m_series_ask;           //!> Ask series
	QLineSeries *m_series_sync_in;       //!> SyincIn series
	QLineSeries *m_series_sync_out;      //!> SyincOut series
	QLineSeries *m_time_line_b;          //!> Time line series (base)
	QLineSeries *m_time_line_t;          //!> Time line series (top)
	QAreaSeries *m_past_time_area;       //!> The time line is made using an area between top and base
	QChart *m_chart;                     //!> The chart
	QChartView *m_chartView;             //!> The chart view

	// this variables configure the chart in the advanced panel  
	// to show all the data lines in different levels
	const double chart_width;          //!> this is the height of section in the chart, 10 units reserve per line, = 10.0;
	const double min_series_pon;       //!> the Pon series will start from this level, = 80.0;
	const double min_series_poff;      //!> the Poff series will start from this level, = 70.0;
	const double min_series_V_recirc;  //!> the Vrecirc series will start from this level, = 60.0;
	const double min_series_V_switch;  //!> the Vswitch series will start from this level, = 50.0;
	const double min_series_solution;  //!> the Solution series will start from this level, = 40.0;
	const double min_series_ask;       //!> the Ask series will start from this level, = 30.0;
	const double min_series_sync_in;   //!> the SyncIn series will start from this level, = 20.0;
	const double min_series_sync_out;  //!> the SyncOut series will start from this level, = 10.0;
	
	const double max_pon;// = 450;
	const double max_poff;// = 450;
	const double max_v_recirc;// = 300;
	const double max_v_switch;// = 300;
	const double max_time_line;// = 100;
	const double min_time_line;// = 0.0;
	const double m_base_sol_value;    //!> Base value for the solution area drawing
	const double m_top_sol_value;     //!> Top value for the solution area drawing
	const double m_time_line_thickness;     //!> Top value for the solution area drawing
};


#endif /* Labonatip_chart_H_ */
