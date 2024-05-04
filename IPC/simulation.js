const {exec , spawn } = require('child_process');


const startSimulation = (event , seedPath, algo, timeSlice) => {

    if(timeSlice === undefined || timeSlice === "")
        timeSlice = "0";

    const subprocess = spawn('/home/shehab/Tux-Edo/Simulation2/pg.out', [seedPath, algo, timeSlice]);

    console.log(subprocess);

    subprocess.stdout.on('data', (data) => {
        console.log(data.toString());
    })

    subprocess.stderr.on('data', (data) => {
        console.log(data.toString());
    })

    subprocess.on('exit', (code) => {
        console.log(`Child exited with code ${code}`);
        event.sender.send('simulation:exit', code);
    });



};

exports.startSimulation = startSimulation;