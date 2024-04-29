const {dialog} = require('electron');

const { exec, execSync } = require('child_process');


async function getSeedFile(event){
    const result = await dialog.showOpenDialog({
        properties: ['openFile'],
        title: "Open Seed File",
        showHiddenFiles: false,
        filters: [
            {name: 'Seed Files', extensions: ['txt']}
        ]
    });

    if(result !== undefined)
        return result.filePaths[0];
    else
        return "No file selected";

}


async function startSimulation(event,seedPath, algo, timeSlice) {
    console.log(seedPath,algo,timeSlice);
    exec(`echo ${seedPath} ${algo} ${timeSlice} >> input.txt`);
    return "Simulation started";
}


exports.getSeedFile = getSeedFile;
exports.startSimulation = startSimulation;