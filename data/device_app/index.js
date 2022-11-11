var yearHistory = [122, 99, 320, 150, 139, 49, 72, 130, 91, 111, 121, 131];
var yearLabels = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'];

const momentaryDataObj = {
    RealPower: 22.35,
    ApparentPower: 31.97,
    PowerFactor: 0.78
}

function calcReactivePower(realPower, apparentPower) {
    return Math.sqrt(apparentPower ** 2 - realPower ** 2);
}