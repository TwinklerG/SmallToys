package com.example.nllm

import androidx.appcompat.app.AppCompatActivity
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody
import okhttp3.RequestBody.Companion.toRequestBody

class SmartClient: AppCompatActivity() {
    companion object {
        fun getAnswer(question: String, access_token: String): String? {
            val API_URL = "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/eb-instant?access_token=$access_token"
            val client = OkHttpClient()
            val jsonContent =
                "{\"messages\": [{\"role\": \"user\", \"content\": \"$question\"}]}"
            val body: RequestBody = jsonContent.toRequestBody("application/json; charset=utf-8".toMediaType())
            val request: Request = Request.Builder()
                .url(API_URL)
                .post(body)
                .build()
            val jsonResponse = client.newCall(request).execute().body?.string() ?: return "请求错误，请重试"
            val stringResponse = jsonResponse.substring(
                jsonResponse.indexOf("result") + 9,
                jsonResponse.indexOf("result") + 9 + jsonResponse.substring(
                    jsonResponse.indexOf(
                        "result"
                    ) + 9
                ).indexOf('"'))
            val response: StringBuffer = StringBuffer()
            for (i in 0 until stringResponse.length - 1){
                if (stringResponse[i] == '\\' && stringResponse[i + 1] == 'n'){
                    response.append("\n")
                }else if (i > 0 && stringResponse[i] == 'n' && stringResponse[i - 1] == '\\'){
                    response.append("")
                } else {
                    response.append(stringResponse[i])
                }
            }
            return response.toString()
        }
    }
}