


const fs = require('fs');
const net = require('net');

const data = new Data(50);

const receiver = net.createServer(usock => {
    console.log('unix domain socket connection open');
    
    usock.on('close', () => {
        
    });

});

try{
    fs.unlinkSync('/tmp/unix.sock');    
}catch(e){
    console.error(e);
}

receiver.listen('/tmp/unix.sock');


/**
 * @classdesc
 * class for data
 */
 class Data{
    constructor(length){
        this.data = Array(length);
        this.time = Array(length);  //time = 10, 20, 30.....    
        this.count = 0;

        //init all elements by 0
        for(let i = 0; i < this.data.length; i++){
            this.data[i] = 0;
        }

        this.clients = [];
    }

    /**
     * 
     * @param {String} line - line should be time, time, val, val, val,....
     */
    addData(line){
        const cols = line.split(',');       //split by ,
        if(cols.length < 8){
            console.error("invalid csv data inputted");
            return;
        }

        const serverTime = cols[0];
        const deviceTime = cols[1];
        const numDatas = cols[2];
        const pressure1 = cols[3];
        const pressure2 = cols[4];
        const velocityX = cols[5];
        const velovityY = cols[6];
        const velocityZ = cols[7];

        this.addValue(pressure1);
        //this.addTime(deviceTime);
        
    }

    /**
     * add value to member 
     * @param {Number} v this should be number
     */
    addValue(v){
        let value;
        try{
            value = Number(v);  
        }catch(e){
            console.error(`error on Data.addData(${v}), ${e}`);
        }
        this.data.push(value);
        this.data.shift();  //remove this.data[0]

        this.count++;
        //console.log(`add ${value}`);
    }

    /**
     * add time to member
     * @param {Number} v this should be number, like deviceTime
     */
    addTime(v){
        let time;
        try{
            time = Number(v);            
        }catch(e){
            console.error(`error on Data.addTime(${v}), ${e}`);
        }

        this.time.push(time);

    }

    /**
     * add connecting client socket 
     * @param {Socket} sock 
     */
    addClient(sock){
        //addclient()
        this.clients.push(sock);
    }

    /**
     * delete specified socket from this.
     * @param {Socket} sock - socket to delete
     */
    deleteClient(sock){
        this.clients.splice(this.clients.indexOf(sock), 1);
    }

    /**
     * return there is client or not
     * @returns {Boolean} - if no connection -> false, else true 
     */
    isNoClient(){
        if(this.clients.length == 0){
            return true;
        }
        return false;
    }
    
    /**
     * get average value of term, from tail
     * @param {Number} term - this should be number
     * @returns average of this.data[tail-term] - this.data[tail]
     */
    getAverage(term){
        let sum = 0;
        for(let i = this.data.length-1; i > this.data.length-term - 1; i--){
            sum += this.data[i];
        }
        return Math.floor(sum/term);
    }

    /**
     * broadCast average value to clients
     */
    broadCast(){
        //if not enough data arrived, dont send
        if(this.count < TERM){
            return;
        }
        let avg = this.getAverage(TERM);
        //console.log(`write ${avg}`);
        for(let i = 0; i < this.clients.length; i++){
            this.clients[i].write(avg.toString() + ",");
        }
        this.count = 0;
    }
}

exports.data = data;