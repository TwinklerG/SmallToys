package com.example.nllm

import android.content.Context
import android.os.Bundle
import android.widget.TextView
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat

class Poem : AppCompatActivity() {
    private lateinit var txvTitle: TextView
    private lateinit var txvContent: TextView
    private lateinit var txvBg: TextView
    private lateinit var txvComp: TextView
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_poem)

        txvTitle = findViewById(R.id.txv_dp_title)
        txvContent = findViewById(R.id.txv_dp_content)
        txvBg = findViewById(R.id.txv_dp_bg)
        txvComp = findViewById(R.id.txv_dp_comp)

        val dateString = getSharedPreferences("dateString", Context.MODE_PRIVATE)
            .getString("dateString", "0101")
        val titleId = resources.getIdentifier("poem_${dateString}_title", "string", packageName)
        val contentId = resources.getIdentifier("poem_${dateString}_content", "string", packageName)
        val bgId = resources.getIdentifier("poem_${dateString}_background", "string", packageName)
        val compId = resources.getIdentifier("poem_${dateString}_comp", "string", packageName)
        val title = resources.getString(titleId)
        val content = resources.getString(contentId)
        val bg = resources.getString(bgId)
        val comp = resources.getString(compId)
        txvTitle.text = title
        txvContent.text = content
        txvBg.text = bg
        txvComp.text = comp
        //        val titleId = resources.getIdentifier(tHtdName, "string", packageName)
    }
}