const {exec , spawn } = require('child_process');
const path = require('path');

let pcbs = []

const startSimulation = (event , seedPath, algo, timeSlice) => {

    if(timeSlice === undefined || timeSlice === "")
        timeSlice = "0";

    pcbs = [];

    const filepath = path.join(process.cwd(), 'Simulation2' ,'pg.out')
    const subprocess = spawn(filepath, [seedPath, algo, timeSlice]);

    console.log(subprocess);


    subprocess.on('exit', (code) => {
        console.log(`Child exited with code ${code}`);
        const filepath = path.join(process.cwd() ,'scheduler.log')
        processFile(filepath)
    });


};

const fs = require('fs');
const { ipcMain, BrowserWindow } = require('electron');


const PCB = (time,processId,action,arrive,total,remain,wait,ta,wta) => {
    return {
        time,processId,action,arrive,total,remain,wait,ta,wta
    }
}

function processLine(line) {
    const data = line.split(/\s+/);
    
    const time = data[2];
    const processId = data[4];
    const action = data[5];
    const arrive = data[7];
    const total = data[9];
    const remain = data[11];
    const wait = data[13];
    const ta = data[14] === 'TA' ? data[15] : null;
    const wta = data[16] === 'WTA' ? data[17] : null;

    const _process = PCB(time,processId,action,arrive,total,remain,wait,ta,wta);

    return _process;
}

function processFile(filename) {



    const fileStream = fs.createReadStream(filename);
    const rl = require('readline').createInterface({
        input: fileStream,
        crlfDelay: Infinity
    });

    rl.on('line', (line) => {
        pcbs.push(processLine(line));
    });

    rl.on('close', () => {
        console.log("closed");
        BrowserWindow.getAllWindows()[0].webContents.send('simulation-end', pcbs);
    });
}



exports.startSimulation = startSimulation;
exports.pcbs = pcbs