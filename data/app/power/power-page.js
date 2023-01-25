const relaySwitch = document.getElementById("switch");

relaySwitch.checked = true;

function handleRelayState() {
    const switchableElements = document.querySelectorAll("#power-page tr");
    if(relaySwitch.checked) {
        switchableElements.forEach((element) => {
            element.style["color"] = "var(--text-color)"
            element.style["text-shadow"] = "2px 2px 10px var(--text-color)";
        });
    }
    else {
        switchableElements.forEach((element) => {
            element.style["color"] = "#a4a4a4";
            element.style["text-shadow"] = "none";
        });
    }
}

document.addEventListener("keydown", (event) => {
    if(activePageIndex === 0 && event.key === ' ') {
        relaySwitch.checked = !relaySwitch.checked;
    }
})