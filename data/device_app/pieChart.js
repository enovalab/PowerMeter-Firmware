const ctx = document.getElementById("myChart");

const myChart = new Chart(ctx, {
    type: 'bar',
    data: {
        labels: yearLabels,
        datasets: [{
            data: yearHistory,
            backgroundColor: [
                'rgba(255, 99, 132, 0.2)',
                'rgba(255, 159, 64, 0.2)',
                'rgba(255, 205, 86, 0.2)',
                'rgba(75, 192, 192, 0.2)',
                'rgba(54, 162, 235, 0.2)',
                'rgba(153, 102, 255, 0.2)',
                'rgba(201, 203, 207, 0.2)',
                'rgba(201, 203, 207, 0.2)',
                'rgba(201, 203, 207, 0.2)',
                'rgba(201, 203, 207, 0.2)',
                'rgba(201, 203, 207, 0.2)',
                'rgba(201, 203, 207, 0.2)'
            ],
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            xAxes: [{
                display: true,
                gridLines: {
                    display: true,
                    color: 'rgba(255, 255, 255, 0.2)'
                },
                scaleLabel: {
                    display: true,
                    color: '#ffffff',
                    labelString: 'Month'
                },
                ticks: { color: '#ffffff', beginAtZero: true }
            }],
            yAxes: [{
                display: true,
                gridLines: {
                    display: true,
                    color: 'rgba(255, 255, 255, 0.2)'
                },
                scaleLabel: {
                    display: true,
                    labelString: 'Power Consuption'
                },
                ticks: { color: '#ffffff', beginAtZero: true }
            }]
        },
        legend: {
            display: false
        },
        elements: {
            arc: {
                borderWidth: 0
            }
        },
        plugins: {
            tooltip:{
                enabled: false
            }
        }
    },
});
