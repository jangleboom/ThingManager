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
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=1">
    <title>Setup</title>
    <style>
        body 
        {
            background-color: #241E4E;
            color: #DFDFDF;
            font-family: Lato, Helvetica, Roboto, sans-serif;
            text-align: center;
            border: 1em;
        }
        .button
        {
            border-radius: 4px;
            border: none;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
            margin: 3px 3px;
            padding: 20px 40px;
            cursor: pointer;
            width: 100%;
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

        .table 
        {
            margin-left: auto;
            margin-right: auto;
            margin-top: auto;
            margin-bottom: auto;
        }

        .text_field 
        {
            border-radius: 4px;
            border: none;
            background-color: #DFDFDF;
            padding: 5px 5px;
            margin: 3px 0px;
            width: 100%;
            box-sizing: border-box;
            color: black;
            text-align: left;
            text-decoration: none;
            font-size: 13px;
        }

        ::placeholder 
        {
            color: black;
            opacity: 0.9;
            transition: opacity 1s;
        }

        :focus::placeholder 
        {
            opacity: 0.2
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
        <table class=table >
            <tbody>
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
                <td colspan=3>
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
            <tr>
                <td colspan=4>
                    <br><br><br>
                </td>
            </tr>
            <tr>
                <td>
                    <input type="submit" form="Form1" class="button green" formaction="/actionUpdateData" value="Save" id="save_button" name="save_button"/>
                </td>
                <td>
                    <input type="submit" form="Form3" class="button green" formaction="/actionRebootESP32" value="Reboot" id="reboot_button" name="reboot_button"/>
                </td>
                <td>
                    <input type="reset" form="Form1" class="button blue" value="Cancel"/>
                </td>
                <td>
                    <input type="submit" form="Form2" class="button red" formaction="/actionWipeData" value="Wipe" id="wipe_button" name="wipe_button"/>
                </td>
            </tr>
            <tr>
                <td colspan=3>
                    <br>
                </td>
            </tr>
        </tbody>
        </table>  
</body>
</html>

)rawliteral";

#endif /* INDEX_HTML_H */


