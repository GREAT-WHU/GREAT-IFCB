#-*- coding:utf-8 -*-
import time
import math
import copy

monthdays = [0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

# ===========================================================
#     Determine if a year is a leap year
# ===========================================================
def leapyear(year):
    if year % 4 == 0 and year % 100 != 0:
        return 1
    if year % 400 == 0:
        return 1
    return 0

# ===========================================================
#     Change year,day of year to year,month,day
# ===========================================================
def doy2ymd(year, doy):
    day = doy
    mon = 0
    for i in range(13):
        monthday = monthdays[i]
        if (i == 2 and leapyear(year) == 1):
            monthday += 1
        if day > monthday:
            day -= monthday
        else:
            mon = i
            break

    return year,mon,day

# ===========================================================
#     Change year,month,day to year,day of year
# ===========================================================
def ymd2doy(year, mon, day):
    doy = day
    for i in range(1, mon):
        doy += monthdays[i]
    if (mon > 2):
        doy += leapyear(year)
    return year,doy

# ===========================================================
#     Change year,month,day to Modified Julian Day
# ===========================================================
def ymd2mjd(year, mon, day):
    mjd = 0.0
    if (mon <= 2):
        mon += 12
        year -= 1
    mjd = 365.25 * year - 365.25 * year % 1.0 - 679006.0
    mjd += math.floor(30.6001 * (mon + 1)) + 2.0 - math.floor(
        year / 100.0) + math.floor(year / 400) + day
    return mjd

def ydoy2mjd(year, doy):
    year, month, day = doy2ymd(year, doy)
    return ymd2mjd(year, month, day)

def mjd2ymd(mjd):
    t1 = 1.0 + mjd - mjd % 1.0 + 2400000.0
    t4 = mjd % 1.0
    ih = int((t1 - 1867216.25) / 36524.25)
    t2 = t1 + 1 + ih - int(ih / 4)
    t3 = t2 - 1720995.0
    ih1 = int((t3 - 122.1) / 365.25)
    t1 = ih1 * 365.25 - ih1 * 365.25 % 1.0
    ih2 = int((t3 - t1) / 30.6001)
    dd = int(t3 - t1 - int(ih2 * 30.6001) + t4)
    mm = ih2 - 1
    if ih2 > 13:
        mm = ih2 - 13
    jj = ih1
    if mm <= 2:
        jj += 1

    return jj, mm, dd

def mjd2ydoy(mjd):
    year,month,day = mjd2ymd(mjd)
    return ymd2doy(year,month,day)


def mjd2gpsweek(mjd):
    week = int((mjd - 44244.0) / 7.0)
    day = math.floor(mjd - 44244.0 - week * 7.0)
    return week, day



class gnss_time:
    def __init__(self, *args):
        try:
            args = [int(num) for num in args]
        except Exception as e:
            raise ValueError("gnss time input arugments must represent number")
        self.mjd = 0
        if len(args) == 0:
            gmt = time.gmtime()
            self.mjd = ydoy2mjd(gmt.tm_year, gmt.tm_yday)
        elif len(args) == 2:
            self.mjd = ydoy2mjd(args[0], args[1])
        elif len(args) == 3:
            self.mjd = ymd2mjd(args[0], args[1], args[2])
        else:
            raise ValueError("gnss_time must be empty[now] or ydoy or ymd!!")

    def __eq__(self, other):
        return self.mjd == other.mjd
    
    def __lt__(self, other):
        return self.mjd < other.mjd

    def __le__(self, other):
        return self.mjd <= other.mjd

    def __gt__(self,other):
        return self.mjd > other.mjd

    def __ge__(self,other):
        return self.mjd >= other.mjd

    def __add__(self,days):
        other = copy.copy(self) 
        other.add(days)
        return other

    def __sub__(self, days):
        other = copy.copy(self)
        other.add(-days)
        return other

    def add(self,days):
        self.mjd += days

    def year(self):
        return mjd2ymd(self.mjd)[0]

    def month(self):
        return mjd2ymd(self.mjd)[1]

    def day(self):
        return mjd2ymd(self.mjd)[2]

    def gwk(self):
        return mjd2gpsweek(self.mjd)[0]

    def gwkd(self):
        return mjd2gpsweek(self.mjd)[1]

    def doy(self):
        return mjd2ydoy(self.mjd)[1]

    def str_yr(self):
        return self.str_year()[-2:]

    def str_year(self):
        return f"{self.year():04}"

    def str_month(self):
        return f"{self.month():02}"

    def str_day(self):
        return f"{self.day():02}"

    def str_doy(self):
        return f"{self.doy():03}"

    def str_yeardoy(self):
        return f"{self.str_year()}{self.str_doy()}"

    def str_ymd(self):
        return f"{self.str_year()}-{self.str_month()}-{self.str_day()}"