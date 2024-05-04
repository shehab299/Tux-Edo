const { app, BrowserWindow, dialog , ipcMain } = require('electron');
const path = require('path');
const {getSeedFile} = require('./IPC/handlers');
const {startSimulation} = require('./IPC/simulation');

function run() {

    const window = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            preload: path.join(__dirname,'IPC','preload.js')
        }
    });

    window.webContents.openDevTools();
    window.loadFile('Frontend/index.html');

}

app.on('window-all-closed', () => {   
    if (process.platform !== 'darwin') 
        app.quit();
});
  
app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) 
        run();
});

async function start(){
    await app.whenReady();
    ipcMain.handle('get-seed-file', getSeedFile);
    ipcMain.handle('start',startSimulation);
    run();
}

start();






