package com.example.nllm

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.EditText
import androidx.fragment.app.Fragment
import java.time.LocalDate
import java.time.format.DateTimeFormatter
import android.content.SharedPreferences
import android.widget.Toast

class DailyPoemFragment : Fragment() {

    private lateinit var beginTodayPoem: Button
    private lateinit var specificDayPoem: Button
    private lateinit var month: EditText
    private lateinit var day: EditText


    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view: View = inflater.inflate(R.layout.fragment_daily_poem, container, false)
        beginTodayPoem = view.findViewById(R.id.btn_begin_today_poem)
        specificDayPoem = view.findViewById(R.id.btn_begin_specific_day)
        month = view.findViewById(R.id.edt_month)
        day = view.findViewById(R.id.edt_day)

        beginTodayPoem.setOnClickListener {
            val currentDate = LocalDate.now()
            val formatter = DateTimeFormatter.ofPattern("MMdd")
            val dateString = currentDate.format(formatter)
            val editor = activity?.getSharedPreferences("dateString", Context.MODE_PRIVATE)?.edit()
            editor?.putString("dateString", dateString)
            editor?.apply()
            val intent = Intent(activity, Poem::class.java)
            startActivity(intent)
        }

        specificDayPoem.setOnClickListener OnClickListener@{
            val mm = month.text.toString()
            val dd = day.text.toString()
            val editor = activity?.getSharedPreferences("dateString", Context.MODE_PRIVATE)?.edit()
            editor?.putString("dateString","${mm}${dd}")
            editor?.apply()
            val tryId = resources.getIdentifier("poem_${mm}${dd}_title", "string",
                activity?.packageName
            )
            if (tryId == 0){
                Toast.makeText( activity, "格式错误或资源不存在", Toast.LENGTH_SHORT).show()
                return@OnClickListener
            }
            val intent = Intent(activity, Poem::class.java)
            startActivity(intent)
        }

        return view
    }
}