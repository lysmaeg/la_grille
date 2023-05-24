// button which select size, penality and time
const buttonValidateData = document.querySelector("#validateData");
const solveButton = document.querySelector("#solveButton");
const form = document.querySelector("form");
const solution_container = document.querySelector("solution_container");
const scoreP = document.querySelector("#score");


let template_cell = document.createElement("input");
template_cell.classList.add("cell");
template_cell.type = "number";
template_cell.required = true;

// let time = 60 + 5
let time = 1;


let size = 0;
const sizeInput = document.querySelector("#size");
let submit = false;
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
    size = Number(sizeInput.value);
    buttonValidateData.disabled = "false";
    clear_cells();
    let d = document.createElement("div");

    for (let i = 0; i < size * size; i++) {
        let new_cell = template_cell.cloneNode(true);
        let n = i+1;
        new_cell.name = n;
        new_cell.id = n;
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
        let gridWithScore =  event.target.responseText;
        alert(gridWithScore);
        let eofEscaped = 0;
        for (let x = 0; x < size ; x++) {
            let spanContainer = document.createElement("div");
            for (let i = 0; i < size; i++) {
                let span = document.createElement("span");
                let n = x*size +i +1;
                switch (gridWithScore[x*size + i + eofEscaped]) {
                    case 'R':
                        document.getElementById(n).style.background = 'red';
                        break; 

                    case 'N':
                        document.getElementById(n).style.background = 'black';
                        break; 
                    case 'B':
                        document.getElementById(n).style.background = 'blue';
                        break; 
                    case 'O':
                        document.getElementById(n).style.background = 'orange';
                        break; 
                    case 'J':
                        document.getElementById(n).style.background = 'yellow';
                        break; 
                    case 'V':
                        document.getElementById(n).style.background = 'green';
                        break; 
                  }        
                //spanContainer.appendChild(span);
            }
            //solution_container.appendChild(spanContainer);
            eofEscaped++;
        }
        let score = Number(gridWithScore.substring(size * (size + 1)))
        scoreP.innerHTML = "Score : "+ score;
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
