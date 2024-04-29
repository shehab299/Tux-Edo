const addSeedBtn = document.getElementById("add-seed-file");
const pathLabel = document.getElementById("seed-path");
const algoSelectBox = document.getElementById("algo");
const sliceInput = document.getElementById("time-slice");
const startBtn = document.getElementById("start-btn");


addSeedBtn.addEventListener('click' , clickHanlder)
startBtn.addEventListener('click' , startHandler)
algoSelectBox.addEventListener('change', selectHandler);

async function clickHanlder(event) {
    const result = await utils.getSeedFile();
    pathLabel.value = result;
}

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

    const result = await utils.start(seedPath, algo, timeSlice);
    console.log(result);
}

