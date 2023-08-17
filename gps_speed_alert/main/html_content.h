/* File used to set HTML content */
#include <iostream>
#include <map>

using namespace std;

//2023-08-16 17:04:13 - Paste here HTML content
string HTML_CONTENT = R"(














<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{TITLE}}</title>

    <style>
    body
    {
        color:antiquewhite;
        background-color: #404040;
    }

    textarea
    {
        width: 80%;
        height:600px;
        border: solid 1px gainsboro;
        background: #100e00;
        color: #aeff2c;
        font-size: 1.1em;
        font-family: monospace;
    }

    button
    {
        margin: 10px;
        min-height: 60px;
        padding: 8px;       
        background: #33b5af; 
        border:solid 1px #141504;
        color: #141504;
        font-size: 1.31em;
        font-family: monospace;
    }

    form{
        display: flex;
        align-content: stretch;
        justify-content: center;
        flex-direction: column;
        flex-wrap: wrap;
        align-items: center;
    }

    .btn-primary
    {
        background:#3d8b4a;
    }
    .btn-danger
    {
        background:#b17b7d;
    }
    .buttons
    {
        display: flex;
        flex-direction: row;
        flex-wrap: nowrap;
        justify-content: space-between;
        align-content: center;
        align-items: flex-end;
    }

    </style>
</head>

<body>    
    <h4>{{TITLE}}</h4>    
    <section action="{{POST_URI}}" method="post">     
       <div class="buttons">
           <button  class="btn-primary" onclick="saveConfig(this);">Save</button> <button class="btn-danger" onclick="restart(this);">Restart</button>
       </div>
            <textarea id="postdata" name="postdata">{{JSON_CONFIG}}</textarea> <br/>
        <div class="buttons">
            <button class="btn-primary" onclick="saveConfig(this);">Save</button> <button class="btn-danger" onclick="restart(this);">Restart</button>
        </div>
    </section>

    <script>

           //2023-08-17 16:58:26 - Send command to server
        function sendCommand(command, value)
        {
                var url = "/config";                
                var payload =value;
                //2023-08-17 18:26:22 - response callback
                var onResponse= function(data)
                {
                    console.log(data);
                }      
                var postData = new FormData();	
                postData.append('command', payload);
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function() {
                    if (this.readyState == 4 && this.status == 200) {
                        onResponse( xhttp.responseText );                       
                    }
                };          
            xhttp.open("POST", url, true);
            xhttp.send(postData);
        }//sendData, sendCommand


        function saveConfig()
        {                
                var area = document.getElementById("postdata");
                sendCommand("data",area.value);
        }//saveConfig



        function restart()
        {           
                var area = document.getElementById("postdata");
                var when = new Date();                
                sendCommand("restart",when.toISOString().substring(0,10) );
        }//saveConfig

     
    </script>

</body>
</html>













































)";//2023-08-16 17:01:03 - End of literal strign