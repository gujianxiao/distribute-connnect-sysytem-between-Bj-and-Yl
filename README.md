# distribute-connnect-sysytem-between-Bj-and-Yl
#this system provide a connection between Beijing and Yulin
#The System consist of two part:gateway in Beijing(e.g GBJ) and gateway in Yuling(e.g GYL).
#We use udp protacal to connect Yuling which serve as a connect and Beijing as a server
#As we can seen in our last project about NDN,the gateway is used to make a protocal change,this system evolve it and 
#implement the remote communication 
#For gateway in Yulin,we recieve the data from wsn and package it into udp,then send it to Beijing.It alse can receive 
#the data from Beijing then parse it into the message that can be read by wsn and write into it
#For gateway in Beijing,we receive the data from Yuling ,parse it from udp and analysis the info in it to build the 
#topology and location map,it also play as real gateway that receive interest from client then send to wsn with a udp 
#package.And get the response data send back to the client.
#
