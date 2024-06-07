package com.example.nllm

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import java.time.LocalDate
import java.time.format.DateTimeFormatter

class HistoryTodayFragment : Fragment() {

    private lateinit var txvHtdItem1: TextView;
    private lateinit var txvHtdItem2: TextView;
    private lateinit var txvHtdItem3: TextView;
    private lateinit var txvHtdItem4: TextView;
    private lateinit var txvHtdItem5: TextView;
    private lateinit var txvHtdItem6: TextView;
    private lateinit var txvHtdItem7: TextView;
    private lateinit var txvHtdItem8: TextView;
    private lateinit var edtHtdE: EditText
    private lateinit var btnHtdBeginE: Button
    private lateinit var txvHtdE: TextView
    private lateinit var txvHtdDay: TextView

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view: View = inflater.inflate(R.layout.fragment_history_today, container, false)

        //找组件
        txvHtdItem1 = view.findViewById(R.id.txv_htd_item1)
        txvHtdItem2 = view.findViewById(R.id.txv_htd_item2)
        txvHtdItem3 = view.findViewById(R.id.txv_htd_item3)
        txvHtdItem4 = view.findViewById(R.id.txv_htd_item4)
        txvHtdItem5 = view.findViewById(R.id.txv_htd_item5)
        txvHtdItem6 = view.findViewById(R.id.txv_htd_item6)
        txvHtdItem7 = view.findViewById(R.id.txv_htd_item7)
        txvHtdItem8 = view.findViewById(R.id.txv_htd_item8)
        edtHtdE = view.findViewById(R.id.edt_htd_e)
        btnHtdBeginE = view.findViewById(R.id.btn_htd_begin_e)
        txvHtdE = view.findViewById(R.id.txv_htd_e)
        txvHtdDay = view.findViewById(R.id.txv_htd_day)

        val txvHtdItems: Array<TextView> = arrayOf(txvHtdItem1, txvHtdItem2, txvHtdItem3, txvHtdItem4, txvHtdItem5, txvHtdItem6, txvHtdItem7, txvHtdItem8)

        // 加载今日关键字
        val currentDate = LocalDate.now()
        val formatter = DateTimeFormatter.ofPattern("MMdd")
        val dateString: String = currentDate.format(formatter)
        val m: String = dateString.substring(0, 2).toInt().toString()
        val d: String = dateString.substring(2, 4).toInt().toString()
        txvHtdDay.text = "${m}月${d}日"

        val htdName = "htd_$dateString"
        for (i in 1..8){
            val tHtdName = htdName + "$i"
            val htdId = resources.getIdentifier(tHtdName, "string", requireActivity().packageName)
            if (htdId == 0){
                txvHtdItems[i - 1].text = "警告：资源加载错误或不存在"
            } else {
                txvHtdItems[i - 1].text = resources.getString(htdId)
            }
        }

        btnHtdBeginE.setOnClickListener{
            var id: String = edtHtdE.text.toString()
            if (id.isEmpty()){
                return@setOnClickListener
            }
            val intId: Int = id.toInt()
            id = intId.toString()
            val htdEName = "htd_e_${dateString}$id"
            val htdEId = resources.getIdentifier(htdEName, "string", requireActivity().packageName)
            if (htdEId == 0){
                Toast.makeText(activity, "资源加载错误或不存在", Toast.LENGTH_SHORT).show()
            } else {
                txvHtdE.text = resources.getString(htdEId)
            }
        }
    return view
    }
}