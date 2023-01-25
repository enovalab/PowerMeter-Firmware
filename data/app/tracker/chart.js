const trackingSpans = {
    last60min: 60,
    last24h: 24,
    last7d: 7,
    last30d: 30,
    last12m: 12,
    last10a: 10
};

const data = [
    10,
    20,
    23,
    44,
    24,
    24,
    32,
    9
];

const constrastColor = getComputedStyle(document.body).getPropertyValue("--contrast-color");
Chart.defaults.global.defaultFontColor = addAlphaToRGB(constrastColor, 0.6);
Chart.defaults.global.defaultFontFamily = "Dosis";
Chart.defaults.global.defaultFontSize = 14;

drawChart(data, trackingSpans.last7d);

function drawChart(data, trackingSpan) {
    const color = "rgb(100, 12, 230)"
    let xAxisMaxTicks;
    if(trackingSpan === trackingSpans.last60min) {
        xAxisMaxTicks = trackingSpan / 4;
    }
    else {
        xAxisMaxTicks = trackingSpan / 2;
    }
    return new Chart("chart", {
        type: 'line',
        data: {
            labels: generateLabels(trackingSpan),
            datasets: [{
                label: "Active Power",
                data: data,
                borderWidth: 1,
                borderColor: color,
                backgroundColor: addAlphaToRGB(color, 0.1)
            }]
        },
        options: {
            responsive: true,
            legend: {
                display: false
            },
            scales: {
                xAxes: [{
                    ticks: {
                        maxTicksLimit: xAxisMaxTicks
                    },
                    gridLines: {
                        color: addAlphaToRGB(constrastColor, 0.1)
                    }
                }],
                yAxes: [{
                    ticks: {
                        beginAtZero: true
                    },
                    scaleLabel: {
                        display: true,
                        labelString: "Power in W"
                    },
                    gridLines: {
                        color: addAlphaToRGB(constrastColor, 0.1)
                    }
                }]
            }
        }
    });
}


function generateLabels(numLabels) {
    let labels = [];
    for (let i = 0; i < numLabels; i++) {
        let now = new Date();
        switch(numLabels) {
            case trackingSpans.last60min:
                now.setMinutes(now.getMinutes() - i);
                labels.unshift(now.toLocaleTimeString([], {hour: "2-digit", minute: "2-digit"}));
                break;
            case trackingSpans.last24h:
                now.setHours(now.getHours() - i);
                labels.unshift(now.toLocaleTimeString([], {hour: "2-digit", minute: "2-digit"}));
                break;
            case trackingSpans.last7d:
                now.setDate(now.getDate() - i);
                labels.unshift(now.toLocaleDateString([], {day: "2-digit", month: "short"}));
                break;
            case trackingSpans.last30d:
                now.setDate(now.getDate() - i);
                labels.unshift(now.toLocaleDateString([], {day: "2-digit", month: "short"}));
                break;
            case trackingSpans.last12m:
                now.setMonth(now.getMonth() - i);
                labels.unshift(now.toLocaleDateString([], {month: "short", year: "2-digit"}));
                break;
            case trackingSpans.last10a:
                now.setFullYear(now.getFullYear() - i);
                labels.unshift(now.toLocaleDateString([], {year: "numeric"}));
                break;
        }
    }
    return labels;
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
