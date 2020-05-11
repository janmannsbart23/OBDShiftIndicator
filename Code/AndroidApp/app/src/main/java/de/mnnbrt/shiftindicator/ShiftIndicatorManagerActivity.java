package de.mnnbrt.shiftindicator;

import android.app.AlertDialog;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.PointF;
import android.graphics.Rect;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.sccomponents.gauges.ScArcGauge;
import com.sccomponents.gauges.ScCopier;
import com.sccomponents.gauges.ScGauge;
import com.sccomponents.gauges.ScNotches;
import com.sccomponents.gauges.ScPointer;
import com.sccomponents.gauges.ScWriter;

import java.util.UUID;

import de.mrapp.android.dialog.ProgressDialog;
import si.inova.neatle.Neatle;
import si.inova.neatle.monitor.Connection;
import si.inova.neatle.monitor.ConnectionMonitor;
import si.inova.neatle.monitor.ConnectionStateListener;
import si.inova.neatle.operation.CharacteristicSubscription;
import si.inova.neatle.operation.CharacteristicsChangedListener;
import si.inova.neatle.operation.CommandResult;
import si.inova.neatle.operation.Operation;
import si.inova.neatle.operation.OperationResults;
import si.inova.neatle.operation.SimpleOperationObserver;
import si.inova.neatle.source.ByteArrayInputSource;
import si.inova.neatle.source.InputSource;

public class ShiftIndicatorManagerActivity extends AppCompatActivity {

    UUID shifterServiceUUID = Neatle.createUUID(0x0000BBB0);
    UUID shifterService2UUID = Neatle.createUUID(0x0000BBB1);

    UUID rpmCharacteristicUUID = Neatle.createUUID(0x0000aaa0);
    UUID engineOilCharacteristicUUID = Neatle.createUUID(0x0000aaa1);
    UUID configSenderCharacteristicUUID = Neatle.createUUID(0x0000aaa2);
    UUID configReaderCharacteristicUUID = Neatle.createUUID(0x0000aaa3);

    UUID shifterActiveReaderCharacteristicUUID = Neatle.createUUID(0x0000aaa0);
    UUID shifterBrightnessReaderCharacteristicUUID = Neatle.createUUID(0x0000aaa1);

    Context context;
    ConnectionMonitor monitor;
    BluetoothDevice btDevice;

    TextView deviceNameTextView;
    TextView embeddedSoftwareVersionTextView;
    TextView btConnectionStateTextView;

    SeekBar brightnessSeekBar;

    Switch shifterActiveSwitch;

    Button saveShifterActiveButton;
    Button saveShifterBrightnessButton;

    ScArcGauge rpmGauge;
    TextView rpmTV;

    ScArcGauge engineOilGauge;
    TextView engineOilTV;

    ProgressDialog.Builder dialogBuilder;

    final String BT_CONNECTION_STATUS = "BT Connection Status: ";
    final String BT_CONNECTION_CONNECTED = "Device connected";
    final String BT_CONNECTION_DISCONNECTED = "Device disconnected";
    final String BT_CONNECTION_CONNECTING = "Connecting...";
    final String ENABLED = "ENABLED";
    final String DISABLED = "DISABLED";

    boolean firstConnect = true;

