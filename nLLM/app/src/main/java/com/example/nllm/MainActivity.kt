package com.example.nllm

import android.os.Bundle
import android.view.View
import android.widget.LinearLayout
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.app.AppCompatDelegate
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentManager

class MainActivity : AppCompatActivity(), View.OnClickListener {

    private lateinit var dailyPoemFragment: Fragment
    private lateinit var aiChatFragment: Fragment
    private lateinit var historyTodayPoemFragment: Fragment
    private lateinit var aboutFragment: Fragment
    private lateinit var flyFlowerCmdFragment: FlyFlowerCmdFragment

    private lateinit var aboutLayout: LinearLayout
    private lateinit var dpLayout: LinearLayout
    private lateinit var acLayout: LinearLayout
    private lateinit var histLayout: LinearLayout
    private lateinit var ffoLayout: LinearLayout

    private var transaction: Int = 0
    private lateinit var manager: FragmentManager


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
//        //禁用夜间模式
//        AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO)

        dailyPoemFragment = DailyPoemFragment()
        aiChatFragment = AiChatFragment()
        historyTodayPoemFragment = HistoryTodayFragment()
        aboutFragment = AboutFragment()
        flyFlowerCmdFragment = FlyFlowerCmdFragment()

        aboutLayout = findViewById(R.id.about)
        dpLayout = findViewById(R.id.daily_poem)
        acLayout = findViewById(R.id.ai_chat)
        histLayout = findViewById(R.id.history_today)
        ffoLayout = findViewById(R.id.fly_flower_cmd)

        //初始化
        manager = supportFragmentManager
        initial()
        hideFragment()
        showFragment(aboutFragment)

        aboutLayout.setOnClickListener(this)
        dpLayout.setOnClickListener(this)
        acLayout.setOnClickListener(this)
        histLayout.setOnClickListener(this)
        ffoLayout.setOnClickListener(this)
    }

    private fun initial(){
        transaction = manager.beginTransaction()
            .add(R.id.content, dailyPoemFragment)
            .add(R.id.content, aiChatFragment)
            .add(R.id.content, historyTodayPoemFragment)
            .add(R.id.content, aboutFragment)
            .add(R.id.content, flyFlowerCmdFragment)
            .commit()
        aboutLayout.setBackgroundColor(ContextCompat.getColor(applicationContext, R.color.purple))
    }

    override fun onClick(view: View){
        hideFragment()
        aboutLayout.setBackgroundColor(ContextCompat.getColor(applicationContext, R.color.white))
        dpLayout.setBackgroundColor(ContextCompat.getColor(applicationContext, R.color.white))
        acLayout.setBackgroundColor(ContextCompat.getColor(applicationContext, R.color.white))
        histLayout.setBackgroundColor(ContextCompat.getColor(applicationContext, R.color.white))
        ffoLayout.setBackgroundColor(ContextCompat.getColor(applicationContext, R.color.white))
        when (view.id){
            R.id.about -> {
                showFragment(aboutFragment)
                aboutLayout.setBackgroundColor(ContextCompat.getColor(applicationContext, R.color.purple))
            }
            R.id.daily_poem -> {
                showFragment(dailyPoemFragment)
                dpLayout.setBackgroundColor(ContextCompat.getColor(applicationContext, R.color.purple))
            }
            R.id.ai_chat -> {
                showFragment(aiChatFragment)
                acLayout.setBackgroundColor(ContextCompat.getColor(applicationContext, R.color.purple))
            }
            R.id.history_today -> {
                showFragment(historyTodayPoemFragment)
                histLayout.setBackgroundColor(ContextCompat.getColor(applicationContext, R.color.purple))
            }
            R.id.fly_flower_cmd -> {
                showFragment(flyFlowerCmdFragment)
                ffoLayout.setBackgroundColor(ContextCompat.getColor(applicationContext, R.color.purple))
            }
        }
    }

    private fun showFragment(fragment: Fragment){
        transaction = manager.beginTransaction()
            .show(fragment)
            .commit()
    }

    private fun hideFragment(){
        transaction = manager.beginTransaction()
            .hide(dailyPoemFragment)
            .hide(aiChatFragment)
            .hide(historyTodayPoemFragment)
            .hide(aboutFragment)
            .hide(flyFlowerCmdFragment)
            .commit()
    }
}