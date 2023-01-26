class TrackingSpan {
    constructor(titleText, titleQuerySelector, numSamples, chartContextQuerySelector, color) {
        this.numSamples = numSamples;
        this.title = document.querySelector(titleQuerySelector);
        this.titleText = titleText;
        this.chartContext = document.querySelector(chartContextQuerySelector);
        this.color = color;
    }

    fetchData() {
        return [12, 34, 36, 89, 23, 32, 45];
    }

    getChart() {
        this.title.innerText = this.titleText;
        const constrastColor = getComputedStyle(document.body).getPropertyValue("--contrast-color");

        Chart.defaults.global.defaultFontColor = addAlphaToRGB(constrastColor, 0.6);
        Chart.defaults.global.defaultFontFamily = "Dosis";
        Chart.defaults.global.defaultFontSize = 14;

        return new Chart(this.chartContext, {
            type: 'line',
            data: {
                labels: this.getLabels(),
                datasets: [{
                    label: "Active Power",
                    data: this.fetchData(),
                    borderWidth: 1,
                    borderColor: this.color,
                    backgroundColor: addAlphaToRGB(this.color, 0.1)
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
                            maxTicksLimit: 15
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

    getLabels() {
        let labels = [];
        for (let i = 0; i < this.numSamples; i++) {
            let now = new Date();
            switch(this.numSamples) {
                case 60:
                    now.setMinutes(now.getMinutes() - i);
                    labels.unshift(now.toLocaleTimeString([], {hour: "2-digit", minute: "2-digit"}));
                    break;
                case 24:
                    now.setHours(now.getHours() - i);
                    labels.unshift(now.toLocaleTimeString([], {hour: "2-digit", minute: "2-digit"}));
                    break;
                case 7:
                    now.setDate(now.getDate() - i);
                    labels.unshift(now.toLocaleDateString([], {day: "2-digit", month: "short"}));
                    break;
                case 30:
                    now.setDate(now.getDate() - i);
                    labels.unshift(now.toLocaleDateString([], {day: "2-digit", month: "short"}));
                    break;
                case 12:
                    now.setMonth(now.getMonth() - i);
                    labels.unshift(now.toLocaleDateString([], {month: "short", year: "2-digit"}));
                    break;
                case 10:
                    now.setFullYear(now.getFullYear() - i);
                    labels.unshift(now.toLocaleDateString([], {year: "numeric"}));
                    break;
            }
        }
        return labels;
    }
}