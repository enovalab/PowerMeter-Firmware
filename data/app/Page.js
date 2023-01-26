class Page {
    constructor(titleText, titleQuerySelector, pageQuerySelector, menuQuerySelector, onActivation = () => {}) {
        this.titleText = titleText;
        this.title = document.querySelector(titleQuerySelector);
        this.page = document.querySelector(pageQuerySelector);
        this.menu = document.querySelector(menuQuerySelector);
        this.onActivation = onActivation;
    }

    setActive(active) {
        if(active) {
            this.page.style = "display: flex";
            this.menu.style = "visibility: visible";
            this.title.innerText = this.titleText;
            this.onActivation();
        }
        else {
            this.page.style = "display: none";
            this.menu.style = "visibility: hidden";
        }
    }
}