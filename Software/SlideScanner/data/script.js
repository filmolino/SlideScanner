// Variables
let slideCurrent = 0;

// LOAD Event
window.addEventListener("load", function(event) {
  requestCableReleaseNames();
  requestIrReleaseNames();
  requestWiFiConfig();
  requestScannerConfig();
});


// Websocket
let ws_connected = false;
let ws_path = "ws://" + window.location.hostname + "/ws";
let socket = new WebSocket(ws_path);

socket.onopen = function(e) {
  console.log("Websocket connection established [open]");
  ws_connected = true;
  setLinkIcon("connected");
};

socket.onmessage = function(event) {
  console.log(`[message] Data received from server: ${event.data}`);
  let id = event.data.substr(0, event.data.indexOf(","));
  switch (id) {
    case "WiFiCfg":
      console.log("SOCKET Event: WiFiCfg");
      parseWiFiConfig(event.data);
      break;

    case "ScannerCfg":
      console.log("SOCKET Event: ScannerCfg");
      parseScannerConfig(event.data);
      break;

    case "getCableRelNames":
      console.log("SOCKET Event: getCableRelNames");
      parseCableReleaseNames(event.data);
      break;

    case "getIrRelNames":
      console.log("SOCKET Event: getIrRelNames");
      parseIrReleaseNames(event.data);
      break;
  
    case "CurrentSlide":
      console.log("SOCKET Event: CurrentSlide");
      parseCurrentSlide(event.data);
      break;

    case "AutoScan":
      console.log("SOCKET Event: AutoScan");
      parseAutoScan(event.data);
      break;
      
    case "NetworksFound":
      console.log("SOCKET Event: NetworksFound");
      console.log(event.data);
      parseNetworksFound(event.data);
      break;

    case "Error":
      console.log("SOCKET Event: Error");
      break;

    case "OK":
      console.log("SOCKET Event: Last Command OK");
      break;

    default:
      console.log("SOCKET Event: DEFAULT (error)!");
      break;
  }
};

