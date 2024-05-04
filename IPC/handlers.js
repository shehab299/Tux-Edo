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


exports.getSeedFile = getSeedFile;
