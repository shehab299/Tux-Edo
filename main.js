const { app, BrowserWindow, dialog , ipcMain } = require('electron');
const path = require('path');
const {getSeedFile,startSimulation} = require('./IPC/handlers');


function run() {

    const window = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            preload: path.join(__dirname,'preload.js')
        }
    });

    window.webContents.openDevTools();
    window.loadFile('App/index.html');

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






