const pages = {
    power: {
        title: "Power",
        content: document.getElementById("power-page"),
        selector: document.getElementById("power-selector")
    },
    tracker: {
        title: "Tracker",
        content: document.getElementById("tracker-page"),
        selector: document.getElementById("tracker-selector")
    },
    wifi: {
        title: "WiFi-Settings",
        content: document.getElementById("wifi-page"),
        selector: document.getElementById("wifi-selector")
    },
    config: {
        title: "Configuration",
        content: document.getElementById("config-page"),
        selector: document.getElementById("config-selector")
    },
}

let activePageIndex = 0;
selectPage(activePageIndex);

Object.values(pages).forEach((page, index) => {
    document.addEventListener("keydown", (event) => {
        if (event.key === (index + 1).toString()) {
            selectPage(index);
        }
    })
    page.selector.addEventListener("click", (event) => {
        selectPage(index);
    });
});

function selectPage(index) {
    activePageIndex = index;
    const pagesValues = Object.values(pages);
    pagesValues.forEach((page) => {
        page.selector.style = "background-color: var(--main-color)";
        page.content.style = "display: none";
    });
    pagesValues[index].selector.style = "background-color: var(--accent-color)";
    pagesValues[index].content.style = "display: flex";
    document.getElementById("page-title").innerText = pagesValues[index].title;
}

document.addEventListener("wheel", (event) => {
    if(!event.target.closest("footer")) {
        return;
    }

    const pagesValues = Object.values(pages);
    if(event.wheelDelta > 0) {
        activePageIndex++;
        if(activePageIndex >= pagesValues.length) {
            activePageIndex = 0
        }
    }
    else {
        activePageIndex--;
        if(activePageIndex < 0) {
            activePageIndex = pagesValues.length - 1;
        }
    }
    selectPage(activePageIndex);
})