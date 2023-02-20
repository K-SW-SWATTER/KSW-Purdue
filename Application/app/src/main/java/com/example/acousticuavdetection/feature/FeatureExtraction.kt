package com.example.acousticuavdetection.feature

import android.app.Application
import android.content.Context
import android.os.Environment
import android.util.Log
import kotlinx.coroutines.Dispatchers
import java.io.File
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.newCoroutineContext
import org.merlyn.kotlinspeechfeatures.MathUtils
import org.merlyn.kotlinspeechfeatures.SpeechFeatures
import java.io.FileOutputStream
import java.security.AccessController.getContext
import kotlin.coroutines.CoroutineContext

class FeatureExtraction(application: Application) : AndroidViewModel(application) {
    private val TAG = "AcousticUAVDetection"
    private val speechFeatures = SpeechFeatures()
    var audio_num = 0
    var outputFile: File? = null
    var fileOutputStream: FileOutputStream? = null
    var fileBody = "audio_feature"
    var filePath:String = "${application.getExternalFilesDir(Environment.DIRECTORY_MUSIC)}"
    val data = arrayOf(arrayOf(1.0f, 2.0f), arrayOf(3.0f, 4.0f))
    fun performMfcc(fileIndex: Int) {
        viewModelScope.launch(Dispatchers.IO) {
            //val wav = loadWavFile(fileFromAsset(filePath, "audio${String.format(fileIndex.toString(),"%02d")}.wav"))
            val wav = loadWavFile(File(filePath + "/uav_audio/audio${String.format(fileIndex.toString(),"%02d")}.wav"))
            val result = speechFeatures.mfcc(MathUtils.normalize(wav), sampleRate = 22050, nfft = 2048, numCep = 40, nFilt = 128)
            val process_result = to2DFloatArray(result)
            outputFile = File("${filePath}/uav_feature/", "audio_feature${String.format(fileIndex.toString(),"%02d")}")
            fileOutputStream = FileOutputStream(outputFile)
            fileOutputStream!!.write(process_result.contentToString().toByteArray())
            fileOutputStream!!.close()
            Log.d(TAG, "${result.javaClass}"+"mfcc output:")
            result.forEach {
                Log.d(TAG, it.contentToString())
            }
        }
    }

    private fun loadWavFile(file: File): IntArray {
        val wavFile = WavFile.openWavFile(file)
        val numFrames = wavFile.numFrames.toInt()
        val channels = wavFile.numChannels
        val loopCounter: Int = numFrames * channels / 4096+1
        val intBuffer = IntArray(numFrames)
        for (i in 0 until loopCounter){
            wavFile.readFrames(intBuffer, numFrames)
        }
        return intBuffer
    }
    private fun to2DFloatArray(array: Array<FloatArray>): FloatArray {
        val rows = array.size
        val cols = array[0].size
        var sum = 0F
        val result = FloatArray(40)
        for (i in 0 until rows) {
            sum = 0F
            for (j in 0 until cols) {
                sum += array[i][j]
            }
            result[i] = sum / cols
        }
        return result
    }

//    fun toNumPyArray(array: Array<FloatArray>): Array<FloatArray> {
//        val tfliteTensor = JniInterface.convertArrayToTfliteTensor(to2DFloatArray(array), intArrayOf(array.size, array[0].size), 1)
//        val buffer = tfliteTensor.buffer
//        val numRows = array.size
//        val numCols = array[0].size
//        val result = Array(numRows) { FloatArray(numCols) }
//        for (i in 0 until numRows) {
//            for (j in 0 until numCols) {
//                result[i][j] = buffer.float
//            }
//        }
//        return result
//    }


    private fun fileFromAsset(directory: String, name: String): File { // Need to modify for collected audio data
        val context = getApplication<Application>()
        val cacheDir = context.cacheDir
        return File("$cacheDir/$name").apply { writeBytes(context.assets.open("$directory/$name").readBytes()) }
    }
}