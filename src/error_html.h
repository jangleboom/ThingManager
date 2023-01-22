#ifndef ERROR_HTML_H
#define ERROR_HTML_H

const char ERROR_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <meta content="text/html" ; charset="UTF-8" ; http-equiv="content-type">
    <meta name="viewport" content="width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
    <title>RTK rover setup</title>
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
            font-family: Lato, Helvetica, Roboto, sans-serif;
            color: var(--ALABASTER_WHITE);
            text-align: center;
            border: 1em;
        }

        .center {
            margin-left: auto;
            margin-right: auto;
        }

        progress {
            position: relative;
            accent-color: var(--EMERALD_GREEN);
        }
    </style>

    <script type="text/javascript">
        let duration = 3
        var timeleft = duration;
        var downloadTimer = setInterval(function() {
            if (timeleft <= -1) {
                clearInterval(downloadTimer);
            }
            document.getElementById("progressBar").value = duration - timeleft;
            let label = `in ${timeleft} second(s)`;
            document.getElementById("countdown").innerHTML = label
            if ((timeleft) == -1) {
                document.getElementById("countdown").innerHTML = `started`
            }
            timeleft -= 1;
        }, 1000);
    </script>
</head>


<body>
    <h2>RTK Rover</h2>
    <p>an error occured - reload form</p>
    <span id="countdown">in 3 second(s)</span> <br>
    <progress value="0" max="3" id="progressBar"> </progress>


</body>

</html>
)rawliteral";
#endif /* ERROR_HTML_H */


