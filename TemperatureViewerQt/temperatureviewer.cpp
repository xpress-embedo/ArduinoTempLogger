#include "temperatureviewer.h"
#include "./ui_temperatureviewer.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCore/QDateTime>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

/* Chart Related Objects and variables Starts */
QLineSeries *series;
QChartView *chartView;
QDateTimeAxis *axisX;
const qint8 SECONDS_SHOW_ON_GRAPH = 20;  // Display 120 seconds on the graph
static qint64 startTime;
/* Chart Related Objects and variables Finished */

TemperatureViewer::TemperatureViewer(QWidget *parent) : QMainWindow(parent) , ui(new Ui::TemperatureViewer)
{
  ui->setupUi(this);

  qDebug() << "Detecting Available Serial Ports";

  QList<QSerialPortInfo> serial_port_infos = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &port_info : serial_port_infos )
  {
    qDebug() << "Port:" << port_info.portName();
    // Add these found com ports to the combo box
    ui->cb_COMP->addItem(port_info.portName());
  }

  // Create a Line Series and the current time and value will be updated
  // when serial data is received
  series = new QLineSeries();
  // Test Code Starts
  /*
  QDateTime now = QDateTime::currentDateTime();
  series->append( now.toMSecsSinceEpoch(), 10 );
  now = now.addSecs(1);
  series->append( now.toMSecsSinceEpoch(), 11 );
  now = now.addSecs(1);
  series->append( now.toMSecsSinceEpoch(), 12 );
  now = now.addSecs(1);
  series->append( now.toMSecsSinceEpoch(), 13 );
  now = now.addSecs(1);
  series->append( now.toMSecsSinceEpoch(), 14 );
  */
  // Test Code Ends

  // Create a chart where we append this data
  QChart *chart = new QChart();
  chart->legend()->hide();
  chart->addSeries(series);
  // chart->createDefaultAxes();
  chart->setTitle("Real Time Temperature Visualization");

  // Formatting X Axis (made as global variable)
  // QDateTimeAxis *axisX = new QDateTimeAxis;
  axisX = new QDateTimeAxis;
  axisX->setTickCount(10);
  axisX->setFormat("hh:mm:ss");
  axisX->setTitleText("Time");
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);

  // Formatting Y-Axis
  QValueAxis *axisY = new QValueAxis;
  axisY->setLabelFormat("%i");
  axisY->setTitleText("Temperature Value");
  axisY->setRange( 0, 60 );
  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);

  // Setting the Chart View
  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setParent(ui->horizontalFrame);
}

TemperatureViewer::~TemperatureViewer()
{
  if( m_serial.isOpen() )
  {
    // if serial port is open close it
    m_serial.close();
  }
  delete ui;
}


void TemperatureViewer::on_btn_ConnectDisconnect_clicked( void )
{
  // if false, we have to connect, else disconnect
  if( connect_status == false )
  {
    qInfo() << "Connecting...";
    m_serial.setBaudRate( QSerialPort::Baud9600 );
    m_serial.setDataBits( QSerialPort::Data8 );
    m_serial.setParity( QSerialPort::NoParity );
    m_serial.setStopBits( QSerialPort::OneStop );
    m_serial.setFlowControl( QSerialPort::NoFlowControl );
    // Select the COM Port from Combo Box
    m_serial.setPortName( ui->cb_COMP->currentText() );;
    if( m_serial.open( QIODevice::ReadWrite ) )
    {
      qDebug() << "Serial Port Opened Successfully";
      m_serial.write("Hello World from Qt\r\n");
      connect_status = true;
      ui->btn_ConnectDisconnect->setText("Disconnect");
      // disable the combo box
      ui->cb_COMP->setEnabled(false);
      // Connect Signal and Slots
      connect(&m_serial, SIGNAL( readyRead() ), this, SLOT(read_data() ) );

      startTime = QDateTime::currentSecsSinceEpoch();
      // Setting Range of 120 seconds (need to take care of series also)
      QDateTime now = QDateTime::currentDateTime();
      axisX->setRange( now, now.addSecs(SECONDS_SHOW_ON_GRAPH) );
    }
    else
    {
      qDebug() << "Unable to open the Selected Serial Port" << m_serial.error();
    }
  }
  else
  {
    qInfo() << "Disconnecting...";
    // close the serial port
    m_serial.close();
    connect_status = false;
    ui->btn_ConnectDisconnect->setText("Connect");
    // Enable the combo box
    ui->cb_COMP->setEnabled(true);
  }
}

void TemperatureViewer::read_data()
{
  char data;
  float temp_value;
  QDateTime now;
  while( m_serial.bytesAvailable() )
  {
    // Read one byte at a time
    m_serial.read(&data, 1);
    if( (data != '\r') && (data != '\n') )
    {
      // convert adc counts back to the temperature value
      // temperature = (adc counts * VCC in mV/ADC Resolution)/10mV
      temp_value = (((uint8_t)data) * 500.0 / 1023.0);
      now = QDateTime::currentDateTime();
      series->append( now.toMSecsSinceEpoch(), temp_value );
      qDebug() << data << "," << temp_value;
      if( (now.toSecsSinceEpoch() - startTime) > SECONDS_SHOW_ON_GRAPH )
      {
        axisX->setMax( now );
        axisX->setMin( now.addSecs((-1)*SECONDS_SHOW_ON_GRAPH));
      }
      chartView->update();
    }
  }
}

