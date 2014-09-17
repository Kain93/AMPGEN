#!/usr/bin/python
# -*- coding: utf-8 -*-

#Первая строка указывает на программу через которую будет выполняться код в этом файле
#Вторая строка указывает на кодировку файла
#Эти две строчки трогать нельзя!!!!


#Подключаем библиотеки
from Tkinter import *
import math
import os 

#Подпрограмма start_piblaster
def start_piblaster():
    os.system("sudo ./pi-blaster") #Запускаем демона Pi-blaster

#Подпрограмма set_piblaster
def set_piblaster(blpin, blpower): 
#    blpin = float(blpin_entry.get()) #Получаем значение из окошка канала
#    blpower = float(blpower_entry.get())  #Получаем значение из окошка скважности
    os.system("sudo echo %(blpin)s = %(blpower)s > /dev/pi-blaster" % { 'blpin' : blpin, 'blpower' : blpower } ) #Передаем параметры программе pi-blaster

#Подпрограмма set_pwm
#def set_pwm(): 
#    freq = float(freq_entry.get()) #Получаем значение из окошка частоты
#    power = float(power_entry.get())  #Получаем значение из окошка мощности
    os.system("sudo ./pwm -f %(freq)s -p %(pwr)s" % { 'freq' : freq, 'pwr' : power } ) #Передаем параметры программе pwm

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


root = Tk() #Создаем окошко с использованием библиотеки Tkinter
root.title("Rele Control") #Заголовок окна
frame = Frame(root) #Создаем в окне рамку
frame.pack() #Используем упаковщик

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

b1_button = Button(frame, text="Запустить pi-blaster", width=20, command=start_piblaster) #Создаем кнопку Запустить, при нажатии вызывается подпрограмма start_piblaster
b1_button.grid(row=0, column=0)


pwm4_label = Label(frame, text="4 канал")
pwm4_label.grid(row=1, column=0)

blpower4_entry = Entry(frame, width=10)
blpower4_entry.insert(0,0)
blpower4_entry.grid(row=2, column=1)
blpower4_label = Label(frame, text="Заполнение ШИМ") #Заполнение задается дробным числом от 0 до 1
blpower4_label.grid(row=2, column=0)

pwm4_on_button = Button(frame, text="Задать ШИМ", width=10, command=lambda:set_piblaster(1, blpower4)) #Создаем кнопку Задать, при нажатии вызывается подпрограмма set_piblaster
pwm4_on_button.grid(row=3, column=0)


pwm18_label = Label(frame, text="18 канал")
pwm18_label.grid(row=4, column=0)

blpower18_entry = Entry(frame, width=10)
blpower18_entry.insert(0,0)
blpower18_entry.grid(row=5, column=1)
blpower18_label = Label(frame, text="Заполнение ШИМ")
blpower18_label.grid(row=5, column=0)

pwm18_on_button = Button(frame, text="Задать ШИМ", width=10, command=lambda:set_piblaster(2, blpower18)) #Создаем кнопку Задать, при нажатии вызывается подпрограмма set_p$
pwm18_on_button.grid(row=6, column=0)


pwm24_label = Label(frame, text="24 канал")
pwm24_label.grid(row=7, column=0)

blpower24_entry = Entry(frame, width=10)
blpower24_entry.insert(0,0)
blpower24_entry.grid(row=8, column=1)
blpower24_label = Label(frame, text="Заполнение ШИМ")
blpower24_label.grid(row=8, column=0)

pwm24_on_button = Button(frame, text="Задать ШИМ", width=10, command=lambda:set_piblaster(3, blpower24)) #Создаем кнопку Задать, при нажатии вызывается подпрограмма set_p$
pwm24_on_button.grid(row=9, column=0)


on22_button = Button(frame, text="Off", width=10, command= lambda:gpio(22,1) )
on22_button.grid(row=10, column=2)

off22_button = Button(frame, text="On", width=10, command= lambda:gpio(22,0) )
off22_button.grid(row=10, column=1)

channel22_label = Label(frame, text="Большая реле")
channel22_label.grid(row=10, column=0)


on17_button = Button(frame, text="Off", width=10, command= lambda:gpio(17,1) )
on17_button.grid(row=11, column=2)

off17_button = Button(frame, text="On", width=10, command= lambda:gpio(17,0) )
off17_button.grid(row=11, column=1)

channel17_label = Label(frame, text="Малая реле(заряд)")
channel17_label.grid(row=11, column=0)


on23_button = Button(frame, text="Off", width=10, command= lambda:gpio(23,1) )
on23_button.grid(row=12, column=2)

off23_button = Button(frame, text="On", width=10, command= lambda:gpio(23,0) )
off23_button.grid(row=12, column=1)

channel23_label = Label(frame, text="Разряд")
channel23_label.grid(row=12, column=0)




exit_button = Button(frame, text="Выход", width=10, command=root.destroy) 
exit_button.grid(row=13, column=0)

root.mainloop()

