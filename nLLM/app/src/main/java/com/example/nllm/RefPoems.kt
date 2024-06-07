package com.example.nllm

import android.os.Bundle
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import java.time.LocalDate
import java.time.format.DateTimeFormatter

class RefPoems : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ref_poems)
        val currentDate = LocalDate.now()
        val formatter = DateTimeFormatter.ofPattern("MMdd")
        val dateString = currentDate.format(formatter)
        for (i in 1..10) {
            val rfpId = resources.getIdentifier("rfp_${dateString}$i", "string", packageName)
            val textViewId = resources.getIdentifier("txv_rfp_$i", "id", packageName)
            val textView: TextView = findViewById(textViewId)
            textView.text = resources.getString(rfpId)
        }
    }
}