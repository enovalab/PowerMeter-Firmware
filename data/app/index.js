const pages = new CircularIteratableArray(
    new Page("Power", "#page-title", "#power-page", "#power-menu"),
    new Page("Tracker", "#page-title", "#tracker-page", "#tracker-menu"),
    new Page("WiFi-Settings", "#page-title", "#wifi-page", "#wifi-menu"),
    new Page("Configuration", "#page-title", "#config-page", "#config-menu"),
);

activateOnlyActive(pages);

document.addEventListener("wheel", (event) => {
    if(!event.target.closest("footer")) {
        return;
    }

    if(event.wheelDelta > 0) {
        pages.next();
    }
    else {
        pages.previous();
    }
    activateOnlyActive(pages);
});

document.querySelectorAll("footer div").forEach((footerDiv, index) => {
    footerDiv.addEventListener("click", (event) => {
        pages.setActive(index);
        activateOnlyActive(pages);
    });
});


function activateOnlyActive(pages) {
    pages.forEach((page) => {
        if(page == pages.getActive()) {
            page.setActive(true);
        }
        else {
            page.setActive(false);
        }
    });   
}

function addAlphaToRGB(rgb, alpha) {
    if (alpha < 0) {
        alpha = 0;
    } else if (alpha > 1) {
        alpha = 1;
    }
    let colorArray = rgb.match(/\d+/g);
    return `rgba(${colorArray[0]}, ${colorArray[1]}, ${colorArray[2]}, ${alpha})`;
}