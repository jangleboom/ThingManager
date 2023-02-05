#ifndef INDEX_HTML_H
#define INDEX_HTML_H

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
 
const char INDEX_HTML[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML>
<html>

<head>
    <meta content="text/html" ; charset="UTF-8" ; http-equiv="content-type">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0">
    <title>Setup</title>
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
            border: 1em;
        }

        .table {
            margin-left: auto;
            margin-right: auto;
        }

        .button {
            border-radius: 4px;
            border: none;
            padding: 15px 30px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
        }

        .blue {
            background-color: var(--ROMAN_SILVER);
            color: var(--OUTER_SPACE_CRAYOLA);
        }

        .green {
            background-color: var(--EMERALD_GREEN);
            color: var(--OUTER_SPACE_CRAYOLA);
        }

        .red {
            background-color: var(--FIRE_ENGINE_RED);
            color: var(--OUTER_SPACE_CRAYOLA);
        }

        .text_field {
            border-radius: 4px;
            border: none; // color: black;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
            width: 275px;
            color: black;
        }

        /* set the opacity of placeholder */

        ::placeholder {
            opacity: 1;
        }

        :focus::placeholder {
            opacity: 0.1;
        }
    </style>
</head>
<script>
    window.onload = function() {

        // check if variable exists
        let ssid = "%ssid%";
        let password = "%password%";
        let broker_ip = "%broker_ip%";
        let thing_name = "%thing_name%";
        let pub_topic_1 = "%pub_topic_1%";
        let pub_topic_2 = "%pub_topic_2%";
        let pub_topic_3 = "%pub_topic_3%";
        let sub_topic_1 = "%sub_topic_1%";
        let sub_topic_2 = "%sub_topic_2%";
        let sub_topic_3 = "%sub_topic_3%";
        let sleep_time_sec = "%sleep_time_sec%";

        if (ssid) {
            document.getElementsByName("ssid")[0].placeholder = ssid;
            document.getElementsByName("ssid")[0].style.color = "black";
        } else {
            document.getElementsByName("ssid")[0].placeholder = "Your WiFi SSID";
            document.getElementsByName("ssid")[0].style.color = "grey";
        }
        if (password) {
            document.getElementsByName("password")[0].placeholder = password;
            document.getElementsByName("password")[0].style.color = "black";
        } else {
            document.getElementsByName("password")[0].placeholder = "Your WiFi password";
            document.getElementsByName("password")[0].style.color = "grey";
        }
        if (broker_ip) {
            document.getElementsByName("broker_ip")[0].placeholder = broker_ip;
            document.getElementsByName("broker_ip")[0].style.color = "black";
        } else {
            document.getElementsByName("broker_ip")[0].placeholder = "Your broker IP";
            document.getElementsByName("broker_ip")[0].style.color = "grey";
        }
        if (thing_name) {
            document.getElementsByName("thing_name")[0].placeholder = thing_name;
            document.getElementsByName("thing_name")[0].style.color = "black";
        } else {
            document.getElementsByName("thing_name")[0].placeholder = "Enter a thing name";
            document.getElementsByName("thing_name")[0].style.color = "grey";
        }
        if (pub_topic_1) {
            document.getElementsByName("pub_topic_1")[0].placeholder = pub_topic_1;
            document.getElementsByName("pub_topic_1")[0].style.color = "black";
        } else {
            document.getElementsByName("pub_topic_1")[0].placeholder = "broker/place/unit/value1";
            document.getElementsByName("pub_topic_1")[0].style.color = "grey";
        }
        if (pub_topic_2) {
            document.getElementsByName("pub_topic_2")[0].placeholder = pub_topic_2;
            document.getElementsByName("pub_topic_2")[0].style.color = "black";
        } else {
            document.getElementsByName("pub_topic_2")[0].placeholder = "broker/place/unit/value2";
            document.getElementsByName("pub_topic_2")[0].style.color = "grey";
        }
        if (pub_topic_3) {
            document.getElementsByName("pub_topic_3")[0].placeholder = pub_topic_3;
            document.getElementsByName("pub_topic_3")[0].style.color = "black";
        } else {
            document.getElementsByName("pub_topic_3")[0].placeholder = "broker/place/unit/value3";
            document.getElementsByName("pub_topic_3")[0].style.color = "grey";
        }
        if (sub_topic_1) {
            document.getElementsByName("sub_topic_1")[0].placeholder = sub_topic_1;
            document.getElementsByName("sub_topic_1")[0].style.color = "black";
        } else {
            document.getElementsByName("sub_topic_1")[0].placeholder = "broker/place/unit/value1";
            document.getElementsByName("sub_topic_1")[0].style.color = "grey";
        }
        if (sub_topic_2) {
            document.getElementsByName("sub_topic_2")[0].placeholder = sub_topic_2;
            document.getElementsByName("sub_topic_2")[0].style.color = "black";
        } else {
            document.getElementsByName("sub_topic_2")[0].placeholder = "broker/place/unit/value2";
            document.getElementsByName("sub_topic_2")[0].style.color = "grey";
        }
        if (sub_topic_3) {
            document.getElementsByName("sub_topic_3")[0].placeholder = sub_topic_3;
            document.getElementsByName("sub_topic_3")[0].style.color = "black";
        } else {
            document.getElementsByName("sub_topic_3")[0].placeholder = "broker/place/unit/value3";
            document.getElementsByName("sub_topic_3")[0].style.color = "grey";
        }
        if (sleep_time_sec) {
            document.getElementsByName("sleep_time_sec")[0].placeholder = sleep_time_sec;
            document.getElementsByName("sleep_time_sec")[0].style.color = "black";
        } else {
            document.getElementsByName("sleep_time_sec")[0].placeholder = "Sleep time in seconds.";
            document.getElementsByName("sleep_time_sec")[0].style.color = "grey";
        }

        console.log(ssid);
        console.log(password);
        console.log(broker_ip);
        console.log(thing_name);
        console.log(pub_topic_1);
        console.log(pub_topic_2);
        console.log(pub_topic_3);
        console.log(sub_topic_1);
        console.log(sub_topic_2);
        console.log(sub_topic_3);
        console.log(sleep_time_sec);
    }
