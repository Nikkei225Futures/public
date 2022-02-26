const PORT = 52002;
const net = require('net');
const receiver = require('./receiver.js');

const data = receiver.data;
let uuid4 = require('uuid4');

const server = net.createServer(sock => {

    //add client
    sock.id = uuid4();
    data.addClient(sock);

    sock.on('data', str => {
        console.log(str);
    });

    sock.on('close', () => {
        //delete client from clients array
        data.deleteClient(sock);
    });

    sock.on('error', () => {
        console.log(`client ${sock.id} disconnected with error`);
        data.deleteClient(sock);
    });

}).listen(PORT);

//broadcast data to clients
setInterval( () => {
    if(data.isNoClient() == false){
        data.broadCast();
    }
}, 10);

process.stdin.setEncoding('utf-8');
let reader = require("readline").createInterface({
    input: process.stdin,
    output: process.stdout
});

//read stdin
reader.on('line', line => {
    data.addData(line);
});

process.stdin.on('end', () => {
    console.log("readline end");
    //ignore EOF
    reader = require("readline").createInterface({
        input: process.stdin,
        output: process.stdout
    });
});