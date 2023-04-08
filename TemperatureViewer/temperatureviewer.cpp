#include "temperatureviewer.h"
#include "./ui_temperatureviewer.h"
#include <QDateTime>

const qint8 DATA_SIZE = 120u;

QList<double> time_axis(DATA_SIZE);           // 120 values
QList<double> temperature_axis(DATA_SIZE);    // 120 values

QList<QCPGraphData> time_data(DATA_SIZE);
static qint8 temp_data_idx = 0;

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

  // Plotting stuff
  // Test Code Starts
  /*
  QList<double> x(101), y(101);     // initialize with entries 0...100
  for (int i=0; i<101; ++i)
  {
    x[i] = i/50.0 - 1; // x goes from -1 to 1
    y[i] = x[i]*x[i]; // let's plot a quadratic function
  }
  ui->customPlot->addGraph();
  ui->customPlot->graph(0)->setData(x, y);
  // give axis some labels
  ui->customPlot->xAxis->setLabel("x");
  ui->customPlot->yAxis->setLabel("y");
  // set axes ranges, so we see all data:
  ui->customPlot->xAxis->setRange(-1, 1);
  ui->customPlot->yAxis->setRange(0, 1);
  ui->customPlot->replot();
  */
  // Test Code Ends

  // Create graph and assign data to it
  ui->customPlot->addGraph();
  // give axis some labels
  ui->customPlot->xAxis->setLabel("Time");
  ui->customPlot->yAxis->setLabel("Temperature");
  // set axes ranges, so we see all data:
  ui->customPlot->xAxis->setRange( QDateTime::currentDateTime().toMSecsSinceEpoch(), \
                                   QDateTime::currentDateTime().addSecs(120).toMSecsSinceEpoch() );
  ui->customPlot->yAxis->setRange(0, 255);
  // configure bottom axis to show date instead of number:
  QSharedPointer<QCPAxisTickerDateTime> date_time_ticker(new QCPAxisTickerDateTime);
  date_time_ticker->setDateTimeFormat("hh:mm:ss");
  ui->customPlot->xAxis->setTicker(date_time_ticker);

  QColor color("pink");
  ui->customPlot->graph()->setLineStyle( QCPGraph::lsLine );
  ui->customPlot->graph()->setPen( QPen(color.lighter(200)) );
  ui->customPlot->graph()->setBrush( QBrush(color) );
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
  uint8_t temp_adc_count;
  QDateTime now;

  while( m_serial.bytesAvailable() )
  {
    // Read one byte at a time
    m_serial.read(&data, 1);
    if( (data != '\r') && (data != '\n') )
    {
      temp_adc_count = data;
      now = QDateTime::currentDateTime();
      // time_axis.append(now.toMSecsSinceEpoch());
      // temperature_axis.append(temp_adc_count);
      // ui->customPlot->graph()->setData( time_axis, temperature_axis);
      if( temp_data_idx >= DATA_SIZE )
      {
        temp_data_idx = 0;
      }
      time_data[temp_data_idx].key = QDateTime::currentDateTime().toMSecsSinceEpoch();
      time_data[temp_data_idx].value = temp_adc_count;
      temp_data_idx++;
      qDebug() << temp_adc_count;
      ui->customPlot->graph()->data()->set(time_data);

      ui->customPlot->replot();
      ui->customPlot->update();
    }
  }
}

