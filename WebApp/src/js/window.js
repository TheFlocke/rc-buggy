import { writeArmCmd } from './main.js'

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


document.addEventListener('DOMContentLoaded', function() {
    // Set up each draggable dot
    setupDraggableDot('arm1-dot', 'arm1');
    setupDraggableDot('arm2-dot', 'arm2');
    setupDraggableDot('arm3-dot', 'arm3');

    function setupDraggableDot(dotClass, armClass) {
        const dot = document.querySelector('.' + dotClass);
        const arm = document.querySelector('.' + armClass);
        const container = dot.parentElement;
        const safeArea = {
            arm1: { min: 12, max: 180 },
            arm2: { min: 12, max: 180 },
            arm3: { min: 0, max: 180},
            arm4: { min: 90, max: 180}
        };

        const current = {
            arm1: 12,
            arm2: 12,
            arm3: 0,
            arm4: 90
        }


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

        // Event listeners
        dot.addEventListener('mousedown', onMouseDown);
        document.addEventListener('mousemove', onMouseMove);
        document.addEventListener('mouseup', onMouseUp);

        function onMouseDown(e) {
            e.preventDefault();
            isDragging = true;

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

        function onMouseMove(e) {
            if (!isDragging) return;

            const containerRect = container.getBoundingClientRect();
            const containerCenterX = containerRect.left + containerRect.width / 2;
            const containerCenterY = containerRect.top + containerRect.height / 2;

            // Calculate angle based on mouse position relative to circle center
            const angle = Math.atan2(
                e.clientY - containerCenterY,
                e.clientX - containerCenterX
            );

            console.log(parseInt(angle * (180 / Math.PI) + 12))

            positionDotOnCircle(angle);

            // Rotate the arm based on the angle
            if (armClass === 'arm1') {
                document.querySelector('.arm1-rotate').style.transform = `rotate(${calculateDegrees(angle, 'arm1')}deg)`;
                current.arm1 = calculateDegrees(angle, 'arm1')
                writeArmCmd(current.arm1+':'+current.arm2+':'+current.arm3+':'+current.arm4);
            } else if (armClass === 'arm2') {
                document.querySelector('.arm2-rotate').style.transform = `rotate(${(calculateDegrees(angle, 'arm2')) * -1}deg)`;
                current.arm2 = calculateDegrees(angle, 'arm2')
                writeArmCmd(current.arm1+':'+current.arm2+':'+current.arm3+':'+current.arm4);
            } else if (armClass === 'arm3') {
                document.querySelector('.arm3-rotate').style.transform = `rotate(${calculateDegrees(angle, 'arm3') - 90}deg)`;
                current.arm3 = calculateDegrees(angle, 'arm3')

                writeArmCmd(current.arm1+':'+current.arm2+':'+current.arm3+':'+current.arm4);
            }
        }

        function onMouseUp() {
            isDragging = false;
        }

        function calculateDegrees(angle, arm) {
            let degree = angle * (180 / Math.PI)

            if (safeArea[arm].min < degree && safeArea[arm].max > degree) {
                console.log(degree)
                return parseInt(degree)
            }

            if (safeArea[arm].min > degree && -100 < degree) {
                console.log(safeArea[arm].min)
                return safeArea[arm].min
            }
            if (safeArea[arm].max < degree || 0 > degree) {
                console.log(safeArea[arm].max)
                return safeArea[arm].max
            }
        }

        function positionDotOnCircle(angle) {
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
