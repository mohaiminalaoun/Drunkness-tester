package com.example.android.drunkness_tester;

import android.content.Context;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

public class TestActivity extends AppCompatActivity {

    private ImageButton mStartButton;
    private TextView mInstructions;
    private Boolean mClicked=false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);
        mStartButton = (ImageButton) findViewById(R.id.imageButtonTest);
        mInstructions=(TextView)findViewById(R.id.instructions);

    }

    public void onResume(Bundle savedInstanceState){
        mStartButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mClicked==true){
                    Context context = getApplicationContext();
                    CharSequence text = "Test in progress";
                    int duration = Toast.LENGTH_SHORT;

                    Toast toast = Toast.makeText(context, text, duration);
                    toast.show();
                }else {
                    mClicked=true;
                    runInBackground();
                    mClicked=false;
                }


            }
        });
    }


             //this method runs the backgrund stuff and returns control to he UI
             public void runInBackground() {
                 //need to make sure button is only clicked once during one testing
                 new Thread(new Runnable() {
                     @Override
                     public void run() {
                         // DO your work here
                         // get the data
                         if (true) {
                             runOnUiThread(new Runnable() {
                                 @Override
                                 public void run() {
                                     //update UI


                                     finish();
                                 }
                             });
                         } else {
                             //do something else
                         }
                     }
                 }).start();
             }
         }



