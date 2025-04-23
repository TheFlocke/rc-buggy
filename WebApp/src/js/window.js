
window.handleSpeedInput = function() {
    // noinspection JSDeprecatedSymbols
    const speed = event.target.value;
    console.log("Current speed:", speed);
};

window.handleSpeedRelease = function() {
    // noinspection JSDeprecatedSymbols
    event.target.value = 0;  // Snap back to 0
    console.log("Speed reset to 0");
};