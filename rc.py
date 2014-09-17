#!/usr/bin/python
# -*- coding: utf-8 -*-

#Первая строка указывает на программу через которую будет выполняться код в этом файле
#Вторая строка указывает на кодировку файла
#Эти две строчки трогать нельзя!!!!


#Подключаем библиотеки
from Tkinter import *
import math
import os 
#Подпрограмма set_pwm
#def set_pwm(): 
#    freq = float(freq_entry.get()) #Получаем значение из окошка частоты
#    power = float(power_entry.get())  #Получаем значение из окошка мощности
#    os.system("sudo ./pwm -f %(freq)s -p %(pwr)s" % { 'freq' : freq, 'pwr' : power } ) #Передаем параметры программе pwm

#def gpio_open():
#    channel = int(channel_entry.get()) #Получаем значение из окошка номера канала
#    os.system("sudo ./gpio.sh %(channel)s 1" % { 'channel' : channel } ) #Передаем параметры программе gpioopen

#    os.system( "echo gpio1" )

#def gpio_close():
#    channel = int(channel_entry.get()) #Получаем значение из окошка номера канала
#    os.system("sudo ./gpio.sh %(channel)s 0" % { 'channel' : channel } ) #Передаем параметры программе gpioclose

#    os.system( "echo gpio2" )


def gpio(num,val):
    os.system("sudo ./gpio.sh %(num)s %(val)s" % { 'num' : num, 'val' : val} )
#    os.system("echo %(num)s %(val)s" % { 'num' : num, 'val' : val} )


#def charge_relay():
#	os.system("echo %(charge_flag)s" % {'charge_flag' : charge.get} )
#	if carge.get() == 1:
 #       	os.system("./gpio.sh 17 0")
#	else:
#        	os.system("./gpio.sh 17 1")


#def charge_relay():
#        os.system("echo %(charge_flag)s" % {'charge_flag' : charge.get} )
#        if carge.get() == 1:
#                os.system("./gpio.sh 17 0")
#        else:
#                os.system("./gpio.sh 17 1")



root = Tk() #Создаем окошко с использованием библиотеки Tkinter
root.title("Rele Control") #Заголовок окна
frame = Frame(root) #Создаем в окне рамку
frame.pack() #Используем упаковщик
charge=IntVar()
#freq_entry = Entry(frame, width=10, text='50') #Создаем в рамке окошко ввода частоты
#freq_entry.insert(0,50) #Задаем значение по умолчанию
#freq_entry.grid(row=0, column=0) #Распологаем по сетке в 0-й строке, 0-м столбце
#freq_label = Label(frame, text="Частота ШИМ") #Создаем подпись
#freq_label.grid(row=0, column=1) #Распологаем по сетке в 0-й строке, 1-м столбце

#power_entry = Entry(frame, width=10)
#power_entry.insert(0,0)
#power_entry.grid(row=1, column=0)
#power_label = Label(frame, text="Мощность")
#power_label.grid(row=1, column=1)

#num_entry = Entry(frame, width=10)
#num_entry.insert(0,15)
#num_entry.grid(row=3, column=0)
#num_label = Label(frame, text="Канал")
#num_label.grid(row=3, column=1)

#b1_button = Button(frame, text="Задать", width=10, command=set_pwm) #Создаем кнопку Задать, при нажатии вызывается подпрограмма set_pwm
#b1_button.grid(row=2, column=0)

#on_button = Button(frame, text="On", width=10, command=lambda:gpio(int(num_entry.get()),1))
#on_button.grid(row=4, column=0)

#off_button = Button(frame, text="Off", width=10, command=lambda:gpio(int(num_entry.get()),0))
#off_button.grid(row=4, column=1)

#on1_button = Button(frame, text="On", width=10, command= lambda:gpio(15,1) )
#on1_button.grid(row=5, column=0)

#off1_button = Button(frame, text="Off", width=10, command= lambda:gpio(15,0) )
#off1_button.grid(row=5, column=1)

#channel15_label = Label(frame, text="15 канал")
#channel15_label.grid(row=5, column=2)



on2_button = Button(frame, text="Off", width=10, command= lambda:gpio(22,1) )
on2_button.grid(row=3, column=0)

off2_button = Button(frame, text="On", width=10, command= lambda:gpio(22,0) )
off2_button.grid(row=3, column=1)

channel17_label = Label(frame, text="Большая реле")
channel17_label.grid(row=3, column=2)

on3_button = Button(frame, text="Off", width=10, command= lambda:gpio(17,1) )
on3_button.grid(row=1, column=0)

off3_button = Button(frame, text="On", width=10, command= lambda:gpio(17,0) )
off3_button.grid(row=1, column=1)

channel21_label = Label(frame, text="Малая реле(заряд)")
channel21_label.grid(row=1, column=2)

check1=Checkbutton(frame, text=u'Заряд',variable=charge,onvalue=1,offvalue=0, command= lambda:charge_relay() )
check1.grid(row=0,column=0)

#on4_button = Button(frame, text="On", width=10, command= lambda:gpio(14,1) )
#on4_button.grid(row=8, column=0)

#off4_button = Button(frame, text="Off", width=10, command= lambda:gpio(14,0) )
#off4_button.grid(row=8, column=1)

#channel22_label = Label(frame, text="14 канал")
#channel22_label.grid(row=8, column=2)

on5_button = Button(frame, text="Off", width=10, command= lambda:gpio(23,1) )
on5_button.grid(row=2, column=0)

off5_button = Button(frame, text="On", width=10, command= lambda:gpio(23,0) )
off5_button.grid(row=2, column=1)

channel23_label = Label(frame, text="Разряд")
channel23_label.grid(row=2, column=2)

#on6_button = Button(frame, text="On", width=10, command= lambda:gpio(24,1) )
#on6_button.grid(row=10, column=0)

#off6_button = Button(frame, text="Off", width=10, command= lambda:gpio(24,0) )
#off6_button.grid(row=10, column=1)

#channel24_label = Label(frame, text="24 канал")
#channel24_label.grid(row=10, column=2)


def charge_relay():
        os.system("echo %(charge_flag)s" % {'charge_flag' : charge.get} )
        if carge.get() == 1:
                os.system("./gpio.sh 17 0")
        else:
                os.system("./gpio.sh 17 1")


exit_button = Button(frame, text="Выход", width=10, command=root.destroy) 
exit_button.grid(row=4, column=0)

root.mainloop()

