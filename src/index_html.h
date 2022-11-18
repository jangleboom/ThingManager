#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <meta content="text/html" ; charset="UTF-8" ; http-equiv="content-type">
    <meta name="viewport" content="width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
    <title>RTK rover setup</title>
    <style>
        body 
        {
            background-color: #4180C8;
            font-family: Lato, Helvetica, Roboto, sans-serif;
            color: GhostWhite;
            text-align: center;
            border: 1em;
        }

        .center 
        {
            margin-left: auto;
            margin-right: auto;
        }

        .button 
        {
            background-color: #F0A03C;
            border: none;
            color: white;
            padding: 13px 18px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
        }

        .text_field 
        {
            border: none;
            color: black;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
        }

        ::placeholder 
        {
            color: navy;
            opacity: 1;
            transition: opacity 1s;
        }

        :focus::placeholder 
        {
            opacity: 0
        }
    </style>
</head>
<script>
</script>

<body>

    <form id="Form1" onsubmit="return confirm('Restart the ESP by pressing the reboot button for your changes to take effect!');" action='actionUpdateData' method='post' target="hidden-form"></form>
    <form id="Form2" onsubmit="return confirm('Are you sure? All saved SPIFFS files will be deleted.');" action='actionWipeData' method='post' target="hidden-form"></form>
    <form id="Form3" onsubmit="return confirm('Connection will be lost during reboot. This server appears in AP-Mode only (push wipe button on device to force this)!');" action='actionRebootESP32' method='post' target="hidden-form"></form>
    <input form="Form1" type="hidden" id="radio_state" value=%location_method%>
    <p>
        <table class=center>
            <tr>
                <td colspan=2>
                    <h2>Smart Home Device</h2>
                </td>
                <td colspan=2></td>
            </tr>
            <tr>
                <td colspan=2>
                    <h3>MQTT broker</h3>
                </td>
                <tr>
                    <td style="text-align:left;">SSID:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" maxlength="30" name="ssid" placeholder=%ssid% style="text-align:center;">
                    </td>
                </tr>
                <tr>
                    <td style="text-align:left;">Password:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" maxlength="30" name="password" placeholder=%password% style="text-align:center;">
                    </td>
                      <tr>
                    <td style="text-align:left;">Broker IP:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" maxlength="30" name="broker_ip" placeholder=%broker_ip% style="text-align:center;">
                    </td>

                </tr>
                <tr>
                    <td colspan=2>
                        <h3>MQTT client</h3>
                    </td>
                    <tr>
                        <td style="text-align:left;">Pub topic 1:</td>
                        <td>
                            <input class="text_field" form="Form1" type="text" maxlength="30" name="pub_topic_1" placeholder=%pub_topic_1% style="text-align:center;">
                        </td>

                    </tr>
                        <tr>
                        <td style="text-align:left;">Pub topic 2:</td>
                        <td>
                            <input class="text_field" form="Form1" type="text" maxlength="30" name="pub_topic_2" placeholder=%pub_topic_2% style="text-align:center;">
                        </td>

                    </tr>
                    <tr>
                        <td style="text-align:left;">Pub topic 3:</td>
                        <td>
                            <input class="text_field" form="Form1" type="text" maxlength="30" name="pub_topic_3" placeholder=%pub_topic_3% style="text-align:center;">
                        </td>

                    </tr>
                    <tr>
                        <td style="text-align:left;">Sub topic 1:</td>
                        <td>
                            <input class="text_field" form="Form1" type="text" maxlength="30" name="sub_topic_1" placeholder=%sub_topic_1% style="text-align:center;">
                        </td>
                    </tr>
                                         <tr>
                        <td style="text-align:left;">Sub topic 2:</td>
                        <td>
                            <input class="text_field" form="Form1" type="text" maxlength="30" name="sub_topic_2" placeholder=%sub_topic_2% style="text-align:center;">
                        </td>
                    </tr>
                                                              <tr>
                        <td style="text-align:left;">Sub topic 3:</td>
                        <td>
                            <input class="text_field" form="Form1" type="text" maxlength="30" name="sub_topic_3" placeholder=%sub_topic_3% style="text-align:center;">
                        </td>
                    </tr>

                    <tr>
                        <td></td>
                        <td style="text-align:right;"> </td>
                    </tr>
                      <tr>
                    <td colspan=2>
                        <h3>Device settings</h3>
                    </td>
                    
                        <tr>
                        <td style="text-align:left;">Sleep time, s:</td>
                        <td>
                            <input class="text_field" form="Form1" type="text" maxlength="30" name="sleep_time" placeholder=%sleep_time% style="text-align:center;">
                        </td>

                    </tr>
                   
        </table>
    </p>                    
    <br>
    <div>
        <input type="submit" form="Form1" class="button" formaction="/actionUpdateData" value="Save" id="save_button" name="save_button" />
        <input type="submit" form="Form3" class="button" formaction="/actionRebootESP32" value="Reboot" id="reboot_button" name="reboot_button" />
        <input type="reset" form="Form1" class="button" value="Cancel" />
        <input type="submit" form="Form2" class="button" formaction="/actionWipeData" value="Wipe" id="wipe_button" name="wipe_button" />
    </div>
</body>

</html>

)rawliteral";

#endif /* INDEX_HTML_H */


