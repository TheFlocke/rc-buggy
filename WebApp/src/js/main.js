export const version = "0.5";

let connectButton;
let disconnectButton;
let versionDisplay;
let bleStateContainer;
let bleCharCmdDrive;
let bleCharCmdDriveState;
let bleCharCmdArm;
let bleCharCmdArmState;
let bleServiceContainer;
let bleServiceList;
let bleSendHistory;
let bleReceiveHistory;
let orientation;
let orientationDevice;
let page0;
let page1;

let retrievedValue;
let latestValueSent;
let sentTimestamp;
let retrievedTimestamp;

let errorMessageContainer;
let infoMessageContainer;

// Define BLE Device Specs
// CMD Service
let UUID_SERVICE_CMD = "5eaf1079-e806-47a9-a1ec-d815bea94805"
let UUID_CHAR_CMD_DRIVE = "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3"
let UUID_CHAR_CMD_DRIVE_STATE = "bd6fbfde-385d-480f-b5eb-64d60cc7be9a"
let UUID_CHAR_CMD_ARM = "99d69805-8efb-450e-ae78-c4ddba09f7f6"
let UUID_CHAR_CMD_ARM_STATE = "f8765d0c-81b5-4780-85a4-44f0999f5474"
// Sensor Service
let UUID_SENSOR_SERVICE_UUID = "8da7a992-e263-4b78-abf2-bdb94808895c"
let UUID_SENSOR_TEMP = "24c53354-de00-42ac-926c-31f805e5d2f5"
let UUID_SENSOR_PRESSURE = "545343fb-93a0-4415-9e2b-6a4c8e2835c4"
let UUID_SENSOR_HUMIDITY = "618c8e95-e436-4a86-9d7d-17c3db9992d0"
let UUID_SENSOR_GAS = "aa4ce7cf-fff0-4d54-b4ec-fb24920b35c1"


// Global variables to Handle Bluetooth
let bleServer;
let bleCmdService;
let bleSensorService;
// CMD
let CharacteristicCmdDrive;
let CharacteristicCmdDriveState;
let CharacteristicCmdArm;
let CharacteristicCmdArmState;
// Sensor
let CharacteristicSensorTemp;
let CharacteristicSensorPressure;
let CharacteristicSensorHumidity;
let CharacteristicSensorGas;

// orientation information
let orientationFrame;
let orientationCenter;

window.onload = () => {
    registerServiceWorker();

    connectButton = document.getElementById('connectBleButton');
    disconnectButton = document.getElementById('disconnectBleButton');
    retrievedValue = document.getElementById('retrievedValue');
    latestValueSent = document.getElementById('valueSent');
    bleStateContainer = document.getElementById('bleState');
    bleServiceContainer = document.getElementById('bleService');
    bleCharCmdDrive = document.getElementById('bleCharCmdDrive');
    bleCharCmdDriveState = document.getElementById('bleCharCmdDriveState');
    bleCharCmdArm = document.getElementById('bleCharCmdArm');
    bleCharCmdArmState = document.getElementById('bleCharCmdArmState')
    bleServiceList = document.getElementById('bleServiceList');
    bleSendHistory = document.getElementById('sendHistory');
    bleReceiveHistory = document.getElementById('receiveHistory')
    sentTimestamp = document.getElementById('sent_timestamp');
    retrievedTimestamp = document.getElementById('retrieved_timestamp');
    errorMessageContainer = document.getElementById('errors');
    infoMessageContainer = document.getElementById('info');
    versionDisplay = document.getElementById('version');
    orientation = document.getElementById('orientation_info');
    orientationDevice = document.getElementById('orientation_info_device');
    orientationFrame = document.getElementById('orientation_frame');
    orientationCenter = document.getElementById('orientation_center');
    page0 = document.getElementById("swPage0");
    page1 = document.getElementById("swPage1");

    document.getElementById('version').innerHTML = version;
    onDisconnected();


    swPage0.addEventListener('click', () => togglePage0());
    swPage1.addEventListener('click', () => togglePage1());

    if (!navigator.bluetooth) {
        errorMessageContainer.innerHTML = "Web Bluetooth API ist für diesen Browser nicht verfügbar!";
    }

    // Connect Button (search for BLE Devices only if BLE is available)
    connectButton.addEventListener('click', () => {
        if (navigator.bluetooth) {
            connectToDevice();
        }
    });

    // Disconnect Button
    disconnectButton.addEventListener('click', disconnectDevice);
}

