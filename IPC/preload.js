const { contextBridge , ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('utils', {
    getSeedFile: () => ipcRenderer.invoke('get-seed-file'),
    start: (seedPath, algo, timeSlice) => ipcRenderer.invoke('start', seedPath, algo, timeSlice),
});


window.addEventListener('DOMContentLoaded', () => {

    ipcRenderer.on('simulation-end', (_event, value) => {

        Array.from(document.getElementsByTagName('table')).forEach(table => table.remove());

        window.console.log(value);
        const table = createTableFromArray(value);
        document.body.appendChild(table);
    })

})


function createTableFromArray(array) {

    const table = document.createElement('table');

    array.forEach(item => {
        const row = table.insertRow();
        Object.values(item).forEach(value => {
            const cell = row.insertCell();
            cell.textContent = value;
        });
    });

    return table;
}