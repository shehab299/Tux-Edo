const {exec , spawn } = require('child_process');
const path = require('path');

const startSimulation = (event , seedPath, algo, timeSlice) => {

    if(timeSlice === undefined || timeSlice === "")
        timeSlice = "0";

    const subprocess = spawn('/home/shehab/Tux-Edo/Simulation2/pg.out', [seedPath, algo, timeSlice]);

    console.log(subprocess);


    subprocess.on('exit', (code) => {
        console.log(`Child exited with code ${code}`);
        const filepath = path.join(process.cwd() ,'scheduler.log')
        processFile(filepath)
    });

};

const fs = require('fs');

function processLine(line) {
    const data = line.split('/\s+/)');
    
    const time = parseInt(data[2]);
    const processId = parseInt(data[4]);
    const action = data[5];
    const arrive = parseInt(data[7]);
    const total = parseInt(data[9]);
    const remain = parseInt(data[11]);
    const wait = parseInt(data[13]);
    const ta = data[14] === 'TA' ? parseInt(data[15]) : null;
    const wta = data[16] === 'WTA' ? parseFloat(data[17]) : null;

    console.log(`Time: ${time}, Process ID: ${processId}, Action: ${action}, Total: ${total}, Remain: ${remain}, Wait: ${wait}, TA: ${ta}, WTA: ${wta}`);
}

function processFile(filename) {
    const fileStream = fs.createReadStream(filename);
    const rl = require('readline').createInterface({
        input: fileStream,
        crlfDelay: Infinity
    });

    rl.on('line', (line) => {
        processLine(line);
    });

    rl.on('close', () => {
        console.log('File reading complete.');
    });
}



exports.startSimulation = startSimulation;