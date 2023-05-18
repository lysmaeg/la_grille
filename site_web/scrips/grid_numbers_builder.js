// button which select size, penality and time
const buttonValidateData = document.querySelector("#validateData");


let template_cell = document.querySelector(".cell_template");
const sizeInput = document.querySelector("#size");
let submit = false

function clear_cells() {
    const cells_container = document.querySelector("#cells_container");
    while (cells_container.lastElementChild) {
        cells_container.removeChild(cells_container.lastElementChild);
    }
}


buttonValidateData.addEventListener("click", event => {
    if (sizeInput.value == "") {
        return;
    }
    const size = Number(sizeInput.value);
    buttonValidateData.disabled = "false";
    clear_cells();
    
    for (let i = 0; i < size * size; i++) {
        let new_cell = template_cell.cloneNode(true);
        new_cell.classList.remove("cell_template");
        new_cell.classList.add("cell");
        new_cell.name = i + 1;
        cells_container.appendChild(new_cell);

    }
})

const handleReset = (event) => {
    clear_cells();
    buttonValidateData.removeAttribute("disabled");
}


document
    .querySelector("form")
    .addEventListener("reset", handleReset);
