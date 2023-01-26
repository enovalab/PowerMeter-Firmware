class RelaySwitch {
    constructor(checkboxQuerySelector, targetsQuerySelector, onStateChange = () => {}) {
        this.checkbox = document.querySelector(checkboxQuerySelector);
        this.targets = document.querySelectorAll(targetsQuerySelector);
        this.onStateChange = onStateChange;

        this.checkbox.addEventListener("click", (event) => {
            this.setState(this.getState());
        });
    }

    toggle() {
        this.setState(!this.getState());
    }

    getState() {
        
        return this.checkbox.checked;
    }

    setState(state) {
        this.checkbox.checked = state;
        if(state) {
            this.targets.forEach((target) => {
                target.style["color"] = "var(--text-color)"
                target.style["text-shadow"] = "2px 2px 10px var(--text-color)";
            });
        }
        else {
            this.targets.forEach((target) => {
                target.style["color"] = "#a4a4a4";
                target.style["text-shadow"] = "none";
            });
        }
        this.onStateChange(state);
    }

    updateTargets() {
        
    }
}