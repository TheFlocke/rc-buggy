export const version = "0.5";

let connectButton;
let disconnectButton;
let versionDisplay;
// BLE
// Global variables to Handle Bluetooth
let bleServer;
let bleCmdService;
let bleSensorService;
let bleStateContainer;
let bleServiceContainer;
let bleServiceList;
// Services
// CMD
let bleCharCmdDrive;
let bleCharCmdDriveState;
let bleCharCmdArm;
let bleCharCmdArmState;
// Sensor
let bleCharSensorTemp;
let bleCharSensorPressure;
let bleCharSensorHumidity;
let bleCharSensorGas;
// BLE History
// Sent
let bleSendHistory;
// Received
let bleReceiveHistory;
// Pages
let page0;
let page1;
let page2;

// For Info box
let retrievedValue;
let latestValueSent;
let sentTimestamp;
let retrievedTimestamp;

let errorMessageContainer;
let infoMessageContainer;

// Sent wait time in ms
let waitTime = 15;


// Define BLE Device Specs
// CMD Service
let UUID_SERVICE_CMD = "5eaf1079-e806-47a9-a1ec-d815bea94805"
let UUID_CHAR_CMD_DRIVE = "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3"
let UUID_CHAR_CMD_DRIVE_STATE = "bd6fbfde-385d-480f-b5eb-64d60cc7be9a"
let UUID_CHAR_CMD_ARM = "99d69805-8efb-450e-ae78-c4ddba09f7f6"
let UUID_CHAR_CMD_ARM_STATE = "f8765d0c-81b5-4780-85a4-44f0999f5474"
// Sensor Service
let UUID_SERVICE_SENSOR = "8da7a992-e263-4b78-abf2-bdb94808895c"
let UUID_CHAR_SENSOR_TEMP = "24c53354-de00-42ac-926c-31f805e5d2f5"
let UUID_CHAR_SENSOR_PRESSURE = "545343fb-93a0-4415-9e2b-6a4c8e2835c4"
let UUID_CHAR_SENSOR_HUMIDITY = "618c8e95-e436-4a86-9d7d-17c3db9992d0"
let UUID_CHAR_SENSOR_GAS = "aa4ce7cf-fff0-4d54-b4ec-fb24920b35c1"


// Values of each char
// CMD
let charCmdDrive;
let charCmdDriveState;
let charCmdArm;
let charCmdArmState;
// Sensor
let charSensorTemp;
let charSensorPressure;
let charSensorHumidity;
let charSensorGas;
// for sending the last value
// CMD
let sendingDrive = false;
let pendingDriveValue = null;
// Arm
let sendingArm = false;
let pendingArmValue = null;



window.onload = () => {
    registerServiceWorker();

    connectButton = document.getElementById('connectBleButton');
    disconnectButton = document.getElementById('disconnectBleButton');
    bleStateContainer = document.getElementById('bleState');
    bleServiceContainer = document.getElementById('bleService');
    bleServiceList = document.getElementById('bleServiceList');
    // CMD
    bleCharCmdDrive = document.getElementById('bleCharCmdDrive');
    bleCharCmdDriveState = document.getElementById('bleCharCmdDriveState');
    bleCharCmdArm = document.getElementById('bleCharCmdArm');
    bleCharCmdArmState = document.getElementById('bleCharCmdArmState');
    // Sensor
    bleCharSensorTemp = document.getElementById('bleCharSensorTemp');
    bleCharSensorPressure = document.getElementById('bleCharSensorPressure');
    bleCharSensorHumidity = document.getElementById('bleCharSensorHumidity');
    bleCharSensorGas = document.getElementById('bleCharSensorGas');
    // History (on main page)
    // Values
    // Sent
    bleSendHistory = document.getElementById('sendHistory');
    // Received
    bleReceiveHistory = document.getElementById('receiveHistory');
    // Info boxes (Top left)
    // Sent
    latestValueSent = document.getElementById('valueSent');
    sentTimestamp = document.getElementById('sent_timestamp');
    // retrieved
    retrievedValue = document.getElementById('retrievedValue');
    retrievedTimestamp = document.getElementById('retrieved_timestamp');

    errorMessageContainer = document.getElementById('errors');
    infoMessageContainer = document.getElementById('info');
    versionDisplay = document.getElementById('version');
    // Pages
    page0 = document.getElementById("swPage0");
    page1 = document.getElementById("swPage1");
    page2 = document.getElementById("swPage2");

    document.getElementById('version').innerHTML = version;
    onDisconnected();


    swPage0.addEventListener('click', () => togglePage0());
    swPage1.addEventListener('click', () => togglePage1());
    swPage2.addEventListener('click', () => togglePage2());

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
    document.getElementById('settings').style.visibility = 'hidden';
}
function togglePage1() {
    document.getElementById('armController').style.visibility = 'visible';
    document.getElementById('speedController').style.display = 'none';
    document.getElementById('settings').style.visibility = 'hidden';
}

