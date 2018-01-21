from datetime import datetime
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

year = 2018

month = input ('Enter Month : ')
date = input ('Enter Date : ')
month = int(month)
date = int(date)

# Hardcoded Values for Testing Purpose
# month = 1
# date = 20

headers = ['Time', 'ADC', 'Temperature']
filename = '%.2d-%.2d-%.2d.txt' % (month, date, year-2000)
# print (filename)

try:
    df = pd.read_table( filename, ',', names=headers,\
                        engine='python', header=None)
except:
    print ('No Such File in Database')
    print ('Exiting Program')
    exit()

FMT = '%H:%M:%S'
# print (df['Time'].head())
# print (type(df['Time'][0]))
df['Time'] = df['Time'].map(lambda x: datetime.strptime(str(x), FMT))
df['Time'] = df['Time'].map(lambda x: x.replace(day=date, month=month, year=year))

#df['New'] = df['Time'].apply(lambda x: x.to_datetime())
#df['New'] = df['Time'].apply(lambda x: x.to_pydatetime())

# Two Subplots, and sharing X-axis
f, (ax1, ax2) = plt.subplots(2, 1, sharex=True)
ax1.plot( df['Time'][0:100], df['ADC'][0:100], color='r', aa=True )
ax1.set_ylim( [0,80])
ax1.set_title('ADC Counts Vs Time')
ax1.set_ylabel('ADC Counts')
ax1.set_gid('True')
ax1.grid(linestyle=':')

ax2.plot( df['Time'][0:100], df['Temperature'][0:100], color='b', aa=True )
ax2.set_ylim( [0,40])
ax2.set_title('Temperature Vs Time')
ax2.set_ylabel('Temperature (C)')
ax2.set_xlabel('Time Axis')
ax2.grid(linestyle=':')
#plt.ylim( [0,200])
plt.gcf().autofmt_xdate()
plt.show()

