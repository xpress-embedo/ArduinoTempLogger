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

plt.plot( df['Time'], df['ADC'])
plt.ylim( [0,200])
plt.gcf().autofmt_xdate()
plt.show()