function togglePage2() {
    document.getElementById('sliderContainer').style.display = 'none';
    document.getElementById('armController').style.visibility = 'hidden';
    document.getElementById('settings').style.visibility = 'visible';
    document.getElementById('settings').style.display = 'block';
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
                {services: [UUID_SERVICE_CMD]}
            ],
            optionalServices: [UUID_SERVICE_SENSOR]
        });

        device.addEventListener('gattservicedisconnected', onDisconnected);

        infoMessageContainer.innerHTML = "connecting GATT server";
        bleServer = await device.gatt.connect();

        infoMessageContainer.innerHTML = "retrieve service list";
        let servicelist = await bleServer.getPrimaryServices();
        let html = "";
        servicelist.forEach(service => html += "<li>" + (service.isPrimary ? "Primär" : "Zusatz") + ": " + service.uuid + "</li>")
        bleServiceList.innerHTML = "<div>services: <ul>" + html + "</ul></div>";

        // Connect to Services
        // try to connect to CMD Service
        infoMessageContainer.innerHTML = "retrieve CMD service " + UUID_SERVICE_CMD;
        bleCmdService = await bleServer.getPrimaryService(UUID_SERVICE_CMD);

        // try to connect to Sensor Service
        infoMessageContainer.innerHTML = "retrieve Sensor service " + UUID_SERVICE_SENSOR;
        bleSensorService = await bleServer.getPrimaryService(UUID_SERVICE_SENSOR);

        // If connected, send log
        // CMD Service
        bleServiceContainer.innerHTML = 'Verbunden mit CMD Service ' + bleCmdService.uuid;
        bleServiceContainer.classList.remove("error");
        bleServiceContainer.classList.add("info");

        // Sensor Service
        bleServiceContainer.innerHTML = 'Verbunden mit Sensor Service ' + bleSensorService.uuid;
        bleServiceContainer.classList.remove("error");
        bleServiceContainer.classList.add("info");

        // Load Characteristics
        // Service: CMD
        // Drive
        infoMessageContainer.innerHTML = "retrieve char CMD Drive " + UUID_CHAR_CMD_DRIVE;
        charCmdDrive = await bleCmdService.getCharacteristic(UUID_CHAR_CMD_DRIVE);

        bleCharCmdDrive.innerHTML = "char CMD Drive OK: " + charCmdDrive.uuid;
        bleCharCmdDrive.classList.remove("error");
        bleCharCmdDrive.classList.add("info");

        // Drive State
        infoMessageContainer.innerHTML = "retrieve char CMD DRIVE_STATE " + UUID_CHAR_CMD_DRIVE_STATE;
        charCmdDriveState = await bleCmdService.getCharacteristic(UUID_CHAR_CMD_DRIVE_STATE);

        bleCharCmdDriveState.innerHTML = "char CMD Drive_State OK: " + charCmdDriveState.uuid;
        bleCharCmdDriveState.classList.remove("error");
        bleCharCmdDriveState.classList.add("info");

        // Arm
        bleCharCmdArm.innerHTML = "retrieve char CMD ARM " + UUID_CHAR_CMD_ARM;
        charCmdArm = await bleCmdService.getCharacteristic(UUID_CHAR_CMD_ARM);

        bleCharCmdArm.innerHTML = "char CMD Arm OK: " + charCmdArm.uuid;
        bleCharCmdArm.classList.remove('error');
        bleCharCmdArm.classList.add('info');

        // Arm State
        bleCharCmdArmState.innerHTML = "retrieve char CMD ARM_STATE " + UUID_CHAR_CMD_ARM_STATE;
        charCmdArmState = await bleCmdService.getCharacteristic(UUID_CHAR_CMD_ARM_STATE);

        bleCharCmdArmState.innerHTML = "char CMD Arm_State  OK: " + charCmdArmState.uuid;
        bleCharCmdArmState.classList.remove("error");
        bleCharCmdArmState.classList.add("info");

        // Service: Sensor
        // Temp
        infoMessageContainer.innerHTML = "retrieve char Sensor Temp " + UUID_CHAR_SENSOR_TEMP;
        charSensorTemp = await bleSensorService.getCharacteristic(UUID_CHAR_SENSOR_TEMP);

        bleCharSensorTemp.innerHTML = "char Sensor Temp OK: " + charSensorTemp.uuid;
        bleCharSensorTemp.classList.remove("error");
        bleCharSensorTemp.classList.add("info");

        // Pressure
        infoMessageContainer.innerHTML = "retrieve char Sensor Pressure " + UUID_CHAR_SENSOR_PRESSURE;
        charSensorPressure = await bleSensorService.getCharacteristic(UUID_CHAR_SENSOR_PRESSURE);

        bleCharSensorPressure.innerHTML = "char Sensor Pressure OK: " + charSensorPressure.uuid;
        bleCharSensorPressure.classList.remove("error");
        bleCharSensorPressure.classList.add("info");

        // Humidity
        infoMessageContainer.innerHTML = "retrieve char Sensor Humidity " + UUID_CHAR_SENSOR_HUMIDITY;
        charSensorHumidity = await bleSensorService.getCharacteristic(UUID_CHAR_SENSOR_HUMIDITY);

        bleCharSensorHumidity.innerHTML = "char Sensor Humidity OK: " + charSensorHumidity.uuid;
        bleCharSensorHumidity.classList.remove("error");
        bleCharSensorHumidity.classList.add("info");

        // Gas
        infoMessageContainer.innerHTML = "retrieve char Sensor Gas " + UUID_CHAR_SENSOR_GAS;
        charSensorGas = await bleSensorService.getCharacteristic(UUID_CHAR_SENSOR_GAS);

        bleCharSensorGas.innerHTML = "char Sensor Gas OK: " + charSensorGas.uuid;
        bleCharSensorGas.classList.remove("error");
        bleCharSensorGas.classList.add("info");

        // Listeners for BLE Notify
        // CMD
        // Drive_State
        charCmdDriveState.addEventListener('characteristicvaluechanged', handleCmdCharChange);
        await charCmdDriveState.startNotifications();
        charCmdDriveState.readValue();

        // Arm_State
        charCmdArmState.addEventListener('characteristicvaluechanged', handleCmdCharChange)
        await charCmdArmState.startNotifications();
        charCmdArmState.readValue();

        // Sensor
        // Temp
        charSensorTemp.addEventListener('characteristicvaluechanged', handleSensorCharChange)
        await charSensorTemp.startNotifications();
        charSensorTemp.readValue();

        // Pressure
        charSensorPressure.addEventListener('characteristicvaluechanged', handleSensorCharChange)
        await charSensorPressure.startNotifications();
        charSensorPressure.readValue();

        // Humidity
        charSensorHumidity.addEventListener('characteristicvaluechanged', handleSensorCharChange)
        await charSensorHumidity.startNotifications();
        charSensorHumidity.readValue();

        // Gas
        charSensorGas.addEventListener('characteristicvaluechanged', handleSensorCharChange)
        await charSensorGas.startNotifications();
        charSensorGas.readValue();


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

    // Char
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

    // Sensor
    // Temp
    bleCharSensorTemp.classList.remove("info");
    bleCharSensorTemp.classList.add("error");
    bleCharSensorTemp.innerHTML = "Kein Sensor Temp";

    // Pressure
    bleCharSensorPressure.classList.remove("info");
    bleCharSensorPressure.classList.add("error");
    bleCharSensorPressure.innerHTML = "Kein Sensor Pressure";

    // Humidity
    bleCharSensorHumidity.classList.remove("info");
    bleCharSensorHumidity.classList.add("error");
    bleCharSensorHumidity.innerHTML = "Kein Sensor Humidity";

    // Gas
    bleCharSensorGas.classList.remove("info");
    bleCharSensorGas.classList.add("error");
    bleCharSensorGas.innerHTML = "Kein Sensor Gas";

    bleServiceList.innerHTML = "";
    document.getElementById('connection').innerHTML = "nicht verbunden";
}

