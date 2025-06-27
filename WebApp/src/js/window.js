import {writeCmd} from './main.js'


let currentSpeed = {
    speed0: 0,
    speed1: 0,
    wheel0: 0,
    wheel1: 0
}

let safeArea = {
    arm: {
        0: {
            min: 0,
            max: 180
        },
        1: {
            min: 0,
            max: 180
        },
        2: {
            min: 0,
            max: 180
        },
        3: {
            min: 0,
            max: 180
        }
    },
    wheel: {
        0: {
            min: 0,
            mid: 90,
            max: 180
        },
        1: {
            min: 0,
            mid: 90,
            max: 180
        }
    }
}


window.handleSpeedInput = function (id) {
    // noinspection JSDeprecatedSymbols
    const speed = event.target.value;
    currentSpeed[id] = speed
    writeCmd(id + ':' + currentSpeed[id])

    console.log("ID: " + id + " -- Current speed:", speed);
};

window.handleSpeedRelease = function (id) {
    // noinspection JSDeprecatedSymbols
    event.target.value = 0;  // Snap back to 0
    currentSpeed[id] = 0
    writeCmd(id + ':' + currentSpeed[id])

    console.log("ID: " + id + " -- Speed reset to 0");
};

window.handleDegreeInput = function (part, number, limit, event) {
    let angle = Number(event.target.value);
    const min = Number(event.target.min);
    const max = Number(event.target.max);

    // Validate the value is a number
    if (isNaN(angle)) {
        console.error('Invalid angle value');
        return;
    }

    // if angle is a integer convert it into a number
    if (!Number.isInteger(angle)) {
        angle = Math.trunc(angle);
        console.warn(`Number was converted into a Integer: ${angle}`);
        event.target.value = angle;
    }

    if (angle <= min) {
        angle = min;
        event.target.value = min;
    } else if (angle >= max) {
        angle = max;
        event.target.value = max;
    }

    safeArea[part][number][limit] = angle;
    console.log(`${part}${number} -- ${limit} ==> updated value: ${angle}`);

    // Create a unique key for this input
    const key = `${part}_${number}_${limit}`;
    // save the value to localStorage
    localStorage.setItem(key, angle);
}


document.addEventListener('DOMContentLoaded', function () {

    const current = {
        arm0: 45,
        arm1: 45,
        arm2: 45,
        arm3: 45
    }

    document.querySelectorAll('input[type="number"][data-part][data-number][data-limit]').forEach(input => {
        const part = input.dataset.part;
        const number = input.dataset.number;
        const limit = input.dataset.limit;
        // Create a unique key for this input
        const key = `${part}_${number}_${limit}`;
        // get the value from localStorage, if it exists
        const savedValue = localStorage.getItem(key);
        // only if a value is saved in localStorage, set the input value to it
        if (savedValue !== null) {
            input.value = savedValue;
            // Update your safeArea object as well:
            if (window.safeArea && window.safeArea[part] && window.safeArea[part][number]) {
                window.safeArea[part][number][limit] = Number(savedValue);
            }
        }
    });


    // Set up each draggable dot
    setupDraggableDot('arm0-dot', 0);
    setupDraggableDot('arm1-dot', 1);
    setupDraggableDot('arm2-dot', 2);
    setupDraggableDot('arm3-dot', 3)

    function setupDraggableDot(dotClass, armClass) {
        let setup = true;
        const dot = document.querySelector('.' + dotClass);
        const container = dot.parentElement;

        // Center coordinates of the circle
        const center = {
            x: container.offsetWidth / 2,
            y: container.offsetHeight / 2
        };

        // Radius of the circle (half the container width)
        const radius = container.offsetWidth / 2;

        // Variables to track the dragging state
        let isDragging = false;
        let startAngle = 0;

        // Initial position (top of the circle)
        positionDotOnCircle(0);

        // Mouse event listeners
        dot.addEventListener('mousedown', onStart);
        document.addEventListener('mousemove', onMove);
        document.addEventListener('mouseup', onEnd);

        // Touch event listeners
        dot.addEventListener('touchstart', onStart);
        document.addEventListener('touchmove', onMove);
        document.addEventListener('touchend', onEnd);
        document.addEventListener('touchcancel', onEnd);

        function onStart(e) {
            e.preventDefault();
            isDragging = true;

            // Calculate the current angle based on the dot position
            const rect = dot.getBoundingClientRect();
            const dotCenterX = rect.left + rect.width / 2;
            const dotCenterY = rect.top + rect.height / 2;

            const containerRect = container.getBoundingClientRect();
            const containerCenterX = containerRect.left + containerRect.width / 2;
            const containerCenterY = containerRect.top + containerRect.height / 2;

            startAngle = Math.atan2(
                dotCenterY - containerCenterY,
                dotCenterX - containerCenterX
            );
        }

        function onMove(e) {
            if (!isDragging) return;

            // Prevent default to stop scrolling on touch devices
            e.preventDefault();

            // Get clientX and clientY regardless of event type
            const clientX = e.clientX || (e.touches && e.touches[0] ? e.touches[0].clientX : 0);
            const clientY = e.clientY || (e.touches && e.touches[0] ? e.touches[0].clientY : 0);

            const containerRect = container.getBoundingClientRect();
            const containerCenterX = containerRect.left + containerRect.width / 2;
            const containerCenterY = containerRect.top + containerRect.height / 2;

            // Calculate an angle based on pointer position relative to a circle center
            const angle = Math.atan2(
                clientY - containerCenterY,
                clientX - containerCenterX
            );

            positionDotOnCircle(angle);

            // Rotate the arm based on the angle
            if (armClass === 0) {
                document.querySelector('.arm0-rotate').style.transform = `rotate(${calculateDegrees(angle, 0)}deg)`;
                current.arm0 = calculateDegrees(angle, 0);
                writeCmd(4 + ":" + current.arm0);
            } else if (armClass === 1) {
                document.querySelector('.arm1-rotate').style.transform = `rotate(${(calculateDegrees(angle, 1)) * -1}deg)`;
                current.arm1 = calculateDegrees(angle, 1);
                writeCmd(5 + ":" + current.arm1);
            } else if (armClass === 2) {
                document.querySelector('.arm2-rotate').style.transform = `rotate(${calculateDegrees(angle, 2) - 90}deg)`;
                current.arm2 = calculateDegrees(angle, 2);
                writeCmd(6 + ":" + current.arm2);
            } else if (armClass === 3) {
                current.arm3 = calculateDegrees(angle, 3, 'notmain');
                writeCmd(7 + ":" + current.arm3);
            }
        }

        function onEnd() {
            isDragging = false;
        }

        function calculateDegrees(angle, arm) {
            let degree = angle * (180 / Math.PI);

            if (safeArea.arm[arm].min < degree && safeArea.arm[arm].max > degree) {
                return parseInt(degree);
            }

            if (safeArea.arm[arm].min > degree && -100 < degree) {
                return safeArea.arm[arm].min;
            }
            if (safeArea.arm[arm] < degree || 0 > degree) {
                return safeArea.arm[arm].max;
            }
        }

        function positionDotOnCircle(angle) {
            if (setup) {
                angle = (safeArea.arm[armClass].min) / (180 * Math.PI);
                setup = false;
            }

            // Calculate position on the circle based on an angle
            const x = center.x + radius * Math.cos(angle);
            const y = center.y + radius * Math.sin(angle);

            // Position the dot
            dot.style.left = `${x}px`;
            dot.style.top = `${y}px`;
            dot.style.transform = 'translate(-50%, -50%)';
        }
    }
});
