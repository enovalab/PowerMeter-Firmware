document.addEventListener("wheel", (event) => {
    if(!event.target.closest("main")) {
        return;
    }

    const pagesValues = Object.values(pages);
    if(event.wheelDelta > 0) {
        handleTrackingSpanSelect(i => ++i);
    }
    else {
        handleTrackingSpanSelect(i => --i);
    }
    selectPage(activePageIndex);
})

let trackingSpanIndex = 0;
function handleTrackingSpanSelect(iterate) {
    trackingSpanIndex = iterate(trackingSpanIndex);
    const trackingSpansValues = Object.values(trackingSpans);
    if(trackingSpanIndex >= trackingSpansValues.length) {
        trackingSpanIndex = 0;
    }
    if(trackingSpanIndex < 0) {
        trackingSpanIndex = trackingSpansValues.length - 1;
    }

    drawChart(data, trackingSpansValues[trackingSpanIndex]);
    const title = document.getElementById("tracking-span-title");
    const trackinSpansKeys = Object.keys(trackingSpans);
    switch(trackinSpansKeys[trackingSpanIndex]) {
        case "last60min":
            title.innerText = "Last 60 Minutes";
            break;
        case "last24h":
            title.innerText = "Last 24 Hours";
            break;
        case "last7d":
            title.innerText = "Last 7 Days";
            break;
        case "last30d":
            title.innerText = "Last 30 Days";
            break;
        case "last12m":
            title.innerText = "Last 12 Months";
            break;
        case "last10a":
            title.innerText = "Last 10 Years";
    }
}