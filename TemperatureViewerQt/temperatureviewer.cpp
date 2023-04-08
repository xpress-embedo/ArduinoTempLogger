
#include "temperatureviewer.h"
#include "./ui_temperatureviewer.h"

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
  while( m_serial.bytesAvailable() )
  {
    // Read one byte at a time
    m_serial.read(&data, 1);
    if( (data != '\r') && (data != '\n') )
    {
      temp_adc_count = data;
      qDebug() << temp_adc_count;
    }
  }
}

