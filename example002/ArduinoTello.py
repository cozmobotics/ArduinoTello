
'''A simple program to fly the Tello quadrocopter (drone) with a controller that sends data via serial (tty) interface
   See https://github.com/cozmobotics/ArduinoTello/ for details '''

import serial
import threading
import time
import socket
import sys
import os

if (sys.platform.startswith('linux') or sys.platform.startswith('cygwin')):
	import glob

#try:
#	from pythonping import ping
#except ImportError:
#	print ("Could not find pythonping")
#	print ("please install it with \"pip3 install pythonping\"")
#	sys.exit()
from pythonping import ping

#---------------------------------------------------------------------------
def waitForConnection (IpAddress):
	''' send pings to IpAddress until ping is successful. No timeout, will wait forever. '''
	Count = 0
	Connected = False
	while (not Connected):
		Count = Count + 1
		PingError = False
		try:
			PingResult = (ping(IpAddress,timeout=1,count=1))
		except Exception as e:
			print (str(e))
			PingError = True
			
		if (not PingError):
			if ((PingResult.success())):
				Connected = True
				print ('') # only a new line
			
		if (not Connected):
			print ('waiting for connection ' + str (Count) + ' ...', end='\r')
			time.sleep (1)


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
		except Exception as e:
			if (str(e) != 'timed out'):
				print (str(e))

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
			Text = Text.decode(encoding="utf-8")
			Text = Text.replace("\r",'')
			Text = Text.replace("\n",'')
			print (Text)
			Text = Text.encode(encoding="utf-8") 
			sent = sock.sendto(Text, tello_address)
	print ("Serial task ended")

#---------------------------------------------------------------------------

Running = True

# find serial port
if sys.platform.startswith('win'):
	ports = ['COM%s' % (i + 1) for i in range(256)]
elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
	# this excludes your current terminal "/dev/tty"
	ports = glob.glob('/dev/tty[A-Za-z]*')
else:
	print ("unknown system")
	sys.exit()
	
Found = False
while (ports and not Found):
	Port = ports.pop(0)
	try:
		ser = serial.Serial(Port, 115200, timeout = 1)
		print ("Opening serial port " + Port)
		Found = True
	except (OSError, serial.SerialException):
		pass
if (not Found):
	print ("No serial port found")
	sys.exit()


waitForConnection ("192.168.10.1")

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.settimeout (1)
tello_address = ('192.168.10.1', 8889)
host = ''
port = 9000
locaddr = (host,port) 
sock.bind(locaddr)

Command = ''


waitForConnection ("192.168.10.1")

# create threads
recvThread = threading.Thread(target=recv)
recvThread.start()

serialTask = threading.Thread(target=getCommandSerial)
serialTask.start()

print ("Here you see the commands that come from the serial line. ")
print ("You can type additional commands like \"battery?\".")
print ("Type \"end\" to quit the program.")

while (Running):
	Command = input()
	print (Command)
	
	if (Command == 'video') or (Command == 'v'):
		Command = "streamon"
		Command = Command.encode(encoding="utf-8")
		sent = sock.sendto(Command, tello_address)
		if (sys.platform.startswith('win')):
			os.system ("start cmd /c FFmpeg -i udp://192.168.10.1:11111 -f sdl \"Tello Video\"")
		if (sys.platform.startswith('linux') or sys.platform.startswith('cygwin')):
			os.system ("x-terminal-emulator -e setsid ffmpeg -i udp://192.168.10.1:11111 -f sdl \"Tello Video\"")
	elif (Command == "end"):
		Running = False
	else:
		Command = Command.encode(encoding="utf-8")
		sent = sock.sendto(Command, tello_address)

print ("ArduinoTello will end in 3 seconds")
time.sleep(3)
print ("Thank you for using ArduinoTello")