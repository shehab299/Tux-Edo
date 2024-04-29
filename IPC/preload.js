
const { contextBridge , ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('utils', {
    getSeedFile: () => ipcRenderer.invoke('get-seed-file'),
    start: (seedPath, algo, timeSlice) => ipcRenderer.invoke('start', seedPath, algo, timeSlice)
});