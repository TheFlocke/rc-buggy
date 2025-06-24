import {errorMessageContainer, infoMessageContainer} from "./main.js";

export function setInfo(value) {
    infoMessageContainer.innerHTML = value;
    console.info(value);
}

export function setError(value) {
    errorMessageContainer.innerHTML = value;
    console.error(value);
}