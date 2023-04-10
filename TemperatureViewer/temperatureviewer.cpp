#include "temperatureviewer.h"
#include "./ui_temperatureviewer.h"
#include <QDateTime>

const qint8 SECONDS_SHOW_ON_GRAPH = 20;  // Display 20 seconds on the graph
QList<double> time_axis;
QList<double> temperature_axis;
static qint64 temp_data_idx = 0;   // to be used later for x-axis range setting
static qint64 startTime;

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

  // Plotting Stuff
  // Create graph and assign data to it
  ui->customPlot->addGraph();
  // give axis some labels
  ui->customPlot->xAxis->setLabel("Time");
  ui->customPlot->yAxis->setLabel("Temperature");
  QColor color(40, 110, 255);
  ui->customPlot->graph(0)->setLineStyle( QCPGraph::lsLine );
  ui->customPlot->graph(0)->setPen( QPen(color.lighter(30)) );
  ui->customPlot->graph(0)->setBrush( QBrush(color) );

  // configure bottom axis to show date instead of number:
  QSharedPointer<QCPAxisTickerDateTime> date_time_ticker(new QCPAxisTickerDateTime);
  date_time_ticker->setDateTimeFormat("hh:mm:ss");
  ui->customPlot->xAxis->setTicker(date_time_ticker);

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
  ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

  // Start Timer to Refresh the graph
  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &TemperatureViewer::refreshGraph );
  // Start Timer @ 1 second
  timer->start(1000);
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
  double now;
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
      now = startTime;
      // set axes ranges, so we see all data:
      ui->customPlot->xAxis->setRange( now, now+SECONDS_SHOW_ON_GRAPH);
      ui->customPlot->yAxis->setRange(0, 60);
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
  double now = QDateTime::currentSecsSinceEpoch();

  while( m_serial.bytesAvailable() )
  {
    // Read one byte at a time
    m_serial.read(&data, 1);
    if( (data != '\r') && (data != '\n') )
    {
      // convert adc counts back to the temperature value
      // temperature = (adc counts * VCC in mV/ADC Resolution)/10mV
      temp_value = (data * 500.0 / 1023.0);
      time_axis.append(now);
      temperature_axis.append(temp_value);
      temp_data_idx++;
      qDebug() << temp_data_idx << temp_value;
    }
  }
}

void TemperatureViewer::refreshGraph( void )
{
  double now = QDateTime::currentSecsSinceEpoch();
  if( connect_status )
  {
    ui->customPlot->graph()->setData( time_axis, temperature_axis);
    // if time has elapsed then only start shifting the graph
    if( ((qint64)(now) - startTime) > SECONDS_SHOW_ON_GRAPH )
    {
      // If SECONDS_SHOW_GRAPH
      ui->customPlot->xAxis->setRange(now, SECONDS_SHOW_ON_GRAPH, Qt::AlignRight);
    }
    ui->customPlot->replot();
  }
}