    ProgressDialog spinnerProgressDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_shift_indicator_manager);

        context = getBaseContext();
        Intent intent = getIntent();

        btDevice = intent.getParcelableExtra("BLEDevice");

        deviceNameTextView = (TextView) findViewById(R.id.deviceNameTV);
        deviceNameTextView.setText("Device Address: " + btDevice.getAddress());

        embeddedSoftwareVersionTextView = (TextView) findViewById(R.id.embeddedSoftwareVersionTV);
        btConnectionStateTextView = (TextView) findViewById(R.id.connectionStateTV);
        btConnectionStateTextView.setText(BT_CONNECTION_DISCONNECTED);

        brightnessSeekBar = (SeekBar) findViewById(R.id.brightnessSeekBar);

        shifterActiveSwitch = (Switch) findViewById(R.id.shifterActiveSwitch);

        saveShifterActiveButton = (Button) findViewById(R.id.shifterActiveSaveButton);

        saveShifterActiveButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                saveShifterActive(v);
            }
        });

        saveShifterBrightnessButton = (Button) findViewById(R.id.brightnessSaveButton);

        saveShifterBrightnessButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                saveShifterBrightness(v);
            }
        });

        dialogBuilder = new ProgressDialog.Builder(this);
        dialogBuilder.setTitle("Please wait");
        dialogBuilder.setMessage("Connecting to Device...");
        dialogBuilder.setProgressBarPosition(de.mrapp.android.dialog.ProgressDialog.ProgressBarPosition.LEFT);
        spinnerProgressDialog = dialogBuilder.create();
        spinnerProgressDialog.show();



        rpmGauge  = (ScArcGauge) this.findViewById(R.id.rpmGauge);
        rpmTV  = (TextView) this.findViewById(R.id.rpmTV);

        // Bring on top
        rpmGauge.bringOnTop(ScGauge.BASE_IDENTIFIER);
        rpmGauge.bringOnTop(ScGauge.NOTCHES_IDENTIFIER);

        // Set the value
        rpmGauge.setHighValue(0, 0, 7000);

        // Each time I will change the value I must write it inside the counter text.
        rpmGauge.setOnEventListener(new ScGauge.OnEventListener() {
            @Override
            public void onValueChange(float lowValue, float highValue) {
                // Write the value
                int value = (int) ScGauge.percentageToValue(highValue, 0, 7000);
                rpmTV.setText(Integer.toString(value));
            }
        });

        // Before draw
        rpmGauge.setOnDrawListener(new ScGauge.OnDrawListener() {
            @Override
            public void onBeforeDrawCopy(ScCopier.CopyInfo info) {
                // Check for the progress
                if (info.source.getTag() == ScGauge.PROGRESS_IDENTIFIER) {
                    // Scale
                    info.scale = new PointF(0.95f, 0.95f);
                    info.offset = new PointF(14.0f, 14.0f);
                }
            }

            @Override
            public void onBeforeDrawNotch(ScNotches.NotchInfo info) {
                // Set the length of the notch
                info.length = info.index == 0 || info.index == info.source.getCount() ?
                        rpmGauge.dipToPixel(15) : rpmGauge.dipToPixel(5);
                info.color = info.index > 6 ? Color.RED : Color.WHITE;
            }

            @Override
            public void onBeforeDrawPointer(ScPointer.PointerInfo info) {
                // Do nothing
            }

            @Override
            public void onBeforeDrawToken(ScWriter.TokenInfo info) {
                // Get the text bounds
                Rect bounds = new Rect();
                info.source.getPainter().getTextBounds(info.text, 0, info.text.length(), bounds);

                // Hide the first
                info.visible = info.index != 0;

                // Reset the angle and the offset
                info.angle = 0.0f;
                info.offset.y = bounds.height() / 2 + 5;
                info.offset.x = -bounds.width() * ((float) info.index / (float) info.source.getTokens().length);
            }
        });




        engineOilGauge  = (ScArcGauge) this.findViewById(R.id.engineOilGauge);
        engineOilTV  = (TextView) this.findViewById(R.id.engineOilTV);

        // Bring on top
        engineOilGauge.bringOnTop(ScGauge.BASE_IDENTIFIER);
        rpmGauge.bringOnTop(ScGauge.NOTCHES_IDENTIFIER);

        // Set the value
        engineOilGauge.setHighValue(0, 10, 170);

        // Each time I will change the value I must write it inside the counter text.
        engineOilGauge.setOnEventListener(new ScGauge.OnEventListener() {
            @Override
            public void onValueChange(float lowValue, float highValue) {
                // Write the value
                int value = (int) ScGauge.percentageToValue(highValue, 10, 170);
                engineOilTV.setText(Integer.toString(value));
            }
        });

        // Before draw
        engineOilGauge.setOnDrawListener(new ScGauge.OnDrawListener() {
            @Override
            public void onBeforeDrawCopy(ScCopier.CopyInfo info) {
                // Check for the progress
                if (info.source.getTag() == ScGauge.PROGRESS_IDENTIFIER) {
                    // Scale
                    info.scale = new PointF(0.95f, 0.95f);
                    info.offset = new PointF(14.0f, 14.0f);
                }
            }

            @Override
            public void onBeforeDrawNotch(ScNotches.NotchInfo info) {
                // Set the length of the notch
                info.length = info.index == 0 || info.index == info.source.getCount() ?
                        rpmGauge.dipToPixel(15) : rpmGauge.dipToPixel(5);
                if(info.index > 5 || info.index < 2){
                    info.color =  Color.RED;
                }else{
                    info.color=  Color.WHITE;
                }
            }

            @Override
            public void onBeforeDrawPointer(ScPointer.PointerInfo info) {
                // Do nothing
            }

            @Override
            public void onBeforeDrawToken(ScWriter.TokenInfo info) {
                // Get the text bounds
                Rect bounds = new Rect();
                info.source.getPainter().getTextBounds(info.text, 0, info.text.length(), bounds);

                // Hide the first
                info.visible = info.index != 0;

                // Reset the angle and the offset
                info.angle = 0.0f;
                info.offset.y = bounds.height() / 2 + 5;
                info.offset.x = -bounds.width() * ((float) info.index / (float) info.source.getTokens().length);
            }
        });




        disableView();
    }

    public void saveShifterActive(View v) {

        long readyData;
        if(shifterActiveSwitch.isChecked()){
            readyData = 0x01;
        }else{
            readyData = 0x00;
        }

        String readyDataString = Long.toString(readyData);
        byte[] dataToWrite = readyDataString.getBytes();

        InputSource inputSource = new ByteArrayInputSource(dataToWrite);

        Operation operation = Neatle.createOperationBuilder(context)
                .write(shifterService2UUID, shifterActiveReaderCharacteristicUUID, inputSource)
                .onFinished(new SimpleOperationObserver() {
                    @Override
                    public void onOperationFinished(Operation op, OperationResults results) {
                        if (results.wasSuccessful()) {
                            System.out.println("Write was successful!");
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    if (!isFinishing()) {
                                        new AlertDialog.Builder(ShiftIndicatorManagerActivity.this)
                                                .setTitle("Success")
                                                .setMessage("Shifter active settings saved to hardware.")
                                                .setCancelable(false)
                                                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                                                    @Override
                                                    public void onClick(DialogInterface dialog, int which) {

                                                    }
                                                }).show();
                                    }
                                }
                            });
                        } else {
                            System.out.println("Write failed! ");
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    if (!isFinishing()) {
                                        new AlertDialog.Builder(ShiftIndicatorManagerActivity.this)
                                                .setTitle("Error")
                                                .setMessage("Could not save shifter active settings to hardware.")
                                                .setCancelable(false)
                                                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                                                    @Override
                                                    public void onClick(DialogInterface dialog, int which) {

                                                    }
                                                }).show();
                                    }
                                }
                            });
                        }
                    }
                })
                .build(btDevice);
        operation.execute();
    }

    public void saveShifterBrightness(View v) {
        long readyData = brightnessSeekBar.getProgress();
        String readyDataString = Long.toString(readyData);
        byte[] dataToWrite = readyDataString.getBytes();

        InputSource inputSource = new ByteArrayInputSource(dataToWrite);

        Operation operation = Neatle.createOperationBuilder(context)
                .write(shifterService2UUID, shifterBrightnessReaderCharacteristicUUID, inputSource)
                .onFinished(new SimpleOperationObserver() {
                    @Override
                    public void onOperationFinished(Operation op, OperationResults results) {
                        if (results.wasSuccessful()) {
                            System.out.println("Write was successful!");
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    if (!isFinishing()) {
                                        new AlertDialog.Builder(ShiftIndicatorManagerActivity.this)
                                                .setTitle("Success")
                                                .setMessage("Shifter brightness settings saved to hardware.")
                                                .setCancelable(false)
                                                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                                                    @Override
                                                    public void onClick(DialogInterface dialog, int which) {

                                                    }
                                                }).show();
                                    }
                                }
                            });
                        } else {
                            System.out.println("Write failed! ");
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    if (!isFinishing()) {
                                        new AlertDialog.Builder(ShiftIndicatorManagerActivity.this)
                                                .setTitle("Error")
                                                .setMessage("Could not save shifter brightness settings to hardware.")
                                                .setCancelable(false)
                                                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                                                    @Override
                                                    public void onClick(DialogInterface dialog, int which) {

                                                    }
                                                }).show();
                                    }
                                }
                            });
                        }
                    }
                })
                .build(btDevice);
        operation.execute();
    }

    @Override
    protected void onStart() {
        super.onStart();

        ConnectionMonitor monitor =
                Neatle.createConnectionMonitor(context, btDevice);
        monitor.setKeepAlive(true);
        monitor.setOnConnectionStateListener(new ConnectionStateListener() {
            @Override
            public void onConnectionStateChanged(Connection connection, int newState) {
                if(connection.isConnected()){
                    btConnectionStateTextView.setText(BT_CONNECTION_STATUS+BT_CONNECTION_CONNECTED);
                    spinnerProgressDialog.setMessage("Loading Configuration...");
                                       

                }else{
                    btConnectionStateTextView.setText(BT_CONNECTION_STATUS+BT_CONNECTION_CONNECTING);
                    spinnerProgressDialog.setMessage("Connecting to Device...");
                    disableView();
                }
            }
        });
        monitor.start();

//############## config sender
        CharacteristicSubscription subscription =
                Neatle.createSubscription(context, btDevice, shifterServiceUUID, configSenderCharacteristicUUID);
        subscription.setOnCharacteristicsChangedListener(new CharacteristicsChangedListener() {
            @Override
            public void onCharacteristicChanged(CommandResult change) {
                if (change.wasSuccessful()) {
                    long y = change.getValueAsInt() & 0x00000000ffffffffL;

                    long versionMajor =y & 0xF;
                    long versionMinor =(y >> 12) & 0xF;
                    long versionBugfix = (y >> 8) & 0xF;
                    long shifterActive = (y >> 16) & 0xFF;
                    long shifterBrightness =(y >> 24) & 0xFF;

                    embeddedSoftwareVersionTextView.setText("Version "+versionMajor+"."+versionMinor+"."+versionBugfix);

                    if(shifterActive == 1) {
                        shifterActiveSwitch.setChecked(true);
                    }else{
                        shifterActiveSwitch.setChecked(false);
                    }

                    brightnessSeekBar.setProgress((int)shifterBrightness);

                    long readyData = 0xEFEFEF;
                    String readyDataString = Long.toString(readyData);
                    byte[] dataToWrite =readyDataString.getBytes();

                    InputSource inputSource = new ByteArrayInputSource(dataToWrite);

                    Operation operation = Neatle.createOperationBuilder(context)
                            .write(shifterServiceUUID, configReaderCharacteristicUUID, inputSource)
                            .onFinished(new SimpleOperationObserver() {
                                @Override
                                public void onOperationFinished(Operation op, OperationResults results) {
                                    if (results.wasSuccessful()) {
                                        System.out.println("Write was successful!");
                                        enableView();
                                    } else {
                                        System.out.println("Write failed! ");
                                    }
                                }
                            })
                            .build(btDevice);
                    operation.execute();
                }
            }
        });
        subscription.start();

        CharacteristicSubscription rpmDataSubscription =
                Neatle.createSubscription(context, btDevice, shifterServiceUUID, rpmCharacteristicUUID);
        rpmDataSubscription.setOnCharacteristicsChangedListener(new CharacteristicsChangedListener() {
            @Override
            public void onCharacteristicChanged(CommandResult change) {
                if (change.wasSuccessful()) {
                    rpmGauge.setHighValue(Integer.reverseBytes(change.getValueAsInt()),0,7000);
                    change.getValue();
                }
            }
        });
        rpmDataSubscription.start();

        CharacteristicSubscription eOilDataSubscription =
                Neatle.createSubscription(context, btDevice, shifterServiceUUID, engineOilCharacteristicUUID);
        eOilDataSubscription.setOnCharacteristicsChangedListener(new CharacteristicsChangedListener() {
            @Override
            public void onCharacteristicChanged(CommandResult change) {
                if (change.wasSuccessful()) {
                    long y = change.getValueAsInt() & 0x00000000ffffffffL;
                    y = y >>> 24;
                    engineOilGauge.setHighValue(y-40,10,170);
                }
            }
        });
        eOilDataSubscription.start();
    }

    @Override
    public void onPause() {
        super.onPause();
        if(monitor!=null) {
            monitor.stop();
        }
    }

    private void disableView(){
        spinnerProgressDialog.show();
            getWindow().setFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE,
                    WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE);
    }
    private void enableView(){
        spinnerProgressDialog.dismiss();
            getWindow().clearFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE);
    }
}