function togglePage0() {
    document.getElementById('armController').style.visibility = 'hidden';
    document.getElementById('speedController').style.display = 'flex';
}

function togglePage1() {
    document.getElementById('armController').style.visibility = 'visible';
    document.getElementById('speedController').style.display = 'none';
}

async function registerServiceWorker() {
    try {
        if ('serviceWorker' in navigator) {
            let registration = await navigator.serviceWorker.register('./src/js/sw.js');
            registration.onupdatefound = () => {
                const installingWorker = registration.installing;
                installingWorker.onstatechange = () => {
                    switch (installingWorker.state) {
                        case 'installed':
                            if (navigator.serviceWorker.controller) {
                                // new update available
                                document.getElementById('update').innerHTML = "UPDATE";
                            } else {
                                // no update available
                            }
                            break;
                    }
                };
            }
        }
    } catch (error) {
        console.error("Error creating service worker: ", error);
        errorMessageContainer.innerHTML = "Error creating service worker: " + error;
    }
}

async function connectToDevice() {
    bleServiceContainer.classList.remove("info");
    bleServiceContainer.classList.add("error");
    bleStateContainer.classList.remove("info");
    bleStateContainer.classList.add("error");
    // CMD
    // Drive
    bleCharCmdDrive.classList.remove("info");
    bleCharCmdDrive.classList.add("error");
    bleCharCmdDriveState.classList.remove("info");
    bleCharCmdDriveState.classList.add("error");
    // Arm
    bleCharCmdArm.classList.remove("info");
    bleCharCmdArm.classList.add("error");
    bleCharCmdArmState.classList.remove("info");
    bleCharCmdArmState.classList.add("error");

    errorMessageContainer.innerHTML = "";

    try {
        infoMessageContainer.innerHTML = "select device";
        const device = await navigator.bluetooth.requestDevice({
            filters: [
                { services: [UUID_SERVICE_CMD] },
            ]
        });

        device.addEventListener('gattservicedisconnected', onDisconnected);

        infoMessageContainer.innerHTML = "connecting GATT server";
        bleServer = await device.gatt.connect();

        infoMessageContainer.innerHTML = "retrieve service list";
        let servicelist = await bleServer.getPrimaryServices();
        let html = "";
        servicelist.forEach(service => html += "<li>" + (service.isPrimary ? "Primär" : "Zusatz") + ": " + service.uuid + "</li>")
        bleServiceList.innerHTML = "<div>services: <ul>" + html + "</ul></div>";

        infoMessageContainer.innerHTML = "retrieve service " + UUID_SERVICE_CMD;
        bleCmdService = await bleServer.getPrimaryService(UUID_SERVICE_CMD);

        bleServiceContainer.innerHTML = 'Verbunden mit Service ' + bleCmdService.uuid;
        bleServiceContainer.classList.remove("error");
        bleServiceContainer.classList.add("info");

        infoMessageContainer.innerHTML = "retrieve char Drive " + UUID_CHAR_CMD_DRIVE;
        CharacteristicCmdDrive = await bleCmdService.getCharacteristic(UUID_CHAR_CMD_DRIVE);

        bleCharCmdDrive.innerHTML = "CMD Characteristik OK: " + CharacteristicCmdDrive.uuid;
        bleCharCmdDrive.classList.remove("error");
        bleCharCmdDrive.classList.add("info");

        infoMessageContainer.innerHTML = "retrieve char DRIVE_STATE " + UUID_CHAR_CMD_DRIVE_STATE;
        CharacteristicCmdDriveState = await bleCmdService.getCharacteristic(UUID_CHAR_CMD_DRIVE_STATE);

        bleCharCmdDriveState.innerHTML = "Drive_State Characteristik OK: " + CharacteristicCmdDriveState.uuid;
        bleCharCmdDriveState.classList.remove("error");
        bleCharCmdDriveState.classList.add("info");


        bleCharCmdArm.innerHTML = "retrieve char ARM " + UUID_CHAR_CMD_ARM;
        CharacteristicCmdArm = await bleCmdService.getCharacteristic(UUID_CHAR_CMD_ARM);

        bleCharCmdArm.innerHTML = "Arm Characteriskik OK: " + CharacteristicCmdArm.uuid;
        bleCharCmdArm.classList.remove('error');
        bleCharCmdArm.classList.add('info');


        bleCharCmdArmState.innerHTML = "retrieve char ARM_STATE " + UUID_CHAR_CMD_ARM_STATE;
        CharacteristicCmdArmState = await bleCmdService.getCharacteristic(UUID_CHAR_CMD_ARM_STATE);

        bleCharCmdArmState.innerHTML = "Arm_State Characteristik OK: " + CharacteristicCmdArmState.uuid;
        bleCharCmdArmState.classList.remove("error");
        bleCharCmdArmState.classList.add("info");


        CharacteristicCmdDriveState.addEventListener('characteristicvaluechanged', handleCharacteristicChange);
        await CharacteristicCmdDriveState.startNotifications();
        CharacteristicCmdDriveState.readValue();

        CharacteristicCmdArmState.addEventListener('characteristicvaluechanged', handleCharacteristicChange)
        await CharacteristicCmdArmState.startNotifications();
        CharacteristicCmdArmState.readValue();


        infoMessageContainer.innerHTML = "erfolgreich verbunden";
        document.getElementById('connection').innerHTML = 'Verbunden mit ' + device.name;

        bleStateContainer.innerHTML = 'Verbunden mit ' + device.name;
        bleStateContainer.classList.remove("error");
        bleStateContainer.classList.add("info");

        disconnectButton.style.display = "block";
        connectButton.style.display = "none";

        document.getElementById('status').style.display = "block";


    } catch (error) {
        errorMessageContainer.innerHTML = error;
    }
}

