const addSeedBtn = document.getElementById("add-seed-file");
const pathLabel = document.getElementById("seed-path");
const algoSelectBox = document.getElementById("algo");
const sliceInput = document.getElementById("time-slice");
const startBtn = document.getElementById("start-btn");
const downBtn = document.getElementById("download-btn");

addSeedBtn.addEventListener('click' , clickHanlder)
startBtn.addEventListener('click' , startHandler)
downBtn.addEventListener('click' , downloadHandler)
// algoSelectBox.addEventListener('change', selectHandler);

async function clickHanlder(event) {
    const result = await utils.getSeedFile();
    pathLabel.value = result;
}

async function downloadHandler(event) {

    const table = document.getElementsByTagName('table')[0];

    if(table === undefined) {
        alert("No data to download");
        return;
    }

    const canvas = await html2canvas(table);

    canvas.toBlob(function (blob) {
        const link = document.createElement('a');
        link.href = URL.createObjectURL(blob);
        link.download = "table_screenshot.png";
        link.click();
    });
};

function selectHandler(event){
    if(algoSelectBox.value === "RR"){
        sliceInput.classList.add("show");
        sliceInput.classList.remove("hidden");
    }else{
        sliceInput.classList.add("hidden");
        sliceInput.classList.remove("show");
    }
} 




async function startHandler(event) {
    const seedPath = pathLabel.value;

    if(seedPath === "No file selected" || seedPath === "") {
        alert("Please select a seed file");
        return;
    }

    const algo = algoSelectBox.value;
    const timeSlice = sliceInput.value;

    if(timeSlice === "" && algo === "RR") {
        alert("Please enter time slice");
        return;
    }

    let result = await utils.start(seedPath, algo, timeSlice);
    console.log(result);
}



