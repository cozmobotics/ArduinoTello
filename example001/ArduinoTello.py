import serial
import threading
import time
import socket
import sys


Running = True
try:
	ser = serial.Serial('COM13', 115200 ) # Establish the connection on a specific port
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
	
	print ("UDP receive task started")
	
	count = 0
	while True: 
		try:
			data, server = sock.recvfrom(1518)
			print(data.decode(encoding="utf-8"))
		except Exception:
			print ('\nExit . . .\n')
			break

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
		else:
			print ('.', end='')

#---------------------------------------------------------------------------
def dummyFunc():
	global Running
	while Running:
		print ("main loop still running")
		time.sleep (10)

#---------------------------------------------------------------------------
# dummyTask = threading.Thread(target=dummyFunc)
# dummyTask.start()

#recvThread create
recvThread = threading.Thread(target=recv)
recvThread.start()



serialTask = threading.Thread(target=getCommandSerial)
serialTask.start()
# getCommandSerial()


while (Running):
	# Command = input()
	# print (Command)
	# if (Command == "end"):
		# Running = False
	time.sleep (10)

