package de.mnnbrt.shiftindicator;

import android.Manifest;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.ImageDecoder;
import android.graphics.drawable.Animatable2;
import android.graphics.drawable.AnimatedImageDrawable;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.widget.ImageView;

import si.inova.neatle.Neatle;
import si.inova.neatle.scan.ScanEvent;
import si.inova.neatle.scan.ScanMode;
import si.inova.neatle.scan.Scanner;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import java.io.IOException;

import de.mrapp.android.dialog.ProgressDialog;
import si.inova.neatle.Neatle;
import si.inova.neatle.scan.ScanMode;

public class MainActivity extends AppCompatActivity implements Scanner.NewDeviceFoundListener{

    Scanner scanner;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        requestPermissions(new String[]{
                Manifest.permission.ACCESS_COARSE_LOCATION,
                Manifest.permission.BLUETOOTH_ADMIN
        }, 123);

        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            // We cannot scan as we do not have location permission
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (!isFinishing()) {
                        new AlertDialog.Builder(MainActivity.this)
                                .setTitle("No location access permission")
                                .setMessage("Location access is neccessary for accessing Bluetooth Devices.")
                                .setCancelable(false)
                                .setPositiveButton("Exit", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        finish();
                                        System.exit(0);
                                    }
                                }).show();
                    }
                }
            });
            return;
        }

        scanner = Neatle.createScannerBuilder().setNewDeviceFoundListener(this).build();
        ScanMode scanMode = new ScanMode();
        scanMode.setMode(ScanMode.SCAN_MODE_BALANCED);
        scanner.setMode(scanMode);

        ImageView imageView = findViewById(R.id.img_gif);


        try {
            ImageDecoder.Source source =
                    ImageDecoder.createSource(getResources(), R.drawable.mini_init);

            Drawable drawable = ImageDecoder.decodeDrawable(source);
            imageView.setImageDrawable(drawable);

            if (drawable instanceof AnimatedImageDrawable) {
                ((AnimatedImageDrawable) drawable).setRepeatCount(0);
                ((AnimatedImageDrawable) drawable).registerAnimationCallback(new Animatable2.AnimationCallback() {
                    @Override
                    public void onAnimationEnd(Drawable drawable) {
                        super.onAnimationEnd(drawable);

                        ProgressDialog.Builder dialogBuilder = new ProgressDialog.Builder(MainActivity.this);
                        dialogBuilder.setTitle("Please wait");
                        dialogBuilder.setMessage("Searching Device...");
                        dialogBuilder.setProgressBarPosition(ProgressDialog.ProgressBarPosition.LEFT);
                        ProgressDialog dialog = dialogBuilder.create();
                        dialog.show();

                        scanner.startScanning(MainActivity.this);

                        Handler handler = new Handler();
                        int delay = 5000; //milliseconds

                        handler.postDelayed(new Runnable() {
                            public void run() {
                                scanner.stopScanning();
                                scanner.startScanning(MainActivity.this);
                            }
                        }, delay);
                        //Intent intent = new Intent(getBaseContext(), BTScanActivity.class);

                        //startActivity(intent);
                        //finish();
                    }
                });
                ((AnimatedImageDrawable) drawable).start();
            }
        } catch (IOException e) {
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if(scanner!=null) {
            scanner.stopScanning();
        }
    }

    @Override
    public void onNewDeviceFound(ScanEvent e) {
        System.out.println(e.toString());
        if ((e.getDevice().getName() != null) && (e.getDevice().getName().equals("SHIFT INDICATOR"))) {
            scanner.stopScanning();
            Intent intent = new Intent(this, ShiftIndicatorManagerActivity.class);
            intent.putExtra("BLEDevice", e.getDevice());
            startActivity(intent);
        }
    }
}
