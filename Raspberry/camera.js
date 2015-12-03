/*
*
*   Advanced Software Engineering
*
*   Members :     Byungwook Lee, Hyesun Lim, Sunghoon Byun
*
*   File name :   camera.js
*   Description : This program perform showing video on Web page for checking visitor.
*                 First, if there is an action which is about pushing doorbell, TIZEN Phone (User) receives a message about alarm.
*                 Then, we can click the URL in the message. (ex, http://192.168.0.185:8000/cam)
*                 But, there is a problem which is buffering I think. So, we need to refresh the page for continue sending the video.
*
*   Execution command : node camera.js
*
*   Final Revision : 2015.11.27
*
*/

var express = require('express'),
    http = require('http'),
    app = express(),
    server = http.createServer(app) ;
app.use(express.static(__dirname + '/images')) ;
var img_flag = 0 ;

var cameraOptions = {
  width : 600,
  height : 420,
  mode : 'timelapse',
  awb : 'off',
  encoding : 'jpg',
  output : 'images/camera.jpg',
  q : 50,
  timeout : 10000,
  timelapse : 0,
  nopreview : true,
  th : '0:0:0'
};

var camera = new require('raspicam')(cameraOptions) ;
camera.start() ;

camera.on('exit', function() {
    camera.stop() ;
    process.exit();
//    console.log('Restart camera') ;
//    camera.start() ;
  }) ;

camera.on('read', function() {
    img_flag = 1 ;
  }) ;

app.get('/cam', function(req, res) {
    res.sendfile('cam.html', {root : __dirname}) ;
  }) ;

app.get('/img', function (req, res) {
    console.log('get /img') ;
      if (img_flag == 1) {
        img_flag = 0 ;
        res.sendfile('images/camera.jpg') ;
      }
  }) ;

server.listen(8000, function() {
    console.log('express server listening on port ' + server.address().port) ;
  }) ;
