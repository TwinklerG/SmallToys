package com.example.nllm

import android.annotation.SuppressLint
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import kotlinx.coroutines.DelicateCoroutinesApi
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch


class SetApi : AppCompatActivity() {

    private lateinit var edtApiKey: EditText
    private lateinit var edtSecretKey: EditText
    private lateinit var btnTrySet: Button
    private lateinit var txvSetResult: TextView
    private lateinit var txvLastResult: TextView

    @OptIn(DelicateCoroutinesApi::class)
    @SuppressLint("SetTextI18n")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_set_api)

        edtApiKey = findViewById(R.id.edt_api_key)
        edtSecretKey = findViewById(R.id.edt_secret_key)
        btnTrySet = findViewById(R.id.btn_try_set)
        txvSetResult = findViewById(R.id.txv_set_result)
        txvLastResult = findViewById(R.id.txv_last_result)

        // 从data.xml读取数据
        val lKeys = getSharedPreferences("data", Context.MODE_PRIVATE)
        val lat = lKeys.getString("accessToken", "666")
        if (lat == "fail" || lat == "666"){
            txvLastResult.text = "API配置失败"
        } else {
            txvLastResult.text = "API配置成功! access_token为$lat"
        }
        btnTrySet.setOnClickListener {
            val apiKey = edtApiKey.text.toString()
            val secretKey = edtSecretKey.text.toString()
            if (apiKey.isEmpty() or secretKey.isEmpty()){
                return@setOnClickListener
            }
            // 存储数据进入data.xml
            val editor = getSharedPreferences("data", Context.MODE_PRIVATE).edit()
            editor.putString("apiKey", apiKey)
            editor.putString("secretKey", secretKey)
            Thread{
                val tem = GetAccessToken().get_access_token(apiKey, secretKey)
                editor.putString("accessToken", tem)
                editor.apply()
                val keys = getSharedPreferences("data", Context.MODE_PRIVATE)
                val accessToken = keys.getString("accessToken", "fail")
                runOnUiThread {
                    if (accessToken == "fail" || accessToken == "666"){
                        txvSetResult.text = "API配置失败"
                    } else {
                        txvSetResult.text = "API配置成功! access_token为$accessToken"
                    }
                    GlobalScope.launch {
                        runOnUiThread { Toast.makeText(this@SetApi, "程序将在3秒后重启", Toast.LENGTH_SHORT).show() }
                        delay(3000) // 异步暂停3秒
                        //重启应用
                        val intent = Intent(
                            applicationContext,
                            MainActivity::class.java
                        )
                        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK)
                        startActivity(intent)
                        finishAffinity()
                    }
                }
            }.start()
        }
    }
}