package com.example.android.drunkness_tester;

import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;
import javax.net.ssl.HttpsURLConnection;
import java.net.URLEncoder;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.LinkedHashMap;

import android.support.annotation.NonNull;
import android.util.Log;
import com.reimaginebanking.api.*;
import com.reimaginebanking.api.java.*;
import com.reimaginebanking.api.java.Constants.TransactionMedium;
import com.reimaginebanking.api.java.models.Transfer;


/*Available Customer IDs:
56c66be6a73e49274150757d
56c66be6a73e49274150757c
56c66be6a73e49274150757b
 */

public class BankAccount {
    //Basic URL Information
    public static String MY_API_KEY = "?key=f5972f689b2b7053bb96f5f28a89f5b9";
    public static String BASE_URL = "http://api.reimaginebanking.com/";
    //Amount to leave in account after transfer
    public static float AMOUNT_TO_LEAVE_IN_ACCOUNT = 20.00f;
    //Easy to read booleans
    public static boolean FAILURE = false;
    public static boolean SUCCESS = true;
    //Keys for Customer Information
    public static String FIRSTNAME = "firstName";
    public static String LASTNAME = "lastName";
    public static String CUSTOMERID = "customerID";
    //Standards for Getting and Posting
    public static DateFormat DATEFORMAT = new SimpleDateFormat("yyyy-MM-dd");
    public static String SPLITEXP = "[,]";


    public NessieClient nClient;
    private Map<String, String> customerInformation;
    private ArrayList<String> allAccounts;
    private String toAccount;
    private String fromAccount;
    private int fromBalance; //FORMAT: xxxx = $xx.xx
    private boolean accountIsVerified = false;


    public BankAccount(String first, String last, String theirID) throws Exception{
        //Verify account first. The account will remain verified after initialization
        if(!verifyAccount(first, last, theirID))
            return;
        NessieClient nClient = NessieClient.getInstance();
        nClient.setAPIKey(MY_API_KEY);
        accountIsVerified = true;
        //Populate customer information
        customerInformation = new HashMap<String, String>();
        customerInformation.put(FIRSTNAME, first);
        customerInformation.put(LASTNAME, last);
        customerInformation.put(CUSTOMERID, ""+theirID);
        allAccounts = new ArrayList<String>();
        //get all the accounts for the customer (stored in allAccounts)
        getAccounts();
    }

    //Set the account that money will be moved to
    public boolean setToAccount(String account){
        if(!verifyAccount())
            return FAILURE;
        if(allAccounts.contains(account) && account != fromAccount) {
            toAccount = account;
            return SUCCESS;
        }
        System.out.println("Customer doesn't have account: " + account + "\nor this account is already set as the \"From\" account");
        return FAILURE;
    }

    public String getFromAccount() {
        return fromAccount;
    }

    //set account that money will be moved from
    public boolean setFromAccount(String account){
        if(!verifyAccount())
            return FAILURE;
        if (allAccounts.contains(account) && account != toAccount) {
            fromAccount = account;
            return SUCCESS;
        }
        System.out.println("Customer doesn't have account: "+account +"\nor this account has already been set to the \"To\" account");
        return FAILURE;
    }

    public String getToAccount() {
        return toAccount;
    }

    //print and update the user's current balance
    public void printBalance() throws Exception{
        if(updateBalance()){
            System.out.print("Your current balance is: $");
            System.out.printf("%.2f\n", (float)(fromBalance/100));
        }
        return;
    }

    //Returns balance
    public float getBalance() throws Exception{
        if(updateBalance())
            return fromBalance/100;
        return -1;
    }


    public Map<String, String> getCustomerInformation() {
        return customerInformation;
    }


    //Post-Initial Verification
    public boolean verifyAccount(){
        if(accountIsVerified)
            return SUCCESS;
        System.out.println("Your account has not been verified. Please return to \nthe home screen to setup your account");
        return FAILURE;
    }

    public void onFailure(HttpURLConnection connection, String section) throws Exception{
        System.out.println("Error " + connection.getResponseCode() +" during " + section );
        InputStream is = connection.getErrorStream();
        int i = is.read();
        do{
            System.out.print((char)i);
            i = is.read();
        }
        while( i != -1);
        System.out.println();
        is.close();
    }


    //Inital Verification: Parameters must match parameters retrieved from server
    public boolean verifyAccount(String first, String last, String theirID) throws Exception{
        //Creating URL
        HttpURLConnection connection = getConnection(BASE_URL + "customers/" + theirID + MY_API_KEY);
        int responseCode = connection.getResponseCode();
        if(responseCode != 200){
            onFailure(connection, "account verification");
            return FAILURE;
        }
        String[] response = getResponse(connection);
        boolean isValidInformation = true;
        for(String current : response){
            String[] line =  current.split(":");
            String value = line[1].substring(1,line[1].length()-1).toLowerCase();
            //Verifying
            if(current.contains("first")){
                if(!value.contains(first.toLowerCase())){
                    //System.out.println("First Name Doesn't Match Account holder");
                    isValidInformation = false;
                }
            }
            else if(current.contains("last")){
                if(!value.contains(last.toLowerCase())) {
                    //System.out.println("Last Name Doesn't Match Account holder");
                    isValidInformation = false;
                }
            }
        }
        if(!isValidInformation){
            System.out.println("Unable to verify your account.");
            return FAILURE;
        }
        System.out.println("Account verified");
        return SUCCESS;
    }

