package com.parrot.bobopdronepiloting;

import java.util.Timer;
import java.util.TimerTask;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.felhr.usbserial.FTDISerialDevice;
import com.felhr.usbserial.UsbSerialInterface;
import com.parrot.bebopdronepiloting.R;

import com.parrot.arsdk.arcommands.ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_ENUM;
import com.parrot.arsdk.ardiscovery.ARDiscoveryDeviceService;

import android.hardware.usb.*;

public class PilotingActivity extends Activity implements DeviceControllerListener
{
    private static String TAG = PilotingActivity.class.getSimpleName();
    public static String EXTRA_DEVICE_SERVICE = "pilotingActivity.extra.device.service";

    public DeviceController deviceController;
    public ARDiscoveryDeviceService service;

    private Button emergencyBt;
    private Button takeoffBt;
    private Button landingBt;

    private Button gazUpBt;
    private Button gazDownBt;
    private Button yawLeftBt;
    private Button yawRightBt;

    private Button forwardBt;
    private Button backBt;
    private Button rollLeftBt;
    private Button rollRightBt;

    private TextView batteryLabel;

    private AlertDialog alertDialog;

    private Button route1Bt;
    private Button route2Bt;

    private Timer timer;

    UsbDevice device;
    UsbDeviceConnection usbConnection;
    FTDISerialDevice cp2102 = new FTDISerialDevice(device, usbConnection);

    private UsbSerialInterface.UsbReadCallback mCallback = new UsbSerialInterface.UsbReadCallback() {

        @Override
        public void onReceivedData(byte[] arg0)
        {
            // Code here :)
        }

    };

    class goFwd extends TimerTask {
        public void run() {
            deviceController.setPitch((byte) 50);
            deviceController.setFlag((byte) 1);
            timer.cancel(); //Terminate the timer thread
        }
    }

    class goBwd extends TimerTask {
        public void run() {
            deviceController.setPitch((byte) -50);
            deviceController.setFlag((byte) 1);
            timer.cancel(); //Terminate the timer thread
        }
    }

    class stopFwd_Bwd extends TimerTask {
        public void run() {
            deviceController.setPitch((byte) 0);
            deviceController.setFlag((byte) 0);
            timer.cancel(); //Terminate the timer thread
        }
    }

    public static void pause (long k){

        long time0, time1;
        time0 = System.currentTimeMillis();
        do{
            time1 = System.currentTimeMillis();
        } while ((time1 - time0) < k);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_piloting);

