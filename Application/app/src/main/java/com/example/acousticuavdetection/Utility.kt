//package com.test.libsvmandroidexample
//
//import android.app.AlertDialog
//import android.content.Context
//import android.util.Log
//import android.view.LayoutInflater
//import android.view.View
//import android.widget.TextView
//import com.example.acousticuavdetection.MainActivity
//import java.io.BufferedReader
//import java.io.IOException
//import java.io.InputStreamReader
//
//object Utility {
//    fun isEmptyOrWhitespace(str: String): Boolean {
//        return if (str.isEmpty() || str.trim { it <= ' ' }.isEmpty()) true else false
//    }
//
//    fun readLogcat(context: Context?, title: String?) {
//        try {
//            val process = Runtime.getRuntime().exec("logcat -d")
//            val bufferedReader = BufferedReader(InputStreamReader(process.inputStream))
//            val log = StringBuilder()
//            var line = ""
//            while (bufferedReader.readLine().also { line = it } != null) {
//                if (line.contains(MainActivity.processId) && line.contains("LibSvm")) {
//                    if (line.contains("=======")) {
//                        log.append("==================\n")
//                    } else if (line.contains("Start of SVM")) {
//                        log.append(
//                            """
//                                ${line.substring(line.indexOf("Start"))}
//
//                                """.trimIndent()
//                        )
//                    } else if (line.contains("End of SVM")) {
//                        log.append(
//                            """
//                                ${line.substring(line.indexOf("End"))}
//
//                                """.trimIndent()
//                        )
//                    } else {
//                        val indexOfProcessId: Int = line.lastIndexOf(ContainerActivity.processId)
//                        val newLine = line.substring(indexOfProcessId)
//                        log.append(
//                            """
//                                $newLine
//
//
//                                """.trimIndent()
//                        )
//                    }
//                }
//            }
//            showResult(context, log.toString(), title)
//        } catch (e: IOException) {
//            e.printStackTrace()
//            Log.e(ContainerActivity.TAG, "readLogcat: failed to read from logcat logger.")
//        }
//    }
//
//    fun showResult(context: Context?, resultText: String?, title: String?) {
//        val li = LayoutInflater.from(context)
//        val promptsView: View = li.inflate(R.layout.dialog_result, null)
//        val alertDialogBuilder = AlertDialog.Builder(context)
//            .setTitle(title)
//            .setView(promptsView)
//            .setNeutralButton(
//                "Ok"
//            ) { dialog, which -> dialog.dismiss() }
//        val resultTextView = promptsView.findViewById<View>(R.id.resulttextview) as TextView
//        resultTextView.text = resultText
//        val alertDialog = alertDialogBuilder.create()
//        alertDialog.show()
//    }
//}