    //Populate the list of accounts
    public boolean getAccounts() throws Exception{
        if(!verifyAccount())
            return FAILURE;
        HttpURLConnection connection = getConnection(BASE_URL + "customers/" + customerInformation.get(CUSTOMERID)
                + "/accounts" + MY_API_KEY);
        int responseCode = connection.getResponseCode();
        if(responseCode == 404){
            onFailure(connection, "account retrevial");
            return FAILURE;
        }
        String[] response = getResponse(connection);
        for(String current : response){
            String[] line = current.split(":");
            String value = line[1].substring(1,line[1].length()-1);
            if(current.contains("id") && !current.contains("customer")){
                allAccounts.add(value);
            }
        }
        if(allAccounts.size() <= 1){
            System.out.println("Error: Customer doesn't have any or only has one account");
            return FAILURE;
        }
        return SUCCESS;
    }

    //Setup a connection
    public HttpURLConnection getConnection(String s_url) throws  Exception{
        URL url = new URL(s_url);
        //Establishing Connection
        HttpURLConnection connection = (HttpURLConnection) url.openConnection();
        return connection;
    }

    //Format Connection Response
    public String[] getResponse(HttpURLConnection connection) throws Exception{
        BufferedReader rawResponse = new BufferedReader(new InputStreamReader(connection.getInputStream()));
        String current;
        String output = "";
        do{
            current = rawResponse.readLine();
            output += current+"\n";
        }while(current != null);
        rawResponse.close();
        return output.split(SPLITEXP);
    }

    //Send a post
    public boolean sendPost(HttpURLConnection connection, Map<String, String> parameters) throws Exception{
        StringBuilder data = new StringBuilder();
        for(Map.Entry<String, String> set : parameters.entrySet()){
            if(data.length() != 0)
                data.append('&');
            data.append(URLEncoder.encode(set.getKey(), "UTF-8"));
            data.append("=");
            if(set.getKey() == "amount"){
                data.append(Integer.parseInt(set.getValue()));
            }
            else
                data.append(URLEncoder.encode(set.getValue(), "UTF-8"));
            //connection.addRequestProperty(set.getKey(), set.getValue());
        }
        connection.setRequestMethod("POST");
        connection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded;charset=utf-8");
        connection.setRequestProperty("Content-Length", ""+data.toString().getBytes().length);
        connection.setDoInput(true);
        connection.setDoOutput(true);
        DataOutputStream wr = new DataOutputStream(connection.getOutputStream());
        BufferedWriter ds = new BufferedWriter(new OutputStreamWriter(wr, "UTF-8"));
        ds.write(data.toString());
        ds.flush(); ds.close();
        if(connection.getResponseCode() != 201){
            onFailure(connection, "send post");
            System.out.println(data.toString());
            return FAILURE;
        }

        return SUCCESS;
    }

    //update the balance
    public boolean updateBalance() throws Exception{
        if(!verifyAccount() && toAccount != null && fromAccount != null)
            return FAILURE;
        //Creating URL
        HttpURLConnection connection = getConnection(BASE_URL + "accounts/" + fromAccount + MY_API_KEY);
        //Evaluating Response Code
        int responseCode = connection.getResponseCode();
        if(responseCode != 200){
            onFailure(connection, "balance update");
            return FAILURE;
        }
        //Sifting Through Response
        String[] response = getResponse(connection);
        for(String current : response){
            //FORMAT: "balance": 34970,
            if(current.contains("balance")){
                String sub = current.split(":")[1];
                int b;
                try {
                    b = Integer.parseInt(sub);
                }catch(Exception e){
                    System.out.println("Cannot parse balance");
                    return FAILURE;
                }
                fromBalance = b;
                return SUCCESS;
            }
        }
        //Didn't find the word "balance"
        System.out.println("Unable to update balance");
        return FAILURE;
    }

    //Make a transfer: Transfer Amount = currentBalance - ammountoleaveinaccount;
    public boolean transfer() throws Exception{
        if(!updateBalance() && toAccount != null && fromAccount != null){
            System.out.println("Unable to complete transfer");
            return FAILURE;
        }
        Date d = new Date();
        Transfer transfer = new Transfer.Builder()
                .transaction_date(DATEFORMAT.format(d))
                .amount((fromBalance - AMOUNT_TO_LEAVE_IN_ACCOUNT*100)/100)
                .payee_id(toAccount)
                .medium(TransactionMedium.BALANCE)
                .status("pending")
                .build();
        NessieResultsListener nrl = new NessieResultsListener(){
            public void onSuccess(Object o, NessieException e){
                if( e != null)
                    System.out.println("Failure creating"
                            + "transfer: " +e.getMessage());
            }
        };
        nClient.createTransfer(fromAccount, transfer, nrl);
        updateBalance();
        //Print Log
        System.out.println("Transfer Successfull:");
        System.out.println("Date: " + DATEFORMAT.format(d));
        System.out.println("Status: " + transfer.getStatus());
        System.out.println("Transfer Amount: " + transfer.getAmount());
        System.out.println("From: " +  transfer.getPayer_id());
        System.out.println("To: " + transfer.getPayee_id());
        printBalance();
        return SUCCESS;
    }
}

