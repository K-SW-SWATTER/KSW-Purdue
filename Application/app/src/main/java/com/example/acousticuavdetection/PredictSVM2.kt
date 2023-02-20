//package com.example.acousticuavdetection
//
//import android.app.Activity
//import android.app.ProgressDialog
//import android.content.Intent
//import android.os.AsyncTask
//import android.os.Bundle
//import android.provider.SyncStateContract
//import android.support.v4.app.Fragment
//import android.text.TextUtils
//import android.util.Log
//import android.view.LayoutInflater
//import android.view.View
//import android.view.ViewGroup
//import android.widget.*
//import androidx.core.app.ActivityCompat.startActivityForResult
//import androidx.fragment.app.Fragment
//import com.aditya.filebrowser.Constants
//import com.aditya.filebrowser.FileChooser
//import umich.cse.yctung.androidlibsvm.LibSVM
//import java.security.AccessController.getContext
//
//class PredictFragment : Fragment() {
//    var progressDialog: ProgressDialog? = null
//    var predictButton: Button? = null
//    var testFilePicker: Button? = null
//    var modelFilePicker: Button? = null
//    var outputFileNameInput: EditText? = null
//    var probabilityInput: EditText? = null
//    var probabilityCheckbox: CheckBox? = null
//    fun onCreate(savedInstanceState: Bundle?) {
//        super.onCreate(savedInstanceState)
//    }
//
//    fun onCreateView(
//        inflater: LayoutInflater, container: ViewGroup?,
//        savedInstanceState: Bundle?
//    ): View {
//        // Inflate the layout for this fragment
//        val view: View = inflater.inflate(R.layout.fragment_predict, container, false)
//        progressDialog = ProgressDialog(getContext())
//        testFilePicker = view.findViewById<View>(R.id.testfilepicker) as Button
//        modelFilePicker = view.findViewById<View>(R.id.modelfilepicker) as Button
//        predictButton = view.findViewById<View>(R.id.predict_btn) as Button
//        outputFileNameInput = view.findViewById<View>(R.id.output_filename) as EditText
//        probabilityInput = view.findViewById<View>(R.id.probabilityinput) as EditText
//        probabilityCheckbox = view.findViewById<View>(R.id.probabilitycheckbox) as CheckBox
//        probabilityCheckbox!!.setOnCheckedChangeListener { buttonView, isChecked ->
//            if (isChecked) {
//                probabilityInput!!.isEnabled = true
//            } else {
//                probabilityInput!!.isEnabled = false
//            }
//        }
//        testFilePicker!!.setOnClickListener {
//            val i2 = Intent(getContext(), FileChooser::class.java)
//            i2.putExtra(
//                SyncStateContract.Constants.SELECTION_MODES,
//                SyncStateContract.Constants.SELECTION_MODES.SINGLE_SELECTION.ordinal()
//            )
//            startActivityForResult(i2, PICK_TESTFILE)
//        }
//        modelFilePicker!!.setOnClickListener {
//            val i2 = Intent(getContext(), FileChooser::class.java)
//            i2.putExtra(
//                SyncStateContract.Constants.SELECTION_MODE,
//                SyncStateContract.Constants.SELECTION_MODES.SINGLE_SELECTION.ordinal()
//            )
//            startActivityForResult(i2, PICK_MODELFILE)
//        }
//        predictButton!!.setOnClickListener(View.OnClickListener {
//            val commands: MutableList<String> = ArrayList()
//            if (probabilityCheckbox!!.isChecked) {
//                if (!Utility.isEmptyOrWhitespace(probabilityInput!!.text.toString())) {
//                    commands.add("-b")
//                    commands.add(probabilityInput!!.text.toString())
//                }
//            }
//            commands.add(testFilePicker!!.text.toString())
//            commands.add(modelFilePicker!!.text.toString())
//            if (Utility.isEmptyOrWhitespace(outputFileNameInput!!.text.toString())) {
//                Toast.makeText(getContext(), "Output file name is required!", Toast.LENGTH_SHORT)
//                    .show()
//                return@OnClickListener
//            }
//            commands.add(ContainerActivity.appFolderPath + outputFileNameInput!!.text.toString())
//            AsyncPredictTask().execute(*commands.toTypedArray())
//        })
//        return view
//    }
//
//    fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
//        super.onActivityResult(requestCode, resultCode, data)
//        if (requestCode == PICK_TESTFILE && data != null) {
//            if (resultCode == Activity.RESULT_OK) {
//                val file = data.data
//                testFilePicker!!.text = file!!.path
//            }
//        }
//        if (requestCode == PICK_MODELFILE && data != null) {
//            if (resultCode == Activity.RESULT_OK) {
//                val file = data.data
//                modelFilePicker!!.text = file!!.path
//            }
//        }
//    }
//
//    private inner class AsyncPredictTask :
//        AsyncTask<String?, Void?, Void?>() {
//        override fun onPreExecute() {
//            super.onPreExecute()
//            progressDialog!!.setTitle("SVM Predict")
//            progressDialog!!.setMessage("Executing svm-predict, please wait...")
//            progressDialog!!.show()
//            Log.d(
//                ContainerActivity.TAG,
//                "==================\nStart of SVM PREDICT\n=================="
//            )
//        }
//
//        protected override fun doInBackground(vararg params: String): Void? {
//            LibSVM.getInstance().predict(TextUtils.join(" ", params))
//            return null
//        }
//
//        override fun onPostExecute(result: Void?) {
//            progressDialog!!.dismiss()
//            Toast.makeText(
//                getContext(),
//                "SVM Predict has executed successfully!",
//                Toast.LENGTH_LONG
//            ).show()
//            Log.d(
//                ContainerActivity.TAG,
//                "==================\nEnd of SVM PREDICT\n=================="
//            )
//            Utility.readLogcat(getContext(), "SVM-Predict Results")
//        }
//    }
//
//    companion object {
//        var PICK_TESTFILE = 3000
//        var PICK_MODELFILE = 3001
//    }
//}