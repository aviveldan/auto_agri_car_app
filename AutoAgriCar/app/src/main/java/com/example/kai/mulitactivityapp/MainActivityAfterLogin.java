package com.example.kai.mulitactivityapp;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.List;

public class MainActivityAfterLogin extends AppCompatActivity implements View.OnClickListener {

    public static int pathViewHeight;
    InputStream in;
    public static OutputStream out;
    android.widget.Button send;
    android.widget.Button sendSavedPathButton;
    android.widget.Button drawModeButton;
    static PathDrawView pathView;
    boolean stop = false;
    byte read[];
    private List<String> stringList;
    ManualControlView manualView;
    public pathMode PathMode;
    public enum pathMode {PATH_MODE, WATER_RIGHT, WATER_LEFT, WATER_STRAIGHT};
    public String DemoPathString = "0 25 2 90.0  4 999 1 90.0 0 25 1 90.0 0 25 2 90.0  4 999 1 90.0 0 25 2 270.0 0 25 2 90.0  4 999 1 90.0 0 25 2 270.0 0 25 2 90.0  4 999 1 90.0 0 25 ";

    // Write a message to the database
    FirebaseDatabase database = FirebaseDatabase.getInstance();
    DatabaseReference myRef = database.getReference("message");
    DatabaseReference PathFireBase = database.getReference("Path");
    DatabaseReference status = database.getReference("Status");

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        PathMode = pathMode.PATH_MODE;
        PreferenceManager.getDefaultSharedPreferences(getApplicationContext()).edit().putString("DrawMode", "PathMode").apply();
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        pathView = (PathDrawView)findViewById(R.id.canvas);

        SharedPreferences shPref = PreferenceManager.getDefaultSharedPreferences(getBaseContext());



        pathView.addSharedPreferences(shPref);
        PreferenceManager.setDefaultValues(this, R.xml.prefrences, true);
        String option = shPref.getString("PREF_LIST", "Medium");
        if(option == "Low"){

        }


        drawModeButton = (android.widget.Button)findViewById(R.id.drawModeButton);

        send = (android.widget.Button)findViewById(R.id.send);
        send.setOnClickListener(this);

        sendSavedPathButton = (android.widget.Button) findViewById(R.id.SendSavedPathButton);
        pathView.setswapButton(sendSavedPathButton);
        manualView = (ManualControlView) findViewById(R.id.manualView);
        myRef.setValue("Hello, World! 1");
    }

    public void onClick(View v) {
        myRef.setValue("MANUAL Button Clicked"); // MANUAL button

        if (v.getId() == R.id.SendSavedPathButton) {
            PathFireBase.setValue(DemoPathString);
        }
            if(v.getId() == R.id.send) {
                myRef.setValue("SEND Button Clicked"); // SEND button
                //PathFireBase.setValue("SEND Button Clicked");

                StringBuilder joinedString = new StringBuilder();
                stringList = pathView.stringList;
                for (String s : stringList) {
                    joinedString.append(s);
                }
                PathFireBase.setValue(joinedString.toString());
                status.setValue("ready");
                if(out!= null) {

                try {
                    if (pathView.stringList == null) {
                        Toast.makeText(getApplicationContext(), "Success",
                                Toast.LENGTH_SHORT).show();
                    } else {
                        if (pathView.stringList.size() > 0) {
                            stringList = pathView.stringList;
                            for (String s : stringList) {
                                out.write(s.getBytes());
                            }
                            Toast.makeText(getApplicationContext(), "Success",
                                    Toast.LENGTH_SHORT).show();
                        } else {
                            Toast.makeText(getApplicationContext(), "Success",
                                    Toast.LENGTH_SHORT).show();
                        }

                    }
                        pathView.resetObstacleDetected();

                    }catch(IOException e){
                        e.printStackTrace();
                    }

                }else{
                    Toast.makeText(getApplicationContext(), "Success",
                            Toast.LENGTH_SHORT).show();
                }
            }
            if(v.getId() == R.id.drawModeButton){
                if(PathMode == pathMode.PATH_MODE){
                    drawModeButton.setText("Water Right");
                    PreferenceManager.getDefaultSharedPreferences(getApplicationContext()).edit().putString("DrawMode", "PlantModeRight").apply();
                    PathMode = pathMode.WATER_RIGHT;
                }

                else if(PathMode == pathMode.WATER_RIGHT){
                    drawModeButton.setText("Water Left");
                    PreferenceManager.getDefaultSharedPreferences(getApplicationContext()).edit().putString("DrawMode", "PlantModeLeft").apply();
                    PathMode = pathMode.WATER_LEFT;
                }
                else if(PathMode == pathMode.WATER_LEFT){
                    drawModeButton.setText("Water Straight");
                    PreferenceManager.getDefaultSharedPreferences(getApplicationContext()).edit().putString("DrawMode", "PlantModeStraight").apply();
                    PathMode = pathMode.WATER_STRAIGHT;
                }
                else if(PathMode == pathMode.WATER_STRAIGHT){
                    drawModeButton.setText("Path Mode");
                    PreferenceManager.getDefaultSharedPreferences(getApplicationContext()).edit().putString("DrawMode", "PathMode").apply();
                    PathMode = pathMode.PATH_MODE;
                }
            }
            if(v.getId() == R.id.resetButton){
                finish();
                startActivity(getIntent());
            }
    }



    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        switch(item.getItemId()){
            case R.id.action_settings:
                Intent settingsIntent = new Intent(MainActivityAfterLogin.this,
                        SettingsActivity.class);

                settingsIntent.putExtra("width",pathView.getWidth());

                startActivity(settingsIntent);
                break;

        }


        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onRestart() {
        pathView.validateLine();
        pathView.invalidate();
        super.onRestart();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        pathViewHeight = pathView.getHeight();
    }
}



