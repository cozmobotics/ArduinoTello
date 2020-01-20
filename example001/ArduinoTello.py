
'''A simple program to fly the Tello quadrocopter (drone) with a controller that sends data via serial (tty) interface
   See https://github.com/cozmobotics/ArduinoTello/ for details '''

import serial
import threading
import time
import socket
import sys


Running = True
try:
	ser = serial.Serial('COM13', 115200, timeout = 1) # Establish the connection on a specific port
except Exception as e:
	print (str(e))
	sys.exit()


# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
tello_address = ('192.168.10.1', 8889)
host = ''
port = 9000
locaddr = (host,port) 
sock.bind(locaddr)

Command = ''

#---------------------------------------------------------------------------
def recv():
	global sock
	global Running
	
	print ("UDP receive task started")
	
	count = 0
	while Running: 
		try:
			data, server = sock.recvfrom(1518)
			print(data.decode(encoding="utf-8"))
		except Exception:
			print ('\nExit . . .\n')
			break
			
	print ("UDP receive task ended")

#---------------------------------------------------------------------------
def getCommandSerial():
	global Running
	global Command
	global sock
	global tello_address
	
	print ("Serial task started")
	
	while Running:
		Text = ser.readline()
		if (Text):
			# Text = str(Text)
			Text = Text.decode(encoding="utf-8")
			Text = Text.replace("\r",'')
			Text = Text.replace("\n",'')
			print (Text)
			Text = Text.encode(encoding="utf-8") 
			sent = sock.sendto(Text, tello_address)
	print ("Serial task ended")

#---------------------------------------------------------------------------

# create threads
recvThread = threading.Thread(target=recv)
recvThread.start()

serialTask = threading.Thread(target=getCommandSerial)
serialTask.start()

print ("Here you see the commands that comr from the serial line. ")
print ("You can type additional commands like \"battery?\".")
print ("Type \"end\" to quit the program.")

while (Running):
	Command = input()
	print (Command)
	if (Command == "end"):
		Running = False
	else:
		Command = Command.encode(encoding="utf-8")
		sent = sock.sendto(Command, tello_address)

print ("ArduinoTello will end in 3 seconds")
time.sleep(3)
print ("Thank you for using ArduinoTello")