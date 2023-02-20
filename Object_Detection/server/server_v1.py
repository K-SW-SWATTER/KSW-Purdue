# -*- coding: utf-8 -*-
"""
Returning classification result

@author: Gwangwon Kim
version 2.0
"""
import os
import glob
import numpy as np
import pandas as pd
import pickle
import librosa
import joblib
import sys

import tensorflow as tf
from sklearn import svm


# Main function
# data_path : sound data file path (.wav)
# model_path : pretrained model file path (.pkl)
def detect_result(data_path, model_path):
    
    # Load model
    svm_model = joblib.load(model_path)

    # Load data
    signal, sr = librosa.load(data_path)
    
    # Extraction
    mfcc = librosa.feature.mfcc(signal,sr=sr,n_mfcc=40)
    #X = np.array(mfcc.tolist()).reshape(1,-1)

    # prediction
    #result = svm_model.predict(X)[0]
    
    return mfcc

result = detect_result('../Dataset/trim_test/audio2.wav', '../save/svm_model.pkl')
print(result, result.shape)




    