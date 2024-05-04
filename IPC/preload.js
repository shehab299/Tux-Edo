const { contextBridge , ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('utils', {
    getSeedFile: () => ipcRenderer.invoke('get-seed-file'),
    start: (seedPath, algo, timeSlice) => ipcRenderer.invoke('start', seedPath, algo, timeSlice),
});


window.addEventListener('DOMContentLoaded', () => {

    ipcRenderer.on('simulation-end', (_event, value) => {

        const tables = document.getElementsByTagName('table');

        if(tables.length > 0)
            Array.from(tables).forEach(table => table.remove());

        window.console.log(value);
        const table = createTableFromArray(value.pcbs);
        const table2 = createTableFromArray(value.summaries);
        document.body.appendChild(table);
        document.body.appendChild(table2);
    })

})


function createTableFromArray(array) {

    const table = document.createElement('table');

    if(array.length === 0 || array === undefined || array === null) 
        return table;

    array.forEach(item => {
        const row = table.insertRow();

        if(item === null || item === undefined)
            return;

        Object.values(item).forEach(value => {
            const cell = row.insertCell();
            cell.textContent = value;
        });
    });

    return table;
}