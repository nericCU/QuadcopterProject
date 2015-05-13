package de.yadrone.apps.controlcenter;

import serialtest.SerialTest;
import de.yadrone.base.ARDrone;

import java.io.BufferedReader;
import java.io.InputStreamReader;


public class YADroneControlCenter
{	
	private ARDrone ardrone=null;
	private SerialTest serial;
	
	public YADroneControlCenter(){
		initializeSerial();
		initialize();
	}
	
	private void initialize(){
		try
		{
			ardrone = new ARDrone();
			System.out.println("Connect drone controller");
			ardrone.start();
			ardrone.takeOff();
			ardrone.hover();
			long start = System.currentTimeMillis();
			long end = start + 30*1000; // 30 seconds * 1000 ms/sec
			BufferedReader input = new BufferedReader(new InputStreamReader(System.in));

			
			while (System.currentTimeMillis() < end)
			{
				if(serial.left_flag == 0){
					ardrone.goLeft();	
					Thread.sleep(500);
				} 
				
				if(serial.forward_flag == 0){
					ardrone.forward();	
					Thread.sleep(500);

				} 		
				
				if(serial.right_flag == 0){
					ardrone.goRight();		
					Thread.sleep(500);
				} 
			}
			ardrone.landing();
			
			//new CCFrame(ardrone);
		}
		catch(Exception exc)
		{
			exc.printStackTrace();
			
			if (ardrone != null)
				ardrone.stop();
			System.exit(-1);
		}
	}
		public void initializeSerial(){
		    serial = new SerialTest();
		    serial.initialize();
		    Thread t=new Thread() {
		        public void run() {
		            //the following line will keep this app alive for 1000    seconds,
		            //waiting for events to occur and responding to them    (printing incoming messages to console).
		            try {Thread.sleep(1000000);} catch (InterruptedException    ie) {}
		        }
		    };
		    t.start();
		    System.out.println("Started");
		}
		
	public static void main(String args[]){
		new YADroneControlCenter();
		return;
	}
}