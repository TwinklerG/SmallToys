package com.example.nllm

import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.GridLayout.Spec
import android.widget.TextView
import android.widget.Toast
import androidx.fragment.app.Fragment
import java.util.Random

class AboutFragment : Fragment() {

    private lateinit var btnSetApi: Button
    private lateinit var btnAbout: Button
    private lateinit var txvRanPoem: TextView
    private lateinit var btnMoreFun: Button

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        val view: View = inflater.inflate(R.layout.fragment_about, container, false)
        btnSetApi = view.findViewById(R.id.btn_set_api)
        btnAbout = view.findViewById(R.id.btn_about)
        txvRanPoem = view.findViewById(R.id.txv_random_poem)
        btnMoreFun = view.findViewById(R.id.btn_more_fun)

        val random = Random()
        val MM = random.nextInt(9) + 1
        val dd = random.nextInt(9) + 1
        val id = random.nextInt(5) + 1
        val rfpId = resources.getIdentifier("rfp_0${MM}0${dd}${id}", "string", requireActivity().packageName)
        txvRanPoem.text = resources.getString(rfpId)


        btnSetApi.setOnClickListener {
            val intent = Intent(activity, SetApi::class.java)
            startActivity(intent)
        }

        btnAbout.setOnClickListener {
            val intent = Intent(activity, About::class.java)
            startActivity(intent)
        }

        btnMoreFun.setOnClickListener {
            val intent = Intent(activity, SpecialHtd::class.java)
            startActivity(intent)
        }

        return view
    }
}