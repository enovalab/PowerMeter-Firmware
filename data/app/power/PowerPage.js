const relaySwitch = new RelaySwitch("#switch", "#power-page tr", function() {});

document.addEventListener("keydown", (event) => {
    if(pages.activeIndex === 0 && event.key === ' ') {
        relaySwitch.toggle();
    }
});

relaySwitch.setState(true);