</script>

<body>
    <form id="Form1" onsubmit="return confirm('Restart the ESP by pressing the reboot button for your changes to take effect!');" action='actionUpdateData' method='post' target="hidden-form"></form>
    <form id="Form2" onsubmit="return confirm('Are you sure? All saved files would be deleted.');" action='actionWipeData' method='post' target="hidden-form"></form>
    <form id="Form3" onsubmit="return confirm('Connection will be lost during reboot.');" action='actionRebootESP' method='post' target="hidden-form"></form>
    <input form="Form1" type="hidden" id="radio_state" value=%location_method%>
    <table class="table">
        <tr>
            <td colspan=4>
                <h2>Thing setup</h2>
            </td>
            <td></td>
        </tr>
        <tr>
            <td colspan=4>
                <h3>MQTT broker</h3>
            </td>
        </tr>
        <tr>
            <td style="text-align:left;">SSID:</td>
            <td colspan=3>
                <input class="text_field" form="Form1" type="text" name="ssid" placeholder={{ssid}}>
            </td>
        </tr>
        <tr>
            <td style="text-align:left;">Password:</td>
            <td colspan=3>
                <input class="text_field" form="Form1" type="text" name="password" placeholder={{password}}>
            </td>
        </tr>
        <tr>
            <td style="text-align:left;">Broker IP:</td>
            <td colspan=3>
                <input class="text_field" form="Form1" type="text" name="broker_ip" placeholder={{broker_ip}}>
            </td>
        </tr>
        <tr>
            <td colspan=4>
                <h3>MQTT client</h3>
            </td>
        </tr>
        <tr>
            <td style="text-align:left;">Thing name:</td>
            <td colspan=3>
                <input class="text_field" form="Form1" type="text" name="thing_name" placeholder={{thing_name}}>
            </td>
        </tr>
        <tr>
            <td style="text-align:left;">Pub topic 1:</td>
            <td>
                <input class="text_field" form="Form1" type="text" name="pub_topic_1" placeholder={{pub_topic_1}}>
            </td>
        </tr>
        <tr>
            <td style="text-align:left;">Pub topic 2:</td>
            <td colspan=3>
                <input class="text_field" form="Form1" type="text" name="pub_topic_2" placeholder={{pub_topic_2}}>
            </td>
        </tr>
        <tr>
            <td style="text-align:left;">Pub topic 3:</td>
            <td colspan=3>
                <input class="text_field" form="Form1" type="text" name="pub_topic_3" placeholder={{pub_topic_3}}>
            </td>
        </tr>
        <tr>
            <td style="text-align:left;">Sub topic 1:</td>
            <td colspan=3>
                <input class="text_field" form="Form1" type="text" name="sub_topic_1" placeholder={{sub_topic_1}}>
            </td>
        </tr>
        <tr>
            <td style="text-align:left;">Sub topic 2:</td>
            <td colspan=3>
                <input class="text_field" form="Form1" type="text" name="sub_topic_2" placeholder={{sub_topic_2}}>
            </td>
        </tr>
        <tr>
            <td style="text-align:left;">Sub topic 3:</td>
            <td colspan=3>
                <input class="text_field" form="Form1" type="text" name="sub_topic_3" placeholder={{sub_topic_3}}>
            </td>
        </tr>
        <tr>
            <td colspan=4> </td>
            <td> </td>
        </tr>
        <tr>
            <td colspan=4>
                <h3>Device settings</h3>
            </td>
        </tr>
        <tr>
            <td style="text-align:left;">Sleep time, s:</td>
            <td colspan=3>
                <input class="text_field" form="Form1" type="text" name="sleep_time_sec" placeholder={{sleep_time_sec}}>
            </td>
        </tr>
    </table>
    <br>
    <br>
    <div>
        <input type="submit" form="Form1" class="button green" formaction="/actionUpdateData" value="Save" id="save_button" name="save_button" />
        <input type="submit" form="Form3" class="button green" formaction="/actionRebootESP" value="Reboot" id="reboot_button" name="reboot_button" />
        <input type="reset" form="Form1" class="button blue" value="Cancel" />
        <input type="submit" form="Form2" class="button red" formaction="/actionWipeData" value="Wipe" id="wipe_button" name="wipe_button" />
    </div>
</body>

</html>

)rawliteral";

#endif /* INDEX_HTML_H */


