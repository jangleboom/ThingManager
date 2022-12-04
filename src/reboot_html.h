#ifndef REBOOT_HTML_H
#define REBOOT_HTML_H


const char REBOOT_HTML[] PROGMEM = R"(
<!DOCTYPE HTML>
<html>

<head>
    <meta content="text/html" ; charset="UTF-8" ; http-equiv="content-type">
    <meta name="viewport" content="width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
    <title>SmartHomeDevice Setup done</title>
    <style>
        body {
            background-color: #4180C8;
            font-family: Lato, Helvetica, Roboto, sans-serif;
            color: GhostWhite;
            text-align: center;
            border: 1em;
        }

        .center {
            margin-left: auto;
            margin-right: auto;
        }

        progress {
            position: relative;
            accent-color: #F0A03C;
        }
    </style>
</head>

<body>
    <h3>Smart home device</h3>
    <h3>setup done</h3>
    <br>
    device will restart<br>
    and connect to <br>
    SSID: %next_ssid%<br>
    <br>
    <br>
    <progress value="0" max="5" id="progressBar"> </progress><br>
    <br>
    <br>
    <span id="countdown">in 5 second(s)</span> <br>

</body>

</html>

)";


#endif /* HTML_H */


