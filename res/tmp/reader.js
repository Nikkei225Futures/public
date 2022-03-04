//read sensor data via pipe
//create connection to server.js with unix domain sock
const net = require('net');
const con = net.createConnection('tmp/unix.sock');

process.stdin.setEncoding('utf-8');
let reader = require("readline").createInterface({
    input: process.stdin,
    output: process.stdout
});

//read stdin
reader.on('line', line => {
    //send data to data.js via unix domain socket
    con.write(line.toString());
});

reader.on('close', () => {
    console.log("EOF detected");
    reader.close();
    process.exit(0);
});

