function loadCards()
{
    // Wenn Power Meters im localStorage gespeichert sind
    if(localStorage.powerMeters != undefined) {
        const powerMeters = JSON.parse(localStorage.powerMeters);
        
        // Power Meters anzeigen
        powerMeters.forEach(function(powerMeter){
            addCard(powerMeter.name, powerMeter.url);
        });
    }
}

function handleDelete(event, name) {
    event.stopPropagation();
    if(!confirm(`Are you shure you want to delete "${name}"?`)) return;
    let powerMeters = JSON.parse(localStorage.powerMeters);

    // Nach Index mit dem Namen suchen und das Element löschen
    const index = powerMeters.findIndex(powerMeter => powerMeter.name == name);
    powerMeters.splice(index, 1);
    localStorage.powerMeters = JSON.stringify(powerMeters);
    deleteCard(name);
}

function handleAdd() {
    const nameInput = document.getElementById('name-input');
    const urlInput = document.getElementById('url-input');

    const name = nameInput.value;
    const url = urlInput.value;

    if(localStorage.powerMeters == undefined) {
        localStorage.powerMeters = "[]";
    }

    // Überprüfen ob Name bereits existiert
    if(localStorage.powerMeters.search(name) == -1) {
        // wenn nein, dann neues powerMeter hinzufügen und speichern
        let powerMeters = JSON.parse(localStorage.powerMeters);
        const newPowerMeter = {name: name, url: url};
        powerMeters.push(newPowerMeter);        
        localStorage.powerMeters = JSON.stringify(powerMeters);
        addCard(name, url);
        closePopup();
    }
    else {
        // wenn ja, alert ausgeben
        alert("Please enter a different name");
        nameInput.value = "";
        return;
    }

    // Text aus Eingabefeldern entfernen
    nameInput.value = "";
    urlInput.value = "";
}

function openPopup() {
    document.getElementById('popup').style.display = 'flex';
}

function closePopup() {
    document.getElementById('popup').style.display = 'none';
}

function addCard(name, url) {
    const htmlString = 
    `<div class="card" id="card-${name}" onclick="location.href='http://${url}';"&nbsp;>
        ${name}
        <i class="material-icons" onclick="handleDelete(event,'${name}')">delete</i>
    </div>`;

    document.getElementById('card-total').insertAdjacentHTML("afterend", htmlString);
}

function deleteCard(name) {
    document.getElementById('card-' + name).remove();
}
