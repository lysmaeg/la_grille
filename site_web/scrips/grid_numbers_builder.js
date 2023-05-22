// button which select size, penality and time
const buttonValidateData = document.querySelector("#validateData");
const solveButton = document.querySelector("#solveButton");
const form = document.querySelector("form");


let template_cell = document.createElement("input");
template_cell.classList.add("cell");
template_cell.type = "number";
template_cell.required = true;

// let time = 60 + 5
let time = 1


const sizeInput = document.querySelector("#size");
let submit = false
let cells_container = document.querySelector("#cells_container");

function clear_cells() {
    while (cells_container.lastElementChild) {
        cells_container.removeChild(cells_container.lastElementChild);
    }
}

// create grid
buttonValidateData.addEventListener("click", event => {
    if (sizeInput.value == "") {
        return;
    }
    const size = Number(sizeInput.value);
    buttonValidateData.disabled = "false";
    clear_cells();
    let d = document.createElement("div");

    for (let i = 0; i < size * size; i++) {
        let new_cell = template_cell.cloneNode(true);
        new_cell.name = i + 1;
        d.appendChild(new_cell);
        if (i % size == size - 1) {
            cells_container.appendChild(d);
            d = document.createElement("div");
        }
    }
    cells_container.appendChild(d);
})

const handleReset = (event) => {
    clear_cells();
    buttonValidateData.removeAttribute("disabled");
}

solveButton.addEventListener("click", event => {
    event.preventDefault();
    const XHR = new XMLHttpRequest();
    const FD = new FormData(form);
    XHR.addEventListener("load", (event) => {
        alert(event.target.responseText);
    });

    // On définit ce qui se produit en cas
    // d'erreur
    XHR.addEventListener("error", (event) => {
        alert('Une erreur est survenue.');
    });

    // On prépare la requête
    XHR.open("POST", "https://leria-etud.univ-angers.fr/~ddasilva/grille/solve_grid.php");

    // On envoie les données avec ce qui a été
    // fourni dans le formulaire
    XHR.send(FD);
    console.log(XHR.responseText);

    /* setTimeout(() => {
        let b = document.querySelectorAll("body");
        console.log(b);
        console.log("khfjksdhf");
        let iframe = document.querySelector("#dummyframe");
        // console.log(grid_id);
        var iframeDocument = iframe.contentDocument;
        console.log(iframeDocument);
    }, time * 1000); */
})

const httpRequest = new XMLHttpRequest();
function get(link) {
    httpRequest.open("GET", link, false);
    httpRequest.send(null);
    return httpRequest.responseText;
}


document
    .querySelector("form")
    .addEventListener("reset", handleReset);
