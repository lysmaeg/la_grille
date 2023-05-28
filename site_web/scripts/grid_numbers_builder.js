// button which select size, penality and time
const buttonValidateData = document.querySelector("#validateData");
const solveButton = document.querySelector("#solveButton");
const form = document.querySelector("form");
const scoreP = document.querySelector("#score");


let template_cell = document.createElement("input");
template_cell.classList.add("cell");
template_cell.type = "text";
template_cell.inputmode = "numeric";
template_cell.pattern = "\d*";
template_cell.required = true;

let time = 1;
let sent = false;

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
    if (sent) {
        alert("Vous venez déjà de demander la résolution d'une grille, veuillez attendre.");
        return;
    }
    if (cells_container.childElementCount == 0) {
        alert("Veuillez générer une grille pour en résoudre une.");
        return;
    }
    sent = true;
    const XHR = new XMLHttpRequest();
    const FD = new FormData(form);
    XHR.addEventListener("load", (event) => {
        let gridWithScore =  event.target.responseText;
        sent = false;
        let eofEscaped = 0;
        for (let x = 0; x < size ; x++) {
            for (let i = 0; i < size; i++) {
                let n = x*size +i +1;
                switch (gridWithScore[x*size + i + eofEscaped]) {
                    case 'R':
                        document.getElementById(n).style.background = 'red';
                        document.getElementById(n).style.color = 'black';
                        break; 

                    case 'N':
                        document.getElementById(n).style.background = 'black';
                        document.getElementById(n).style.color = 'white';
                        break; 
                    case 'B':
                        document.getElementById(n).style.background = 'rgb(42, 127, 255)';
                        document.getElementById(n).style.color = 'black';
                        break; 
                    case 'O':
                        document.getElementById(n).style.background = 'orange';
                        document.getElementById(n).style.color = 'black';
                        break; 
                    case 'J':
                        document.getElementById(n).style.background = 'yellow';
                        document.getElementById(n).style.color = 'black';
                        break; 
                    case 'V':
                        document.getElementById(n).style.background = 'rgb(18, 197, 18)';
                        document.getElementById(n).style.color = 'black';
                        break; 
                  }        
            }
            eofEscaped++;
        }
        let score = Number(gridWithScore.substring(size * (size + 1)))
        scoreP.innerHTML = "Score : "+ "<b>"+score+"</b>";
    });

    // On définit ce qui se produit en cas
    // d'erreur
    XHR.addEventListener("error", (event) => {
        alert('Un erreur est survenur, le problème peut provenir du fait que vous exécuter le site en local, pour pouvoir exécuter une grille, veuillez aller sur le site suivant : https://leria-etud.univ-angers.fr/~ddasilva/grille/grille.html');
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
