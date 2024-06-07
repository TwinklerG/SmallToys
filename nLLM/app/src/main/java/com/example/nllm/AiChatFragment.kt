package com.example.nllm

import android.content.Context
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.EditText
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView

class AiChatFragment : Fragment() {

    private val msgList = ArrayList<Msg>()
    private lateinit var adapter: MsgAdapter
    private lateinit var recyclerView: RecyclerView
    private lateinit var inputText: EditText
    private lateinit var send: Button

    private lateinit var accessToken: String

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view = inflater.inflate(R.layout.fragment_ai_chat, container, false)

        recyclerView = view.findViewById<RecyclerView>(R.id.recyclerView)
        inputText = view.findViewById<EditText>(R.id.inputText)
        send = view.findViewById<Button>(R.id.send)
        initMsg()
        val layoutManager = LinearLayoutManager(activity)
        recyclerView.layoutManager = layoutManager
        if (!::adapter.isInitialized) {
            adapter = MsgAdapter(msgList)
        }
        recyclerView.adapter = adapter
        send.setOnClickListener{
            val content = inputText.text.toString()
            if (content.isNotEmpty()) {
                val msg = Msg(content, Msg.TYPE_SENT)
                msgList.add(msg)
                adapter.notifyItemInserted(msgList.size - 1) // 当有新消息时，刷新RecyclerView中的显示
                recyclerView.scrollToPosition(msgList.size - 1)  // 将 RecyclerView 定位到最后一行
                inputText.setText("") // 清空输入框中的内容
                Thread{sendMessageToAI(content)}.start()
            }
        }


        return view
    }

    private fun initMsg() {
        msgList.clear()
        val lKeys = activity?.getSharedPreferences("data", Context.MODE_PRIVATE)
        val lat = lKeys?.getString("accessToken", "fail")
        if (lat != null) {
            if (lat == "fail" || lat == "666"){
                msgList.add(Msg("API配置失败", Msg.TYPE_RECEIVED))
            } else {
                msgList.add(Msg("API配置成功", Msg.TYPE_RECEIVED))
            }
            accessToken = lat
        }
    }

    private fun showResponse(response: String) {
        // 在子线程中修改UI需要回到UI线程，采用runOnUiThread方法
        activity?.runOnUiThread {
            val msg = Msg(response, Msg.TYPE_RECEIVED)
            msgList.add(msg)
            adapter.notifyItemInserted(msgList.size - 1) // 当有新消息时，刷新RecyclerView中的显示
            recyclerView.scrollToPosition(msgList.size - 1)  // 将 RecyclerView 定位到最后一行
        }
    }

    private fun sendMessageToAI(message: String) {
        Thread{
            // 向封装好的SmartClient类提供消息和密钥，获取回复
            val response: String? = SmartClient.getAnswer(message, accessToken)
            if (response == "cod") {
                showResponse("请求错误，请重新配置API。")
            } else if (response != null) {
                showResponse(response)
            }
        }.start()
    }
}