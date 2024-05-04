const {exec , spawn } = require('child_process');


const startSimulation = (event , seedPath, algo, timeSlice) => {

    if(timeSlice === undefined || timeSlice === "")
        timeSlice = "0";

    const subprocess = spawn('/home/shehab/Tux-Edo/Simulation2/pg.out', [seedPath, algo, timeSlice]);

    subprocess.stdout.on('data', (data) => {
        console.log(data.toString());
    })

};

exports.startSimulation = startSimulation;