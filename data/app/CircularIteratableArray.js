class CircularIteratableArray extends Array {
    activeIndex = 0;
    setActive(activeIndex) {
        this.activeIndex = activeIndex;
    }

    getActive() {
        return this[this.activeIndex];
    }

    next() {
        if(++this.activeIndex >= this.length) {
            this.activeIndex = 0;
        }
        return this[this.activeIndex];
    }

    previous() {
        if(--this.activeIndex < 0) {
            this.activeIndex = this.length - 1;
        }
        return this[this.activeIndex];
    }
}