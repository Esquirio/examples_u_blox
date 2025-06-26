import socket
import datetime

localIP     = "192.168.0.151"
localPort   = 3333
bufferSize  = 1024

msgFromServer       = "Hello UDP Client"
bytesToSend         = str.encode(msgFromServer)

# Create a datagram socket
UDPServerSocket = socket.socket(family=socket.AF_INET, 
type=socket.SOCK_DGRAM)

# Bind to address and ip
UDPServerSocket.bind((localIP, localPort))

print("UDP server up and listening")
print ("------------------------------------------------")

# Listen for incoming datagrams
while(True):
    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
    message = bytesAddressPair[0]
    address = bytesAddressPair[1]

    clientMsg = "Message from Client:{}".format(message)
    clientIP  = "Client IP Address:{}".format(address)
    
    time_now = datetime.datetime.now()
    print (f"Current date and time = {time_now}")
    print(clientMsg)
    print(clientIP)
    print ("------------------------------------------------")

    # Sending a reply to client
    UDPServerSocket.sendto(bytesToSend, address)