function onDisconnected() {
    disconnectButton.style.display = "none";
    connectButton.style.display = "block";

    bleServiceContainer.classList.remove("info");
    bleServiceContainer.classList.add("error");
    bleServiceContainer.innerHTML = "Kein Service";

    bleStateContainer.classList.remove("info");
    bleStateContainer.classList.add("error");
    bleStateContainer.innerHTML = "Nicht verbunden";

    // CMD
    // Drive
    bleCharCmdDrive.classList.remove("info");
    bleCharCmdDrive.classList.add("error");
    bleCharCmdDrive.innerHTML = "Kein CMD Drive";

    bleCharCmdDriveState.classList.remove("info");
    bleCharCmdDriveState.classList.add("error");
    bleCharCmdDriveState.innerHTML = "Kein CMD Drive State";

    // Arm
    bleCharCmdArm.classList.remove("info");
    bleCharCmdArm.classList.add("error");
    bleCharCmdArm.innerHTML = "Kein CMD Arm";

    bleCharCmdArmState.classList.remove("info");
    bleCharCmdArmState.classList.add("error");
    bleCharCmdArmState.innerHTML = "Kein CMD Arm State";

    bleServiceList.innerHTML = "";
    document.getElementById('connection').innerHTML = "nicht verbunden";
}

function handleCharacteristicChange(event) {
    console.log(event.target.value)
    const newValueReceived = new TextDecoder().decode(event.target.value);
    retrievedValue.innerHTML = newValueReceived;
    retrievedTimestamp.innerHTML = getDateTime();
    orientationDevice.innerHTML = newValueReceived;

    const div = document.createElement('div');
    const header = document.createElement('h1');
    const text = document.createElement('p');
    header.innerHTML = 'RECEIVE';
    text.innerHTML = newValueReceived;
    div.appendChild(header)
    div.appendChild(text)
    div.classList.add('entry')
    bleReceiveHistory.prepend(div)
    // Remove the oldest entry if more than 6 children
    if (bleReceiveHistory.children.length > 6) {
        bleReceiveHistory.removeChild(bleReceiveHistory.lastElementChild);
    }
}

