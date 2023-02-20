package com.example.acousticuavdetection

import android.app.Application
import android.content.Context
import android.media.MediaRecorder
import android.os.Environment
import android.provider.MediaStore.Audio.Media
import android.util.Log
import android.widget.Toast
import androidx.annotation.MainThread
import androidx.core.os.EnvironmentCompat
import java.io.File
import java.io.IOException
import androidx.core.content.ContextCompat
import com.example.acousticuavdetection.feature.FeatureExtraction
import com.github.squti.androidwaverecorder.RecorderState
import com.github.squti.androidwaverecorder.WaveRecorder
import kotlinx.coroutines.MainScope
import java.util.*
import kotlin.concurrent.fixedRateTimer
import kotlin.concurrent.timerTask

class AudioRecorder(context: Context) {
    private var mIsRecording: Boolean? = false
    private var mContext: Context = context
    private var mStartTime: Long = 0
    var filePath:String = "${Environment.getExternalStorageDirectory()}/uav_audio/audio"
    val waveRecorder = WaveRecorder(filePath)
    var divideTimer: Timer? = null
    private var mfcc: FeatureExtraction? = null
    init {
    }
    fun startRecording() {
        mfcc = FeatureExtraction(mContext.applicationContext as Application)
        var fileIndex = 0 //index of file
        //var basePath = "${Environment.getExternalStorageDirectory()}/uav_audio/audio" //Base path of audio files
        val basePath = "${mContext.getExternalFilesDir(Environment.DIRECTORY_MUSIC)}/uav_audio/audio"
        //Look for old audio files and delete them
        while (File(basePath + String.format(fileIndex.toString(), "%02d") + ".wav").exists()) {
            File(basePath + String.format(fileIndex.toString(), "%02d") + ".wav").delete()
            fileIndex++
        }

        fileIndex = 0 //reset fileIndex counter to 0
        mIsRecording = true

        mStartTime = System.currentTimeMillis()
        if (mIsRecording as Boolean) {
            //Run on new thread since it'll be in the background
            waveRecorder.changeFilePath(
                basePath + String.format(
                    fileIndex.toString(),
                    "%02d"
                ) + ".wav"
            )
            waveRecorder.waveConfig.sampleRate = 22050
            waveRecorder.startRecording()
            waveRecorder.onAmplitudeListener = {
                Log.i(TAG, "Amplitude : $it")
            }
            divideTimer = Timer()
            divideTimer!!.scheduleAtFixedRate(
                timerTask {
                    // SomethingToDo..
                    waveRecorder.stopRecording()
                    mfcc!!.performMfcc(fileIndex)
                    fileIndex++ //increment file name
                    waveRecorder.changeFilePath(
                        basePath + String.format(
                            fileIndex.toString(),
                            "%02d"
                        ) + ".wav"
                    )
                    waveRecorder.startRecording()
                }, 5000, 5000)
        }
    }


    fun stopRecording() {
        waveRecorder.stopRecording()
        mIsRecording = false
    }
    fun stopTimer() {
        divideTimer?.cancel()
    }
    companion object {
        private const val TAG = "AudioRecorder"
    }
}

