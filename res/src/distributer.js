const PORT = 52002;
const net = require('net');
const receiver = require('./receiver.js');
const data = receiver.data;
let uuid4 = require('uuid4');


const clientAvgTerm = 10;
const serverAvgMill = receiver.TERM * 10;

const server = net.createServer(sock => {

    //add client
    sock.id = uuid4();
    data.addClient(sock);
    
    //notice average term to client
    sock.write(clientAvgTerm.toString() + "/" + serverAvgMill.toString() + "/");
    console.log("write head: " + clientAvgTerm.toString() + "/" + serverAvgMill.toString() + "/");

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
const timeout = 10000;   //milli sec
const interval = 10;     //milli sec
let fails = 0;
setInterval( () => {
    if(data.isNoClient() == false){

        //if enough data is not alived witin timeout[msec]
        if(!(data.broadCast())){
            fails += interval;

            //send -1 if no sensor data alived
            if(fails >= timeout){
                for(let i = 0; i < data.clients.length; i++){
                    data.clients[i].write("-1,");       
                }
            }

        }
    }
}, interval);

process.stdin.setEncoding('utf-8');
let reader = require("readline").createInterface({
    input: process.stdin,
    output: process.stdout
});

//read stdin
reader.on('line', line => {
    data.addData(line);
});

reader.on('close', () => {
    console.log("EOF detected");
    reader.close();
    process.exit(0)
});

