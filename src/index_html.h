#ifndef INDEX_HTML_H
#define INDEX_HTML_H

/*  
    RUSSIAN_VIOLETT     #241E4E
    ROSE_WOOD           #520100
    MEDIUM_CHAMPAGNE    #EADAA2
    SUN_GLOW            #FFD046
    COPPER_RED          #CE6C47
    CERISE              #DA4167
    GAINSBORO           #DFDFDF
    BRONZE              #D58936
    TEAL_BLUE           #40798C
    SHAMROCK_GREEN      #399E5A
    MIDDLE_PURPLE       #D67AB1
*/
 
const char INDEX_HTML[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML>
<html>
<head>
    <meta content="text/html" ; charset="UTF-8" ; http-equiv="content-type">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0">
    <title>Setup</title>
    <style>
        body 
        {
            background-color: #241E4E;
            background-image: radial-gradient(#15112C, #241E4E, #312966);
            color: #DFDFDF;
            font-family: Lato, Helvetica, Roboto, sans-serif;
            text-align: center;
            border: 1em;
        }

        .table 
        {
            margin-left: auto;
            margin-right: auto;
        }

        .button 
        {
            border-radius: 4px;
            border: none;
            padding: 15px 30px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
        }
        .blue 
        {
            background-color: #40798C;
            color: #241E4E;
        }
        .green 
        {
            background-color: #399E5A;
            color: #241E4E;
        }
        .red
        {
            background-color: #CE6C47;
            color: #241E4E;
        }

        .text_field 
        {
            border-radius: 4px;
            border: none;
            color: black;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
        }

        ::placeholder 
        {
            color: black;
            opacity: 1;
            transition: opacity 1s;
        }

        :focus::placeholder 
        {
            opacity: 0.1;
        }
    </style>
</head>
<script>
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
                    <input class="text_field" form="Form1" type="text" name="ssid" placeholder=%ssid% >
                </td>
            </tr>
            <tr>
                <td style="text-align:left;">Password:</td>
                <td colspan=3>
                    <input class="text_field" form="Form1" type="text" name="password" placeholder=%password% >
                </td>
            </tr>
            <tr>
                <td style="text-align:left;">Broker IP:</td>
                <td colspan=3>
                    <input class="text_field" form="Form1" type="text" name="broker_ip" placeholder=%broker_ip% >
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
                    <input class="text_field" form="Form1" type="text" name="thing_name" placeholder=%thing_name%>
                </td>
            </tr>
            <tr>
                <td style="text-align:left;">Pub topic 1:</td>
                <td>
                    <input class="text_field" form="Form1" type="text" name="pub_topic_1" placeholder=%pub_topic_1%>
                </td>
            </tr>
            <tr>
                 <td style="text-align:left;">Pub topic 2:</td>
                <td colspan=3>
                    <input class="text_field" form="Form1" type="text" name="pub_topic_2" placeholder=%pub_topic_2%>
                </td>
            </tr>
            <tr>
                <td style="text-align:left;">Pub topic 3:</td>
                   <td colspan=3>
                    <input class="text_field" form="Form1" type="text" name="pub_topic_3" placeholder=%pub_topic_3%>
                </td>
            </tr>
            <tr>
                <td style="text-align:left;">Sub topic 1:</td>
                <td colspan=3>
                    <input class="text_field" form="Form1" type="text" name="sub_topic_1" placeholder=%sub_topic_1%>
                </td>
            </tr>
               <tr>
                <td style="text-align:left;">Sub topic 2:</td>
                <td colspan=3>
                    <input class="text_field" form="Form1" type="text" name="sub_topic_2" placeholder=%sub_topic_2%>
                </td>
            </tr>
            <tr>
                <td style="text-align:left;">Sub topic 3:</td>
                <td colspan=3>
                    <input class="text_field" form="Form1" type="text" name="sub_topic_3" placeholder=%sub_topic_3%>
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
                      <input class="text_field" form="Form1" type="text" name="sleep_time_sec" placeholder=%sleep_time_sec%>
                </td>
            </tr>
        </table>
        <br>
        <br>
        <div>
            <input type="submit" form="Form1" class="button green" formaction="/actionUpdateData" value="Save" id="save_button" name="save_button" />
            <input type="submit" form="Form3" class="button green" formaction="/actionRebootESP32" value="Reboot" id="reboot_button" name="reboot_button" />
            <input type="reset" form="Form1" class="button blue" value="Cancel" />
            <input type="submit" form="Form2" class="button red" formaction="/actionWipeData" value="Wipe" id="wipe_button" name="wipe_button" />
        </div>
</body>
</html>

)rawliteral";

#endif /* INDEX_HTML_H */


