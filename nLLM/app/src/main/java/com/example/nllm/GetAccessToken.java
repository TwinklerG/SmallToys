package com.example.nllm;

import android.content.Context;

import okhttp3.*;

import java.io.*;

public class GetAccessToken {
    static final OkHttpClient HTTP_CLIENT = new OkHttpClient().newBuilder().build();

    public String apiKey = "";
    public String secretKey = "";

    String get_access_token() throws IOException {

        MediaType mediaType = MediaType.parse("application/json");
        RequestBody body = RequestBody.create(mediaType, "");

        Request request = new Request.Builder()
                .url("https://aip.baidubce.com/oauth/2.0/token?client_id=" + apiKey + "&client_secret=" + secretKey + "&grant_type=client_credentials")
                .method("POST", body)
                .addHeader("Content-Type", "application/json")
                .addHeader("Accept", "application/json")
                .build();
        Response response = HTTP_CLIENT.newCall(request).execute();
        assert response.body() != null;
        String bodyString = response.body().string();
        if (bodyString.indexOf("\"scope") - 2 <= 0){
            return "fail";
        }
        return bodyString.substring(bodyString.indexOf("access_token") + 15, bodyString.indexOf("\"scope") - 2);
    }

    String get_access_token(String aK, String sK) throws IOException {
        MediaType mediaType = MediaType.parse("application/json");
        RequestBody body = RequestBody.create(mediaType, "");

        Request request = new Request.Builder()
                .url("https://aip.baidubce.com/oauth/2.0/token?client_id=" + aK + "&client_secret=" + sK + "&grant_type=client_credentials")
                .method("POST", body)
                .addHeader("Content-Type", "application/json")
                .addHeader("Accept", "application/json")
                .build();
        Response response = HTTP_CLIENT.newCall(request).execute();
        assert response.body() != null;
        String bodyString = response.body().string();
        if (bodyString.indexOf("\"scope") - 2 <= 0){
            return "fail";
        }
        return bodyString.substring(bodyString.indexOf("access_token") + 15, bodyString.indexOf("\"scope") - 2);
    }
}
