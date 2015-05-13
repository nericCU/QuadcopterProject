package serialtest;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import gnu.io.CommPortIdentifier; 
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent; 
import gnu.io.SerialPortEventListener; 
import java.util.Enumeration;


public class SerialTest implements SerialPortEventListener {
SerialPort serialPort;
    /** The port we're normally going to use. */
private static final String PORT_NAMES[] = {                  "/dev/tty.usbserial-A9007UX1", // Mac OS X
        "/dev/ttyUSB0", // Linux
        "COM3", // Windows
};
private BufferedReader input;
private OutputStream output;
private static final int TIME_OUT = 2000;
private static final int DATA_RATE = 4800;
public int left_flag = 0, forward_flag = 0, right_flag = 0;

public void initialize() {
    CommPortIdentifier portId = null;
    Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

    //First, Find an instance of serial port as set in PORT_NAMES.
    while (portEnum.hasMoreElements()) {
        CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement();
        for (String portName : PORT_NAMES) {
            if (currPortId.getName().equals(portName)) {
                portId = currPortId;
                break;
            }
        }
    }
    if (portId == null) {
        System.out.println("Could not find COM port.");
        return;
    }

    try {
        serialPort = (SerialPort) portId.open(this.getClass().getName(),
                TIME_OUT);
        serialPort.setSerialPortParams(DATA_RATE,
                SerialPort.DATABITS_8,
                SerialPort.STOPBITS_1,
                SerialPort.PARITY_NONE);

        // open the streams
        input = new BufferedReader(new InputStreamReader(serialPort.getInputStream()));
        output = serialPort.getOutputStream();

        serialPort.addEventListener(this);
        serialPort.notifyOnDataAvailable(true);
    } catch (Exception e) {
        System.err.println(e.toString());
    }
}


public synchronized void close() {
    if (serialPort != null) {
        serialPort.removeEventListener();
        serialPort.close();
    }
}

public synchronized void serialEvent(SerialPortEvent oEvent) {
    if (oEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
        try {
            String inputLine=null;
            char char_flag = ' ';
            int distance = 0;
            
            if (input.ready()) {
                inputLine = input.readLine();
                char_flag = inputLine.charAt(0);
                distance = Integer.valueOf(inputLine.substring(1));
                System.out.println(inputLine);

                if(char_flag == 'L'){
                	
                    if(distance < 15 && distance != 0){
                        //System.out.println("left_flag set");
                        left_flag = 1;
                    } else if( distance > 15){
                        //System.out.println("left_flag reset");
                        left_flag = 0;
                    }
                    
                } else if(char_flag == 'F'){
                	
                    if(distance < 15 && distance != 0){
                        //System.out.println("forward_flag set");
                    	forward_flag = 1;
                    } else if( distance > 15){
                        //System.out.println("forward_flag reset");
                    	forward_flag = 0;
                    }
                    
                } else if(char_flag == 'R'){
                	
                    if(distance < 15 && distance != 0){
                        //System.out.println("right_flag set");
                        right_flag = 1;
                    } else if( distance > 15){
                        //System.out.println("right_flag reset");
                        right_flag = 0;
                    }
                    
                }

            }

        } catch (Exception e) {
            System.err.println(e.toString());
        }
    }
    // Ignore all the other eventTypes, but you should consider the other ones.
}
}