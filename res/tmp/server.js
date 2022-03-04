const net = require('net');
const uuid4 = require('uuid4');
const PORT = 52002;
const d = require('./data.js');
const data = d.data;

const clientAvgterm = 10;
const serverAvgMill = 200;


//connection with remote client via tcp
const server = net.createServer(socket => {
    
    //add client
    socket.id = uuid4();
    data.addClient(socket);
    


}).listen(PORT);
