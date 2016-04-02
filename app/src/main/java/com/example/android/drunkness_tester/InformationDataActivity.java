package com.example.android.drunkness_tester;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;


public class InformationDataActivity extends AppCompatActivity{

    public static final String TAG = InformationDataActivity.class.getSimpleName();

    private String mName;
    private EditText mNumberDrinks;
    private EditText mHeight;
    private ImageButton mTestButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_math_test);

        mNumberDrinks=(EditText)findViewById(R.id.numberDrinks);
        mHeight =(EditText) findViewById(R.id.heightInput);
        mTestButton=(ImageButton)findViewById(R.id.imageButton);

        if (savedInstanceState == null) {
            Bundle extras = getIntent().getExtras();
            if (extras == null) {
                mName = null;
            } else {
                mName = extras.getString("name");
            }
        } else {
            mName = (String) savedInstanceState.getSerializable("name");
        }

        writeMyName(mName);

        mTestButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                int numDrinks = Integer.parseInt(mNumberDrinks.getText().toString());
                int height = Integer.parseInt(mHeight.getText().toString());

                if(numDrinks>4 && height<1.6){
                    startTest();
                }else{
                    startNotDrunkActivity();
                }


            }
        });

    }

    public void writeMyName(String name){
        TextView mText=(TextView) findViewById(R.id.welcomeTextView);
        String welcomeText= "Hey "+ name+".Looks like you're looking to for a little fun. Let us be the grown up for tonight. ";
        mText.setText(welcomeText);
    }

    public void startTest(){
        Intent intent= new Intent(this,TestActivity.class);
        startActivity(intent);
    }

    public void startNotDrunkActivity(){
        Intent intent= new Intent(this,NotDrunkActivity.class);
        startActivity(intent);
    }




}