        emergencyBt = (Button) findViewById(R.id.emergencyBt);
        emergencyBt.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v)
            {
                if (deviceController != null)
                {
                    deviceController.sendEmergency();
                }
            }
        });

        takeoffBt = (Button) findViewById(R.id.takeoffBt);
        takeoffBt.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v)
            {
                if (deviceController != null)
                {
                    deviceController.sendTakeoff();
                }
            }
        });

        landingBt = (Button) findViewById(R.id.landingBt);
        landingBt.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v)
            {
                if (deviceController != null)
                {
                    deviceController.sendLanding();
                }
            }
        });

        gazUpBt = (Button) findViewById(R.id.gazUpBt);
        gazUpBt.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event)
            {
                switch (event.getAction())
                {
                    case MotionEvent.ACTION_DOWN:
                        v.setPressed(true);
                        if (deviceController != null)
                        {
                            deviceController.setGaz((byte) 50);
                        }
                        break;

                    case MotionEvent.ACTION_UP:
                        v.setPressed(false);
                        if (deviceController != null)
                        {
                            deviceController.setGaz((byte)0);

                        }
                        break;

                    default:

                        break;
                }

                return true;
            }
        });

        gazDownBt = (Button) findViewById(R.id.gazDownBt);
        gazDownBt.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event)
            {
                switch (event.getAction())
                {
                    case MotionEvent.ACTION_DOWN:
                        v.setPressed(true);
                        if (deviceController != null)
                        {
                            deviceController.setGaz((byte)-50);

                        }
                        break;

                    case MotionEvent.ACTION_UP:
                        v.setPressed(false);
                        if (deviceController != null)
                        {
                            deviceController.setGaz((byte)0);
                        }
                        break;

                    default:

                        break;
                }

                return true;
            }
        });
        yawLeftBt = (Button) findViewById(R.id.yawLeftBt);
        yawLeftBt.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event)
            {
                switch (event.getAction())
                {
                    case MotionEvent.ACTION_DOWN:
                        v.setPressed(true);
                        if (deviceController != null)
                        {
                            deviceController.setYaw((byte)-50);

                        }
                        break;

                    case MotionEvent.ACTION_UP:
                        v.setPressed(false);
                        if (deviceController != null)
                        {
                            deviceController.setYaw((byte) 0);
                        }
                        break;

                    default:

                        break;
                }

                return true;
            }
        });
        yawRightBt = (Button) findViewById(R.id.yawRightBt);
        yawRightBt.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View v, MotionEvent event)
            {
                switch (event.getAction())
                {
                    case MotionEvent.ACTION_DOWN:
                        v.setPressed(true);
                        if (deviceController != null)
                        {
                            deviceController.setYaw((byte) 50);

                        }
                        break;

                    case MotionEvent.ACTION_UP:
                        v.setPressed(false);
                        if (deviceController != null)
                        {
                            deviceController.setYaw((byte) 0);
                        }
                        break;

                    default:

                        break;
                }

                return true;
            }
        });

        forwardBt = (Button) findViewById(R.id.forwardBt);
        forwardBt.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View v, MotionEvent event)
            {
                switch (event.getAction())
                {
                    case MotionEvent.ACTION_DOWN:
                        v.setPressed(true);
                        if (deviceController != null)
                        {
                            deviceController.setPitch((byte) 50);
                            deviceController.setFlag((byte) 1);
                        }
                        break;

                    case MotionEvent.ACTION_UP:
                        v.setPressed(false);
                        if (deviceController != null)
                        {
                            deviceController.setPitch((byte) 0);
                            deviceController.setFlag((byte) 0);
                        }
                        break;

                    default:

                        break;
                }

                return true;
            }
        });
        backBt = (Button) findViewById(R.id.backBt);
        backBt.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event)
            {
                switch (event.getAction())
                {
                    case MotionEvent.ACTION_DOWN:
                        v.setPressed(true);
                        if (deviceController != null)
                        {
                            deviceController.setPitch((byte)-50);
                            deviceController.setFlag((byte)1);
                        }
                        break;

                    case MotionEvent.ACTION_UP:
                        v.setPressed(false);
                        if (deviceController != null)
                        {
                            deviceController.setPitch((byte)0);
                            deviceController.setFlag((byte)0);
                        }
                        break;

                    default:

                        break;
                }

                return true;
            }
        });
        rollLeftBt = (Button) findViewById(R.id.rollLeftBt);
        rollLeftBt.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View v, MotionEvent event)
            {
                switch (event.getAction())
                {
                    case MotionEvent.ACTION_DOWN:
                        v.setPressed(true);
                        if (deviceController != null)
                        {
                            deviceController.setRoll((byte) -50);
                            deviceController.setFlag((byte) 1);
                        }
                        break;

                    case MotionEvent.ACTION_UP:
                        v.setPressed(false);
                        if (deviceController != null)
                        {
                            deviceController.setRoll((byte) 0);
                            deviceController.setFlag((byte) 0);
                        }
                        break;

                    default:

                        break;
                }

                return true;
            }
        });
        rollRightBt = (Button) findViewById(R.id.rollRightBt);
        rollRightBt.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event)
            {
                switch (event.getAction())
                {
                    case MotionEvent.ACTION_DOWN:
                        v.setPressed(true);
                        if (deviceController != null)
                        {
                            deviceController.setRoll((byte)50);
                            deviceController.setFlag((byte)1);
                        }
                        break;

                    case MotionEvent.ACTION_UP:
                        v.setPressed(false);
                        if (deviceController != null)
                        {
                            deviceController.setRoll((byte)0);
                            deviceController.setFlag((byte)0);
                        }
                        break;

                    default:

                        break;
                }

                return true;
            }
        });


        route1Bt = (Button) findViewById(R.id.route1Bt);
        route1Bt.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v)
            {
                if (deviceController != null)
                {
                    new Timer().schedule(new goFwd(), 2000);
//                    deviceController.sendTakeoff();
//                    pause(2000);
//                    deviceController.setPitch((byte) 50);
//                    deviceController.setFlag((byte) 1);
//                    pause(2000);
//                    deviceController.setPitch((byte) 0);
//                    deviceController.setFlag((byte) 0);
//                    pause(3000);
//                    deviceController.sendLanding();
                }
            }
        });

        route2Bt = (Button) findViewById(R.id.route2Bt);
        route2Bt.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                cp2102.open();
                cp2102.setBaudRate(115200);
                cp2102.setDataBits(UsbSerialInterface.DATA_BITS_8);
                cp2102.setParity(UsbSerialInterface.PARITY_ODD);

            }
        });

        batteryLabel = (TextView) findViewById(R.id.batteryLabel);

        Intent intent = getIntent();
        service = intent.getParcelableExtra(EXTRA_DEVICE_SERVICE);

        deviceController = new DeviceController(this, service);
        deviceController.setListener(this);
    }

    @Override
    public void onStart()
    {
        super.onStart();

        if (deviceController != null)
        {
            final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(PilotingActivity.this);

            // set title
            alertDialogBuilder.setTitle("Connecting ...");


            // create alert dialog
            alertDialog = alertDialogBuilder.create();
            alertDialog.show();

            new Thread(new Runnable() {
                @Override
                public void run()
                {
                    boolean failed = false;

                    failed = deviceController.start();

                    runOnUiThread(new Runnable() {
                        @Override
                        public void run()
                        {
                            //alertDialog.hide();
                            alertDialog.dismiss();
                        }
                    });

                    if (failed)
                    {
                        finish();
                    }
                }
            }).start();

        }
    }

    private void stopDeviceController()
    {
        if (deviceController != null)
        {
            final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(PilotingActivity.this);

            // set title
            alertDialogBuilder.setTitle("Disconnecting ...");

            // show it
            runOnUiThread(new Runnable()
            {
                @Override
                public void run()
                {
                    // create alert dialog
                    alertDialog = alertDialogBuilder.create();
                    alertDialog.show();

                    new Thread(new Runnable() {
                        @Override
                        public void run()
                        {
                            deviceController.stop();
                            deviceController = null;

                            runOnUiThread(new Runnable() {
                                @Override
                                public void run()
                                {
                                    //alertDialog.hide();
                                    alertDialog.dismiss();
                                    finish();
                                }
                            });

                        }
                    }).start();
                }
            });
            //alertDialog.show();

        }
    }

    @Override
    protected void onStop()
    {
        if (deviceController != null)
        {
            deviceController.stop();
            deviceController = null;
        }

        super.onStop();
    }

    @Override
    public void onBackPressed()
    {
        stopDeviceController();
    }

    @Override
    public void onDisconnect()
    {
        stopDeviceController();
    }

    @Override
    public void onUpdateBattery(final byte percent)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run()
            {
                batteryLabel.setText(String.format("%d%%", percent));
            }
        });

    }

    @Override
    public void onFlyingStateChanged(final ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_ENUM state)
    {
        // on the UI thread, disable and enable buttons according to flying state
        runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                switch (state) {
                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDED:
                        takeoffBt.setEnabled(true);
                        landingBt.setEnabled(false);
                        break;
                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_FLYING:
                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_HOVERING:
                        takeoffBt.setEnabled(false);
                        landingBt.setEnabled(true);
                        break;
                    default:
                        // in all other cases, take of and landing are not enabled
                        takeoffBt.setEnabled(false);
                        landingBt.setEnabled(false);
                        break;
                }
            }
        });
    }
}