socket.onclose = function(event) {
  if (event.wasClean) {
    console.log(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
  } else {
    // e.g. server process killed or network down
    // event.code is usually 1006 in this case
    console.log('[close] Connection died');
  }
  ws_connected = false;
  setLinkIcon("disconnected");
};

socket.onerror = function(error) {
  alert(`ERROR: Lost Connection to Scanner ${error.message}`);
  ws_connected = false;
  setLinkIcon("disconnected");
};


function setLinkIcon(state) {
  let elem = document.getElementById("link-icon");

  if (state === "connected") {
    elem.classList.remove("filter-inactive");
    elem.classList.add("filter-active");
  } else if (state === "disconnected") {
    elem.classList.remove("filter-active");
    elem.classList.add("filter-inactive");
  }
}

function setPowerButton(state) {
  let elem = document.getElementById("power-button");

  if (state === "on") {
    elem.classList.remove("filter-inactive");
    elem.classList.add("filter-active");
  } else if (state === "off") {
    elem.classList.remove("filter-active");
    elem.classList.add("filter-inactive");
  }
}

function updateSlidesCounter() {
  console.log(`Update Slides Counter`);
  console.log(document.getElementById('max-slides').value);
  document.getElementById("slides-counter").innerHTML = slideCurrent + " / " + document.getElementById('slides-to-scan').value;
}


function btnPressed(payload) {
  if (ws_connected && (payload !== "")) {
    console.log(`send socket Data payload = ${payload}`);
    socket.send(payload);
  }
}

function btnReset() {
  if (ws_connected) {
    socket.send("setCurrentSlide,0");
  }
}

function btnFindNetworks() {
  socket.send("findNetworks");
  let elem = document.getElementById("sta-network");     // Delete all current entries
  while (elem.options.length > 0) {
    elem.remove(0);
  }
}

function btnSaveConfig() {
  sendWiFiConfig();
  sendScannerConfig();
  sendSaveConfig();
  requestWiFiConfig();        // Re-read saved config
  requestScannerConfig();
}

function btnRestartSystem() {
  socket.send("restartSystem");
}

function btnPowerOff() {
  socket.send("powerOff");
  socket.close();
  setLinkIcon("disconnected");
  setPowerButton("off");
}


function btnDeleteConfig() {
  socket.send("deleteConfig");
}


function wifiModeChanged(chkBox) {
  var APchkBox = document.getElementById("ap-enable");
  var STAchkBox = document.getElementById("sta-enable");

  if (chkBox.id == "ap-enable") {
    STAchkBox.checked = false;
  } else if (chkBox.id == "sta-enable") {
    APchkBox.checked = false;
  }
}


function clickSlides() {
  console.log("Slides clicked");
  let dialog = document.getElementById("dialog-slides");
  document.getElementById("max-slides").value = document.getElementById("slides-to-scan").value;
  dialog.showModal();
}

function btnSlidesCancel() {
  console.log("Slides CANCEL clicked");
  let dialog = document.getElementById("dialog-slides");
  dialog.close();
}

function btnSlidesSave() {
  let val = document.getElementById('max-slides').value;

  console.log(`updateMaxSlides to ${val}`);
  if (val > 100) { val = 100; }
  else if (val < 1) { val = 1; }
  console.log("M" + val);
  document.getElementById("slides-to-scan").value = val;
  sendScannerConfig();
  updateSlidesCounter();
  document.getElementById("dialog-slides").close();
}


function staNetworkChanged() {
  let elem = document.getElementById("sta-network");
  document.getElementById("sta-ssid").value = elem.options[elem.selectedIndex].innerHTML;
}


// ---------------------
// ---- WiFi CONFIG ----
// ---------------------
function requestWiFiConfig() {
  // Send socket get command
  socket.send("getWiFiCfg");
}

function parseWiFiConfig(payload) {
  let data = [];

  data = readCSV(payload);
  document.getElementById("ap-ssid").value = data[1];
  document.getElementById("ap-pwd").value = data[2];
  document.getElementById("sta-ssid").value = data[3];
  document.getElementById("sta-pwd").value = data[4];
  data[5] === "0" ? document.getElementById("ap-enable").checked = false : document.getElementById("ap-enable").checked = true;
  data[6] === "0" ? document.getElementById("sta-enable").checked = false : document.getElementById("sta-enable").checked = true;
}

function sendWiFiConfig() {
  let csv = escapeString(document.getElementById("ap-ssid").value) + ",";
  csv += escapeString(document.getElementById("ap-pwd").value) + ",";
  csv += escapeString(document.getElementById("sta-ssid").value) + ",";
  csv += escapeString(document.getElementById("sta-pwd").value) + ",";
  document.getElementById("ap-enable").checked === false ? csv += "0" : csv += "1";
  csv += ",";
  document.getElementById("sta-enable").checked === false ? csv += "0" : csv += "1";
  console.log(`send csv = ${csv}`);
  socket.send("setWiFiCfg" + "," + csv);
}


// ------------------------
// ---- Scanner CONFIG ----
// ------------------------
function requestScannerConfig() {
  socket.send("getScanCfg");
}

function parseScannerConfig(payload) {
  let data = [];

  data = readCSV(payload);
  document.getElementById("delay-slide-change").value = data[1];
  document.getElementById("time-slide-change-pulse").value = data[2];
  document.getElementById("delay-slide-scan").value = data[3];
  document.getElementById("slides-to-scan").value = data[4];

  let elementIr = document.getElementById("ir-selected-code");
  if (data[5] === "1") {
    elementIr.selectedIndex = data[6];
  } else {
    elementIr.selectedIndex = 0;
  }

  let elementCable = document.getElementById("cable-selected-code");
  if (data[7] === "1") {
    elementCable.selectedIndex = data[8];
  } else {
    elementCable.selectedIndex = 0;
  }

  updateSlidesCounter();
 
}

function sendScannerConfig() {
  let csv = escapeString(document.getElementById("delay-slide-change").value) + ",";
  csv += escapeString(document.getElementById("time-slide-change-pulse").value) + ",";
  csv += escapeString(document.getElementById("delay-slide-scan").value) + ",";
  csv += escapeString(document.getElementById("slides-to-scan").value) + ",";
  
  let element = document.getElementById("ir-selected-code");
  if (element.selectedIndex === 0) {
    csv += "0,";
    csv += "1,";
  } else {
    csv += "1,";
    csv += element.selectedIndex.toString() + ',';
  }

  element = document.getElementById("cable-selected-code");
  if (element.selectedIndex === 0) {
    csv += "0,";
    csv += "1,";
  } else {
    csv += "1,";
    csv += element.selectedIndex.toString();
  }
  console.log(`send csv = ${csv}`);
  socket.send("setScanCfg" + "," + csv);

}


function requestCableReleaseNames() {
  socket.send("getCableRelNames");
}


function parseCableReleaseNames(payload) {
  let data = [];
  let option = document.createElement("option");  


  console.log(`payload =`);
  console.log(payload);


  data = readCSV(payload);

  option.text = "OFF";
  option.value = "0";
  document.getElementById("cable-selected-code").add(option.cloneNode(true));

  for (n = 1; n < 33; n++) {
    option.innerHTML = data[n];
    option.value = n;
    document.getElementById("cable-selected-code").add(option.cloneNode(true));
  }
}


function requestIrReleaseNames() {
  socket.send("getIrRelNames");
}


function parseIrReleaseNames(payload) {
  let data = [];
  let option = document.createElement("option");  

  console.log(`payload =`);
  console.log(payload);

  data = readCSV(payload);

  option.text = "OFF";
  option.value = "0";
  document.getElementById("ir-selected-code").add(option.cloneNode(true));

  for (n = 1; n < 33; n++) {
    option.innerHTML = data[n];
    option.value = n;
    document.getElementById("ir-selected-code").add(option.cloneNode(true));
  }
}


function sendSaveConfig() {
  socket.send("saveConfig");
}


function parseCurrentSlide(payload) {
  let data = [];

  data = readCSV(payload);
  slideCurrent = data[1];
  updateSlidesCounter();
}


function parseAutoScan(payload) {
  let data = [];
  let elem = document.getElementById("btn-start-stop");

  data = readCSV(payload);
  if (data[1] === "1") {
    console.log(`parseAutoScan ===1`);
    elem.classList.add("auto-scan-bg-color");
  } else if (data[1] === "0") {
    console.log(`parseAutoScan data1 ===0`);
    elem.classList.remove("auto-scan-bg-color");
  }
}


function parseNetworksFound(payload) {
  let data = [];
  let elem = document.getElementById("sta-network");
  let option = document.createElement("option");  
  
  data = readCSV(payload);

  option.innerHTML = n;
  option.value = n;
  if (data[1]) {
    for (let n = 0; n < data[1]; n++) {
      option.innerHTML = data[2 + n * 2];
      option.value = n;
      elem.add(option.cloneNode(true));
    }
  } else {
    option.innerHTML = "None";
    option.value = 0;
  }
}


function escapeString(string) {
  let data = "";

  for (let n = 0; n < string.length; n++) {
    char = string.charAt(n);
    data += char;
    if (char === ",") { data+= ","; }
  }
  return data;
}


function readCSV(csv) {
  let string = "";
  let end = false;
  let data = [];

  for (let pos = 0; pos < csv.length; pos++) {
    let char = csv.charAt(pos);

    if (char !== ",") {
      string += char;
    }
    else if (csv.charAt(pos + 1) === ",") {
      while (csv.charAt(pos + 1) === ",") {
        string += char;
        pos++;
      }
    }
    else {
      end = true;
    }
    
    if (end || pos === (csv.length - 1)) {
      data.push(string);
      string = "";
      end = false;      
    }
  }
  return data;
}


function switchMenu(evt) {
  let control = document.getElementById("control");
  let config = document.getElementById("config");
  let title = document.getElementById("header-title");
  let menu = document.getElementById("menu-link");
 
  evt.preventDefault();
  if (control.style.display === "none") {
    // Show Control Menu
    control.style.display = "block";
    config.style.display = "none";
    title.innerHTML = "Scan";
    menu.innerHTML = "Config";
  }
  else {
    // Show Config Menu
    control.style.display = "none";
    config.style.display = "block";
    title.innerHTML = "Config";
    menu.innerHTML = "Scan";
  }
}

