#!/usr/bin/env python3
import datetime
import time
from datetime import datetime

import pymysql as mdb
import serial
from pytz import timezone


def utcnow():
    return datetime.now(tz=amsterdam)


mdb.install_as_MySQLdb()

arduino = serial.Serial("/dev/ttyACM0")
arduino.baudrate = 9600

time.sleep(1)

succesfull = True

while succesfull:
    data = str(arduino.readline())
    pieces = data.split('|')

    if pieces[0] != "" and pieces[1] != "":
        # Split data to specific data parts
        temperature = pieces[0]
        temperature1 = pieces[1]

        # Get time
        amsterdam = timezone('Europe/Amsterdam')
        when_are_we_alive = utcnow()

        # Strip temperature to good unit
        temperature = temperature[2:]

        if temperature[0:1] == "-":
            if temperature[3:4] == ".":
                temperature = temperature[0:6]
            if temperature[2:3] == ".":
                temperature = temperature[0:5]
        else:
            if temperature[2:3] == ".":
                temperature = temperature[0:5]
            if temperature[3:4] == ".":
                temperature = temperature[0:4]

        # Same stripping, but for second data source
        if temperature1[0:1] == "-":
            if temperature1[3:4] == ".":
                temperature1 = temperature1[0:6]
            if temperature1[2:3] == ".":
                temperature1 = temperature1[0:5]
        else:
            if temperature1[2:3] == ".":
                temperature1 = temperature1[0:5]
            if temperature1[3:4] == ".":
                temperature1 = temperature1[0:4]

        # Connect to db
        con = mdb.connect('169.254.77.74', 'root', 'secret', 'temperature')
        with con:
            cursor = con.cursor()
            cursor.execute("""INSERT INTO temperature_data (`time`, `temperature`, `temperature1`) VALUES(%s,%s,%s)""",
                           (when_are_we_alive, temperature, temperature1))
            con.commit()
            cursor.close()  
            # Print the the time and data.
            print("The data (Temperature: " + temperature + " and " + temperature1 + ") has been send.")
            succesfull = False
    else:
        print("Exception. Did not recieve any data from the arduino")
