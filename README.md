# Server

So Server and clients have interested me recently. However, when I began researching for this it was very difficult to find much documentation and code on it. I also always 
struggled with c++ pointers and memory allocation/controlling. So i embarked on this project.
Currently you can connect multiple sockets to the server. The client (https://github.com/MturnerTufts/Server) and Server (https://github.com/MturnerTufts/Client).

The Client is a Qt application written in C++ that will connect to the server and send information. currently you can input your desired username, and also take screenshots.
Its similar ot a snipping tool. Click the butotn and make a square, it will send the information to the server. The goal is to eventually have two clients or more connected to 
each other through the server and be able to quickly send images but clicking a button, making a square and having it send through the server and to the people. You can connect to the 
server through other clients however there is a special way the information si send. Its sent with a the lenght of the label, the label, (if its an image its also send with the length of the
height string and width stright) along with the heights and width. I then use memcpy to copy the infomration to the buffer. Say you have 2LB222323. That means the label is 2 long so
I copy the next 2 characters and call them my label. Then the next 2 represent the width lenght and the heigh length. This means that both of them are 2 charactes long. So then 23 and
23 are the width and ehgiht. Then the image data would come next. (or if it was a message it would be liek 2MSMessagePutHere).
