#include "pch.h"
#include "MLManager.h"


MLManager::MLManager()
{
    Init();
}

MLManager::~MLManager()
{
    
}

void MLManager::Init()
{

    Py_Initialize();
    _pName = PyUnicode_DecodeFSDefault(_MLFileName.c_str());
    /* Error checking of pName left out */

    _pModule = PyImport_Import(_pName);
    Py_DECREF(_pName);

    if (_pModule == NULL)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", _MLFileName);
        ASSERT_CRASH(true == false);
    }
    _MLFunctionName.push_back("detect_result");
}


int8 MLManager::RunModel(const float data[], uint16 featureCount)
{
	if (Py_IsInitialized() == false)
	{
		Py_Initialize();
	}
	WRITE_LOCK;
    PyObject* pArgs, * pValue, * pData, * pModelPath;


    ASSERT_CRASH(_pModule != NULL)
    _pFunc = PyObject_GetAttrString(_pModule, _MLFunctionName[0].c_str());
    /* pFunc is a new reference */
	if (_pFunc && PyCallable_Check(_pFunc)) // python func can be called
	{
		pModelPath = PyUnicode_DecodeFSDefault(_modelPath.c_str());
		pArgs = PyTuple_New(2);
		pData = PyTuple_New(featureCount);

		// Set arguments put to python Func
		for (int elementNum = 0; elementNum < featureCount; ++elementNum)
		{
			pValue = PyFloat_FromDouble(data[elementNum]);
			if (!pValue)
			{
				Py_DECREF(pData);
				Py_DECREF(_pModule);
				fprintf(stderr, "Cannot convert argument\n");
				return 1;
			}
			/* pValue reference stolen here: */
			if (PyTuple_SetItem(pData, elementNum, pValue) == -1)
			{
				Py_DECREF(pData);
				Py_DECREF(_pModule);
				fprintf(stderr, "Cannot append argument\n");
				return 1;
			}
		}

		PyTuple_SetItem(pArgs, 0, pData); // First argument of detect_result func
		PyTuple_SetItem(pArgs, 1, pModelPath); // Second argument

		pValue = PyObject_CallObject(_pFunc, pArgs); // Get the result
		Py_DECREF(pArgs);
		if (pValue != NULL) // Succeeded to call the python function
		{
			int8 result = PyLong_AsLong(pValue);
			printf("Result of call: %d\n", result);
			Py_DECREF(pValue);
			return result;
		}
		else
		{
			Py_DECREF(_pFunc);
			Py_DECREF(_pModule);
			PyErr_Print();
			fprintf(stderr, "Call failed\n");
			ASSERT_CRASH(true == false);
		}
	}
	else
	{
		if (PyErr_Occurred())
			PyErr_Print();
		fprintf(stderr, "Cannot find function \"%s\"\n", _MLFunctionName[0].c_str());
		ASSERT_CRASH(true == false);
	}
	Py_XDECREF(_pFunc);
	Py_DECREF(_pModule);

	if (Py_FinalizeEx() < 0)
	{
		printf("Py_FinalizeEx() < 0 \n");
		ASSERT_CRASH(true == false);
	}
}