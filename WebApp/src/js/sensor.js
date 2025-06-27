export function updateIAQIcon(iaqValue) {
    const icon = document.getElementById('iaq_icon');
    icon.classList.remove('iaq-good', 'iaq-moderate', 'iaq-bad', 'iaq-disconnected');
    if (iaqValue === -1) {
        icon.classList.add('iaq-disconnected');
    } else if (iaqValue < 100) {
        icon.classList.add('iaq-good');
    } else if (iaqValue < 200) {
        icon.classList.add('iaq-moderate');
    } else {
        icon.classList.add('iaq-bad');
    }
}
