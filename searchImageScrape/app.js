/*

Bulk download images from BING
Author: fito_segrera

USAGE: node recursive.js query startIndex endIndex subFolderName
Example: node app.js wood_pipe 39 100 pipes

This starts downloading from image 39 until 100 in the search result list
*/

var Scraper = require('images-scraper');
var bing = new Scraper.Bing();
var request = require('request');
var fs = require('fs');
var Jimp = require('jimp');
var exec = require('child_process').exec;

var q = process.argv[2].split("_");
q = q[0] + " " + q[1];
var count = parseInt(process.argv[3]);
var total = parseInt(process.argv[4]);

var startTime = new Date();


var path = __dirname + "/images/" + process.argv[5] + "/";

function makeImgRequest(quer) {
    console.log("QUERY: " + quer);
    bing.list({
            keyword: quer,
            num: parseInt(total),
            detail: true
        })
        .then(function(res) {
            console.log(res.length);
            var index = Math.floor(Math.random() * res.length);


            var format = res[count]['format'];
            console.log("-----------------");
            console.log(res[count]);
            console.log("-----------------");
            //Evaluate the image type.
            var checked = evaluateImageType(count, format, res).then(function(data) {
                if (data != 'jpg') {
                    Jimp.read(path + "image_" + count + "." + data).then(function(lenna) {
                        lenna.write(path + "image_" + count + ".jpg", function() {
                            exec("rm " + path + "image_" + count + "." + data, function(error, stdout, stderror) {
                                if (error) throw error;
                                // console.log(stdout);
                                // console.log(stderror);
                                // process.exit();
                                count++;
                                loop();
                            });
                        });
                    }).catch(function(err) {
                        console.error(err);
                    });
                } else {
                    console.log("different data");
                    process.exit();
                }
            });

        }).catch(function(err) {
            //console.log('err', err);
            console.log(err + "\nNOTHING FOUND: retrying...");
            count++;
        });
}

var download = function(uri, filename, callback) {
    request.head(uri, function(err, res, body) {
        //if there is a request error or the image type is GIF
        //Also make sure the size of the image is les than 5MB
        if (!err && parseInt(res.headers['content-length']) < 500000) {
            console.log("-----------------");
            console.log('content-type:', res.headers['content-type']);
            console.log('content-length:', res.headers['content-length']);
            console.log("=================");
            request(uri).pipe(fs.createWriteStream(filename)).on('close', callback);
            var endTime = new Date();
            var totalTime = endTime - startTime;
            console.log("ELAPSED-TIME: " + totalTime / 1000 + " secs");
        } else {
            console.log("ERROR: " + err + "\nretrying...");
            count++;
            makeImgRequest(q);
        }
    });
};

//Evaluate the image type.
function evaluateImageType(index, f, res) {
    return {
        then: function(callback) {

            //Timeout event: if the request takes too long, retry 15 Secs later.
            var timeoutInterval = setTimeout(function() {
                console.log("TIMED-OUT: Retrying...");
                count++;
                makeImgRequest(q);
            }, 15000);

            //The image is downloaded in its original filetype and in case it
            //is not a jpg or jpeg, then convert it and errase the original type.
            if (f == 'jpg' || f == 'jpeg' || f == 'png' || f == 'bmp') {
                download(res[index]['url'], path + 'image_' + count + '.' + f, function() {
                    callback(f);
                    clearTimeout(timeoutInterval);
                });
            } else {
                //in case the image type not a jpg, jpeg, png or bmp
                count++;
                makeImgRequest(q);
            }
        }
    };
}

function loop() {
    if (count <= total) {
        makeImgRequest(q);
    } else {
        console.log("loop ended");
        process.exit();
    }
};

makeImgRequest(q);
