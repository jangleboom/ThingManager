#ifndef REBOOT_HTML_H
#define REBOOT_HTML_H

/* Colors
Set 1:
            --OUTER_SPACE_CRAYOLA:  #1c3738;
            --EMERALD_GREEN:        #14cc60;
            --FIRE_ENGINE_RED:      #c1292e;
            --ALABASTER_WHITE:      #f1f2eb;
            --ROMAN_SILVER:         #878e99;
Set 2:
            --RUSSIAN_VIOLETT:      #241E4E;
            --GREEN_COLOR:          #399E5A;
            --RED_COLOR:            #CE6C47;
            --GAINSBORO:            #DFDFDF;
            --TEAL_BLUE:            #40798C;
*/

const char REBOOT_HTML[] PROGMEM = R"(
<!DOCTYPE HTML>
<html>

<head>
    <meta content="text/html; charset=UTF-8" http-equiv="content-type">
    <meta name="viewport" content="width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
    <title>Thing Setup done</title>
    <style>
        :root {
            --OUTER_SPACE_CRAYOLA: #1c3738;
            --EMERALD_GREEN: #14cc60;
            --FIRE_ENGINE_RED: #c1292e;
            --ALABASTER_WHITE: #f1f2eb;
            --ROMAN_SILVER: #878e99;
        }

        body {
            background-color: var(--OUTER_SPACE_CRAYOLA);
            color: var(--ALABASTER_WHITE);
            font-family: Lato, Helvetica, Roboto, sans-serif;
            text-align: center;
        }

        h3 {
            margin: 0;
            padding: 0;
        }

        .center {
            margin: 0 auto;
        }

        progress {
            position: relative;
            accent-color: var(--EMERALD_GREEN);
            width: 30%;
            height: 20px;
            margin-top: 20px;
        }

        a {
            color: var(--ROMAN_SILVER);
        }
    </style>
</head>

<body>
    <h3 class="center">Thing manager</h3>
    <h3 class="center">setup done</h3>
    <br>
    <p class="center">Device will restart and connect to SSID: %next_ssid%</p>
    <br>
    <br>
    <progress value="0" max="5" id="progressBar"></progress><br>
    <br>
    <br>
    <p class="center" id="countdown">in 5 second(s)</p> <br>

</body>

</html>

)";


#endif /* HTML_H */


