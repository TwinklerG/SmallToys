package com.example.nllm

import android.content.Context
import android.content.Intent
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

class FlyFlowerCmdFragment : Fragment() {

    private lateinit var btnFfoSend: Button
    private lateinit var edtFfoAns: EditText
    private lateinit var txvFfoDailyWord: TextView
    private lateinit var txvFfoAiResponse: TextView
    private lateinit var btnRefPoems: Button
    private lateinit var txvFfoCnt: TextView

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view = inflater.inflate(R.layout.fragment_fly_flower_cmd, container, false)

        btnFfoSend = view.findViewById<Button>(R.id.btn_ffo_send)
        edtFfoAns = view.findViewById(R.id.edt_ffo_ans)
        txvFfoDailyWord = view.findViewById(R.id.txv_ffo_daily_word)
        txvFfoAiResponse = view.findViewById(R.id.txv_ffo_ai_response)
        btnRefPoems = view.findViewById(R.id.btn_ref_poems)

        // 加载今日关键字
        val currentDate = LocalDate.now()
        val formatter = DateTimeFormatter.ofPattern("MMdd")
        val dateString = currentDate.format(formatter)
        val kwdName = "kwd_$dateString"
        val kwdId = resources.getIdentifier(kwdName, "string", requireActivity().packageName)
        txvFfoDailyWord.text = resources.getString(kwdId)

        btnFfoSend.setOnClickListener {
            var message: String = edtFfoAns.text.toString()
            if (message.isEmpty()){
                return@setOnClickListener
            }
            message =
                "$message。前面的这一句是中国古代诗词吗？且要求包含关键字${txvFfoDailyWord.text}。如果是的话请以True结束你的回答,否则以False结束你的回答。"
            Thread{
                val response: String? = SmartClient.getAnswer(message, requireActivity().getSharedPreferences("data", Context.MODE_PRIVATE).getString("accessToken", "666").toString())
                requireActivity().runOnUiThread{
                    if (response == "cod"){
                        txvFfoAiResponse.text = "请求错误，请重新配置API"
                    } else {
                        txvFfoAiResponse.text = response
                    }
                }
                if (response != null) {
                    if (response.substring(0.coerceAtLeast(response.length - 4), response.length) == "True"){
                        requireActivity().runOnUiThread {
                            Toast.makeText(activity, "回答正确", Toast.LENGTH_SHORT).show()
                        }
                    } else {
                        requireActivity().runOnUiThread {
                            Toast.makeText(activity, "回答错误", Toast.LENGTH_SHORT).show()
                        }
                    }
                }
            }.start()
        }

        btnRefPoems.setOnClickListener {
            val intent = Intent(activity, RefPoems::class.java)
            startActivity(intent)
        }


    return view
    }

}