let sending = false;

export async function writeCmdDrive(value) {
    if (sending) return "busy";
    sending = true;
    let sent = "failed";
    if (bleServer && bleServer.connected) {
        const textEncoder = new TextEncoder();
        const uint8Array = textEncoder.encode(value);
        sentTimestamp.innerHTML = getDateTime();
        try {
            await CharacteristicCmdDrive.writeValue(uint8Array);
            latestValueSent.innerHTML = value;
            const div = document.createElement('div');
            const header = document.createElement('h1');
            const text = document.createElement('p');
            header.innerHTML = 'CMD';
            text.innerHTML = value;
            div.appendChild(header)
            div.appendChild(text)
            div.classList.add('entry')
            bleSendHistory.prepend(div)
            // Remove the oldest entry if more than 6 children
            if (bleSendHistory.children.length > 6) {
                bleSendHistory.removeChild(bleSendHistory.lastElementChild);
            }
            sent = "ok";
        } catch (error) {
            console.error("Error writing to the CMD characteristic: ", error);
        }
    } else {
        console.error("Bluetooth is not connected. Cannot write to characteristic.")
        onDisconnected();
        sent = "disconnected"
    }
    // 10ms Verzögerung einbauen, um nicht zu häufig zu schicken!
    await new Promise((resolve) => setTimeout(resolve, 10));
    sending = false;
    return sent;
}

export async function writeCmdArm(value) {
    if (sending) return "busy";
    sending = true;
    let sent = "failed";
    if (bleServer && bleServer.connected) {
        const textEncoder = new TextEncoder();
        const uint8Array = textEncoder.encode(value);
        sentTimestamp.innerHTML = getDateTime();
        try {
            await CharacteristicCmdArm.writeValue(uint8Array);
            latestValueSent.innerHTML = value;
            const div = document.createElement('div');
            const header = document.createElement('h1');
            const text = document.createElement('p');
            header.innerHTML = 'ARM';
            text.innerHTML = value;
            div.appendChild(header)
            div.appendChild(text)
            div.classList.add('entry')
            bleSendHistory.prepend(div)
            // Remove the oldest entry if more than 6 children
            if (bleSendHistory.children.length > 6) {
                bleSendHistory.removeChild(bleSendHistory.lastElementChild);
            }
            sent = "ok";
        } catch (error) {
            console.error("Error writing to ARM characteristic: ", error);
        }
    } else {
        console.error("Bluetooth is not connected. Cannot write to characteristic.")
        onDisconnected();
        sent = "disconnected"
    }
    // 10ms Verzögerung einbauen, um nicht zu häufig zu schicken!
    await new Promise((resolve) => setTimeout(resolve, 10));
    sending = false;
    return sent;
}


// arm function must be added
function disconnectDevice() {
    console.log("Disconnect Device.");
    if (bleServer && bleServer.connected) {
        if (CharacteristicCmdDrive) {
            CharacteristicCmdDrive.stopNotifications()
                .then(() => {
                    console.log("Notifications Stopped");
                    return bleServer.disconnect();
                })
                .then(() => {
                    onDisconnected();
                })
                .catch(error => {
                    console.log("An error occurred:", error);
                });
        } else {
            console.log("Characteristic: Cmd Drive not found for disconnect.");
        }
    } else {
        // Throw an error if Bluetooth is not connected
        console.error("Bluetooth is not connected.");
        window.alert("Bluetooth is not connected.")
    }
}

function getDateTime() {
    let currentdate = new Date();
    let day = ("00" + currentdate.getDate()).slice(-2); // Convert day to string and slice
    let month = ("00" + (currentdate.getMonth() + 1)).slice(-2);
    let year = currentdate.getFullYear();
    let hours = ("00" + currentdate.getHours()).slice(-2);
    let minutes = ("00" + currentdate.getMinutes()).slice(-2);
    let seconds = ("00" + currentdate.getSeconds()).slice(-2);

    return day + "." + month + "." + year + ", " + hours + ":" + minutes + ":" + seconds;
}


