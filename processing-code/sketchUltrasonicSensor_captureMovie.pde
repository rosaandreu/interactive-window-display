import processing.video.*;
import processing.serial.*; 

Serial myPort;
float value; // Data received from the serial port

Movie theMov; 
boolean moviePlaying = false;
Capture cam;


void setup() {
    size(1280, 780);

    theMov = new Movie(this, "black.mov");

    //theMov.play();  // plays the movie once
    //theMov.loop();  // plays the movie over and over
    //theMov.pause();

    String[] portNames = Serial.list(); // This gets the first port on your computer

    println("Available ports: ");
    int wantedPort = -1;
    for (int i = 0; i < portNames.length; i++) {
        println(i + ". " + portNames[i]);
        if (portNames[i].contains("cu.usbmodem")) {
            wantedPort = i;
        }
    }

    myPort = new Serial(this, portNames[wantedPort], 9600);
    myPort.bufferUntil('\n');

    String[] cameras = Capture.list();

    if (cameras.length == 0) {
        println("There are no cameras available for capture.");
        exit();
    } else {
        println("Available cameras:");

        int wantedCamera = -1;
        for (int i = 0; i < cameras.length; i++) {
            println(cameras[i]);
            if (cameras[i].equals("name=Webcam C170,size=1024x768,fps=30")) {
                wantedCamera = i;
            }
        }

        // The camera can be initialized directly using an 
        // element from the array returned by list():
        cam = new Capture(this, cameras[wantedCamera]);
        cam.start();
    }
}

void draw() {
    /*USING DISTANCE VALUE FROM ULTRASONIC SENSOR THROUGH THE SERIAL PORT*/
    if (value >= 48) { // plays the video
        println("target fuera del rango de detección; reproduciendo video promocional");
        if (moviePlaying != true) {
            //theMov.jump(0.0); // Jumps to a specific location within a movie (if we want the video to continue where it was)
            theMov.loop();
            moviePlaying = true;
        }
        background(0);
        imageMode(CENTER);
        image(theMov, width/2, height/2);
    } else {
        if (moviePlaying == true) {
            //theMov.pause(); // If we want the video to continue where it was paused
            theMov.stop();
            moviePlaying = false;
        }
    }

    if (value >=3 && value < 48) { // plays the camera
        println("target detectado; mostrando lo que captura la webcam");
        if (cam.available() == true) {
            cam.read();
        } else {
            println("Camera not available");
        }
        background(0);
        // image(cam, 0, 0);
        imageMode(CENTER);
        image(cam, width/2, height/2);  // Draw image using CENTER mode
    }

    /*USING KEYS*/
    //if (keyPressed && (key=='M' || key=='m')) { // plays the video
    //    if (moviePlaying != true) {
    //        //theMov.jump(0.0); // Jumps to a specific location within a movie (if we want the video to continue where it was)
    //        theMov.loop();
    //        moviePlaying = true;
    //    }
    //    image(theMov, 0, 0);
    //} else {
    //    if (moviePlaying == true) {
    //        //theMov.pause(); // If we want the video to continue where it was paused
    //        theMov.stop();
    //        moviePlaying = false;
    //    }
    //}

    //if (keyPressed && (key=='C' || key=='c')) { // plays the camera
    //    if (cam.available() == true) {
    //        cam.read();
    //    }
    //    image(cam, 0, 0);
    //    // The following does the same, and is faster when just drawing the image
    //    // without any additional resizing, transformations, or tint.
    //    //set(0, 0, cam);
    //}
}

void serialEvent (Serial myPort) {
    String inString = myPort.readStringUntil('\n');   
    value = float(inString);
    println("Received " + value + " from serialEvent");
}

void movieEvent(Movie e) { 
    e.read();
}

void exit() {
    theMov.stop();
    cam.stop();
}