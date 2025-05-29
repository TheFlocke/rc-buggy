import { writeCmdArm, writeCmdDrive } from './main.js'


let currentSpeed = {
    speed1: 0,
    speed2: 0,
    wheel1: 0,
    wheel2: 0
}

window.handleSpeedInput = function(motor) {
    // noinspection JSDeprecatedSymbols
    const speed = event.target.value;
    currentSpeed[motor] = speed
    console.log("Motor: "+motor+" -- Current speed:", speed);
    writeCmdDrive(currentSpeed.speed1+':'+currentSpeed.speed2+':'+currentSpeed.wheel1+':'+currentSpeed.wheel2)
};

window.handleSpeedRelease = function(motor) {
    // noinspection JSDeprecatedSymbols
    event.target.value = 0;  // Snap back to 0
    currentSpeed[motor] = 0
    writeCmdDrive(currentSpeed.speed1+':'+currentSpeed.speed2+':'+currentSpeed.wheel1+':'+currentSpeed.wheel2)
    console.log("Motor: "+ motor +" -- Speed reset to 0");
};


document.addEventListener('DOMContentLoaded', function() {

    const current = {
        arm1: 12,
        arm2: 12,
        arm3: 0,
        arm4: 90
    }

    // Set up each draggable dot
    setupDraggableDot('arm1-dot', 'arm1');
    setupDraggableDot('arm2-dot', 'arm2');
    setupDraggableDot('arm3-dot', 'arm3');
    setupDraggableDot('arm4-dot', 'arm4')

    function setupDraggableDot(dotClass, armClass) {
        let setup = true;
        const dot = document.querySelector('.' + dotClass);
        const arm = document.querySelector('.' + armClass);
        const container = dot.parentElement;
        const safeArea = {
            arm1: { min: 12, max: 180 },
            arm2: { min: 12, max: 180 },
            arm3: { min: 0, max: 180},
            arm4: { min: 90, max: 180}
        };

        // Center coordinates of the circle
        const center = {
            x: container.offsetWidth / 2,
            y: container.offsetHeight / 2
        };

        // Radius of the circle (half the container width)
        const radius = container.offsetWidth / 2;

        // Variables to track dragging state
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

            // Get clientX and clientY regardless of event type
            const clientX = e.clientX || (e.touches && e.touches[0] ? e.touches[0].clientX : 0);
            const clientY = e.clientY || (e.touches && e.touches[0] ? e.touches[0].clientY : 0);

            // Calculate the current angle based on dot position
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

            // Calculate angle based on pointer position relative to circle center
            const angle = Math.atan2(
                clientY - containerCenterY,
                clientX - containerCenterX
            );

            positionDotOnCircle(angle);

            // Rotate the arm based on the angle
            if (armClass === 'arm1') {
                document.querySelector('.arm1-rotate').style.transform = `rotate(${calculateDegrees(angle, 'arm1')}deg)`;
                current.arm1 = calculateDegrees(angle, 'arm1');
                writeCmdArm(current.arm1+':'+current.arm2+':'+current.arm3+':'+current.arm4);
            } else if (armClass === 'arm2') {
                document.querySelector('.arm2-rotate').style.transform = `rotate(${(calculateDegrees(angle, 'arm2')) * -1}deg)`;
                current.arm2 = calculateDegrees(angle, 'arm2');
                writeCmdArm(current.arm1+':'+current.arm2+':'+current.arm3+':'+current.arm4);
            } else if (armClass === 'arm3') {
                document.querySelector('.arm3-rotate').style.transform = `rotate(${calculateDegrees(angle, 'arm3') - 90}deg)`;
                current.arm3 = calculateDegrees(angle, 'arm3');
                writeCmdArm(current.arm1+':'+current.arm2+':'+current.arm3+':'+current.arm4);
            } else if (armClass === 'arm4') {
                current.arm4 = calculateDegrees(angle, 'arm4', 'notmain');
                writeCmdArm(current.arm1+':'+current.arm2+':'+current.arm3+':'+current.arm4);
            }
        }

        function onEnd(e) {
            isDragging = false;
        }

        function calculateDegrees(angle, arm) {
            let degree = angle * (180 / Math.PI);

            if (safeArea[arm].min < degree && safeArea[arm].max > degree) {
                return parseInt(degree);
            }

            if (safeArea[arm].min > degree && -100 < degree) {
                return safeArea[arm].min;
            }
            if (safeArea[arm].max < degree || 0 > degree) {
                return safeArea[arm].max;
            }
        }

        function positionDotOnCircle(angle) {
            if (setup) {
                angle = (safeArea[armClass].min)/ (180 * Math.PI);
                setup = false;
            }

            // Calculate position on the circle based on angle
            const x = center.x + radius * Math.cos(angle);
            const y = center.y + radius * Math.sin(angle);

            // Position the dot
            dot.style.left = `${x}px`;
            dot.style.top = `${y}px`;
            dot.style.transform = 'translate(-50%, -50%)';
        }
    }
});
