const trackingSpans = new CircularIteratableArray(
    new TrackingSpan("Last 60 Minutes", "#tracking-span-title", 60, "#chart", "rgb(100, 255, 200"),
    new TrackingSpan("Last 24 Hours", "#tracking-span-title", 24, "#chart", "rgb(255, 92, 92)"),
    new TrackingSpan("Last 7 Days", "#tracking-span-title", 7, "#chart", "rgb(157, 92, 255)"),
    new TrackingSpan("Last 30 Days", "#tracking-span-title", 30, "#chart", "rgb(92, 255, 134)"),
    new TrackingSpan("Last 12 Months", "#tracking-span-title", 12, "#chart", "rgb(214, 255, 92)"),
    new TrackingSpan("Last 10 Years", "#tracking-span-title", 10, "#chart", "rgb(255, 130, 92)")
);

trackingSpans.getActive().getChart();

document.addEventListener("wheel", (event) => {
    if(!event.target.closest("main") && pages.activeIndex !== 1) {
        return;
    }

    if(event.wheelDelta > 0) {
        trackingSpans.next();
    }
    else {
        trackingSpans.previous();
    }
    trackingSpans.getActive().getChart();
});

document.getElementById("next-tracking-span").addEventListener("click", (event) => {
    trackingSpans.next();
    trackingSpans.getActive().getChart();
});

document.getElementById("previous-tracking-span").addEventListener("click", (event) => {
    trackingSpans.previous();
    trackingSpans.getActive().getChart();
});