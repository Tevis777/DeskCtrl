#include "WebPage.h"

const char* WEB_PAGE = "<!DOCTYPE html>\n"
"<html lang=\"en\">\n"
"<head>\n"
"    <meta charset=\"UTF-8\">\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"    <title>Control Panel</title>\n"
"</head>\n"
"<body onload=\"initializePage()\">\n"
"\n"
"    <h1>Drive</h1>\n"
"\n"
"    <div style=\"display: flex; justify-content: space-between;\">\n"
"        <!-- Column 1 - Presets -->\n"
"        <div>\n"
"            <h2>Presets</h2>\n"
"\n"
"            <!-- Display presets entries and corresponding \"Go\" buttons -->\n"
"            <div id=\"presetsColumn\">\n"
"                <!-- Data entries and buttons will be dynamically added here -->\n"
"            </div>\n"
"\n"
"            <!-- Save button for presets -->\n"
"            <button onclick=\"savePresets()\">Save</button>\n"
"        </div>\n"
"\n"
"        <!-- Column 2 - Manual -->\n"
"        <div>\n"
"            <h2>Manual</h2>\n"
"\n"
"            <!-- Manual control buttons -->\n"
"            <button onclick=\"sendDirection('up')\">Up</button>\n"
"            <button onclick=\"sendDirection('down')\">Down</button>\n"
"            <button onclick=\"stopDrive()\">Stop</button>\n"
"        </div>\n"
"    </div>\n"
"\n"
"    <h1>Calibrate</h1>\n"
"\n"
"    <!-- Calibrate section -->\n"
"    <div>\n"
"        <label for=\"calibrateHeight\">Height:</label>\n"
"        <input type=\"text\" id=\"calibrateHeight\" disabled>\n"
"        <button onclick=\"saveCalibration()\">Save</button>\n"
"    </div>\n"
"\n"
"    <h1>Network</h1>\n"
"\n"
"    <!-- Network section -->\n"
"    <div>\n"
"        <label for=\"wifiSsid\">Wifi SSID:</label>\n"
"        <input type=\"text\" id=\"wifiSsid\">\n"
"\n"
"        <label for=\"wifiPass\">Wifi Password:</label>\n"
"        <input type=\"password\" id=\"wifiPass\">\n"
"\n"
"        <label for=\"ip\">IP:</label>\n"
"        <input type=\"text\" id=\"ip\">\n"
"\n"
"        <label for=\"subnet\">Subnet Mask:</label>\n"
"        <input type=\"text\" id=\"subnet\">\n"
"\n"
"        <button onclick=\"saveNetworkConfig()\">Save</button>\n"
"    </div>\n"
"\n"
"    <script>\n"
"        function initializePage() {\n"
"            var xhr = new XMLHttpRequest();\n"
"            xhr.open('GET', '/health', true);\n"
"            xhr.onreadystatechange = function () {\n"
"                if (xhr.readyState == 4 && xhr.status == 200) {\n"
"                    var data = JSON.parse(xhr.responseText);\n"
"                    updateUI(data);\n"
"                }\n"
"            };\n"
"            xhr.send();\n"
"        }\n"
"\n"
"        function updateUI(data) {\n"
"            document.getElementById('calibrateHeight').value = data.height;\n"
"\n"
"            document.getElementById('wifiSsid').value = data.wifiSsid;\n"
"            document.getElementById('wifiPass').value = data.wifiPass;\n"
"            document.getElementById('ip').value = data.ip;\n"
"            document.getElementById('subnet').value = data.subnet;\n"
"\n"
"            // Clear previous entries and buttons\n"
"            var presetsColumn = document.getElementById('presetsColumn');\n"
"            presetsColumn.innerHTML = '';\n"
"\n"
"            // Create entries and buttons for presets\n"
"            data.presets.forEach(function (preset) {\n"
"                var entry = document.createElement('input');\n"
"                entry.type = 'text';\n"
"                entry.value = preset;\n"
"\n"
"                var goButton = document.createElement('button');\n"
"                goButton.innerText = 'Go';\n"
"                goButton.onclick = function () {\n"
"                    sendHeightPreset(preset);\n"
"                };\n"
"\n"
"                presetsColumn.appendChild(entry);\n"
"                presetsColumn.appendChild(goButton);\n"
"                presetsColumn.appendChild(document.createElement('br'));\n"
"            });\n"
"        }\n"
"\n"
"        function savePresets() {\n"
"            var entries = document.querySelectorAll('#presetsColumn input');\n"
"            var presets = [];\n"
"\n"
"            entries.forEach(function (entry) {\n"
"                presets.push(parseFloat(entry.value));\n"
"            });\n"
"\n"
"            var xhr = new XMLHttpRequest();\n"
"            xhr.open('POST', '/config', true);\n"
"            xhr.setRequestHeader('Content-Type', 'application/json');\n"
"\n"
"            xhr.onreadystatechange = function () {\n"
"                if (xhr.readyState == 4 && xhr.status == 200) {\n"
"                    console.log('Presets saved successfully.');\n"
"                }\n"
"            };\n"
"\n"
"            xhr.send(JSON.stringify({ presets: presets }));\n"
"        }\n"
"\n"
"        function sendDirection(direction) {\n"
"            var xhr = new XMLHttpRequest();\n"
"            xhr.open('POST', '/drive/direction', true);\n"
"            xhr.setRequestHeader('Content-Type', 'application/json');\n"
"\n"
"            xhr.onreadystatechange = function () {\n"
"                if (xhr.readyState == 4 && xhr.status == 200) {\n"
"                    console.log('Direction sent successfully.');\n"
"                }\n"
"            };\n"
"\n"
"            xhr.send(JSON.stringify({ direction: direction }));\n"
"        }\n"
"\n"
"        function stopDrive() {\n"
"            var xhr = new XMLHttpRequest();\n"
"            xhr.open('POST', '/drive/stop', true);\n"
"\n"
"            xhr.onreadystatechange = function () {\n"
"                if (xhr.readyState == 4 && xhr.status == 200) {\n"
"                    console.log('Drive stopped successfully.');\n"
"                }\n"
"            };\n"
"\n"
"            xhr.send();\n"
"        }\n"
"\n"
"        function saveCalibration() {\n"
"            var height = document.getElementById('calibrateHeight').value;\n"
"\n"
"            var xhr = new XMLHttpRequest();\n"
"            xhr.open('POST', '/calibration', true);\n"
"            xhr.setRequestHeader('Content-Type', 'application/json');\n"
"\n"
"            xhr.onreadystatechange = function () {\n"
"                if (xhr.readyState == 4 && xhr.status == 200) {\n"
"                    console.log('Calibration saved successfully.');\n"
"                }\n"
"            };\n"
"\n"
"            xhr.send(JSON.stringify({ height: parseFloat(height) }));\n"
"        }\n"
"\n"
"        function saveNetworkConfig() {\n"
"            var wifiSsid = document.getElementById('wifiSsid').value;\n"
"            var wifiPass = document.getElementById('wifiPass').value;\n"
"            var ip = document.getElementById('ip').value;\n"
"            var subnet = document.getElementById('subnet').value;\n"
"\n"
"            var xhr = new XMLHttpRequest();\n"
"            xhr.open('POST', '/config', true);\n"
"            xhr.setRequestHeader('Content-Type', 'application/json');\n"
"\n"
"            xhr.onreadystatechange = function () {\n"
"                if (xhr.readyState == 4 && xhr.status == 200) {\n"
"                    console.log('Network configuration saved successfully.');\n"
"                }\n"
"            };\n"
"\n"
"            xhr.send(JSON.stringify({ wifiSsid: wifiSsid, wifiPass: wifiPass, ip: ip, subnet: subnet }));\n"
"        }\n"
"\n"
"        function sendHeightPreset(height) {\n"
"            var xhr = new XMLHttpRequest();\n"
"            xhr.open('POST', '/drive/height', true);\n"
"            xhr.setRequestHeader('Content-Type', 'application/json');\n"
"\n"
"            xhr.onreadystatechange = function () {\n"
"                if (xhr.readyState == 4 && xhr.status == 200) {\n"
"                    console.log('Height preset sent successfully.');\n"
"                }\n"
"            };\n"
"\n"
"            xhr.send(JSON.stringify({ height: height }));\n"
"        }\n"
"    </script>\n"
"\n"
"</body>\n"
"</html>\n"
"";


const char* GetWebPage()
{
    return WEB_PAGE;
}