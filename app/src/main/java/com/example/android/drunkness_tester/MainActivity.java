package com.example.android.drunkness_tester;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {

    private EditText mNameField;
    private Button mStartButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mNameField = (EditText) findViewById(R.id.nameEditText);
        mStartButton=(Button)findViewById(R.id.startButton);
        //anonymous inner class as parameter
        mStartButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                String name=mNameField.getText().toString();
                startTest(name);
            }
        });
    }


    public void startTest(String name){
        Intent intent= new Intent(this, InformationDataActivity.class);
        intent.putExtra("name",name);
        startActivity(intent);
    }


}
