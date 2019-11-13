#!/usr/bin/env python3
import os
from apscheduler.schedulers.blocking import BlockingScheduler

os.system("./insert-db.py")

def runInsert():
    os.system("./insert-db.py")


scheduler = BlockingScheduler()
scheduler.add_job(runInsert, 'interval', minutes=5)
scheduler.start()
