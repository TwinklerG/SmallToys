package com.example.nllm

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity

class SpecialHtd : AppCompatActivity() {

    private lateinit var edtMmHtd : EditText
    private lateinit var edtDdHtd : EditText
    private lateinit var btnSpHtd : Button

    private lateinit var txvHthdItem1: TextView;
    private lateinit var txvHthdItem2: TextView;
    private lateinit var txvHthdItem3: TextView;
    private lateinit var txvHthdItem4: TextView;
    private lateinit var txvHthdItem5: TextView;
    private lateinit var txvHthdItem6: TextView;
    private lateinit var txvHthdItem7: TextView;
    private lateinit var txvHthdItem8: TextView;
    private lateinit var txvHthdDay: TextView
//    private lateinit var btnBeginHSpDay: Button
//    private lateinit var edtHthdE: EditText
//    private lateinit var txvHthdE: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_special_htd)

        edtMmHtd = findViewById(R.id.edt_month_htd)
        edtDdHtd = findViewById(R.id.edt_day_htd)
        btnSpHtd = findViewById(R.id.btn_begin_special_htd)

        btnSpHtd.setOnClickListener {
            val mm = edtMmHtd.text.toString()
            val dd = edtDdHtd.text.toString()
            val editor = getSharedPreferences("dateString", Context.MODE_PRIVATE)?.edit()
            editor?.putString("dateString","${mm}${dd}")
            editor?.apply()
            val tryId = resources.getIdentifier("htd_${mm}${dd}1", "string",
                packageName
            )
            if (tryId == 0){
                Toast.makeText(this, "格式错误或资源不存在", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            setContentView(R.layout.activity_history_that_day)
            txvHthdItem1 = findViewById(R.id.txv_hthd_item1)
            txvHthdItem2 = findViewById(R.id.txv_hthd_item2)
            txvHthdItem3 = findViewById(R.id.txv_hthd_item3)
            txvHthdItem4 = findViewById(R.id.txv_hthd_item4)
            txvHthdItem5 = findViewById(R.id.txv_hthd_item5)
            txvHthdItem6 = findViewById(R.id.txv_hthd_item6)
            txvHthdItem7 = findViewById(R.id.txv_hthd_item7)
            txvHthdItem8 = findViewById(R.id.txv_hthd_item8)
            txvHthdDay = findViewById(R.id.txv_htd_that_day)
//            btnBeginHSpDay = findViewById(R.id.btn_hthd_begin_e)
//            txvHthdE = findViewById(R.id.txv_hthd_e)

            val txvHtdItems: Array<TextView> = arrayOf(txvHthdItem1, txvHthdItem2, txvHthdItem3, txvHthdItem4, txvHthdItem5, txvHthdItem6, txvHthdItem7, txvHthdItem8)

            txvHthdDay.text = "${mm.toInt()}月${dd.toInt()}日"

            for (i in 1..8){
                val tHtdName = "htd_${mm}${dd}" + "$i"
                val htdId = resources.getIdentifier(tHtdName, "string", packageName)
                if (htdId == 0){
                    txvHtdItems[i - 1].text = ""
                } else {
                    txvHtdItems[i - 1].text = resources.getString(htdId)
                }
            }
        }
    }
}