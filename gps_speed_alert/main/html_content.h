/* File used to set HTML content */
#include <stdio.h>
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
</head>
<body>
    


    <!doctype html><html><head><title>Serveur HTTP ESP32</title></head><body>get_handler
        <form action="{{POST_URI}}" method="post"><label for="mytext">Contenu :</label><textarea id="mytext" name="mytext">text content</textarea> <br/><button>Envoyer POST</button>
    </form>


</body>
</html>

















)";//2023-08-16 17:01:03 - End of literal strign