function handleCmdCharChange(event) {
    console.log(event.target.value)
    const newValueReceived = new TextDecoder().decode(event.target.value);
    retrievedValue.innerHTML = newValueReceived;
    retrievedTimestamp.innerHTML = getDateTime();

    const div = document.createElement('div');
    const header = document.createElement('h1');
    const text = document.createElement('p');
    header.innerHTML = 'RECEIVED CMD STATE';
    text.innerHTML = newValueReceived;
    div.appendChild(header)
    div.appendChild(text)
    div.classList.add('entry')
    bleReceiveHistory.prepend(div)
    if (bleReceiveHistory.children.length > 1) {
        bleReceiveHistory.removeChild(bleReceiveHistory.lastElementChild);
    }
}

function handleSensorCharChange(event) {
    console.log(event.target.value)
    retrievedValue.innerHTML = new TextDecoder().decode(event.target.value);
    retrievedTimestamp.innerHTML = getDateTime();
}

let sending = false;

export async function writeCmdDrive(value) {
    if (sendingDrive) {
        // Overwrite with the latest value if already busy
        pendingDriveValue = value;
        return "busy";
    }
    sendingDrive = true;
    let sent = "failed";
    if (bleServer && bleServer.connected) {
        const textEncoder = new TextEncoder();
        const uint8Array = textEncoder.encode(value);
        sentTimestamp.innerHTML = getDateTime();
        try {
            await charCmdDrive.writeValue(uint8Array);
            latestValueSent.innerHTML = value;
            const div = document.createElement('div');
            const header = document.createElement('h1');
            const text = document.createElement('p');
            header.innerHTML = 'CMD Drive';
            text.innerHTML = value;
            div.appendChild(header)
            div.appendChild(text)
            div.classList.add('entry')
            bleSendHistory.prepend(div)
            if (bleSendHistory.children.length > 1) {
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
    await new Promise((resolve) => setTimeout(resolve, waitTime));
    sendingDrive = false;

    // Process pending value after completing the current send
    if (pendingDriveValue !== null) {
        const nextValue = pendingDriveValue;
        pendingDriveValue = null; // Reset before recursion
        writeCmdDrive(nextValue); // Send the queued value
    }

    return sent;
}


export async function writeCmdArm(value) {
    if (sendingArm) {
        // Overwrite with the latest value requested
        pendingArmValue = value;
        return "busy";
    }
    sending = true;
    let sent = "failed";
    if (bleServer && bleServer.connected) {
        const textEncoder = new TextEncoder();
        const uint8Array = textEncoder.encode(value);
        sentTimestamp.innerHTML = getDateTime();
        try {
            await charCmdArm.writeValue(uint8Array);
            latestValueSent.innerHTML = value;
            const div = document.createElement('div');
            const header = document.createElement('h1');
            const text = document.createElement('p');
            header.innerHTML = 'CMD ARM';
            text.innerHTML = value;
            div.appendChild(header)
            div.appendChild(text)
            div.classList.add('entry')
            bleSendHistory.prepend(div)
            if (bleSendHistory.children.length > 1) {
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
    await new Promise((resolve) => setTimeout(resolve, waitTime));
    sending = false;

    // After sending, check if a new value was queued during a busy state
    if (pendingArmValue !== null) {
        const nextValue = pendingArmValue;
        pendingArmValue = null; // Clear before sending to avoid loops
        // Recursively calls to send the latest value
        writeCmdArm(nextValue);
    }

    return sent;
}


// arm function must be added
function disconnectDevice() {
    console.log("Disconnect Device.");
    if (bleServer && bleServer.connected) {
        if (charCmdDrive) {
            charCmdDrive.stopNotifications()
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


